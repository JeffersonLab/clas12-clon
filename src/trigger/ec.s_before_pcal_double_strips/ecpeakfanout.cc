
/* ecpeakfanout.cc */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

using namespace std;
#include <strstream>


#ifdef USE_PCAL

#include "../pc.s/pclib.h"
#define ecpeakfanout pcpeakfanout

#else

#include "eclib.h"

#endif

//#define DEBUG


/* 0.0/5/4/0%/0%/(210)~0%/(231)~0% II=4 */

void
ecpeakfanout(hls::stream<ECPeak_s> &s_peak, hls::stream<ECPeak_s> &s_peak1, hls::stream<ECPeak_s> &s_peak2, volatile ap_uint<1> &peak_scaler_inc)
{
#pragma HLS INTERFACE ap_none port=peak_scaler_inc
#pragma HLS DATA_PACK variable=s_peak2
#pragma HLS INTERFACE axis register both port=s_peak2
#pragma HLS DATA_PACK variable=s_peak1
#pragma HLS INTERFACE axis register both port=s_peak1
#pragma HLS DATA_PACK variable=s_peak
#pragma HLS INTERFACE axis register both port=s_peak
#pragma HLS PIPELINE II=4

  ECPeak_s fifo;

  /*
  hls::stream<ECPeak_s> s_peak2_fifo;
#ifdef USE_PCAL
#pragma HLS STREAM variable=s_peak2_fifo depth=32 dim=1
#else
#pragma HLS STREAM variable=s_peak2_fifo depth=16 dim=1
#endif
  */

  for(int i=0; i<NPEAK; i++)
  {
	fifo = s_peak.read();

	if(fifo.energy) peak_scaler_inc = 1;
	else            peak_scaler_inc = 0;

    s_peak1.write(fifo);

	/*
	s_peak2_fifo.write(fifo);
	fifo = s_peak2_fifo.read();
	*/
	s_peak2.write(fifo);
  }

}


