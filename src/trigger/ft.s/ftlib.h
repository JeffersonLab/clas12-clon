#ifndef _FTLIB_

#include <ap_int.h>
#include <hls_stream.h>

#ifdef  __cplusplus
extern "C" {
#endif

/* ftlib.h */

#define NCLSTR (22*20)
#define NCHAN (22*22)

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
