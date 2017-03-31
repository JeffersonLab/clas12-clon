
/* ecpeaksort.c */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include <iostream>
using namespace std;


#ifdef USE_PCAL

#include "../pc.s/pclib.h"
#define ecpeaksort pcpeaksort

#else

#include "eclib.h"

#endif


//#define DEBUG


#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)
#define ABS(x)      ((x) < 0 ? -(x) : (x))

/*
#define PEAKMAX(a,b)    (a##.energy > b##.energy ? a : b)
#define PEAKMIN(a,b)    (a##.energy < b##.energy ? a : b)
*/

/*xc7vx550tffg1158-1*/



int
ecpeaksort(ECPeak0 peakin[NSTRIP], ECPeak0 peakout[NPEAK])
{
#pragma HLS PIPELINE II=1
#pragma HLS ARRAY_PARTITION variable=peakin complete dim=1
#pragma HLS ARRAY_PARTITION variable=peakout complete dim=1

  ap_uint<8> i, j, k, stage;
  ap_uint<2> ii, jj;

  ap_uint<ECPEAK0_ENERGY_BITS> test1, test2;

  //ECPeak0 tmp1, tmp2, z[NSTRIP];
  //ap_uint<52> tmp1, tmp2;
  //ap_uint<52> z[NSTRIP];
  ap_uint<ECPEAK0_BITS> tmp1, tmp2;
  ap_uint<ECPEAK0_BITS> z[NSTRIP];
#pragma HLS ARRAY_RESHAPE variable=z complete dim=1


#if 0

  ECPeak0 peakswap, peaktmp, peak0[NSTRIP];
#pragma HLS ARRAY_PARTITION variable=peak0 complete dim=1

  ap_uint<16> enswap, enn, en[NSTRIP], en0[NPEAK], en1[NSTRIP];
#pragma HLS ARRAY_PARTITION variable=en complete dim=1

  ap_uint<8> indswap, ind[NSTRIP];
#pragma HLS ARRAY_PARTITION variable=ind complete dim=1

#endif



#ifdef DEBUG
  printf("\n\n++ ecpeaksort ++\n");
  printf("BEFOR:\n");
  for(i=0; i<NSTRIP; i++)
  {
    cout<<"peakin["<<i<<"]: energy="<<peakin[i].energy<<", energysum4coord="<<peakin[i].energysum4coord<<", first strip="<<peakin[i].strip1<<", number of strips="<<peakin[i].stripn<<endl;
  }
#endif







/* 3.36/26/1/0%/0%/2%/7% */
#if 1
  /* sorting network */

  /* copy input data locally */
  //for(i=0; i<NSTRIP; i++) z[i] = peakin[i];
  for(i=0; i<NSTRIP; i++)
  {
	z[i] = ((peakin[i].energy&ECPEAK0_ENERGY_MASK)<<(ECPEAK0_ENESUM_BITS+ECPEAK0_STRIPN_BITS+ECPEAK0_STRIP1_BITS)) |
		   ((peakin[i].strip1&ECPEAK0_STRIP1_MASK)<<(ECPEAK0_ENESUM_BITS+ECPEAK0_STRIPN_BITS)) |
		   ((peakin[i].stripn&ECPEAK0_STRIPN_MASK)<<ECPEAK0_ENESUM_BITS) |
			(peakin[i].energysum4coord&ECPEAK0_ENESUM_MASK);

	//z[i] = (peakin[i].energy, peakin[i].strip1, peakin[i].stripn, peakin[i].energysum4coord);
    //z[i](15,0) = peakin[i].energy

	//z[i] = ((peakin[i].energy&0xFFFF)<<(24+6+6)) | ((peakin[i].strip1&0x3F)<<(24+6)) | ((peakin[i].stripn&0x3F)<<24) | (peakin[i].energysum4coord&0xFFFFFF);
    //cout<<"ZZ "<<peakin[i].energy<<" "<<peakin[i].energysum4coord<<" -> "<<z[i]<<endl;
  }

  /* sorting network loop */
  for(stage=1; stage<=NSTRIP; stage++)
  {
    if((stage%2)==1) k=0;
    if((stage%2)==0) k=1;

    for(i=k; i<NSTRIP-1; i=i+2)
    {
      //tmp1 = (z[i].energy > z[i+1].energy ? z[i] : z[i+1]);
      //tmp2 = (z[i].energy < z[i+1].energy ? z[i] : z[i+1]);

      //tmp1 = (z[i] > z[i+1] ? z[i] : z[i+1]);
      //tmp2 = (z[i] < z[i+1] ? z[i] : z[i+1]);

      test1 = (z[i]>>(ECPEAK0_ENESUM_BITS+ECPEAK0_STRIPN_BITS+ECPEAK0_STRIP1_BITS)) & ECPEAK0_ENERGY_MASK;
      test2 = (z[i+1]>>(ECPEAK0_ENESUM_BITS+ECPEAK0_STRIPN_BITS+ECPEAK0_STRIP1_BITS)) & ECPEAK0_ENERGY_MASK;
      tmp1 = ((test1 > test2) ? z[i] : z[i+1]);
      tmp2 = ((test1 < test2) ? z[i] : z[i+1]);

      z[i  ] = tmp1;
      z[i+1] = tmp2;
    }
  } /* end of sorting network loop */

  //peakout[i].energy = z[i](15,0) /* bits from 15 to 0 */

  for(i=0; i<NPEAK; i++)
  {
	 //peakout[i] = z[i];
	 peakout[i].energy          = (z[i]>>(ECPEAK0_ENESUM_BITS+ECPEAK0_STRIPN_BITS+ECPEAK0_STRIP1_BITS)) & ECPEAK0_ENERGY_MASK;
	 peakout[i].strip1          = (z[i]>>(ECPEAK0_ENESUM_BITS+ECPEAK0_STRIPN_BITS)) & ECPEAK0_STRIP1_MASK;
	 peakout[i].stripn          = (z[i]>>ECPEAK0_ENESUM_BITS) & ECPEAK0_STRIPN_MASK;
	 peakout[i].energysum4coord = z[i] & ECPEAK0_ENESUM_MASK;
  }

#endif








/* 3.36/97/1/0%/0%/1%/8% - pipeline, all complete */
/* 3.36/97/4/0%/0%/1%/6% - pipeline II=4, all complete */
#if 0
  for(i=0; i<NPEAK; i++)
  {
#pragma HLS UNROLL
    peakout[i] = peakin[i];
    en0[i] = peakin[i].energy;
  }

  for(i=NPEAK; i<NSTRIP; i++)
  {
	peaktmp = peakin[i];
	enn = peakin[i].energy;
	if     ((en0[0]<=en0[1])&&(en0[0]<=en0[2])&&(en0[0]<=en0[3]) && (en0[0] < enn)) {en0[0] = enn; peakout[0] = peaktmp;}
	else if((en0[1]<=en0[0])&&(en0[1]<=en0[2])&&(en0[1]<=en0[3]) && (en0[1] < enn)) {en0[1] = enn; peakout[1] = peaktmp;}
	else if((en0[2]<=en0[0])&&(en0[2]<=en0[1])&&(en0[2]<=en0[3]) && (en0[2] < enn)) {en0[2] = enn; peakout[2] = peaktmp;}
	else if((en0[3]<=en0[0])&&(en0[3]<=en0[1])&&(en0[3]<=en0[2]) && (en0[3] < enn)) {en0[3] = enn; peakout[3] = peaktmp;}
  }

#endif




/* 3.46/97/98/0%/0%/~0%/~0% - no pipeline, all complete */
/* 3.39/65/66/0%/0%/~0%/~0% - no pipeline, all complete, peaktmp in the beginning of the loop */
/* 3.39/143/33/0%/0%/4%/9% - pipeline, all complete, peaktmp in the beginning of the loop */
#if 0
  for(i=0; i<NPEAK; i++)
  {
#pragma HLS UNROLL
    peakout[i] = peakin[i];
    en0[i] = peakin[i].energy;
  }

  for(i=NPEAK; i<NSTRIP; i++)
  {
	peaktmp = peakin[i];
	if     ((en0[0]<=en0[1])&&(en0[0]<=en0[2])&&(en0[0]<=en0[3])) jj=0;
	else if((en0[1]<=en0[0])&&(en0[1]<=en0[2])&&(en0[1]<=en0[3])) jj=1;
	else if((en0[2]<=en0[0])&&(en0[2]<=en0[1])&&(en0[2]<=en0[3])) jj=2;
	else if((en0[3]<=en0[0])&&(en0[3]<=en0[1])&&(en0[3]<=en0[2])) jj=3;
	if(peakout[jj].energy < peaktmp.energy)
    {
	  peakout[jj] = peaktmp;
      en0[jj] = peaktmp.energy;
    }
  }
#endif



/* 3.36/72/72/0%/0%/3%/8% - pipeline, complete peakout */
/* 3.36/69/ 1/0%/0%/4%/13% - pipeline, complete peakin and peakout */
#if 0
  for(i=0; i<NSTRIP; i++)
  {
    en[i]    = peakin[i].energy;
    ind[i]   = i;
  }

  /*bubble*/
  for(i=1; i<NSTRIP; i++)
  {
    for(j=0; j<(NSTRIP-i); j++)
    {
      if (en[j] < en[j+1])
      {
        enswap = en[j];
        en[j]    = en[j+1];
        en[j+1]  = enswap;

        indswap  = ind[j];
        ind[j]   = ind[j+1];
        ind[j+1] = indswap;
      }
    }
  }

  ecpeaksort_label0:for(i=0; i<NPEAK; i++) peakout[i] = peakin[ind[i]];
#endif



/* 2.39/71/18/0%/0%/11%/23% - bubble sort: pipeline, params not complete */
/* 2.31/68/1/0%/0%/11%/24% - bubble sort: pipeline, params complete */
#if 0
  /* bubble sort */
  for(i=0; i<NSTRIP; i++) peak0[i] = peakin[i];

  for(i=1; i<NSTRIP; i++)
  {
    for(j=0; j<(NSTRIP-i); j++)
    {
      if (peak0[j].energy < peak0[j+1].energy)
      {
        peakswap = peak0[j];
        peak0[j]    = peak0[j+1];
        peak0[j+1]  = peakswap;
      }
    }
  }
  for(i=0; i<NPEAK; i++) peakout[i] = peak0[i];
#endif




#ifdef DEBUG
  printf("AFTER:\n");
  for(i=0; i<NPEAK; i++)
  {
    cout<<"peakout["<<i<<"]: energy="<<peakout[i].energy<<", energysum4coord="<<peakout[i].energysum4coord<<", first strip="<<peakout[i].strip1<<", number of strips="<<peakout[i].stripn<<endl;
  }
#endif

  return(0);
}
