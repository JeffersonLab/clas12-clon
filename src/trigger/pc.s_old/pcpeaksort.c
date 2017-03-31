
/* pcpeaksort.c */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "pclib.h"

#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)
#define ABS(x)      ((x) < 0 ? -(x) : (x))


/*PPEAK=3*/
/* 3.36/ 55/9/0%/0%/0%/2% */
/* 2.39/ 35/9/0%/0%/2%/5% - bubble sort */
/* 3.39/ 38/39/0%/0%/0%/2% - bunch if if's using whole peaks */
/* 3.39/ 38/39/0%/0%/0%/2% - bunch if if's using en[]'s */

#define DEBUG

uint8_t
pcal_get_least_energy(uint16_t en[PPEAK])
{
#pragma HLS PIPELINE
#pragma HLS INLINE
  uint8_t i, j;
  uint16_t energy;
  j = 0;
  energy = en[0];
  for(i=1; i<PPEAK; i++)
  {
    if(en[i] < energy)
	{
      j=i;
      energy = en[j];
	}
  }
  return(j);
}



#define N1(i_macro)   en1[i_macro]
#define DON1(i_macro,j_macro) {peakout[j_macro]=peak1[i_macro]; en2[i_macro]=0;en3[i_macro]=0;}

#define N2(i_macro)   en2[i_macro]
#define DON2(i_macro,j_macro) {peakout[j_macro]=peak1[i_macro]; en3[i_macro]=0;}

#define N3(i_macro)   en3[i_macro]
#define DON3(i_macro,j_macro) {peakout[j_macro]=peak1[i_macro]; }



#define M1(i_macro)   peak1[i_macro].energy
#define DOM1(i_macro,j_macro) {peakout[j_macro]=peak1[i_macro];peak2[i_macro].energy=0;peak3[i_macro].energy=0;}

#define M2(i_macro)   peak2[i_macro].energy
#define DOM2(i_macro,j_macro) {peakout[j_macro]=peak2[i_macro];peak3[i_macro].energy=0;}

#define M3(i_macro)   peak3[i_macro].energy
#define DOM3(i_macro,j_macro) {peakout[j_macro]=peak3[i_macro];/*peak4[i_macro].energy=0;*/}

int
pcpeaksort(PCPeak0 peakin[PPEAKMAX], PCPeak0 peakout[PPEAK])
{
#pragma HLS PIPELINE
  uint16_t temp1, temp2, temp3;
  uint8_t n,n1,n2; /* shift needed for addr1 and addr2 to be fitted into 7 bit */
  uint8_t i, j, k, strip1, nstrip, npeak, ipeak, mpeak, peak_opened;
  uint16_t energy, energysum, epeak, energysum4coord;
  uint16_t en[PPEAKMAX];
#pragma HLS ARRAY_PARTITION variable=en complete dim=1
  uint16_t en1[PPEAKMAX];
#pragma HLS ARRAY_PARTITION variable=en1 complete dim=1
  uint16_t en2[PPEAKMAX];
#pragma HLS ARRAY_PARTITION variable=en2 complete dim=1
  uint16_t en3[PPEAKMAX];
#pragma HLS ARRAY_PARTITION variable=en3 complete dim=1
  uint8_t ind[PPEAKMAX];
#pragma HLS ARRAY_PARTITION variable=ind complete dim=1
  PCPeak0 peak1[PPEAKMAX];
#pragma HLS ARRAY_PARTITION variable=peak1 complete dim=1
  PCPeak0 peak2[PPEAKMAX];
#pragma HLS ARRAY_PARTITION variable=peak2 complete dim=1
  PCPeak0 peak3[PPEAKMAX];
#pragma HLS ARRAY_PARTITION variable=peak3 complete dim=1
  PCPeak0 peakswap[PPEAKMAX];
#pragma HLS ARRAY_PARTITION variable=peakswap complete dim=1

#ifdef DEBUG
  printf("\n\n++ pcpeaksort ++\n");
  printf("BEFOR:\n");
  for(i=0; i<PPEAKMAX; i++)
  {
    printf("peakin[%2d]: energy=%d, energysum4coord=%d, first strip=%d, number of strips=%d\n",
      i,peakin[i].energy,peakin[i].energysum4coord,peakin[i].strip1,peakin[i].stripn);
  }
#endif

#if 0
  for(i=0; i<PPEAKMAX; i++)
  {
    peak1[i] = peakin[i];
    en[i]    = peakin[i].energy;
    ind[i]   = i;
  }
  for(i=PPEAK; i<PPEAKMAX; i++)
  {
    j=0;
    if(en[0]<en[1] && en[0]<en[2]) j=0;
    if(en[1]<en[0] && en[1]<en[2]) j=1;
    if(en[2]<en[0] && en[2]<en[1]) j=2;
	/*
    j = pcal_get_least_energy(en);
	*/

    if(en[j] < en[i])
    {
      en[j] = en[i];
      ind[j] = ind[i];
    }
  }
  for(i=0; i<PPEAK; i++) peakout[i] = peak1[ind[i]];
#endif



#if 1
  /* bubble sort */
  for(i=0; i<PPEAKMAX; i++) peak1[i] = peakin[i];
  for(i=1; i<PPEAKMAX; i++)
  {
    for(j=0; j<(PPEAKMAX-i); j++)
    {
      if (peak1[j].energy < peak1[j+1].energy)
      {
        peakswap[j] = peak1[j];
        peak1[j]    = peak1[j+1];
        peak1[j+1]  = peakswap[j];
      }
    }
  }
  for(i=0; i<PPEAK; i++) peakout[i] = peak1[i];

  /* bubble sort 
  for(i=0; i<PPEAKMAX; i++) peak1[i] = peakin[i];
  for(i=0; i<(PPEAKMAX-1); i++)
  {
    for(j=0; j<(PPEAKMAX-i-1); j++)
    {
      if (peak1[j].energy < peak1[j+1].energy)
      {
        peakswap[j] = peak1[j];
        peak1[j]    = peak1[j+1];
        peak1[j+1]  = peakswap[j];
      }
    }
  }
  for(i=0; i<PPEAK; i++) peakout[i] = peak1[i];
*/
#endif



  /*
volatile int input_data[16];
volatile int output_data[16];
int
main()
{
  int i, temp[16];
  for(i = 0; i < 16; ++i) temp[i] = input_data[i];
  compAndSwap(&temp[0], &temp[1]);
  compAndSwap(&temp[2], &temp[3]);
  compAndSwap(&temp[4], &temp[5]);
  compAndSwap(&temp[6], &temp[7]);
  ...
  for(i = 0; i < 16; ++i) output_data[i] = temp[i];
  return 0;
 }
  */







#ifdef DEBUG
  printf("AFTER:\n");
  for(i=0; i<PPEAK; i++)
  {
    printf("peakout[%2d]: energy=%d, energysum4coord=%d(%d), first strip=%d, number of strips=%d\n",
      i,peakout[i].energy,peakout[i].energysum4coord,peakout[i].strip1,peakout[i].stripn);
  }
#endif

  return(0);
}
