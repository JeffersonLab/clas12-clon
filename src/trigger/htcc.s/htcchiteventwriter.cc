
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
  hit_ram_t buf_ram_val;

  trig_t trig = trig_stream.read();
  ap_uint<RAM_BITS> w = (trig.t_stop - trig.t_start); /* readout window width */
  ap_uint<RAM_BITS> start_addr = trig.t_start.range(10,0); /* starting position in buf_ram_read[] */
  ap_uint<RAM_BITS> stop_addr = trig.t_stop.range(10,0); /* ending position in buf_ram_read[] */
  ap_uint<RAM_BITS> addr = start_addr;

#ifdef DEBUG
  cout<<"htcchiteventwriter: start_addr="<<start_addr<<endl;
  cout<<"htcchiteventwriter: stop_addr="<<stop_addr<<endl;
  cout<<"htcchiteventwriter: addr="<<addr<<endl<<endl;
#endif

  while(addr != stop_addr)
  {
#ifdef DEBUG
    cout<<"htcchiteventwriter: addr0="<<addr<<endl;
#endif
    buf_ram_val = buf_ram_read[addr];
    //buf_ram_val.output = buf_ram_read[addr].output;
#ifdef DEBUG
    cout<<"htcchiteventwriter: addr00="<<addr<<endl;
#endif

    if(buf_ram_val.output)
	{
      eventdata.end = 0;
      eventdata.data(31,31)  = 1;
      eventdata.data(30,27)  = HTCCHIT_TAG;
      eventdata.data(26,16)  = addr - start_addr;  /*11 bits*/
      eventdata.data(15, 0)  = 0;
      event_stream.write(eventdata);
        
      eventdata.end = 0;
      eventdata.data(31,17)   = 0;
      eventdata.data(16,0)    = buf_ram_val.output(47,31); /*17 bits*/
      event_stream.write(eventdata);

      eventdata.end = 0;
      eventdata.data(31,31)  = 0;
      eventdata.data(30,0)   = buf_ram_val.output(30,0); /*31 bit*/
      event_stream.write(eventdata);
	}

    addr ++;
  }

  eventdata.data = 0xFFFFFFFF;
  eventdata.end = 1;
  event_stream.write(eventdata);
  
}



