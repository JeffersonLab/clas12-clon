
/* pcpeakcoord.c */

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
/* 3.39/41/1/0%/0%/0%/1% */

#undef DEBUG



#if 0

#define WIN1_LEN  16384 /* max size available for now !!! */

/* dividing lookup table */
static void coeff1_init(uint8_t rom_array[WIN1_LEN])
{
  int i,j,k;

  /* fraction(8bit) = energy(7bit) / energy(7bit) */
  for(k=0; k<WIN1_LEN; k++)
  {
    j = k&0x7F;
    i = (k>>7)&0x7F;
    if(j==0) rom_array[k] = 0;
    else rom_array[k] = (uint8_t)( ((float)(i))/((float)(j)) );
    /*if(i==110&j==79) printf("lookup: i=%3d j=%4d k=%7d (0x%04x) -> data=%5d\n",i,j,k,k,rom_array[k]);*/
  }
  /*exit(0);*/
}

static uint8_t get_addr_shift(addr)
{
#pragma HLS INLINE
#pragma HLS PIPELINE
	uint8_t nn;

	if(addr&0x8000)      nn = 9;
	else if(addr&0x4000) nn = 8;
	else if(addr&0x2000) nn = 7;
	else if(addr&0x1000) nn = 6;
	else if(addr&0x0800) nn = 5;
	else if(addr&0x0400) nn = 4;
	else if(addr&0x0200) nn = 3;
	else if(addr&0x0100) nn = 2;
	else if(addr&0x0080) nn = 1;
	else                 nn = 0;

    return(nn);
}

static uint8_t coeff1_get(uint16_t addr1, uint16_t addr2, uint8_t n, uint8_t rom_array[WIN1_LEN])
{
#pragma HLS INLINE
#pragma HLS PIPELINE
  uint8_t ret;
  uint8_t temp1,temp2;
  uint16_t addr;

  /*printf("addr1=%d addr2=%d n=%d\n",addr1,addr2,n);*/

  temp1 = addr1>>n;
  temp2 = addr2>>n;

  addr = (temp1<<7)|temp2;

  /*printf("temp1=%d temp2=%d addr=%d(0x%08x)\n",temp1,temp2,addr,addr);*/

  ret = rom_array[addr];

  return(ret);
}

#endif



int
pcpeakcoord(uint8_t view, PCPeak0 peakin[PPEAK], PCPeak peakout[PPEAK])
{
#pragma HLS ARRAY_PARTITION variable=peakout complete dim=1
#pragma HLS ARRAY_PARTITION variable=peakin complete dim=1
#pragma HLS PIPELINE
#if 0
  static uint8_t coeff1[WIN1_LEN];
#endif
  uint16_t temp1, temp2, temp3;
  uint8_t n,n1,n2; /* shift needed for addr1 and addr2 to be fitted into 7 bit */
  uint8_t i, j, k, strip1, nstrip, npeak, ipeak, mpeak, peak_opened;
  uint16_t energy, tmp;

#ifdef DEBUG
  printf("\n\n++ pcpeakcoord ++\n");
  printf("BEFOR:\n");
  for(i=0; i<PPEAK; i++)
  {
    printf("peakin[%2d]: energy=%d, coord=%d(%d), first strip=%d, number of strips=%d\n",
      i,peakin[i].energy,peakin[i].coord,peakin[i].coord>>3,peakin[i].strip1,peakin[i].stripn);
  }
#endif

#if 0
  coeff1_init(coeff1);

  temp1 = energysum4coord; \
  temp2 = energysum; \
  n1 = get_addr_shift(temp1); \
  n2 = get_addr_shift(temp2); \
  n = MAX(n1,n2); \
  temp3 = coeff1_get(temp1,temp2,n,coeff1)

#endif


  for(i=0; i<PPEAK; i++)
  {
    energy = peakin[i].energy;
    strip1 = peakin[i].strip1;

    peakout[i].energy = energy;
    /*peakout[i].time = peakin[i].time;*/
    peakout[i].strip1 = strip1;
    peakout[i].stripn = peakin[i].stripn;
	if(energy>0)
	{
      tmp = (peakin[i].energysum4coord * fview[view]) / energy;
	  /*printf("energysum4coord=%d, fview[view]=%d, energy=%d -> tmp=%d\n",peakin[i].energysum4coord,fview[view],energy,tmp);*/
      peakout[i].coord = strip1*fview[view] + UVWADD + tmp;
	}
  }




#ifdef DEBUG
  printf("AFTER:\n");
  for(i=0; i<PPEAK; i++)
  {
    printf("peakout[%2d]: energy=%d, coord=%d(%d), first strip=%d, number of strips=%d\n",
      i,peakout[i].energy,peakout[i].coord,peakout[i].coord>>3,peakout[i].strip1,peakout[i].stripn);
  }
#endif

  return(0);
}
