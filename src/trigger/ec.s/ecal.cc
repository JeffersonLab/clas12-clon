
/* ecal.c */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>


#ifdef USE_PCAL

#include "../pc.s/pclib.h"
#define ecstrip pcstrip
#define ecpeak1 pcpeak1
#define ecpeak2 pcpeak2
#define ecpeaksort pcpeaksort
#define ecpeakcoord pcpeakcoord
#define ecpeakfanout pcpeakfanout
#define echit pchit
#define ecfrac pcfrac
#define eccorr pccorr
#define echitsort pchitsort
#define echitfanout pchitfanout

#else

#include "eclib.h"

#endif



/*3.48/286/8/14%/6%/20%/40% II=8*/

/*3.48/132/8/0%/4%/10%/24% II=8 - bypass hit finder*/

/*3.48/126/8/0%/1%/3%/8% II=8 - one view and bypass hit finder*/



/*3.76/293/16/14%/6%/23%/47% II=8 */

void
ecal(ap_uint<16> threshold[3], nframe_t nframes, ap_uint<4> dipfactor, ap_uint<12> dalitzmin, ap_uint<12> dalitzmax, ap_uint<4> nstripmax,
     hls::stream<fadc_word_t> s_fadc_words[NFADCS], hls::stream<ECHit> &s_hits1,
	 peak_ram_t buf_ram_u[NPEAK][256],
	 peak_ram_t buf_ram_v[NPEAK][256],
	 peak_ram_t buf_ram_w[NPEAK][256],
	 hit_ram_t buf_ram[NHIT][256])
{
#pragma HLS ARRAY_PARTITION variable=threshold complete dim=1
#pragma HLS INTERFACE ap_stable port=threshold
#pragma HLS INTERFACE ap_stable port=nframes
#pragma HLS INTERFACE ap_stable port=dipfactor
#pragma HLS INTERFACE ap_stable port=dalitzmin
#pragma HLS INTERFACE ap_stable port=dalitzmax
#pragma HLS INTERFACE ap_stable port=stripmax
#pragma HLS INTERFACE axis register port=s_hits
#pragma HLS INTERFACE axis register port=s_fadc_words
#pragma HLS ARRAY_PARTITION variable=s_fadc_words complete dim=1
#pragma HLS DATA_PACK variable=s_fadc_words
#pragma HLS DATA_PACK variable=s_hits
#pragma HLS PIPELINE II=8

  hls::stream<ECStrip_s> s_strip0_u[NF1], s_strip0_v[NF1], s_strip0_w[NF1];
  hls::stream<ECStrip_s> s_strip_u[NF1], s_strip_v[NF1], s_strip_w[NF1];
  hls::stream<ECStrip_s> s_strip1_u[NF1], s_strip1_v[NF1], s_strip1_w[NF1];
  hls::stream<ECStrip_s> s_strip2_u[NF1], s_strip2_v[NF1], s_strip2_w[NF1];
  hls::stream<ECPeak_s> s_peak_u, s_peak_v, s_peak_w;
  hls::stream<ECPeak_s> s_peak1_u, s_peak1_v, s_peak1_w;
  hls::stream<ECPeak_s> s_peak2_u, s_peak2_v, s_peak2_w;
  hls::stream<ECStream6_s> s_pcount[NH_FIFOS];
  hls::stream<ECStream16_s> s_energy[NH_FIFOS];
  hls::stream<ECStream16_s> s_energy1[NH_FIFOS];
  hls::stream<ECStream16_s> s_energy2[NH_FIFOS];
  hls::stream<ECStream10_s> s_coord[NH_FIFOS];
  hls::stream<ECStream10_s> s_coord1[NH_FIFOS];
  hls::stream<ECStream10_s> s_coord2[NH_FIFOS];
  hls::stream<ECStream16_s> s_frac[NH_FIFOS];
  hls::stream<ECStream16_s> s_enpeak[NH_FIFOS];
  hls::stream<hitsume_t> s_hitout1[NH_FIFOS];
  hls::stream<hitsume_t> s_hitout2[NH_FIFOS];
  hls::stream<ECPeak0_s> s_peak0strip_u[NF2], s_peak0strip_v[NF2], s_peak0strip_w[NF2];
  hls::stream<ECPeak0_s> s_peak0max_u[NF3], s_peak0max_v[NF3], s_peak0max_w[NF3];
  hls::stream<sortz_t> z1[NF4];
  hls::stream<sortz_t> z2[NF4];
  hls::stream<ECPeak0_s> s_peak0_u, s_peak0_v, s_peak0_w;
  hls::stream<ECfml_t> s_first_u, s_first_v, s_first_w;
  hls::stream<ECfml_t> s_middle_u, s_middle_v, s_middle_w;
  hls::stream<ECfml_t> s_last_u, s_last_v, s_last_w;
  volatile ap_uint<1> peak_scaler_inc_u, peak_scaler_inc_v, peak_scaler_inc_w, hit_scaler_inc;
  hls::stream<ECHit> s_hits;
  hls::stream<ECHit> s_hits2;

#pragma HLS ARRAY_PARTITION variable=s_strip_u complete dim=1
#pragma HLS ARRAY_PARTITION variable=s_strip_v complete dim=1
#pragma HLS ARRAY_PARTITION variable=s_strip_w complete dim=1
#pragma HLS ARRAY_PARTITION variable=s_energy1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=s_energy2 complete dim=1
#pragma HLS ARRAY_PARTITION variable=s_coord1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=s_coord2 complete dim=1
#pragma HLS ARRAY_PARTITION variable=s_frac complete dim=1
#pragma HLS ARRAY_PARTITION variable=s_enpeak complete dim=1
#pragma HLS ARRAY_PARTITION variable=s_hitout1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=s_hitout2 complete dim=1
#pragma HLS ARRAY_PARTITION variable=s_peak0strip_u complete dim=1
#pragma HLS ARRAY_PARTITION variable=s_peak0strip_v complete dim=1
#pragma HLS ARRAY_PARTITION variable=s_peak0strip_w complete dim=1
#pragma HLS ARRAY_PARTITION variable=s_peak0max_u complete dim=1
#pragma HLS ARRAY_PARTITION variable=s_peak0max_v complete dim=1
#pragma HLS ARRAY_PARTITION variable=s_peak0max_w complete dim=1


  ecstrips(threshold[0], s_fadc_words, s_strip0_u, s_strip0_v, s_strip0_w);

  ecstripspersistence0(nframes, s_strip0_u, s_strip_u); /* 'static' inside ! */
  ecstripspersistence1(nframes, s_strip0_v, s_strip_v); /* 'static' inside ! */
  ecstripspersistence2(nframes, s_strip0_w, s_strip_w); /* 'static' inside ! */
  ecstripsfanout(s_strip_u, s_strip1_u, s_strip2_u);
  ecstripsfanout(s_strip_v, s_strip1_v, s_strip2_v);
  ecstripsfanout(s_strip_w, s_strip1_w, s_strip2_w);

  ecpeak1(threshold[0], dipfactor, 0, s_strip1_u, s_first_u, s_middle_u, s_last_u);
  ecpeak2(threshold[1], s_strip2_u, s_first_u, s_middle_u, s_last_u, s_peak0strip_u);

  ecpeakzerosuppress(s_peak0strip_u, s_peak0max_u);
  ecpeaksort(s_peak0max_u, s_peak0_u);
  ecpeakcoord(0, s_peak0_u, s_peak_u);
  ecpeakfanout(s_peak_u, s_peak1_u, s_peak2_u, peak_scaler_inc_u);

  ecpeak1(threshold[0], dipfactor, nstripmax, s_strip1_v, s_first_v, s_middle_v, s_last_v);
  ecpeak2(threshold[1], s_strip2_v, s_first_v, s_middle_v, s_last_v, s_peak0strip_v);

  ecpeakzerosuppress(s_peak0strip_v, s_peak0max_v);
  ecpeaksort(s_peak0max_v, s_peak0_v);
  ecpeakcoord(0, s_peak0_v, s_peak_v);
  ecpeakfanout(s_peak_v, s_peak1_v, s_peak2_v, peak_scaler_inc_v);

  ecpeak1(threshold[0], dipfactor, 0, s_strip1_w, s_first_w, s_middle_w, s_last_w);
  ecpeak2(threshold[1], s_strip2_w, s_first_w, s_middle_w, s_last_w, s_peak0strip_w);

  ecpeakzerosuppress(s_peak0strip_w, s_peak0max_w);
  ecpeaksort(s_peak0max_w, s_peak0_w);
  ecpeakcoord(0, s_peak0_w, s_peak_w);
  ecpeakfanout(s_peak_w, s_peak1_w, s_peak2_w, peak_scaler_inc_w);

  ecpeakeventfiller0(s_peak2_u, buf_ram_u); /* 'static' inside ! */
  ecpeakeventfiller1(s_peak2_v, buf_ram_v); /* 'static' inside ! */
  ecpeakeventfiller2(s_peak2_w, buf_ram_w); /* 'static' inside ! */

  echit(dalitzmin, dalitzmax, s_peak1_u, s_peak1_v, s_peak1_w, s_pcount, s_energy, s_coord);

  ecenergyfanout(s_energy, s_energy1, s_energy2);
  eccoordfanout(s_coord, s_coord1, s_coord2);

  ecfrac1(s_pcount, s_energy1, s_hitout1);
  ecfrac2(s_hitout1, s_hitout2);
  ecfrac3(s_hitout2, s_frac);
  eccorr(threshold[2], s_energy2, s_coord1, s_frac, s_enpeak);
  echitsortin(s_coord2, s_enpeak, z1);
  echitsort1(z1, z2);
  echitsort2(z2, z1);
  echitsort3(z1, z2);
  echitsort4(z2, z1);
  echitsort5(z1, z2);
  echitsortout(z2, s_hits);
  echitfanout(s_hits, s_hits1, s_hits2, hit_scaler_inc);

  echiteventfiller(s_hits2, buf_ram);
}
