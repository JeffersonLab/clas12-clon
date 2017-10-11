#ifndef _FTOFLIB_

#include <ap_int.h>
#include <hls_stream.h>

#ifdef	__cplusplus
extern "C" {
#endif

/* ftoflib.h */

#define NSLOT 12

#define NSTRIP1 62
#define NSTRIP2 23
#define NSTRIP3 5
#define NSTRIP (NSTRIP1+NSTRIP2+NSTRIP3) /*90*/

#define NCHAN 48
#define NHIT NSTRIP


#define NH_READS  8   /* the number of reads-write for streams, AND the number of 4ns intervals inside 32ns interval */
#define MAXTIMES 2
#define TIMECORR 7 /*TEMPORARY !!!!!!!!!!!!!!!! */

/* strip persistency */
#define NPER 8
typedef ap_uint<3> nframe_t;


typedef struct
{
  ap_uint<13>	e0;
  ap_uint<3>	t0;
  ap_uint<13>	e1;
  ap_uint<3>	t1;
} fadc_word_t;



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





#define FTOFHIT_TAG 0x5 /*?????????????*/

typedef struct
{
  ap_uint<11> t_start;
  ap_uint<11> t_stop;
} trig_t;

typedef struct
{
  ap_uint<32> data;
  ap_uint<1>  end;
} eventdata_t;


#define NBIT_HIT_ENERGY 16
#define NBIT_COORD_EB 10 /*in event builder coordinates always 10 bits*/

typedef struct
{
  ap_uint<NSTRIP> output;
} hit_ram_t;




int ftoflib(int handler);
void ftofhiteventreader(hls::stream<trig_t> &trig_stream, hls::stream<eventdata_t> &event_stream, FTOFHit &hit);

void ftofstrips(ap_uint<16> strip_threshold, hls::stream<fadc_word_t> s_fadc_words[NSLOT], hls::stream<FTOFStrip_s> &s_strip0);
void ftofstripspersistence(nframe_t nframes, hls::stream<FTOFStrip_s> &s_stripin, hls::stream<FTOFStrip_s> &s_stripout);
void ftofhit(ap_uint<16> strip_threshold, ap_uint<16> mult_threshold, ap_uint<16> cluster_threshold, hls::stream<FTOFStrip_s> &s_strip, hls::stream<FTOFHit> &s_hit);
void ftofhitfanout(hls::stream<FTOFHit> &s_hit, hls::stream<FTOFHit> &s_hit1, hls::stream<FTOFHit> &s_hit2, volatile ap_uint<1> &hit_scaler_inc);
void ftofhiteventfiller(hls::stream<FTOFHit> &s_hitin, hit_ram_t buf_ram[256]);
void ftofhiteventwriter(hls::stream<trig_t> &trig_stream, hls::stream<eventdata_t> &event_stream, hit_ram_t buf_ram_read[256]);

#ifdef	__cplusplus
}
#endif


#define _FTOFLIB_
#endif
