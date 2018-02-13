#ifndef _PCULIB_

#include <ap_int.h>
#include <hls_stream.h>

#include "hls_fadc_sum.h"

#ifdef	__cplusplus
extern "C" {
#endif

/* pculib.h */

#define NSECTOR 6

#define NSLOT 5

#define NSTRIP 68
#define NHIT NSTRIP


#define NH_READS  8   /* the number of reads-write for streams, AND the number of 4ns intervals inside 32ns interval */

#define TIMECORR 7 /*TEMPORARY !!!!!!!!!!!!!!!! */

/* strip persistency */
#define NPER 8
typedef ap_uint<6> nframe_t;




/* array PCUStrip[NSTRIP] passed from pcustrips() to pcuhit() every 32ns; we assumes that only one pulse can exist in every 32ns interval */
typedef struct pcustrip
{
  ap_uint<13> en;
  ap_uint<3>  tm;
} PCUStrip;








typedef struct pcuhit
{
  ap_uint<NSTRIP> output;
} PCUHit;



typedef struct
{
  ap_uint<4> output[NSTRIP];
} PCUHit_out;






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




void pcuhiteventreader(hls::stream<eventdata_t> &event_stream, PCUHit_out &hit, uint32_t *bufout);

void pcu(ap_uint<16> threshold[3], nframe_t nframes, hls::stream<fadc_256ch_t> &s_fadcs, hls::stream<PCUHit_out> &s_hits, volatile ap_uint<1> &hit_scaler_inc, hit_ram_t buf_ram[512]);

void pcustrips(ap_uint<16> strip_threshold, hls::stream<fadc_256ch_t> &s_fadcs, PCUStrip strip[NSTRIP]);
void pcuhit(ap_uint<16> thresholdmin, ap_uint<16> thresholdmax, nframe_t nframes, PCUStrip strip[NSTRIP], ap_uint<4> outtime[NSTRIP]);
void pcuhitfanout(ap_uint<4> outtime[NSTRIP], hls::stream<PCUHit_out> &s_hits, PCUHit s_hit2[NH_READS], volatile ap_uint<1> &hit_scaler_inc);
void pcuhiteventfiller(PCUHit s_hitin[NH_READS], hit_ram_t buf_ram[512]);
void pcuhiteventwriter(hls::stream<trig_t> &trig_stream, hls::stream<eventdata3_t> &event_stream, event_ram_t buf_ram_read[2048]);

#ifdef	__cplusplus
}
#endif


#define _PCULIB_
#endif
