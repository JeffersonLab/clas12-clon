
/* ecstripspersistence.c */

	


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include <iostream>
using namespace std;


#ifdef USE_PCAL

#include "../pc.s/pclib.h"
#define ecstripspersistence pcstripspersistence
#define ecstripspersistence0 pcstripspersistence0
#define ecstripspersistence1 pcstripspersistence1
#define ecstripspersistence2 pcstripspersistence2

#else

#include "eclib.h"

#endif


//#define DEBUG


/*xc7vx550tffg1158-1*/



/* 4.77/6/4/0%/0%/(9086)1%/(4302)1% II=4 */

/* nframes from 0 to (NPER-1) */
void
ecstripspersistence(nframe_t nframes, hls::stream<ECStrip_s> s_stripin[NF1], hls::stream<ECStrip_s> s_stripout[NF1])
{
#pragma HLS DATA_PACK variable=s_stripout
#pragma HLS INTERFACE axis register both port=s_stripout
#pragma HLS DATA_PACK variable=s_stripin
#pragma HLS INTERFACE axis register both port=s_stripin
#pragma HLS INTERFACE ap_stable register port=nframes
#pragma HLS ARRAY_PARTITION variable=s_stripin complete dim=1
#pragma HLS ARRAY_PARTITION variable=s_stripout complete dim=1
#pragma HLS PIPELINE II=4

  ECStrip_s fifosum;

  static ECStrip_s fifo[NPER][NF1][NH_READS];
#pragma HLS ARRAY_PARTITION variable=fifo complete dim=1
#pragma HLS ARRAY_PARTITION variable=fifo complete dim=2
#pragma HLS ARRAY_PARTITION variable=fifo complete dim=3

#ifdef DEBUG
  printf("=== ecstripspersistence starts ===\n");
#endif

  for(int j=0; j<NH_READS; j++)
  {
    for(int i=0; i<NF1; i++)
    {
	  for(int k=(NPER-1); k>0; k--)
	  {
		fifo[k][i][j].energy0 = fifo[k-1][i][j].energy0;
		fifo[k][i][j].energy1 = fifo[k-1][i][j].energy1;
	  }
	  fifo[0][i][j] = s_stripin[i].read();
#ifdef DEBUG
      cout<<"s_stripin["<<i<<"]="<<fifo[0][i][j].energy0<<" "<<fifo[0][i][j].energy1<<endl;
#endif
	}
  }

  for(int j=0; j<NH_READS; j++)
  {
    for(int i=0; i<NF1; i++)
    {
      fifosum.energy0 = 0;
      fifosum.energy1 = 0;
      for(int k=0; k<NPER; k++)
	  {
    	if(k<=nframes)
    	{
          fifosum.energy0 += fifo[k][i][j].energy0;
          fifosum.energy1 += fifo[k][i][j].energy1;
    	}
	  }
	  s_stripout[i].write(fifosum);
	}
  }


  /*
  for(int j=0; j<NH_READS; j++)
  {
    for(int i=0; i<NF1; i++)
    {
	  fifosum = s_stripin[i].read();
      s_stripout[i].write(fifosum);
	}
  }
  */

#ifdef DEBUG
  printf("=== ecstripspersistence ends ===\n");
#endif

}



#ifndef __SYNTHESIS__

#define ECSTRIPSPERSISTENCE \
  ECStrip_s fifosum; \
  static ECStrip_s fifo[NPER][NF1][NH_READS]; \
  for(int j=0; j<NH_READS; j++) \
  { \
    for(int i=0; i<NF1; i++) \
    { \
	  for(int k=(NPER-1); k>0; k--) \
	  { \
		fifo[k][i][j].energy0 = fifo[k-1][i][j].energy0; \
		fifo[k][i][j].energy1 = fifo[k-1][i][j].energy1; \
	  } \
	  fifo[0][i][j] = s_stripin[i].read(); \
	} \
  } \
  for(int j=0; j<NH_READS; j++) \
  { \
    for(int i=0; i<NF1; i++) \
    { \
      fifosum.energy0 = 0; \
      fifosum.energy1 = 0; \
      for(int k=0; k<NPER; k++) \
	  { \
    	if(k<=nframes) \
    	{ \
          fifosum.energy0 += fifo[k][i][j].energy0; \
          fifosum.energy1 += fifo[k][i][j].energy1; \
    	} \
	  } \
	  s_stripout[i].write(fifosum); \
	} \
  }





void
ecstripspersistence0(nframe_t nframes, hls::stream<ECStrip_s> s_stripin[NF1], hls::stream<ECStrip_s> s_stripout[NF1])
{
  ECSTRIPSPERSISTENCE;
}
void
ecstripspersistence1(nframe_t nframes, hls::stream<ECStrip_s> s_stripin[NF1], hls::stream<ECStrip_s> s_stripout[NF1])
{
  ECSTRIPSPERSISTENCE;
}
void
ecstripspersistence2(nframe_t nframes, hls::stream<ECStrip_s> s_stripin[NF1], hls::stream<ECStrip_s> s_stripout[NF1])
{
  ECSTRIPSPERSISTENCE;
}
#endif
