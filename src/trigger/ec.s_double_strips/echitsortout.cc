/* echitsortout.cc */

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
#define echitsortout pchitsortout
#define echitsort_2words pchitsort_2words
#define echitsort_2slices pchitsort_2slices


#else

#include "eclib.h"

#endif



/* 2.31/0/1/0/0/0/104 II=1, z complete */
static void
echitsort_2words(ap_uint<ECHIT_BITS> z[2])
{
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





/* 4.62/0/1/0/0/0/3640 II=1, z complete */
static void
echitsort_2slices(ap_uint<ECHIT_BITS> z[NHITMAX])
{
#pragma HLS ARRAY_PARTITION variable=z complete dim=1
#pragma HLS PIPELINE II=1

  ap_uint<ECHIT_BITS> zz[2];

  for(int i=0; i<NHITMAX-1; i=i+2)
  {
	zz[0] = z[i];
    zz[1] = z[i+1];
 	echitsort_2words(zz);
 	z[i] = zz[0];
    z[i+1] = zz[1];
  }

  for(int i=1; i<NHITMAX-1; i=i+2)
  {
	zz[0] = z[i];
	zz[1] = z[i+1];
	echitsort_2words(zz);
 	z[i] = zz[0];
    z[i+1] = zz[1];
  }
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


/*
void
ethitsort2slice(hls::stream<sortz_t> z1[NF4], hls::stream<sortz_t> z2[NF4])
{
  ap_uint<ECHIT_BITS> z[NHITMAX];
#pragma HLS ARRAY_PARTITION variable=z complete dim=1
#pragma HLS PIPELINE II=1

  for(int i=0; i<NF4; i++)
  {
    for(int j=0; j<NR4; j++)
    {
      z[i*NR4+j] = z1[i].read();
    }
  }

  echitsort_2slices(z);

  for(int i=0; i<NF4; i++)
  {
    for(int j=0; j<NR4; j++)
    {
      z2[i].write(z[i*NR4+j]);
    }
  }

}
*/

/* 3.17/81/8/0/0/15857/26724(2%/7%) II=8 macros */

/* 2.54/7/8/0%/0%/~0%/~0% II=8 macros with streams WITHOUT SORTING */

/* 3.17/93/8/0%/0%/2%/9% II=8 macros with streams */

void
echitsortout(hls::stream<sortz_t> z2[NF4], hls::stream<ECHit> &s_hits)
{
#pragma HLS DATA_PACK variable=s_hits
#pragma HLS INTERFACE axis register both port=s_hits
#pragma HLS DATA_PACK variable=z2
#pragma HLS INTERFACE axis register both port=z2
#pragma HLS ARRAY_PARTITION variable=z2 complete dim=1
#pragma HLS PIPELINE II=4

  ap_uint<6> stage;
  uint8_t i, k;
  uint8_t ind;

  ECHit hitin[NHITMAX];
  ap_uint<ECHIT_ENERGY_BITS> en, test1, test2;
  ap_uint<ECHIT_BITS> z[NHITMAX];
#pragma HLS ARRAY_PARTITION variable=z complete dim=1
  ECHit hitout[NHIT];
#pragma HLS ARRAY_PARTITION variable=hitout complete dim=1

  ap_uint<NBIT_COORD> coord[NHITMAX][3];
  ECStream10_s coord_fifo;

  uint16_t enpeak[NHITMAX][3];
  ECStream16_s enpeak_fifo;


#ifdef DEBUG
  printf("\n+++ echitsort start +++\n\n");
  printf("ECHIT_BITS=%d\n",ECHIT_BITS);
#endif


  for(int i=0; i<NF4; i++)
  {
    for(int j=0; j<NR4; j++)
    {
      z[i*NR4+j] = z2[i].read();
    }
  }



  SORTING(0,20);
  SORTING(1,19);

  SORTING(0,18);
  SORTING(1,17);

  SORTING(0,16);
  SORTING(1,15);

  SORTING(0,14);
  SORTING(1,13);

  SORTING(0,12);
  SORTING(1,11);

  SORTING(0,10);
  SORTING(1,9);

  SORTING(0,8);
  SORTING(1,7);

  SORTING(0,6);
  SORTING(1,5);

  SORTING(0,4);
  SORTING(1,3);

  SORTING(0,2);
  SORTING(1,1);


  //for(i=0; i<NHITMAX; i++) cout<<"after: z["<<+i<<"] ="<<z[i](ECHIT_ENERGY-1, 0)<<" "<<z[i](ECHIT_COORDU-1, ECHIT_ENERGY)<<endl;

  /* extract data from sorted array to output array of structures */
  for(i=0; i<NHIT; i++)
  {
	 hitout[i].energy    = z[i](ECHIT_ENERGY-1, 0);
	 hitout[i].coord[0]  = z[i](ECHIT_COORDU-1, ECHIT_ENERGY);
	 hitout[i].coord[1]  = z[i](ECHIT_COORDV-1, ECHIT_COORDU);
	 hitout[i].coord[2]  = z[i](ECHIT_COORDW-1, ECHIT_COORDV);
#ifndef __SYNTHESIS__
	 hitout[i].ind       = z[i](ECHIT_IND-1, ECHIT_COORDW);
	 hitout[i].enpeak[0] = z[i](ECHIT_EU-1, ECHIT_IND);
	 hitout[i].enpeak[1] = z[i](ECHIT_EV-1, ECHIT_EU);
	 hitout[i].enpeak[2] = z[i](ECHIT_EW-1, ECHIT_EV);
#endif
  }

  /* sorting */
  /***********/

#ifdef DEBUG
  for(i=0; i<NHIT; i++) cout<<"hitout["<<+i<<"] energy="<<hitout[i].energy<<endl;
  printf("\n+++ echitsort done +++\n\n");
#endif

  for(int i=0; i<NHIT; i++) s_hits.write(hitout[i]);

  return;
}
