/* echitsort11.cc */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

using namespace std;
#include <strstream>


#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)
#define ABS(x)      ((x) < 0 ? -(x) : (x))


//#define DEBUG

#ifdef USE_PCAL

#include "../pc.s/pclib.h"
#define echitsort11 pchitsort11
#define echitsort_2words pchitsort_2words

#else

#include "eclib.h"

#endif



/* 2.31/0/1/0/0/0/104 II=1, z complete */
static void
echitsort_2words(ap_uint<ECHIT_BITS> z[2])
{
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION variable=z complete dim=1
#pragma HLS PIPELINE II=1

  ap_uint<ECHIT_ENERGY_BITS> test1, test2;
  ap_uint<ECHIT_BITS> tmp1, tmp2;

  test1 = z[0](ECHIT_ENERGY-1, 0);
  test2 = z[1](ECHIT_ENERGY-1, 0);
  tmp1 = ((test1 > test2) ? z[0] : z[1]);
  tmp2 = ((test1 < test2) ? z[0] : z[1]);
  z[0] = tmp1;
  z[1] = tmp2;
}


#define SORTING(I1,I2) \
  { \
    ap_uint<ECHIT_BITS> zz[2]; \
    for(int i=I1; i<=I2; i=i+2) \
    { \
      zz[0] = z[i]; \
      zz[1] = z[i+1]; \
      echitsort_2words(zz); \
      z[i] = zz[0]; \
      z[i+1] = zz[1]; \
    } \
  }


/**/

void
echitsort11(hls::stream<sortz_t> z2[NF4], hls::stream<sortz_t> z1[NF4])
{
#pragma HLS DATA_PACK variable=z1
#pragma HLS INTERFACE axis register both port=z1
#pragma HLS DATA_PACK variable=z2
#pragma HLS INTERFACE axis register both port=z2
#pragma HLS ARRAY_PARTITION variable=z2 complete dim=1
#pragma HLS ARRAY_PARTITION variable=z1 complete dim=1
#pragma HLS PIPELINE II=4

  ap_uint<ECHIT_BITS> z[NHITMAX];
#pragma HLS ARRAY_PARTITION variable=z complete dim=1

  for(int i=0; i<NF4; i++)
  {
    for(int j=0; j<NR4; j++)
    {
      z[i*NR4+j] = z2[i].read();
    }
  }


  SORTING(0,58);
  SORTING(1,57);


  for(int i=0; i<NF4; i++)
  {
    for(int j=0; j<NR4; j++)
    {
      z1[i].write(z[i*NR4+j]);
    }
  }

  return;
}
