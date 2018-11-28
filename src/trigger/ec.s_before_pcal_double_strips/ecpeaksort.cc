
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
#define ecpeaksort_2words pcpeaksort_2words
#define ecpeaksort_2slices pcpeaksort_2slices

#else

#include "eclib.h"

#endif


//#define DEBUG




/* 2.31/0/1/0/0/0/120 II=1, z complete */
void
ecpeaksort_2words(ap_uint<ECPEAK0_BITS> z[2])
{
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION variable=z complete dim=1
#pragma HLS PIPELINE

  ap_uint<ECPEAK0_ENERGY_BITS> test1, test2;
  ap_uint<ECPEAK0_BITS> tmp1, tmp2;

  test1 = z[0](ECPEAK0_ENERGY-1, 0);
  test2 = z[1](ECPEAK0_ENERGY-1, 0);
  tmp1 = ((test1 > test2) ? z[0] : z[1]);
  tmp2 = ((test1 < test2) ? z[0] : z[1]);
  z[0] = tmp1;
  z[1] = tmp2;
}



/* 4.62/0/1/0/0/   0/4200 II=1, z complete */
void
ecpeaksort_2slices(ap_uint<ECPEAK0_BITS> z[NPEAKMAX])
{
#pragma HLS ARRAY_PARTITION variable=z complete dim=1
#pragma HLS PIPELINE

  ap_uint<ECPEAK0_BITS> zz[2];

  for(int i=0; i<NPEAKMAX-1; i=i+2)
  {
	zz[0] = z[i];
    zz[1] = z[i+1];
 	ecpeaksort_2words(zz);
 	z[i] = zz[0];
    z[i+1] = zz[1];
  }

  for(int i=1; i<NPEAKMAX-1; i=i+2)
  {
	zz[0] = z[i];
	zz[1] = z[i+1];
	ecpeaksort_2words(zz);
 	z[i] = zz[0];
    z[i+1] = zz[1];
  }

}

#define SORTING(I1,I2) \
  { \
    ap_uint<ECPEAK0_BITS> zz[2]; \
    for(int i=I1; i<=I2; i=i+2) \
    { \
      zz[0] = z[i]; \
      zz[1] = z[i+1]; \
      ecpeaksort_2words(zz); \
      z[i] = zz[0]; \
      z[i+1] = zz[1]; \
    } \
  }


/*xc7vx550tffg1158-1*/


/* 2.62/27/4/0%/0%/(5195)~0%/(4719)1% II=4 */

