
/* dc2.cc - stage 2 - road finder, run in 6 crates */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include <iostream>
using namespace std;

//#define DEBUG

#include "dclib.h"
#include "dctrans.h"

#include "hls_fadc_sum.h"

void
dc2(ap_uint<16> threshold[3], nframe_t nframes, hls::stream<segment_word_t> s_segments[NSLS], hls::stream<road_word_t> &s_roads, volatile ap_uint<1> &hit_scaler_inc, hit_ram_t buf_ram[RAMSIZE])
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

  static int first = 0;

  hls::stream<DCSL_s> s_superlayer;
  segment_word_t segment_word[NSLS];
  road_word_t road, road2;

  if(nframes>NPER) nframes = NPER;

  for(int isl=0; isl<NSLS; isl++) segment_word[isl] = s_segments[isl].read();

  dcroads(threshold[2], segment_word, road);

  /*
  if(first==0)
  {
    first = 1;
    return;
  }
  */

  dcroadfanout(road, s_roads, road2, hit_scaler_inc);
  dcroadeventfiller(road2, buf_ram);
}
