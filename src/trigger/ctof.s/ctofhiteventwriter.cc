
/* ctofhiteventwriter.cc */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

using namespace std;
#include <strstream>


#include "ctoflib.h"


//#define DEBUG


/* 3.93/?/?/0%/0%/(233)~0%/(148)~0% II=4 */

void
ctofhiteventwriter(hls::stream<trig_t> &trig_stream, hls::stream<eventdata3_t> &event_stream, event_ram_t buf_ram_read[NRAM])
{
#pragma HLS DATA_PACK variable=buf_ram_read
#pragma HLS DATA_PACK variable=event_stream
#pragma HLS INTERFACE ap_fifo port=event_stream
#pragma HLS DATA_PACK variable=trig_stream
#pragma HLS INTERFACE ap_fifo port=trig_stream
//#pragma HLS ARRAY_PARTITION variable=buf_ram_read block factor=4

  eventdata3_t eventdata;

  trig_t trig = trig_stream.read();
  ap_uint<11> w = (trig.t_stop - trig.t_start); /* readout window width */
  ap_uint<11> start_addr = trig.t_start.range(10,0); /* starting position in buf_ram_read[] */
  ap_uint<11> stop_addr = trig.t_stop.range(10,0); /* ending position in buf_ram_read[] */
  ap_uint<11> addr = start_addr;

  while(addr != stop_addr)
  {
#pragma HLS PIPELINE enable_flush
    event_ram_t buf_ram_val = buf_ram_read[addr];

#ifdef DEBUG
	std::cout<<endl<<"ctofhiteventwriter: addr="<<addr<<", buf_ram_val.output="<<hex<<buf_ram_val.output<<dec<<endl;
#endif

    if(buf_ram_val.output != 0)
	{
      //eventdata.end = 0;
      eventdata.data[0](31,31)  = 1;
      eventdata.data[0](30,27)  = CTOFHIT_TAG;
      eventdata.data[0](26,16)  = addr - start_addr;  /*11 bits*/ /* +8 for OLD - see ctofstrips.cc */
      eventdata.data[0](15,0)   = 0;/*buf_ram_val.standalone;*/
      //event_stream.write(eventdata);
#ifdef DEBUG
	  std::cout<<"ctofhiteventwriter: write1="<<hex<<eventdata.data[0]<<dec<<endl;
#endif

      //eventdata.end = 0;
      eventdata.data[1](31,31)  = 0;
      eventdata.data[1](30,17)  = 0;
      eventdata.data[1](16,0)   = buf_ram_val.output(47,31); /*17 bits*/
      //event_stream.write(eventdata);
#ifdef DEBUG
	  std::cout<<"ctofhiteventwriter: write2="<<hex<<eventdata.data[1]<<dec<<endl;
#endif

      eventdata.end = 0;
      eventdata.data[2](31,31)  = 0;
      eventdata.data[2](30,0)   = buf_ram_val.output(30,0); /*31 bits*/
      event_stream.write(eventdata);
#ifdef DEBUG
	  std::cout<<"ctofhiteventwriter: write3="<<hex<<eventdata.data[2]<<dec<<endl;
#endif
	}

    addr ++; /*jump to the next 32ns interval*/
  }

  for(int i=0; i<3; i++) eventdata.data[i] = 0xFFFFFFFF;
  eventdata.end = 1;
  event_stream.write(eventdata);
  
}



