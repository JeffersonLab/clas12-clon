
/* ftofstripspersistence.c */

	


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include <iostream>
using namespace std;



#include "ftoflib.h"



//#define DEBUG


/*xc7vx550tffg1158-1*/



/* 4.77/6/4/0%/0%/(9086)1%/(4302)1% II=4 */

/* nframes from 0 to (NPER-1) */


void
ftofstripspersistence(nframe_t nframes, hls::stream<FTOFStrip_s> &s_stripin, hls::stream<FTOFStrip_s> &s_stripout, ap_uint<3> jj)
{
#pragma HLS INTERFACE ap_stable register port=nframes
#pragma HLS DATA_PACK variable=s_stripin
#pragma HLS INTERFACE axis register both port=s_stripin
#pragma HLS DATA_PACK variable=s_stripout
#pragma HLS INTERFACE axis register both port=s_stripout
#pragma HLS PIPELINE II=1

  FTOFStrip_s fifosum;

  static FTOFStrip_s fifo[NPER];
#pragma HLS ARRAY_PARTITION variable=fifo complete dim=1

#ifdef DEBUG
  printf("=== ftofstripspersistence starts ===\n");
#endif


    for(int k=(NPER-1); k>0; k--)
	{
	  fifo[k].outL = fifo[k-1].outL;
	  fifo[k].outR = fifo[k-1].outR;
	}

    fifo[0] = s_stripin.read();

    fifosum.outL = 0;
    fifosum.outR = 0;
    for(int k=0; k<NPER; k++)
	{
      if(k<=nframes)
      {
        fifosum.outL += fifo[k].outL;
        fifosum.outR += fifo[k].outR;
      }
	}
	s_stripout.write(fifosum);


#ifdef DEBUG
  printf("=== ftofstripspersistence ends ===\n");
#endif

}
