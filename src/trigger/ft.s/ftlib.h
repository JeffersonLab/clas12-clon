#ifndef _FTLIB_

#include <ap_int.h>
#include <hls_stream.h>

#ifdef  __cplusplus
extern "C" {
#endif

/* ftlib.h */

#define NCLSTR (22*20)
#define NCHAN (22*22)

#define NH_READS  8   /* the number of reads-writes for streams */

typedef struct
{
  ap_uint<13>	e0;
  ap_uint<3>	t0;
  ap_uint<13>	e1;
  ap_uint<3>	t1;

} fadc_2ch_t;

typedef struct
{
  ap_uint<13> energy00;
  ap_uint<13> energy01;
  ap_uint<13> energy02;
  ap_uint<13> energy03;
  ap_uint<13> energy04;
  ap_uint<13> energy05;
  ap_uint<13> energy06;
  ap_uint<13> energy07;
  ap_uint<13> energy08;
  ap_uint<13> energy09;
  ap_uint<13> energy10;
  ap_uint<13> energy11;
  ap_uint<13> energy12;
  ap_uint<13> energy13;
  ap_uint<13> energy14;
  ap_uint<13> energy15;
  ap_uint<13> energy16;
  ap_uint<13> energy17;
  ap_uint<13> energy18;
  ap_uint<13> energy19;
  ap_uint<13> energy20;
  ap_uint<13> energy21;

} FT1Strip_s;

typedef struct
{
  ap_uint<13> energy00;
  ap_uint<13> energy01;
  ap_uint<13> energy02;
  ap_uint<13> energy03;
  ap_uint<13> energy04;
  ap_uint<13> energy05;
  ap_uint<13> energy06;
  ap_uint<13> energy07;
  ap_uint<13> energy08;
  ap_uint<13> energy09;
  ap_uint<13> energy10;
  ap_uint<13> energy11;
  ap_uint<13> energy12;
  ap_uint<13> energy13;
  ap_uint<13> energy14;
  ap_uint<13> energy15;
  ap_uint<13> energy16;
  ap_uint<13> energy17;
  ap_uint<13> energy18;
  ap_uint<13> energy19;

} FT2Strip_s;

typedef struct
{
  ap_uint<NCLSTR> mask;

} FTHit;

void
ftcal(ap_uint<16> strip_threshold, ap_uint<16> mult_threshold, ap_uint<16> cluster_threshold,
      hls::stream<FT1Strip_s> s_strip_ft1, hls::stream<FT2Strip_s> s_strip_ft2,
	  hls::stream<FTHit> &s_hit);

#ifdef  __cplusplus
}
#endif


#define _FTLIB_
#endif
