
/* htcclib.h */

#ifndef _HTCCLIB_

#include <ap_int.h>
#include <hls_stream.h>

#include "hls_fadc_sum.h"


#ifdef	__cplusplus
extern "C" {
#endif

#define NSECTOR 1

#define NSLOT 3

#define NSTRIP 48

#define NCHAN 48
#define NCLSTR 36
#define NHIT NSTRIP

#define NBIT_OUT NCHAN /* the number of bits in output */

#define NH_READS  8   /* the number of reads-write for streams, AND the number of 4ns intervals inside 32ns interval */

#define TIMECORR 7 /*TEMPORARY !!!!!!!!!!!!!!!! */

/* strip persistency */
#define NPER 8
typedef ap_uint<6> nframe_t;



typedef struct htccstrip_s
{
  ap_uint<13> en[NSTRIP];
  ap_uint<3>  tm[NSTRIP];
} HTCCStrip_s;



typedef struct htcchit
{
  ap_uint<NSTRIP> output;
} HTCCHit;

typedef struct
{
  ap_uint<NSTRIP> output[NPER];
} HTCCHit_8slices;


typedef struct
{
  ap_uint<NBIT_OUT> out[NPER];
} HTCCOut_8slices;



#define NBIT_HIT_ENERGY 16
#define NBIT_COORD_EB 10 /*in event builder coordinates always 10 bits*/




/* will be asymetric ram in vhdl */
typedef struct
{
  ap_uint<NSTRIP> output[NPER];
} hit_ram_t;

typedef struct
{
  ap_uint<NSTRIP> output;
} event_ram_t;
/* will be asymetric ram in vhdl */




void htcchiteventreader(hls::stream<eventdata_t> &event_stream, HTCCHit_8slices &hit, uint32_t *bufout);

void htcc(ap_uint<16> threshold[3], nframe_t nframes, hls::stream<fadc_256ch_t> &s_fadcs, hls::stream<HTCCOut_8slices> &s_hits, volatile ap_uint<1> &hit_scaler_inc, hit_ram_t buf_ram[512]);

void htccstrips(ap_uint<16> strip_threshold, hls::stream<fadc_256ch_t> &s_fadcs, HTCCStrip_s &s_strip);
void htcchit(ap_uint<16> strip_threshold, ap_uint<16> mult_threshold, ap_uint<16> cluster_threshold, nframe_t nframes, HTCCStrip_s s_strip, HTCCHit s_hit[NH_READS]);
void htcchitfanout(HTCCHit s_hit[NH_READS], hls::stream<HTCCOut_8slices> &s_hits, HTCCHit s_hit2[NH_READS], volatile ap_uint<1> &hit_scaler_inc);
void htcchiteventfiller(HTCCHit s_hitin[NH_READS], hit_ram_t buf_ram[512]);
void htcchiteventwriter(hls::stream<trig_t> &trig_stream, hls::stream<eventdata3_t> &event_stream, event_ram_t buf_ram_read[2048]);

#ifdef	__cplusplus
}
#endif


#define _HTCCLIB_
#endif
