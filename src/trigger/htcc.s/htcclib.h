#ifndef _HTCCLIB_

#include <ap_int.h>
#include <hls_stream.h>

#ifdef	__cplusplus
extern "C" {
#endif

/* eclib.h */





#define NCHAN 48
#define NCLSTR 36





ap_uint<6>  htcc1(ap_uint<13> threshold, ap_uint<3> mult_threshold, ap_uint<16> cluster_threshold,
		ap_uint<13> d[NCHAN], ap_uint<3> mult[NCLSTR], ap_uint<16> clusters[NCLSTR]);


#ifdef	__cplusplus
}
#endif


#define _HTCCLIB_
#endif
