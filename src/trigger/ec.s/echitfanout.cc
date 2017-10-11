
/* echitfanout.cc */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

using namespace std;
#include <strstream>


#ifdef USE_PCAL

#include "../pc.s/pclib.h"
#define echitfanout pchitfanout

#else

#include "eclib.h"

#endif

//#define DEBUG


/* 0.0/5/4/0%/0%/(306)~0%/(231)~0% II=4 */

void
echitfanout(hls::stream<ECHit> &s_hit, hls::stream<ECHit> &s_hit1, hls::stream<ECHit> &s_hit2, volatile ap_uint<1> &hit_scaler_inc)
{
#pragma HLS INTERFACE ap_none port=hit_scaler_inc
#pragma HLS DATA_PACK variable=s_hit2
#pragma HLS INTERFACE axis register both port=s_hit2
#pragma HLS DATA_PACK variable=s_hit1
#pragma HLS INTERFACE axis register both port=s_hit1
#pragma HLS DATA_PACK variable=s_hit
#pragma HLS INTERFACE axis register both port=s_hit
#pragma HLS PIPELINE II=4

  ECHit fifo;

  for(int i=0; i<NHIT; i++)
  {
	fifo = s_hit.read();

	if(fifo.energy) hit_scaler_inc = 1;
	else            hit_scaler_inc = 0;

    s_hit1.write(fifo);
    s_hit2.write(fifo);
  }

}


