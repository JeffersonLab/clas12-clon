/* ecfrac.c - energy fractions calculation

  input:  

  output: 
*/


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


#ifdef USE_PCAL

#include "../pc.s/pclib.h"

#define ecfrac pcfrac
#define ecfrac1 pcfrac1
#define ecfrac2 pcfrac2
#define ecfrac3 pcfrac3

#else

#include "eclib.h"

#endif


//#define DEBUG

#include <ap_fixed.h>




/*xc7vx550tffg1158-1*/


/* 3.41/12/1/32%/0%/6%/14% */

void ecfrac1(fp0201S_t peakcount[3][NPEAK], ap_uint<16> energy[NHITMAX][3], hitsume_t hitouttmp1[NHITMAX]);
void ecfrac2(hitsume_t hitouttmp1[NHITMAX], hitsume_t hitouttmp2[NHITMAX]);
void ecfrac3(hitsume_t hitouttmp2[NHITMAX], uint16_t fracout[NHITMAX][3]);


void
ecfrac(fp0201S_t peakcount[3][NPEAK], ap_uint<16> energy[NHITMAX][3], uint16_t fracout[NHITMAX][3])
{
#pragma HLS ARRAY_PARTITION variable=peakcount complete dim=1
#pragma HLS ARRAY_PARTITION variable=peakcount complete dim=2
#pragma HLS ARRAY_PARTITION variable=energy complete dim=1
#pragma HLS ARRAY_PARTITION variable=energy complete dim=2
#pragma HLS ARRAY_PARTITION variable=fracout complete dim=1
#pragma HLS ARRAY_PARTITION variable=fracout complete dim=2
#pragma HLS PIPELINE

  uint8_t u, v, w, ind, i;
  hitsume_t hitouttmp1[NHITMAX];
  hitsume_t hitouttmp2[NHITMAX];

#ifdef DEBUG
  printf("\n\n\n+++ ecfrac +++\n\n\n");
#endif


  /* loop for all hits */

#ifdef DEBUG
  printf("entering loop over hits\n\n");
#endif

  ecfrac1(peakcount, energy, hitouttmp1);
  ecfrac2(hitouttmp1, hitouttmp2);
  ecfrac3(hitouttmp2, fracout);


#ifdef DEBUG
  printf("\n\n\n+++ ecfrac done +++\n\n\n");
#endif



  return;
}


