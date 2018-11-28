
/* dc1.cc - signle crate stage 1 - segment finder for one region (two superlayers), run in all 18 crates */

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
dc1(ap_uint<16> threshold[3], nframe_t nframes, hls::stream<dcrb_96ch_t> s_dcrb_words[NSLOT], hls::stream<segment_word_t> &s_segments, volatile ap_uint<1> &hit_scaler_inc, hit_ram_t buf_ram[RAMSIZE])
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

  DCSL_s superlayer;
  segment_word_t segment, segment2;

  if(nframes>NPER) nframes = NPER;

  //printf("dc1 0\n"); fflush(stdout);

  dcwires(threshold[0], s_dcrb_words, superlayer);
  //printf("dc1 1\n"); fflush(stdout);

  dcsegments(threshold[1], superlayer, segment);
  //printf("dc1 2\n"); fflush(stdout);

  /*
  if(first==0)
  {
    first = 1;
    return;
  }
  */

  dcsegmentfanout(segment, s_segments, segment2, hit_scaler_inc);
  dcsegmenteventfiller(segment2, buf_ram);

}