void
ecpeaksort(hls::stream<ECPeak0_s> s_peak0max[NF3], hls::stream<ECPeak0_s> &s_peak0)
{
#pragma HLS DATA_PACK variable=s_peak0
#pragma HLS INTERFACE axis register both port=s_peak0
#pragma HLS DATA_PACK variable=s_peak0max
#pragma HLS INTERFACE axis register both port=s_peak0max
#pragma HLS ARRAY_PARTITION variable=s_peak0max complete dim=1
#pragma HLS PIPELINE II=4

  int i, stage;

  ECPeak0 peakin[NPEAKMAX];
#pragma HLS ARRAY_PARTITION variable=peakin complete dim=1
  ECPeak0 peakout[NPEAK];
#pragma HLS ARRAY_PARTITION variable=peakout complete dim=1

  ap_uint<ECPEAK0_BITS> z[NPEAKMAX];
#pragma HLS ARRAY_PARTITION variable=z complete dim=1


  ECPeak0_s fifo[NF3*NH_READS];
#pragma HLS ARRAY_PARTITION variable=fifo complete dim=1

  for(int i=0; i<NF3; i++)
  {
    for(int j=0; j<NH_READS; j++)
    {
	  fifo[i*NH_READS+j] = s_peak0max[i].read();
    }
  }

  for(int i=0; i<NPEAKMAX; i++)
  {
    peakin[i].energy = fifo[i].energy;
    peakin[i].energysum4coord = fifo[i].energysum4coord;
    peakin[i].strip1 = fifo[i].strip1;
    peakin[i].stripn = fifo[i].stripn;
  }


#ifdef DEBUG
  printf("\n\n++ ecpeaksort ++\n");
  printf("BEFOR:\n");
  for(i=0; i<NPEAKMAX; i++)
  {
    cout<<"peakin["<<i<<"]: energy="<<peakin[i].energy<<", energysum4coord="<<peakin[i].energysum4coord<<", first strip="<<peakin[i].strip1<<", number of strips="<<peakin[i].stripn<<endl;
  }
#endif



#if 0
  /******************************************************/
  /* selecting - something is wrong, need to be fixed ! */
  /* 3.36/ 97/1/0/0/9574/31583 (1%/9%) II=1 */
  /* 3.36/ 97/2/0/0/14781/24710(2%/7%) II=2 */
  /* 3.36/ 97/8/0/0/9114/23344(1%/6%) II=8 */

  /* selecting structure array */
  {
    ap_uint<16> enn, en0[NPEAK];
    ECPeak0 peaktmp;

    for(i=0; i<NPEAK; i++)
    {
      peakout[i] = peakin[i];
      en0[i] = peakin[i].energy;
    }

    for(i=NPEAK; i<NPEAKMAX; i++)
    {
        peaktmp = peakin[i];
        enn = peakin[i].energy;
        if((en0[0]<=en0[1])&&(en0[0]<=en0[2])&&(en0[0]<=en0[3]) && (en0[0] < enn)) {en0[0] = enn; peakout[0] = peaktmp;}
        else if((en0[1]<=en0[0])&&(en0[1]<=en0[2])&&(en0[1]<=en0[3]) && (en0[1] < enn)) {en0[1] = enn; peakout[1] = peaktmp;}
        else if((en0[2]<=en0[0])&&(en0[2]<=en0[1])&&(en0[2]<=en0[3]) && (en0[2] < enn)) {en0[2] = enn; peakout[2] = peaktmp;}
        else if((en0[3]<=en0[0])&&(en0[3]<=en0[1])&&(en0[3]<=en0[2]) && (en0[3] < enn)) {en0[3] = enn; peakout[3] = peaktmp;}
    }
  }
  /* end of selecting                                   */
  /******************************************************/
#endif



#if 1
  /*********************/
  /* 'sorting network' */

  /* 2.31/35/1/0/0/68076/75600(9%/21%) II=1 */
  /* 3.02/36/2/0/0/35013/38747(5%/11%) II=2 */
  /* 3.95/36/8/0/0/11677/18438(1%/5%) II=8 */

  /* copy input data from input array of structures to local array */
  for(i=0; i<NPEAKMAX; i++)
  {
	z[i] = (peakin[i].energysum4coord, peakin[i].stripn, peakin[i].strip1, peakin[i].energy);
  }

 /* 'sorting network' loop */

/*
  for(stage=0; stage<(NPEAKMAX/2); stage++)
  {
	ecpeaksort_2slices(z);
  }
*/



#ifdef USE_PCAL // NPEAKMAX=42

  SORTING(0,40);
  SORTING(1,39);
  SORTING(0,40);
  SORTING(1,39);

  SORTING(0,38);
  SORTING(1,37);

  SORTING(0,36);
  SORTING(1,35);

  SORTING(0,34);
  SORTING(1,33);

  SORTING(0,32);
  SORTING(1,31);

  SORTING(0,30);
  SORTING(1,29);

  SORTING(0,28);
  SORTING(1,27);

  SORTING(0,26);
  SORTING(1,25);

  SORTING(0,24);
  SORTING(1,23);

  SORTING(0,22);
  SORTING(1,21);

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

#else // NPEAKMAX=18

 
//#ifndef __SYNTHESIS__ /* sergey: TEMPORARY !!! */

  SORTING(0,16);
  SORTING(1,15);
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

//#endif /* #ifndef __SYNTHESIS__ */

#endif



  /* end of 'sorting network' loop */

  /* extract data from sorted array to output array of structures */
  for(i=0; i<NPEAK; i++)
  {
	 peakout[i].energy          = z[i](ECPEAK0_ENERGY-1, 0);
	 peakout[i].strip1          = z[i](ECPEAK0_STRIP1-1, ECPEAK0_ENERGY);
	 peakout[i].stripn          = z[i](ECPEAK0_STRIPN-1, ECPEAK0_STRIP1);
	 peakout[i].energysum4coord = z[i](ECPEAK0_ENESUM-1, ECPEAK0_STRIPN);
  }

  /* end of 'sorting network' */
  /****************************/
#endif




#ifdef DEBUG
  printf("AFTER:\n");
  for(i=0; i<NPEAK; i++)
  {
    cout<<"peakout["<<i<<"]: energy="<<peakout[i].energy<<", energysum4coord="<<peakout[i].energysum4coord<<", first strip="<<peakout[i].strip1<<", number of strips="<<peakout[i].stripn<<endl;
  }
#endif


  ECPeak0_s fifo1[NPEAK];
#pragma HLS ARRAY_PARTITION variable=fifo1 complete dim=1


  for(int i=0; i<NPEAK; i++)
  {
    fifo1[i].energy = peakout[i].energy;
    fifo1[i].energysum4coord = peakout[i].energysum4coord;
    fifo1[i].strip1 = peakout[i].strip1;
    fifo1[i].stripn = peakout[i].stripn;
  }


  for(int j=0; j<NPEAK; j++)
  {
	s_peak0.write(fifo1[j]);
  }

}

