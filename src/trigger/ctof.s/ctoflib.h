
/* ctoflib.h */

#ifndef _CTOFLIB_

#include <ap_int.h>
#include <hls_stream.h>

#include "hls_fadc_sum.h"


#ifdef	__cplusplus
extern "C" {
#endif

#define NSECTOR 1

#define NSLOT 6

#define NSTRIP 48

#define NCHAN 48
#define NHIT NSTRIP

#define NBIT_OUT NCHAN /* the number of bits in output */

#define NH_READS  8   /* the number of reads-write for streams, AND the number of 4ns intervals inside 32ns interval */

#define TIMECORR 7 /*TEMPORARY !!!!!!!!!!!!!!!! */

/* strip persistency */
#define NPER 8
typedef ap_uint<6> nframe_t;

  /*
#define NSTANDALONEBITS 16
  */

#define NLR 2 /* the number of 'left-rights' */
typedef struct ctofstrip_s
{
  ap_uint<13> enL[NSTRIP];
  ap_uint<3>  tmL[NSTRIP];
  ap_uint<13> enR[NSTRIP];
  ap_uint<3>  tmR[NSTRIP];
} CTOFStrip_s;



typedef struct ctofhit
{
  ap_uint<NSTRIP> output;
  /*ap_uint<NSTANDALONEBITS> standalone;*/
} CTOFHit;

typedef struct
{
  ap_uint<NSTRIP> output[NPER];
  /*ap_uint<NSTANDALONEBITS> standalone[NPER];*/
} CTOFHit_8slices;


typedef struct
{
  ap_uint<NBIT_OUT> out[NPER];
} CTOFOut_8slices;



#define NBIT_HIT_ENERGY 16
#define NBIT_COORD_EB 10 /*in event builder coordinates always 10 bits*/




/* will be asymetric ram in vhdl */
typedef struct
{
  ap_uint<NSTRIP> output[NPER];
  /*ap_uint<NSTANDALONEBITS> standalone[NPER];*/
} hit_ram_t;

typedef struct
{
  ap_uint<NSTRIP> output;
  /*ap_uint<NSTANDALONEBITS> standalone;*/
} event_ram_t;
/* will be asymetric ram in vhdl */


#define NRAM 2048

void ctofhiteventreader(hls::stream<eventdata_t> &event_stream, CTOFHit_8slices &hit, uint32_t *bufout);

void ctof(ap_uint<16> threshold[3], nframe_t nframes, hls::stream<fadc_256ch_t> &s_fadcs, hls::stream<CTOFOut_8slices> &s_hits, volatile ap_uint<1> &hit_scaler_inc, hit_ram_t buf_ram[(NRAM/8)]);

void ctofstrips(ap_uint<16> strip_threshold, nframe_t nframes, hls::stream<fadc_256ch_t> &s_fadcs, CTOFStrip_s &s_strip, CTOFStrip_s &s_clust);
void ctofhit(ap_uint<16> threshold1, ap_uint<16> threshold2, nframe_t nframes, CTOFStrip_s s_strip, CTOFStrip_s s_clust, CTOFHit s_hit[NH_READS]);
void ctofhitfanout(CTOFHit s_hit[NH_READS], hls::stream<CTOFOut_8slices> &s_hits, CTOFHit s_hit2[NH_READS], volatile ap_uint<1> &hit_scaler_inc);
void ctofhiteventfiller(CTOFHit s_hitin[NH_READS], hit_ram_t buf_ram[(NRAM/8)]);
void ctofhiteventwriter(hls::stream<trig_t> &trig_stream, hls::stream<eventdata3_t> &event_stream, event_ram_t buf_ram_read[NRAM]);

#ifdef	__cplusplus
}
#endif


#define _CTOFLIB_
#endif
