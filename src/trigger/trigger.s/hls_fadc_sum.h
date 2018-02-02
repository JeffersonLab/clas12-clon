
/* hls_fadc_sum.h */

#ifndef hls_fadc_sum_h
#define hls_fadc_sum_h

#include <ap_int.h>
#include <ap_fixed.h>
#include <hls_stream.h>


#define NFADCS 16 /* max possible number of fadcs in crate */

#define ECPEAK_TAG     0x4
#define ECHIT_TAG      0x5
#define HTCCHIT_TAG    0x6
#define FTCLUSTER_TAG  0x7
#define FTOFHIT_TAG    0x8
#define CTOFHIT_TAG    0x9
#define CNDHIT_TAG     0xA
#define PCUHIT_TAG     0xB

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
  fadc_16ch_t fadc[NFADCS];

} fadc_256ch_t;

/* stream to initiate readout */
typedef struct
{
  ap_uint<11> t_start;
  ap_uint<11> t_stop;
} trig_t;

/* readout data */
typedef struct
{
  ap_uint<32> data;
  ap_uint<1>  end;
} eventdata_t;

typedef struct
{
  ap_uint<32> data[3];
  ap_uint<1>  end;
} eventdata3_t;

#endif
