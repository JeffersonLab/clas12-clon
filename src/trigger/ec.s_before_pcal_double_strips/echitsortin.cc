
/* echitsortin,cc */

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
#define echitsortin pchitsortin
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
echitsortin(hls::stream<ECStream10_s> s_coord2[NH_FIFOS], hls::stream<ECStream16_s> s_enpeak[NH_FIFOS], hls::stream<sortz_t> z1[NF4])
{
#pragma HLS DATA_PACK variable=z1
#pragma HLS INTERFACE axis register both port=z1
#pragma HLS DATA_PACK variable=s_enpeak
#pragma HLS INTERFACE axis register both port=s_enpeak
#pragma HLS DATA_PACK variable=s_coord2
#pragma HLS INTERFACE axis register both port=s_coord2
#pragma HLS ARRAY_PARTITION variable=s_coord2 complete dim=1
#pragma HLS ARRAY_PARTITION variable=s_enpeak complete dim=1
#pragma HLS ARRAY_PARTITION variable=z1 complete dim=1
#pragma HLS PIPELINE II=4

  ap_uint<6> stage;
  uint8_t i, ihit, k;
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



  for(int i=0; i<NH_FIFOS; i++)
  {
    for(int j=0; j<NH_READS; j++)
    {
      coord_fifo = s_coord2[i].read();
      for(int k=0; k<3; k++) coord[i*NH_READS+j][k] = coord_fifo.word10[k];
    }
  }

  for(int i=0; i<NH_FIFOS; i++)
  {
    for(int j=0; j<NH_READS; j++)
    {
      enpeak_fifo = s_enpeak[i].read();
      for(int k=0; k<3; k++) enpeak[i*NH_READS+j][k] = enpeak_fifo.word16[k];
    }
  }

  /* combine coordinates and energies into hits */
  for(ind=0; ind<NHITMAX; ind++)
  {
    hitin[ind].energy = enpeak[ind][0]+enpeak[ind][1]+enpeak[ind][2];
    for(i=0; i<3; i++) hitin[ind].coord[i] = coord[ind][i];
#ifndef __SYNTHESIS__
    hitin[ind].ind = ind;
    for(i=0; i<3; i++) hitin[ind].enpeak[i] = enpeak[ind][i];
#endif
  }


  //for(i=0; i<NHITMAX; i++) cout<<"hitin["<<+i<<"] energy="<<hitin[i].energy<<endl;


  /***********/
  /* sorting */

  /* copy input data from input array of structures to local array */
  for(i=0; i<NHITMAX; i++)
  {
#ifndef __SYNTHESIS__
	z[i] = (hitin[i].enpeak[2], hitin[i].enpeak[1], hitin[i].enpeak[0], hitin[i].ind, hitin[i].coord[2], hitin[i].coord[1], hitin[i].coord[0], hitin[i].energy);
#else
	z[i] = (hitin[i].coord[2], hitin[i].coord[1], hitin[i].coord[0], hitin[i].energy);
#endif
  }

  //for(i=0; i<NHITMAX; i++) cout<<"befor: z["<<+i<<"] ="<<z[i](ECHIT_ENERGY-1, 0)<<" "<<z[i](ECHIT_COORDU-1, ECHIT_ENERGY)<<endl;

  /* 'sorting network' loop
  for(stage=0; stage<(NHITMAX/2); stage++)
  {
	echitsort_2slices(z);
  }
*/


//#ifndef __SYNTHESIS__ /* sergey: TEMPORARY !!! */

  SORTING(0,62);
  SORTING(1,61);

  SORTING(0,62);
  SORTING(1,61);



//#endif /* #ifndef __SYNTHESIS__ */


  /* end of 'sorting network' loop */

  //for(i=0; i<NHITMAX; i++) cout<<"after: z["<<+i<<"] ="<<z[i](ECHIT_ENERGY-1, 0)<<" "<<z[i](ECHIT_COORDU-1, ECHIT_ENERGY)<<endl;

  for(int i=0; i<NF4; i++)
  {
    for(int j=0; j<NR4; j++)
    {
      z1[i].write(z[i*NR4+j]);
    }
  }

  return;
}
