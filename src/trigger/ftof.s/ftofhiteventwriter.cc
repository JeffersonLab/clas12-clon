
/* ftofhiteventwriter.cc */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

using namespace std;
#include <strstream>


#include "ftoflib.h"


//#define DEBUG


/* 3.93/?/?/0%/0%/(233)~0%/(148)~0% II=4 */

void
ftofhiteventwriter(hls::stream<trig_t> &trig_stream, hls::stream<eventdata_t> &event_stream, event_ram_t buf_ram_read[2048])
{
#pragma HLS DATA_PACK variable=buf_ram_read
#pragma HLS DATA_PACK variable=event_stream
#pragma HLS INTERFACE ap_fifo port=event_stream
#pragma HLS DATA_PACK variable=trig_stream
#pragma HLS INTERFACE ap_fifo port=trig_stream
//#pragma HLS ARRAY_PARTITION variable=buf_ram_read block factor=4

  eventdata_t eventdata;

  trig_t trig = trig_stream.read();
  ap_uint<11> w = (trig.t_stop - trig.t_start); /* readout window width */
  ap_uint<11> start_addr = trig.t_start.range(10,0); /* starting position in buf_ram_read[] */
  ap_uint<11> stop_addr = trig.t_stop.range(10,0); /* ending position in buf_ram_read[] */
  ap_uint<11> addr = start_addr;

  while(addr != stop_addr)
  {
    event_ram_t buf_ram_val = buf_ram_read[addr];

	{
      if(buf_ram_val.output != 0)
	  {
        eventdata.end = 0;
        eventdata.data(31,31)  = 1;
        eventdata.data(30,27)  = FTOFHIT_TAG;
        eventdata.data(26,24)  = 0;
        eventdata.data(23,13)  = addr - start_addr;  /*8 bits*/
        eventdata.data(12,0)   = 0;
        event_stream.write(eventdata);

        eventdata.end = 0;
        eventdata.data(31,31)  = 0;
        eventdata.data(30,0)   = buf_ram_val.output(61,31); /*31 bits*/
        event_stream.write(eventdata);

        eventdata.end = 0;
        eventdata.data(31,31)  = 0;
        eventdata.data(30,0)   = buf_ram_val.output(30,0); /*31 bits*/
        event_stream.write(eventdata);
	  }
	}

    addr ++; /*jump to the next 32ns interval*/
  }

  eventdata.data = 0xFFFFFFFF;
  eventdata.end = 1;
  event_stream.write(eventdata);
  
}



