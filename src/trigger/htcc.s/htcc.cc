
/* htcc.cc */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include <iostream>
using namespace std;

#include "htcclib.h"
#include "htcctrans.h"

#include "hls_fadc_sum.h"




void
htcc(ap_uint<16> threshold[3], nframe_t nframes, hls::stream<fadc_2ch_t> s_fadc_words[NFADCS], hls::stream<HTCCHit> &s_hits1, hit_ram_t buf_ram[512])
{
  ap_uint<16> strip_threshold = threshold[0];
  ap_uint<16> mult_threshold = threshold[1];
  ap_uint<16> cluster_threshold = threshold[3];

  hls::stream<HTCCStrip_s> s_strip0[NSTREAMS1];
  hls::stream<HTCCStrip_s> s_strip[NSTREAMS1];
  hls::stream<HTCCHit> s_hits;
  hls::stream<HTCCHit> s_hits2;
  volatile ap_uint<1> hit_scaler_inc;

  htccstrips(strip_threshold, s_fadc_words, s_strip0);
  for(int ii=0; ii<NH_READS; ii++)
  {
    for(int jj=0; jj<NSTREAMS1; jj++)
	{
      htccstripspersistence(nframes, s_strip0[jj], s_strip[jj], jj);
	}
    htcchit(strip_threshold, mult_threshold, cluster_threshold, s_strip, s_hits);
    htcchitfanout(s_hits, s_hits1, s_hits2, hit_scaler_inc);

    htcchiteventfiller(s_hits2, buf_ram);
  }

}
