
/* htcchitfanout.cc */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

using namespace std;
#include <strstream>



#include "htcclib.h"


//#define DEBUG


/* 2.15/2/1/0%/0%/(256)~0%/(157)~0% II=1 */

void
htcchitfanout(hls::stream<HTCCHit> &s_hit, hls::stream<HTCCHit> &s_hit1, hls::stream<HTCCHit> &s_hit2, volatile ap_uint<1> &hit_scaler_inc)
{
#pragma HLS INTERFACE ap_none port=hit_scaler_inc
#pragma HLS DATA_PACK variable=s_hit2
#pragma HLS INTERFACE axis register both port=s_hit2
#pragma HLS DATA_PACK variable=s_hit1
#pragma HLS INTERFACE axis register both port=s_hit1
#pragma HLS DATA_PACK variable=s_hit
#pragma HLS INTERFACE axis register both port=s_hit
#pragma HLS PIPELINE II=1

  HTCCHit fifo;

  fifo = s_hit.read();

  if(fifo.output) hit_scaler_inc = 1;
  else            hit_scaler_inc = 0;

  s_hit1.write(fifo);
  s_hit2.write(fifo);

}


