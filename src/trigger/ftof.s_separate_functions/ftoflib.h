#ifndef _FTOFLIB_

#include <ap_int.h>
#include <hls_stream.h>

#include "hls_fadc_sum.h"

#ifdef	__cplusplus
extern "C" {
#endif

/* ftoflib.h */

#define NSECTOR 6

#define NSLOT 12

#define NSTRIP 62

#define NCHAN 48
#define NHIT NSTRIP


#define NH_READS  8   /* the number of reads-write for streams, AND the number of 4ns intervals inside 32ns interval */

#define TIMECORR 7 /*TEMPORARY !!!!!!!!!!!!!!!! */

/* strip persistency */
#define NPER 8
typedef ap_uint<3> nframe_t;




#define NLR 2 /* the number of 'left-rights' */
typedef struct ftofstrip_s
{
  ap_uint<NSTRIP> outL;
  ap_uint<NSTRIP> outR;
} FTOFStrip_s;



typedef struct ftofhit
{
  ap_uint<NSTRIP> output;
} FTOFHit;

typedef struct
{
  ap_uint<NSTRIP> output[NPER];
} FTOFHit_8slices;



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




void ftofhiteventreader(hls::stream<eventdata_t> &event_stream, FTOFHit_8slices &hit, uint32_t *bufout);

void ftof(ap_uint<16> threshold[3], nframe_t nframes, hls::stream<fadc_16ch_t> s_fadc_words[NSLOT], hls::stream<FTOFHit> s_hits[NH_READS], hit_ram_t buf_ram[512]);

void ftofstrips(ap_uint<16> strip_threshold, hls::stream<fadc_16ch_t> s_fadc_words[NSLOT], hls::stream<FTOFStrip_s> s_strip[NH_READS]);
void ftofhit(nframe_t nframes, hls::stream<FTOFStrip_s> s_strip[NH_READS], hls::stream<FTOFHit> s_hit[NH_READS]);
void ftofhitfanout(hls::stream<FTOFHit> s_hit[NH_READS], hls::stream<FTOFHit> s_hit1[NH_READS], hls::stream<FTOFHit> s_hit2[NH_READS], volatile ap_uint<1> &hit_scaler_inc);
void ftofhiteventfiller(hls::stream<FTOFHit> s_hitin[NH_READS], hit_ram_t buf_ram[256]);
void ftofhiteventwriter(hls::stream<trig_t> &trig_stream, hls::stream<eventdata_t> &event_stream, event_ram_t buf_ram_read[2048]);

#ifdef	__cplusplus
}
#endif


#define _FTOFLIB_
#endif
