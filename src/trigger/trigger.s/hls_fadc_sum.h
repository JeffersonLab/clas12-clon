
/* hls_fadc_sum.h */

#ifndef hls_fadc_sum_h
#define hls_fadc_sum_h

#include <ap_int.h>
#include <ap_fixed.h>
#include <hls_stream.h>

typedef struct
{
  ap_uint<13>	e;
  ap_uint<3>	t;

} fadc_hit_t;



typedef struct
{
  ap_uint<13>	e0;
  ap_uint<3>	t0;
  ap_uint<13>	e1;
  ap_uint<3>	t1;

} fadc_2ch_t;

typedef struct
{
  ap_uint<13>	e0;
  ap_uint<3>	t0;
  ap_uint<13>	e1;
  ap_uint<3>	t1;
  ap_uint<13>	e2;
  ap_uint<3>	t2;
  ap_uint<13>	e3;
  ap_uint<3>	t3;

} fadc_4ch_t;

typedef struct
{
  ap_uint<13>	e0;
  ap_uint<3>	t0;
  ap_uint<13>	e1;
  ap_uint<3>	t1;
  ap_uint<13>	e2;
  ap_uint<3>	t2;
  ap_uint<13>	e3;
  ap_uint<3>	t3;
  ap_uint<13>	e4;
  ap_uint<3>	t4;
  ap_uint<13>	e5;
  ap_uint<3>	t5;
  ap_uint<13>	e6;
  ap_uint<3>	t6;
  ap_uint<13>	e7;
  ap_uint<3>	t7;
  ap_uint<13>	e8;
  ap_uint<3>	t8;
  ap_uint<13>	e9;
  ap_uint<3>	t9;
  ap_uint<13>	e10;
  ap_uint<3>	t10;
  ap_uint<13>	e11;
  ap_uint<3>	t11;
  ap_uint<13>	e12;
  ap_uint<3>	t12;
  ap_uint<13>	e13;
  ap_uint<3>	t13;
  ap_uint<13>	e14;
  ap_uint<3>	t14;
  ap_uint<13>	e15;
  ap_uint<3>	t15;

} fadc_16ch_t;


typedef struct
{
  fadc_hit_t hits[16];

} fadc_hits_t;

typedef ap_uint<14> esum_t;

void hls_fadc_sum(ap_uint<16> esum_en, hls::stream<fadc_4ch_t> &s_fadc_words, hls::stream<esum_t> &s_esum);

#define NFADCS 16 /* max possible number of fadcs in crate */


#ifdef	__cplusplus
extern "C" {
#endif

int fadcs(unsigned int *bufptr, unsigned short threshold, int sec, int detector, hls::stream<fadc_16ch_t> s_fadc_words[NFADCS],
          int dtimestamp, int dpulsetime, int *iev, unsigned long long *timestamp);
void fadcs_32ns_to_8ns(hls::stream<fadc_16ch_t> &s_fadc_in, hls::stream<fadc_4ch_t> &s_fadc_out);
void fadcs_32ns_to_4ns(hls::stream<fadc_16ch_t> &s_fadc_in, hls::stream<fadc_2ch_t> &s_fadc_out);

#ifdef	__cplusplus
}
#endif


#endif
