
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
ftof(ap_uint<16> threshold[3], nframe_t nframes, hls::stream<fadc_16ch_t> s_fadc_words[NFADCS], hls::stream<FTOFHit> &s_hits1, hit_ram_t buf_ram[512])
{
  ap_uint<16> strip_threshold = threshold[0];

  hls::stream<FTOFStrip_s> s_strip[NH_READS];
  hls::stream<FTOFHit> s_hits;
  hls::stream<FTOFHit> s_hits2;
  volatile ap_uint<1> hit_scaler_inc;

  ftofstrips(strip_threshold, s_fadc_words, s_strip);
  ftofhit(nframes, s_strip, s_hits);
  ftofhitfanout(s_hits, s_hits1, s_hits2, hit_scaler_inc);
  ftofhiteventfiller(s_hits2, buf_ram);
}
