
/* ecenergyfanout.c */

	


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include <iostream>
using namespace std;



#ifdef USE_PCAL

#include "../pc.s/pclib.h"
#define ecenergyfanout pcenergyfanout

#else

#include "eclib.h"

#endif


//#define DEBUG




#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)
#define ABS(x)      ((x) < 0 ? -(x) : (x))



/*xc7vx550tffg1158-1*/



/* 0.0/5/4/0%/0%/(4806)~0%/(1889)~0% II=4 */

void
ecenergyfanout(hls::stream<ECStream16_s> s_energy[NH_FIFOS], hls::stream<ECStream16_s> s_energy1[NH_FIFOS], hls::stream<ECStream16_s> s_energy2[NH_FIFOS])
{
#pragma HLS DATA_PACK variable=s_energy2
#pragma HLS INTERFACE axis register both port=s_energy2
#pragma HLS DATA_PACK variable=s_energy1
#pragma HLS INTERFACE axis register both port=s_energy1
#pragma HLS DATA_PACK variable=s_energy
#pragma HLS INTERFACE axis register both port=s_energy
#pragma HLS ARRAY_PARTITION variable=s_energy complete dim=1
#pragma HLS ARRAY_PARTITION variable=s_energy1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=s_energy2 complete dim=1
#pragma HLS PIPELINE II=4

  ECStream16_s fifo;

//  hls::stream<ECStream16_s> s_energy1_fifo[NH_FIFOS]; /* depth: ecfrac1+ecfrac2+ecfrac3+eccorr=8+9+8=25*/
//#pragma HLS STREAM variable=s_energy1_fifo depth=256 dim=1
  hls::stream<ECStream16_s> s_energy2_fifo[NH_FIFOS]; /* depth: ecfrac1+ecfrac2+ecfrac3+eccorr=8+9+8=25*/
#pragma HLS STREAM variable=s_energy2_fifo depth=32 dim=1

  for(int i=0; i<NH_FIFOS; i++)
  {
    for(int j=0; j<NH_READS; j++)
    {
      fifo = s_energy[i].read();

      s_energy1[i].write(fifo);
//	  s_energy1_fifo[i].write(fifo);
//	  if(!s_energy1[i].full() && !s_energy1_fifo[i].empty())
//        s_energy1[i].write(s_energy1_fifo[i].read());

	  s_energy2_fifo[i].write(fifo);
	  if(!s_energy2[i].full() && !s_energy2_fifo[i].empty())
        s_energy2[i].write(s_energy2_fifo[i].read());
    }
  }

}
