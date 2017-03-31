/* ecfrac1.c - energy fractions calculation

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

#define ecfrac1 pcfrac1

#else

#include "eclib.h"

#endif


//#define DEBUG

#include <ap_fixed.h>



/*xc7vx550tffg1158-1*/



/* 3.23/4/2/0%/0%/~0%/5% */



void
ecfrac1(fp0201S_t peakcount[3][NPEAK], ap_uint<16> energy[NHITMAX][3], hitsume_t hitouttmp1[NHITMAX])
{
#pragma HLS ARRAY_PARTITION variable=peakcount complete dim=1
#pragma HLS ARRAY_PARTITION variable=peakcount complete dim=2
#pragma HLS ARRAY_PARTITION variable=energy complete dim=1
#pragma HLS ARRAY_PARTITION variable=energy complete dim=2
#pragma HLS ARRAY_PARTITION variable=hitouttmp1 complete dim=1
#pragma HLS PIPELINE

  uint8_t i,u,v,w,ind;
  uint16_t enU, enV, enW;
  uint8_t Nvalid;
  ap_uint<19> ensumE; /* contains sum of three 16-bit energies, multiplied by 2 - four extra bits */

#ifdef DEBUG
  cout <<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!ecfrac1!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<< endl;
  cout <<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!ecfrac1!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<< endl;
#endif


  for(ind=0; ind<NHITMAX; ind++)
  {
    u = U4(ind);
    v = V4(ind);
    w = W4(ind);

    if( (energy[ind][0]==0) || ((peakcount[0][u]==1)&&(peakcount[1][v]==1)&&(peakcount[2][w]==1)) )
    {
#ifdef DEBUG
      cout<<"!!! hitin[peakU="<<+u<<"][peakV="<<+v<<"][peakW="<<+w<<"] empty" << endl;
#endif
      hitouttmp1[ind].sumE = 0;
      for(i=0; i<3; i++) hitouttmp1[ind].peak_sumE[i] = 0;
    }
    else
	{

#ifdef DEBUG
      cout<<"!!! hitin[peakU="<<+u<<"][peakV="<<+v<<"][peakW="<<+w<<"]";
      cout<<" energies="<<energy[ind][0]<<" "<<energy[ind][1]<<" "<<energy[ind][2];
      cout<<" peakcount="<<peakcount[0][u]<<" "<<peakcount[1][v]<<" "<<peakcount[2][w] << endl;
#endif


      /* for current hit, get energies for unique peaks only */
      if(peakcount[0][u]==1) enU = energy[ind][0];
      else                   enU = 0;
	  if(peakcount[1][v]==1) enV = energy[ind][1];
      else                   enV = 0;
	  if(peakcount[2][w]==1) enW = energy[ind][2];
      else                   enW = 0;






      /* normalize and get 'UNIQUE energy sum' ensumE */
      Nvalid=0;
      if(enU>0) Nvalid++;
      if(enV>0) Nvalid++;
      if(enW>0) Nvalid++;
      ensumE = enU + enV + enW;
      if(Nvalid == 1) ensumE = ensumE<<1; /* multiply it by 2, instead of dividing next by 2, will divide one to another later so what important is ratio (?!) */
      /*else if(Nvalid == 2) ensumE = ensumE/2;*/
      /*else if(Nvalid == 3) {printf("3.0 should never be here !!!\n"); exit(0);}*/


	  /* if it was any UNIQUEs, fill output array(s) */
      if(ensumE>0)
      {

        /* attach 'UNIQUE energy sum' ensumE to the hit to be used in stage 3 of this function (DELIMOE) */
        hitouttmp1[ind].sumE = ensumE;


        /* insert 'UNIQUE energy sum' ensumE into hitouttmp1[] only for NOT UNIQUE peaks participating in this hit */
        /* so we are summing/normalizing energies from unique peaks, and placing it into NOT-unique peak for following processing */
        if(enU==0) hitouttmp1[ind].peak_sumE[0] = ensumE;
        else       hitouttmp1[ind].peak_sumE[0] = 0;

        if(enV==0) hitouttmp1[ind].peak_sumE[1] = ensumE;
        else       hitouttmp1[ind].peak_sumE[1] = 0;

        if(enW==0) hitouttmp1[ind].peak_sumE[2] = ensumE;
        else       hitouttmp1[ind].peak_sumE[2] = 0;




#ifdef DEBUG
        cout<<"   !!! enU="<<enU<<" enV="<<enV<<" enW="<<enW<<" -> Nvalid="<<+Nvalid<<" -> hitouttmp1["<<+ind<<"].sumE="<<hitouttmp1[ind].sumE<<endl;
        cout<<endl;
#endif

      }
      else
	  {
        hitouttmp1[ind].sumE = 0;
        for(i=0; i<3; i++) hitouttmp1[ind].peak_sumE[i] = 0;
	  }

	}
  }
#ifdef DEBUG
  cout <<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!ecfrac1!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<< endl;
  cout <<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!ecfrac1!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<< endl;
#endif

  return;
}
