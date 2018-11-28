
/* pcu.cc */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include <iostream>
using namespace std;

#include "pculib.h"
#include "pcutrans.h"

#include "hls_fadc_sum.h"

void
pcu(ap_uint<16> threshold[3], nframe_t nframes, hls::stream<fadc_256ch_t> &s_fadcs, hls::stream<PCUHit_8slices> &s_hits, volatile ap_uint<1> &hit_scaler_inc, hit_ram_t buf_ram[512])
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

  PCUStrip_s s_strip;
#pragma HLS DATA_PACK variable=s_strip


  PCUHit hit1[NH_READS];
#pragma HLS ARRAY_PARTITION variable=hit1 complete dim=1
  PCUHit hit2[NH_READS];
#pragma HLS ARRAY_PARTITION variable=hit2 complete dim=1


  if(nframes>NPER) nframes = NPER;

  pcustrips(threshold[0], s_fadcs, s_strip);
  pcuhit(threshold[1], threshold[2], nframes, s_strip, hit1);
  pcuhitfanout(hit1, s_hits, hit2, hit_scaler_inc);
  pcuhiteventfiller(hit2, buf_ram);
}
