
/* dcwires.cc */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

//#define DEBUG

#ifdef DEBUG
#include <iostream>
using namespace std;
#endif


#include "dctrans.h"
#include "dclib.h"


/* reads one timing slice */
void
dcwires(ap_uint<16> strip_threshold, hls::stream<dcrb_96ch_t> s_dcrb_words[NSLOT], DCSL_s &superlayer)
{
#pragma HLS DATA_PACK variable=s_dcrb_words
#pragma HLS INTERFACE axis register both port=s_dcrb_words
#pragma HLS INTERFACE ap_stable port=strip_threshold
#pragma HLS ARRAY_PARTITION variable=s_dcrb_words complete dim=1
#pragma HLS ARRAY_PARTITION variable=s_strip_u complete dim=1
#pragma HLS ARRAY_PARTITION variable=s_strip_v complete dim=1
#pragma HLS ARRAY_PARTITION variable=s_strip_w complete dim=1
#pragma HLS PIPELINE II=1

  ap_uint<7> chan; /*0-95*/
  ap_uint<3> layer;/*0-6*/
  ap_uint<7> wire; /*0-112*/

  dcrb_96ch_t dcrbs;

#ifdef DEBUG
  printf("== dcwires starts ==\n");fflush(stdout);
#endif

  for(int i=0; i<NLAYERS; i++) superlayer.layer[i] = 0;

  for(int slot=0; slot<NSLOT; slot++)
  {
    /* one read - we expect only one timing slice per slot */
	dcrbs = s_dcrb_words[slot].read();
#ifdef DEBUG
    cout<<"dcrbs[slot="<<slot<<"]="<<hex<<dcrbs.chan<<" "<<dec<<endl;
#endif
    for(chan=0; chan<NCHAN; chan++)
	{
      layer = board_layer[chan]-1;
      if(slot<7) wire = (board_wire[chan]-1)+slot*16;
      else       wire = (board_wire[chan]-1)+(slot-7)*16;
#ifdef DEBUG_
      cout<<"slot="<<slot<<" chan="<<chan<<" -> layer="<<layer<<" wire="<<wire<<endl;
#endif
	  if(dcrbs.chan(chan,chan)) superlayer.layer[layer](wire,wire) = 1;
	}
  }

#ifdef DEBUG
  cout<<"superlayer="<<hex<<superlayer.layer[5]<<" "<<superlayer.layer[4]<<" "<<superlayer.layer[3]<<" "<<superlayer.layer[2]<<" "<<superlayer.layer[1]<<" "<<superlayer.layer[0]<<dec<<endl;
#endif


#ifdef DEBUG
  printf("== dcwires ends ==\n");fflush(stdout);
#endif

}
