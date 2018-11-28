
/* ftofhitfanout.cc */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

using namespace std;
#include <strstream>



#include "ftoflib.h"


//#define DEBUG


/* 0.0/5/4/0%/0%/(306)~0%/(231)~0% II=4 */

void
ftofhitfanout(hls::stream<FTOFHit> s_hit[NH_READS], hls::stream<FTOFHit> s_hit1[NH_READS], hls::stream<FTOFHit> s_hit2[NH_READS], volatile ap_uint<1> &hit_scaler_inc)
{
#pragma HLS INTERFACE ap_none port=hit_scaler_inc
//#pragma HLS DATA_PACK variable=s_hit2
#pragma HLS INTERFACE axis register both port=s_hit2
#pragma HLS ARRAY_PARTITION variable=s_hit2 complete dim=1
//#pragma HLS DATA_PACK variable=s_hit1
#pragma HLS INTERFACE axis register both port=s_hit1
#pragma HLS ARRAY_PARTITION variable=s_hit1 complete dim=1
//#pragma HLS DATA_PACK variable=s_hit
#pragma HLS INTERFACE axis register both port=s_hit
#pragma HLS ARRAY_PARTITION variable=s_hit complete dim=1
#pragma HLS PIPELINE II=1

  FTOFHit fifo;
  ap_uint<1> scaler[NH_READS];
  ap_uint<1> scaler_tmp = 0;

  for(int i=0; i<NH_READS; i++)
  {
    fifo = s_hit[i].read();

    scaler[i] = 0;
    if(fifo.output != 0) scaler[i] = 1;
    //else                 scaler[i] = 0;

    s_hit1[i].write(fifo);
    s_hit2[i].write(fifo);
  }

  for(int i=0; i<NH_READS; i++)
  {
    scaler_tmp |= scaler[i];
  }

  if(scaler_tmp) hit_scaler_inc = 1;
  else           hit_scaler_inc = 0;
}


