
/* echiteventwriter.cc */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

using namespace std;
#include <strstream>


#ifdef USE_PCAL

#include "../pc.s/pclib.h"
#define echiteventwriter pchiteventwriter

#else

#include "eclib.h"

#endif

//#define DEBUG


/* 3.93/?/?/0%/0%/(233)~0%/(148)~0% II=4 */

void
echiteventwriter(ap_uint<1> inst, hls::stream<trig_t> &trig_stream, hls::stream<eventdata_t> &event_stream,
                 hit_ram_t buf_ram_read[NHIT][256])
{
#pragma HLS DATA_PACK variable=buf_ram_read
#pragma HLS DATA_PACK variable=event_stream
#pragma HLS INTERFACE ap_fifo port=event_stream
#pragma HLS DATA_PACK variable=trig_stream
#pragma HLS INTERFACE ap_fifo port=trig_stream
#pragma HLS DATA_PACK variable=inst
#pragma HLS ARRAY_PARTITION variable=buf_ram_read block factor=4

  eventdata_t eventdata;

/*#ifdef __SYNTHESIS__*/
  trig_t trig = trig_stream.read();
  ap_uint<11> w = (trig.t_stop - trig.t_start); /* readout window width */
  ap_uint<8> start_addr = trig.t_start.range(10,3); /* starting position in buf_ram_read[] */
  ap_uint<8> stop_addr = trig.t_stop.range(10,3); /* ending position in buf_ram_read[] */
  ap_uint<8> addr = start_addr;

  while(addr != stop_addr)
/*#else
  ap_uint<8> start_addr = 0;
  ap_uint<8> addr = 0;
#endif*/
  {
    for(int i=0; i<NHIT; i++)
	{
      hit_ram_t buf_ram_val = buf_ram_read[i][addr];

      if(buf_ram_val.energy)
	  {
        eventdata.end = 0;
        eventdata.data(31,31)  = 1;
        eventdata.data(30,27)  = ECHIT_TAG;
        eventdata.data(26,26)  = inst;
        eventdata.data(25,24)  = 0;
        eventdata.data(23,16)  = addr - start_addr;  /*8 bits*/
        eventdata.data(15,0)   = buf_ram_val.energy; /*16 bits*/
        event_stream.write(eventdata);
        
        eventdata.end = 0;
        eventdata.data(31,30)  = 0;
        eventdata.data(29,20)  = buf_ram_val.coordW; /*10 bits*/
        eventdata.data(19,10)  = buf_ram_val.coordV; /*10 bits*/
        eventdata.data(9,0)    = buf_ram_val.coordU; /*10 bits*/
        event_stream.write(eventdata);

#ifndef __SYNTHESIS__
        eventdata.end = 0;
        eventdata.data(31,22)  = 0;
        eventdata.data(21,16)  = buf_ram_val.ind;     /*6 bits*/
        eventdata.data(15,0)   = buf_ram_val.enpeakW; /*16 bits*/
        event_stream.write(eventdata);

        eventdata.end = 0;
        eventdata.data(31,16)  = buf_ram_val.enpeakV; /*16 bits*/
        eventdata.data(15,0)   = buf_ram_val.enpeakU; /*16 bits*/
        event_stream.write(eventdata);
#endif
	  }
	}

    addr ++;
  }

  eventdata.data = 0xFFFFFFFF;
  eventdata.end = 1;
  event_stream.write(eventdata);
  
}



