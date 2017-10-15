
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


/* data format between FADCs and VTPs 
typedef struct
{
  ap_uint<13>	e0;
  ap_uint<3>	t0;
  ap_uint<13>	e1;
  ap_uint<3>	t1;

} fadc_word_t;
*/


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

} fadc_word_t;



typedef struct
{
  fadc_hit_t hits[16];

} fadc_hits_t;

typedef ap_uint<14> esum_t;

void hls_fadc_sum(ap_uint<16> esum_en, hls::stream<fadc_word_t> &s_fadc_words, hls::stream<esum_t> &s_esum);

#define NFADCS 16 /* max possible number of fadcs in crate */


#ifdef	__cplusplus
extern "C" {
#endif

int fadcs(unsigned int *bufptr, unsigned short threshold, int sec, int detector, hls::stream<fadc_word_t> s_fadc_words[NFADCS],
          int dtimestamp, int dpulsetime, int *iev, unsigned long long *timestamp);

#ifdef	__cplusplus
}
#endif


#endif
