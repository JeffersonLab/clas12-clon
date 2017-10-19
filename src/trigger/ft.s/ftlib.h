#ifndef _FTLIB_

#include <ap_int.h>
#include <hls_stream.h>

#ifdef  __cplusplus
extern "C" {
#endif

/* ftlib.h */



#define ADCFT1_ROC_ID    70
#define ADCFT2_ROC_ID    71
#define ADCFT3_ROC_ID    72

#define FT_MIN_X         0
#define FT_MAX_X         21
#define FT_MIN_Y         0
#define FT_MAX_Y     	 21
#define FT_CRYSTAL_NUM   332
#define FT_MAX_CLUSTERS  64 //a conservative estimate of how many clusters are reported at each time slice

#include "fttypes.h"

//void
//ftcal(ap_uint<16> strip_threshold, ap_uint<16> mult_threshold, ap_uint<16> cluster_threshold,
//      hls::stream<FT1Strip_s> s_strip_ft1, hls::stream<FT2Strip_s> s_strip_ft2,
//	  hls::stream<FTHit> &s_hit);

#ifdef  __cplusplus
}
#endif

#define _FTLIB_
#endif
