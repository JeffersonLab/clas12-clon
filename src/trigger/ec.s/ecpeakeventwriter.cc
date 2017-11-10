
/* ecpeakeventwriter.cc - writes peaks to the event */

/*
getting peaks [and clusters] written to events. I attached the event builder code I use for peaks and clusters to demonstrate
the interface to the trigger and event stream. In both functions there are two types common type that interface to the readout system:

    hls::stream<trig_t> &trig_stream
         Trigger stream - there is one word to read this stream for each trigger the VTP receives.
            trig_t.t_start  is the 4ns timestamp the VTP readout window starts processing from
            trig_t.t_stop   is the 4ns timestamp the VTP readout window stops processing from
            These timestamps are 11bits, so that's 8192ns of time that constantly wraps around

    hls::stream<eventdata_t> &event_stream
         For each trigger you 32bit data words into event_stream:
            eventdata_t.data - the 32bit data words you want to go into the event
            eventdata_t.end  - a flag that must be set 1 with a dummy value on eventdata_t.data to indicate the end of event

Ben
*/


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

using namespace std;
#include <strstream>


#ifdef USE_PCAL

#include "../pc.s/pclib.h"
#define ecpeakeventwriter pcpeakeventwriter

#else

#include "eclib.h"

#endif

//#define DEBUG



void
ecpeakeventwriter(ap_uint<2> view, ap_uint<1> inst, hls::stream<trig_t> &trig_stream, hls::stream<eventdata_t> &event_stream,
                  peak_ram_t buf_ram_read[NPEAK][256])
{
#pragma HLS DATA_PACK variable=buf_ram_read
#pragma HLS DATA_PACK variable=event_stream
#pragma HLS INTERFACE ap_fifo port=event_stream
#pragma HLS DATA_PACK variable=trig_stream
#pragma HLS INTERFACE ap_fifo port=trig_stream
#pragma HLS INTERFACE ap_stable port=inst
#pragma HLS INTERFACE ap_stable port=view
#pragma HLS ARRAY_PARTITION variable=buf_ram_read block factor=4
  eventdata_t eventdata;

  trig_t trig = trig_stream.read();
  ap_uint<11> w = (trig.t_stop - trig.t_start); /* readout window width */
  ap_uint<8> start_addr = trig.t_start.range(10,3); /* starting position in buf_ram_read[] */
  ap_uint<8> stop_addr = trig.t_stop.range(10,3); /* ending position in buf_ram_read[] */
  ap_uint<8> addr = start_addr;
#ifdef DEBUG
  cout<<"-ecwriter-> start_addr="<<start_addr<<" stop_addr="<<stop_addr<<endl;
#endif

  while(addr != stop_addr)
  {
#ifdef DEBUG
    cout << "-ecwriter-> addr="<<addr<<endl;
#endif

    for(int i=0; i<NPEAK; i++)
    {
      peak_ram_t buf_ram_val = buf_ram_read[i][addr];

      if(buf_ram_val.energy)
	  {
        eventdata.end = 0;
        eventdata.data(31,31)  = 1;
        eventdata.data(30,27)  = ECPEAK_TAG;
        eventdata.data(26,26)  = inst;
        eventdata.data(25,24)  = buf_ram_val.view; /*2 bits*/
	    eventdata.data(23,16)  = addr - start_addr; /*8 bits*/
#ifdef __SYNTHESIS__
	    eventdata.data(15,0)   = 0;
#else
	    eventdata.data(15,14)  = 0;
        eventdata.data(13,7)   = buf_ram_val.strip1;
        eventdata.data(6,0)    = buf_ram_val.stripn;
#endif


        event_stream.write(eventdata);

        eventdata.end = 0;
	    eventdata.data(31,26)  = 0;
        eventdata.data(25,16)  = buf_ram_val.coord; /*10 bits*/;
  	    eventdata.data(15,0)   = buf_ram_val.energy; /*16 bits*/
        event_stream.write(eventdata);
	  }
	}

    addr ++;
  }

  eventdata.data = 0xFFFFFFFF;
  eventdata.end = 1;
  event_stream.write(eventdata);  
}
