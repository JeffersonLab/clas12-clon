
/* ftof.cc */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include <iostream>
using namespace std;

#include "ftoflib.h"
#include "ftoftrans.h"

#include "hls_fadc_sum.h"




void
ftof(ap_uint<16> threshold[3], nframe_t nframes, hls::stream<fadc_16ch_t> s_fadc_words[NSLOT], hls::stream<FTOFHit> s_hits1[NH_READS], hit_ram_t buf_ram[512])
{
#pragma HLS INTERFACE ap_stable port=threshold
#pragma HLS DATA_PACK variable=s_fadc_words
#pragma HLS INTERFACE axis register both port=s_fadc_words
#pragma HLS ARRAY_PARTITION variable=s_fadc_words complete dim=1
#pragma HLS DATA_PACK variable=s_hits1
#pragma HLS INTERFACE axis register both port=s_hits1
#pragma HLS ARRAY_PARTITION variable=s_hits1 complete dim=1
#pragma HLS DATA_PACK variable=buf_ram
#pragma HLS PIPELINE II=1

  ap_uint<16> strip_threshold = threshold[0];

  hls::stream<FTOFStrip_s> s_strip[NH_READS];
  hls::stream<FTOFHit> s_hits[NH_READS];
  hls::stream<FTOFHit> s_hits2[NH_READS];
  volatile ap_uint<1> hit_scaler_inc;

  ftofstrips(strip_threshold, s_fadc_words, s_strip);
  ftofhit(nframes, s_strip, s_hits);
  ftofhitfanout(s_hits, s_hits1, s_hits2, hit_scaler_inc);
  ftofhiteventfiller(s_hits2, buf_ram);
}
