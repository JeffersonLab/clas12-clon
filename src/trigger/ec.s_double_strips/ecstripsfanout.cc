
/* ecstripsfanout.c */

	


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include <iostream>
using namespace std;



#ifdef USE_PCAL

#include "../pc.s/pclib.h"
#define ecstripsfanout pcstripsfanout

#else

#include "eclib.h"

#endif


//#define DEBUG




#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)
#define ABS(x)      ((x) < 0 ? -(x) : (x))



/*xc7vx550tffg1158-1*/



/* 2.47/6/4/0%/0%/(1546)~0%/(1076)~0% II=4 */

void
ecstripsfanout(hls::stream<ECStrip_s> s_strip[NF1], hls::stream<ECStrip_s> s_strip1[NF1], hls::stream<ECStrip_s> s_strip2[NF1])
{
#pragma HLS DATA_PACK variable=s_strip2
#pragma HLS INTERFACE axis register both port=s_strip2
#pragma HLS DATA_PACK variable=s_strip1
#pragma HLS INTERFACE axis register both port=s_strip1
#pragma HLS DATA_PACK variable=s_strip
#pragma HLS INTERFACE axis register both port=s_strip
#pragma HLS ARRAY_PARTITION variable=s_strip complete dim=1
#pragma HLS ARRAY_PARTITION variable=s_strip1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=s_strip2 complete dim=1
#pragma HLS PIPELINE II=4

  ECStrip_s fifo;

//  hls::stream<ECStrip_s> s_strip1_fifo[NF1];
  hls::stream<ECStrip_s> s_strip2_fifo[NF1];
#ifdef USE_PCAL
//#pragma HLS STREAM variable=s_strip1_fifo depth=256 dim=1
#pragma HLS STREAM variable=s_strip2_fifo depth=32 dim=1
#else
#pragma HLS STREAM variable=s_strip2_fifo depth=16 dim=1
#endif

  for(int j=0; j<NH_READS; j++)
  {
    for(int i=0; i<NF1; i++)
    {
	  fifo = s_strip[i].read();

	  s_strip1[i].write(fifo);

	  /*
      s_strip2[i].write(fifo);
	  */
	  s_strip2_fifo[i].write(fifo);
	  if(!s_strip2[i].full() && !s_strip2_fifo[i].empty())
		s_strip2[i].write(s_strip2_fifo[i].read());
	}
  }

}
