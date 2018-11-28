
/* ecpeakzerosuppress.cc */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include <iostream>
using namespace std;


#ifdef USE_PCAL

#include "../pc.s/pclib.h"
#define ecpeakzerosuppress pcpeakzerosuppress

#else

#include "eclib.h"

#endif


//#define DEBUG

/* 3.36/7/4/0%/0%/~0%(2654)/~0%(1581) II=4 */

void
ecpeakzerosuppress(hls::stream<ECPeak0_s> s_peak0strip[NF2], hls::stream<ECPeak0_s> s_peak0max[NF3])
{
#pragma HLS DATA_PACK variable=s_peak0max
#pragma HLS INTERFACE axis register both port=s_peak0max
#pragma HLS DATA_PACK variable=s_peak0strip
#pragma HLS INTERFACE axis register both port=s_peak0strip
#pragma HLS ARRAY_PARTITION variable=s_peak0strip complete dim=1
#pragma HLS ARRAY_PARTITION variable=s_peak0max complete dim=1
#pragma HLS PIPELINE II=4

  int ii;

  ECPeak0 peakin[NSTRIP];
#pragma HLS ARRAY_PARTITION variable=peakin complete dim=1
  ECPeak0 peakout[NPEAKMAX];
#pragma HLS ARRAY_PARTITION variable=peakout complete dim=1

  ECPeak0_s fifo[NF2*NH_READS];
#pragma HLS ARRAY_PARTITION variable=fifo complete dim=1



  for(int j=0; j<NH_READS; j++)
  {
    for(int i=0; i<NF2; i++)
    {
	  fifo[j*NF2+i] = s_peak0strip[i].read();
    }
  }


  for(int i=0; i<NSTRIP; i++)
  {
    peakin[i].energy = fifo[i].energy;
    peakin[i].energysum4coord = fifo[i].energysum4coord;
    peakin[i].strip1 = fifo[i].strip1;
    peakin[i].stripn = fifo[i].stripn;
  }


#ifdef DEBUG
  printf("\n\n++ ecpeakzerosuppress ++\n");
  printf("BEFOR:\n");
  for(ii=0; ii<NSTRIP; ii++)
  {
    cout<<"peakin["<<ii<<"]: energy="<<peakin[ii].energy<<", energysum4coord="<<peakin[ii].energysum4coord<<", first strip="<<peakin[ii].strip1<<", number of strips="<<peakin[ii].stripn<<endl;
  }
#endif

  for(ii=0; ii<NPEAKMAX; ii++)
  {
    if(peakin[ii*2].energy>0)        peakout[ii] = peakin[ii*2];
    else if(peakin[ii*2+1].energy>0) peakout[ii] = peakin[ii*2+1];
    else                             peakout[ii].energy = 0;
  }

#ifdef DEBUG
  printf("AFTER:\n");
  for(ii=0; ii<NPEAKMAX; ii++)
  {
    cout<<"peakin["<<ii<<"]: energy="<<peakin[ii].energy<<", energysum4coord="<<peakin[ii].energysum4coord<<", first strip="<<peakin[ii].strip1<<", number of strips="<<peakin[ii].stripn<<endl;
  }
#endif


  ECPeak0_s fifo1[NF3*NH_READS];
#pragma HLS ARRAY_PARTITION variable=fifo1 complete dim=1

  for(int i=0; i<NPEAKMAX; i++)
  {
    fifo1[i].energy = peakout[i].energy;
    fifo1[i].energysum4coord = peakout[i].energysum4coord;
    fifo1[i].strip1 = peakout[i].strip1;
    fifo1[i].stripn = peakout[i].stripn;
  }

  for(int i=0; i<NF3; i++)
  {
    for(int j=0; j<NH_READS; j++)
    {
	  s_peak0max[i].write(fifo1[i*NH_READS+j]);
    }
  }


}
