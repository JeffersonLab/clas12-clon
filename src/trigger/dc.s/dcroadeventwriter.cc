
/* dcroadeventwriter.cc */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

using namespace std;
#include <strstream>


#include "dclib.h"


//#define DEBUG


/* 3.93/?/?/0%/0%/(233)~0%/(148)~0% II=4 */

void
dcroadeventwriter(hls::stream<trig_t> &trig_stream, hls::stream<eventdata5_t> &event_stream, hit_ram_t buf_ram_read[RAMSIZE])
{
#pragma HLS DATA_PACK variable=buf_ram_read
#pragma HLS DATA_PACK variable=event_stream
#pragma HLS INTERFACE ap_fifo port=event_stream
#pragma HLS DATA_PACK variable=trig_stream
#pragma HLS INTERFACE ap_fifo port=trig_stream
//#pragma HLS ARRAY_PARTITION variable=buf_ram_read block factor=4

  eventdata5_t eventdata;

  trig_t trig = trig_stream.read();
  ap_uint<RAMBITS> w = (trig.t_stop - trig.t_start); /* readout window width */
  ap_uint<RAMBITS> start_addr = trig.t_start.range(10,2); /* starting position in buf_ram_read[] */
  ap_uint<RAMBITS> stop_addr = trig.t_stop.range(10,2); /* ending position in buf_ram_read[] */
  ap_uint<RAMBITS> addr = start_addr;

  while(addr != stop_addr)
  {
    hit_ram_t buf_ram_val = buf_ram_read[addr];

#ifdef DEBUG
	std::cout<<"dcroadeventwriter: buf_ram_val.output="<<buf_ram_val.output<<endl;
#endif

    if(buf_ram_val.output != 0)
	{
      //eventdata.end = 0;
      eventdata.data[0](31,31)  = 1;
      eventdata.data[0](30,23)  = DC1ROAD_TAG;
      eventdata.data[0](26,16)  = addr - start_addr;  /*8 bits*/
      eventdata.data[0](15,0)   = 0;
      //event_stream.write(eventdata);
#ifdef DEBUG
	  std::cout<<"dcroadeventwriter: write1="<<eventdata.data<<endl;
#endif

      //eventdata.end = 0;
      eventdata.data[1](31,19)  = 0;
      eventdata.data[1](18,0)   = buf_ram_val.output(111,93); /*19 bits*/
      //event_stream.write(eventdata);
#ifdef DEBUG
	  std::cout<<"dcroadeventwriter: write2="<<eventdata.data<<endl;
#endif

      //eventdata.end = 0;
      eventdata.data[2](31,31)  = 0;
      eventdata.data[2](30,0)   = buf_ram_val.output(92,62); /*31 bits*/
      //event_stream.write(eventdata);
#ifdef DEBUG
	  std::cout<<"dcroadeventwriter: write3="<<eventdata.data<<endl;
#endif

      //eventdata.end = 0;
      eventdata.data[3](31,31)  = 0;
      eventdata.data[3](30,0)   = buf_ram_val.output(61,31); /*31 bits*/
      //event_stream.write(eventdata);
#ifdef DEBUG
	  std::cout<<"dcroadeventwriter: write4="<<eventdata.data<<endl;
#endif

      eventdata.end = 0;
      eventdata.data[4](31,31)  = 0;
      eventdata.data[4](30,0)   = buf_ram_val.output(30,0); /*31 bits*/
      event_stream.write(eventdata);
#ifdef DEBUG
	  std::cout<<"dcroadeventwriter: write5="<<eventdata.data<<endl;
#endif
	}

    addr ++; /*jump to the next 32ns interval*/
  }

  for(int i=0; i<5; i++) eventdata.data[i] = 0xFFFFFFFF;
  eventdata.end = 1;
  event_stream.write(eventdata);
  
}



