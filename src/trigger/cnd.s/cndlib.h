
/* cndlib.h */

#ifndef _CNDLIB_

#include <ap_int.h>
#include <hls_stream.h>

#include "hls_fadc_sum.h"

#ifdef	__cplusplus
extern "C" {
#endif

#define NSECTOR 1

#define NSLOT 9

#define NSTRIP 72

#define NBIT_OUT 64 /* the number of bits in output */

#define NH_READS  8   /* the number of reads-write for streams, AND the number of 4ns intervals inside 32ns interval */

#define TIMECORR 7 /*TEMPORARY !!!!!!!!!!!!!!!! */

/* strip persistency */
#define NPER 8
typedef ap_uint<6> nframe_t;



#define NLR 2 /* the number of 'left-rights' */
typedef struct cndstrip_s
{
  ap_uint<NSTRIP> outL;
  ap_uint<NSTRIP> outR;
} CNDStrip_s;



typedef struct cndhit
{
  ap_uint<NSTRIP> output;
} CNDHit;

typedef struct
{
  ap_uint<NSTRIP> output[NPER];
} CNDHit_8slices;

typedef struct
{
  ap_uint<NBIT_OUT> out[NPER];
} CNDOut_8slices;


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




void cndhiteventreader(hls::stream<eventdata_t> &event_stream, CNDHit_8slices &hit, uint32_t *bufout);

void cnd(ap_uint<16> threshold[3], nframe_t nframes, hls::stream<fadc_256ch_t> &s_fadcs, hls::stream<CNDOut_8slices> &s_hits, volatile ap_uint<1> &hit_scaler_inc, hit_ram_t buf_ram[512]);

void cndstrips(ap_uint<16> strip_threshold, hls::stream<fadc_256ch_t> &s_fadcs, CNDStrip_s s_strip[NH_READS]);
void cndhit(nframe_t nframes, CNDStrip_s s_strip[NH_READS], CNDHit s_hit[NH_READS]);
void cndhitfanout(CNDHit s_hit[NH_READS], hls::stream<CNDOut_8slices> &s_hits, CNDHit s_hit2[NH_READS], volatile ap_uint<1> &hit_scaler_inc);
void cndhiteventfiller(CNDHit s_hitin[NH_READS], hit_ram_t buf_ram[512]);
void cndhiteventwriter(hls::stream<trig_t> &trig_stream, hls::stream<eventdata3_t> &event_stream, event_ram_t buf_ram_read[2048]);

#ifdef	__cplusplus
}
#endif


#define _CNDLIB_
#endif
