
/* eccoordfanout.c */

	


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>




#ifdef USE_PCAL

#include "../pc.s/pclib.h"
#define eccoordfanout pccoordfanout

#else

#include "eclib.h"

#endif


//#define DEBUG

#ifdef DEBUG
#include <iostream>
using namespace std;
#endif


#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)
#define ABS(x)      ((x) < 0 ? -(x) : (x))



/*xc7vx550tffg1158-1*/



/* 0.0/5/4/0%/0%/(3270)~0%/(2753)~0% II=4*/

void
eccoordfanout(hls::stream<ECStream10_s> s_coord[NH_FIFOS], hls::stream<ECStream10_s> s_coord1[NH_FIFOS], hls::stream<ECStream10_s> s_coord2[NH_FIFOS])
{
#pragma HLS DATA_PACK variable=s_coord2
#pragma HLS INTERFACE axis register both port=s_coord2
#pragma HLS DATA_PACK variable=s_coord1
#pragma HLS INTERFACE axis register both port=s_coord1
#pragma HLS DATA_PACK variable=s_coord
#pragma HLS INTERFACE axis register both port=s_coord
#pragma HLS ARRAY_PARTITION variable=s_coord complete dim=1
#pragma HLS ARRAY_PARTITION variable=s_coord1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=s_coord2 complete dim=1
#pragma HLS PIPELINE II=4

  ECStream10_s fifo;

  hls::stream<ECStream10_s> s_coord1_fifo[NH_FIFOS]; /* depth: ecfrac1+ecfrac2+ecfrac3+eccorr=8+9+8+16=41*/
#pragma HLS STREAM variable=s_coord1_fifo depth=32 dim=1
  hls::stream<ECStream10_s> s_coord2_fifo[NH_FIFOS]; /* depth: ecfrac1+ecfrac2+ecfrac3+eccorr=8+9+8+16=41*/
#pragma HLS STREAM variable=s_coord2_fifo depth=32 dim=1

  for(int i=0; i<NH_FIFOS; i++)
  {
    for(int j=0; j<NH_READS; j++)
    {
      fifo = s_coord[i].read();

	  s_coord1_fifo[i].write(fifo);
	  if(!s_coord1[i].full() && !s_coord1_fifo[i].empty())
        s_coord1[i].write(s_coord1_fifo[i].read());

	  s_coord2_fifo[i].write(fifo);
	  if(!s_coord2[i].full() && !s_coord2_fifo[i].empty())
        s_coord2[i].write(s_coord2_fifo[i].read());
    }
  }

}
