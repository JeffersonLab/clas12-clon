#ifndef _HTCCLIB_

#include <ap_int.h>
#include <hls_stream.h>

#include "hls_fadc_sum.h"

#ifdef	__cplusplus
extern "C" {
#endif

/* htcclib.h */

#define NSLOT 3
#define NH_READS  8   /* the number of reads-write for streams, AND the number of 4ns intervals inside 32ns interval */

/* event ram dimension in 4ns ticks: 11 bits, 2048 size */
#define NRAM 2048
#define RAM_BITS 11

#define NSTRIP 48
#define NCHAN 48
#define NCLSTR 36
#define NHIT NCLSTR


#define TIMECORR 7 /*TEMPORARY !!!!!!!!!!!!!!!! */

/* strip persistency */
#define NPER 8
typedef ap_uint<3> nframe_t;

typedef struct htccstrip
{
  ap_uint<13> energy;       /* strip energy */
} HTCCStrip;

#define N1 6 /* the number of elements in following struct */
#define NSTREAMS1 (NCHAN/N1)
typedef struct htccstrip_s
{
  ap_uint<13> energy00;
  ap_uint<13> energy01;
  ap_uint<13> energy02;
  ap_uint<13> energy03;
  ap_uint<13> energy04;
  ap_uint<13> energy05;
} HTCCStrip_s;



typedef struct htcchit
{
  ap_uint<NCHAN> output;
} HTCCHit;



#define NBIT_HIT_ENERGY 16
#define NBIT_COORD_EB 10 /*in event builder coordinates always 10 bits*/

typedef struct
{
  ap_uint<NCHAN> output;
} hit_ram_t;




void htcchiteventreader(hls::stream<trig_t> &trig_stream, hls::stream<eventdata_t> &event_stream, HTCCHit &hit, uint32_t *bufout);

void htcc(ap_uint<16> threshold[3], nframe_t nframes, hls::stream<fadc_2ch_t> s_fadc_words[NFADCS], hls::stream<HTCCHit> &s_hits1, hit_ram_t buf_ram[NRAM]);

void htccstrips(ap_uint<16> strip_threshold, hls::stream<fadc_2ch_t> s_fadc_words[/*NSLOT*/NFADCS], hls::stream<HTCCStrip_s> s_strip0[NSTREAMS1]);
void htccstripspersistence(nframe_t nframes, hls::stream<HTCCStrip_s> &s_stripin, hls::stream<HTCCStrip_s> &s_stripout, ap_uint<3> jj);
void htcchit(ap_uint<16> strip_threshold, ap_uint<16> mult_threshold, ap_uint<16> cluster_threshold, hls::stream<HTCCStrip_s> s_strip[NSTREAMS1], hls::stream<HTCCHit> &s_hit);
void htcchitfanout(hls::stream<HTCCHit> &s_hit, hls::stream<HTCCHit> &s_hit1, hls::stream<HTCCHit> &s_hit2, volatile ap_uint<1> &hit_scaler_inc);
void htcchiteventfiller(hls::stream<HTCCHit> &s_hitin, hit_ram_t buf_ram[NRAM]);
void htcchiteventwriter(hls::stream<trig_t> &trig_stream, hls::stream<eventdata_t> &event_stream, hit_ram_t buf_ram_read[NRAM]);

#ifdef	__cplusplus
}
#endif


#define _HTCCLIB_
#endif
