/* ecfrac1.c - energy fractions calculation

  input:  

  output: 
*/


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>


//#define DEBUG

#ifdef DEBUG
using namespace std;
#include <strstream>
#endif

#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)
#define ABS(x)      ((x) < 0 ? -(x) : (x))


#ifdef USE_PCAL

#include "../pc.s/pclib.h"

#define ecfrac1 pcfrac1
#define ecfrac1_1 pcfrac1_1

#else

#include "eclib.h"

#endif



#include <ap_fixed.h>



/*xc7vx550tffg1158-1*/



/* 3.23/4/2/0%/0%/~0%/~0% II=1*/

inline void
ecfrac1_1(ECPeakCount peakcount[3], ap_uint<16> energy[3], hitsume_t &hitouttmp1)
{
#pragma HLS ARRAY_PARTITION variable=peakcount complete dim=1
#pragma HLS ARRAY_PARTITION variable=energy complete dim=1
#pragma HLS PIPELINE II=1

  ap_uint<16> enU, enV, enW;
  ap_uint<2> Nvalid;
  ap_uint<19> ensumE; /* contains sum of three 16-bit energies, multiplied by 2 - four extra bits */

#ifdef DEBUG
  cout <<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!ecfrac1!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<< endl;
  cout <<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!ecfrac1!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<< endl;
#endif


  if( (energy[0]==0) || ((peakcount[0]==1)&&(peakcount[1]==1)&&(peakcount[2]==1)) )
  {
    hitouttmp1.sumE = 0;
    for(int i=0; i<3; i++) hitouttmp1.peak_sumE[i] = 0;
  }
  else
  {

#ifdef DEBUG
    cout<<" energies="<<energy[0]<<" "<<energy[1]<<" "<<energy[2];
    cout<<" peakcount="<<peakcount[0]<<" "<<peakcount[1]<<" "<<peakcount[2] << endl;
#endif


    /* for current hit, get energies for unique peaks only */
    enU = (peakcount[0]==1) ? energy[0] : (ap_uint<16>)0;
    enV = (peakcount[1]==1) ? energy[1] : (ap_uint<16>)0;
    enW = (peakcount[2]==1) ? energy[2] : (ap_uint<16>)0;





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
      hitouttmp1.sumE = ensumE;


      /* insert 'UNIQUE energy sum' ensumE into hitouttmp1[] only for NOT UNIQUE peaks participating in this hit */
      /* so we are summing/normalizing energies from unique peaks, and placing it into NOT-unique peak for following processing */
	  hitouttmp1.peak_sumE[0] = (enU==0) ? ensumE : (ap_uint<19>)0;
	  hitouttmp1.peak_sumE[1] = (enV==0) ? ensumE : (ap_uint<19>)0;
	  hitouttmp1.peak_sumE[2] = (enW==0) ? ensumE : (ap_uint<19>)0;



#ifdef DEBUG
      cout<<"   !!! enU="<<enU<<" enV="<<enV<<" enW="<<enW<<" -> Nvalid="<<+Nvalid<<" -> hitouttmp1.sumE="<<hitouttmp1.sumE<<endl;
      cout<<endl;
#endif

    }
    else
	{
      hitouttmp1.sumE = 0;
      for(int i=0; i<3; i++) hitouttmp1.peak_sumE[i] = 0;
	}

  }

#ifdef DEBUG
  cout <<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!ecfrac1!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<< endl;
  cout <<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!ecfrac1!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<< endl;
#endif

  return;
}






/* 5.0/8/4/0%/0%/(10119)1%/(8959)2% II=4 */

void
ecfrac1(hls::stream<ECStream6_s> s_pcount[NH_FIFOS], hls::stream<ECStream16_s> s_energy1[NH_FIFOS], hls::stream<hitsume_t> s_hitout1[NH_FIFOS])
{
#pragma HLS INTERFACE axis register both port=s_hitout1
#pragma HLS DATA_PACK variable=s_hitout1
#pragma HLS INTERFACE axis register both port=s_energy1
#pragma HLS DATA_PACK variable=s_energy1
#pragma HLS DATA_PACK variable=s_pcount
#pragma HLS INTERFACE axis register both port=s_pcount
#pragma HLS ARRAY_PARTITION variable=s_pcount complete dim=1
#pragma HLS ARRAY_PARTITION variable=s_energy1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=s_hitout1 complete dim=1
#pragma HLS PIPELINE II=4

  ECPeakCount peakcount[3];
#pragma HLS ARRAY_PARTITION variable=peakcount complete dim=1
  ECStream6_s pc_fifo;

  ap_uint<16> energy[3];
#pragma HLS ARRAY_PARTITION variable=energy complete dim=1
  ECStream16_s en_fifo;

  hitsume_t hitouttmp1;

#ifdef DEBUG
  cout <<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!ecfrac1!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<< endl;
  cout <<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!ecfrac1!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<< endl;
#endif


  for(int i=0; i<NH_FIFOS; i++)
  {
    for(int j=0; j<NH_READS; j++)
    {
      pc_fifo = s_pcount[i].read();
      for(int k=0; k<3; k++)
	  {
        peakcount[k] = pc_fifo.word6[k];
	  }

      en_fifo = s_energy1[i].read();
      for(int k=0; k<3; k++)
      {
    	energy[k] = en_fifo.word16[k];
      }

      ecfrac1_1(peakcount, energy, hitouttmp1);

      s_hitout1[i].write(hitouttmp1);
    }
  }


#ifdef DEBUG
  cout <<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!ecfrac1!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<< endl;
  cout <<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!ecfrac1!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<< endl;
#endif

  return;
}
