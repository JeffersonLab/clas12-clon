
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
ctof(ap_uint<16> threshold[3], nframe_t nframes, hls::stream<fadc_256ch_t> &s_fadcs, hls::stream<CTOFOut_8slices> &s_hits, volatile ap_uint<1> &hit_scaler_inc, hit_ram_t buf_ram[512])
{
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

  static int first = 0;

  CTOFStrip_s s_strip;
  CTOFHit hit1[NH_READS];
  CTOFHit hit2[NH_READS];
  ap_uint<32> thresh;

  if(nframes>NPER) nframes = NPER;
  thresh = threshold[1]*threshold[1];

  ctofstrips(threshold[0], s_fadcs, s_strip);
  ctofhit(thresh, nframes, s_strip, hit1);

  if(first==0)
  {
    first = 1;
    return;
  }

  ctofhitfanout(hit1, s_hits, hit2, hit_scaler_inc);
  ctofhiteventfiller(hit2, buf_ram);
}
