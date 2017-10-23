
/* ftofstrips.cc */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

//#define DEBUG

#ifdef DEBUG
#include <iostream>
using namespace std;
#endif


#include "ftoftrans.h"
#include "ftoflib.h"



/* 1.96/16/8/0%/0%/~0%(638)/~0%(519) II=8 */

/* reads one 32-ns timing slice */
void
ftofstrips(ap_uint<16> strip_threshold, hls::stream<fadc_16ch_t> s_fadc_words[NFADCS], hls::stream<FTOFStrip_s> s_strip[NH_READS])
{
#pragma HLS INTERFACE ap_stable port=strip_threshold
#pragma HLS DATA_PACK variable=s_fadc_words
#pragma HLS INTERFACE axis register both port=s_fadc_words
#pragma HLS ARRAY_PARTITION variable=s_fadc_words complete dim=1
#pragma HLS DATA_PACK variable=s_strip
#pragma HLS INTERFACE axis register both port=s_strip
#pragma HLS ARRAY_PARTITION variable=s_strip complete dim=1
#pragma HLS PIPELINE II=1

  ap_uint<13> energy;
  ap_uint<4> chan; /*0-15*/
  ap_uint<2> lay; /*0-2*/
  ap_uint<6> str; /*0-61*/
  ap_uint<1> lr; /*0-1*/

  fadc_16ch_t fadcs;

  ap_uint<4> timetmp; /* 3 low bits for time interval (0-7), high bit to mark missing hits */

  ap_uint<4> time[2][NSTRIP];
#pragma HLS ARRAY_PARTITION variable=time complete dim=1
#pragma HLS ARRAY_PARTITION variable=time complete dim=2
  ap_uint<NSTRIP> out[2];
#pragma HLS ARRAY_PARTITION variable=out complete dim=1



#ifdef DEBUG
  printf("== ftofstrips starts ==\n");fflush(stdout);
#endif


  for(int i=0; i<2; i++)
  {
    for(int j=0; j<NSTRIP; j++) time[i][j] = 0;
    out[i] = 0;
  }



  /********************************************************************/
  /* get FADC data for 32-ns interval, and fill time[2][NSTRIP] array */

  for(int isl=0; isl<NFADCS; isl++)
  {
    /* read one timing slice */
	for(int j=0; j<NH_READS; j++)
	{
	  fadcs = s_fadc_words[isl].read();
#ifdef DEBUG
      if(fadcs.e0>0 || fadcs.e1>0) cout<<"fadcs[slot="<<isl<<"][read="<<j<<"]="<<fadcs.e0<<" "<<fadcs.e1<<endl;
#endif

      energy = fadcs.e0;
      chan   = j*2;
      lay    = adclayer[isl][chan] - 1;
      str    = adcstrip[isl][chan] - 1;
      lr     = adclr[isl][chan] - 1;
	  //printf("isl=%d chan=%d -> str=%d\n",isl,(uint16_t)chan,(uint16_t)str);
      timetmp = ((energy >= strip_threshold) ? ((uint8_t)fadcs.t0) : 8); /* error in '?' without (uint16_t) ...*/
#ifdef DEBUG
      if(timetmp<8) cout<<"   str="<<str<<" -> t0="<<timetmp<<endl;
#endif
      if(lay==0) time[lr][str] = timetmp;

      energy = fadcs.e1;
      chan   = j*2+1;
      lay    = adclayer[isl][chan] - 1;
      str    = adcstrip[isl][chan] - 1;
      lr     = adclr[isl][chan] - 1;
	  //printf("isl=%d chan=%d -> str=%d\n",isl,(uint16_t)chan,(uint16_t)str);
      timetmp = ((energy >= strip_threshold) ? ((uint8_t)fadcs.t1) : 8);
#ifdef DEBUG
      if(timetmp<8) cout<<"   str="<<str<<" -> t0="<<timetmp<<endl;
#endif
      if(lay==0) time[lr][str] = timetmp;
	}
  }

  /************************************/
  /************************************/



  /* filling 4ns-slices using 3 timing bits, and send it over - one slice per stream, all in parallel - we are 32ns domain ! */

  FTOFStrip_s fifo;

  for(int j=0; j<NH_READS; j++)
  {
    for(int k=0; k<NLR; k++)
    {
      for(int i=0; i<NSTRIP; i++)
      {
        if(time[k][i]==j) out[k][i] = 1;
        else              out[k][i] = 0;
      }
    }

    fifo.outL = out[0];
    fifo.outR = out[1];
    s_strip[j].write(fifo);
  }



#ifdef DEBUG
  printf("== ftofstrips ends ==\n");fflush(stdout);
#endif

}
