
/* ctof.cc */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include <iostream>
using namespace std;

#include "ctoflib.h"
#include "ctoftrans.h"

#include "hls_fadc_sum.h"

void
ctof(ap_uint<16> threshold[3], nframe_t nframes, hls::stream<fadc_256ch_t> &s_fadcs, hls::stream<CTOFOut_8slices> &s_hits, volatile ap_uint<1> &hit_scaler_inc, hit_ram_t buf_ram[(NRAM/8)])
{
//#pragma HLS DATAFLOW

#pragma HLS INTERFACE ap_stable port=threshold
#pragma HLS ARRAY_PARTITION variable=threshold dim=1
#pragma HLS INTERFACE ap_stable port=nframes

#pragma HLS DATA_PACK variable=s_fadcs
#pragma HLS INTERFACE axis register both port=s_fadcs

#pragma HLS DATA_PACK variable=s_hits
#pragma HLS INTERFACE axis register both port=s_hits

#pragma HLS INTERFACE ap_none port=hit_scaler_inc

#pragma HLS DATA_PACK variable=buf_ram
//#pragma HLS ARRAY_PARTITION variable=buf_ram block factor=8 dim=1

#pragma HLS PIPELINE II=1

  CTOFStrip_s s_strip;
  CTOFStrip_s s_clust;
  CTOFHit hit1[NH_READS];
  CTOFHit hit2[NH_READS];
  static int skip = 0;

  ctofstrips(threshold[0], nframes, s_fadcs, s_strip, s_clust);
  ctofhit(threshold[1], threshold[2], nframes, s_strip, s_clust, hit1);

#ifdef __SYNTHESIS__
  if(skip<2)
  {
    skip ++;
    return;
  }
#endif

  ctofhitfanout(hit1, s_hits, hit2, hit_scaler_inc);
  ctofhiteventfiller(hit2, buf_ram);
}
