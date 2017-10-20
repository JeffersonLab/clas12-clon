
/* htcchiteventwriter.cc */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

using namespace std;
#include <strstream>


#include "htcclib.h"


//#define DEBUG


/* 2.66/?/?/0%/0%/(187)~0%/(150)~0% II=1 */

/* called every event, read chunk of ram and create one event */

void
htcchiteventwriter(hls::stream<trig_t> &trig_stream, hls::stream<eventdata_t> &event_stream, hit_ram_t buf_ram_read[NRAM])
{
#pragma HLS DATA_PACK variable=buf_ram_read
#pragma HLS DATA_PACK variable=event_stream
#pragma HLS INTERFACE ap_fifo port=event_stream
#pragma HLS DATA_PACK variable=trig_stream
#pragma HLS INTERFACE ap_fifo port=trig_stream
//#pragma HLS ARRAY_PARTITION variable=buf_ram_read block factor=4

  eventdata_t eventdata;

  trig_t trig = trig_stream.read();
  ap_uint<RAM_BITS> w = (trig.t_stop - trig.t_start); /* readout window width */
  ap_uint<RAM_BITS> start_addr = trig.t_start.range(10,0/*3*/); /* starting position in buf_ram_read[] */
  ap_uint<RAM_BITS> stop_addr = trig.t_stop.range(10,0/*3*/); /* ending position in buf_ram_read[] */
  ap_uint<RAM_BITS> addr = start_addr;

  while(addr != stop_addr)
  {
    hit_ram_t buf_ram_val = buf_ram_read[addr];

    if(buf_ram_val.output)
	{
      eventdata.end = 0;
      eventdata.data(31,31)  = 1;
      eventdata.data(30,27)  = HTCCHIT_TAG;
      eventdata.data(26,16)  = addr - start_addr;  /*11 bits*/
      eventdata.data(15,12)  = 0;
      eventdata.data(11,0)   = buf_ram_val.output(35,24); /*12 bits*/
      event_stream.write(eventdata);
        
      eventdata.end = 0;
      eventdata.data(31,24)  = 0;
      eventdata.data(23,0)    = buf_ram_val.output(23,0); /*24 bits*/
      event_stream.write(eventdata);
	}

    //cout<<"htcchiteventwriter: addr="<<addr<<endl;
    addr ++;
  }

  eventdata.data = 0xFFFFFFFF;
  eventdata.end = 1;
  event_stream.write(eventdata);
  
}



