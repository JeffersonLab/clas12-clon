
/* ftofhit.c - 

  input:  

  output: 

*/

	
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include <iostream>
using namespace std;



#include "ftoflib.h"



//#define DEBUG


#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)
#define ABS(x)      ((x) < 0 ? -(x) : (x))



/*xc7vx550tffg1158-1*/


/* 1.96/46/8/0%/0%/(10270)1%/(5734)1% II=8 */

void
ftofhit(ap_uint<16> strip_threshold, ap_uint<16> mult_threshold, ap_uint<16> cluster_threshold, hls::stream<FTOFStrip_s> &s_strip, hls::stream<FTOFHit> &s_hit)
{
#pragma HLS INTERFACE ap_stable port=strip_threshold
#pragma HLS INTERFACE ap_stable port=mult_threshold
#pragma HLS INTERFACE ap_stable port=cluster_threshold
#pragma HLS DATA_PACK variable=s_strip
#pragma HLS INTERFACE axis register both port=s_strip
#pragma HLS DATA_PACK variable=s_hit
#pragma HLS INTERFACE axis register both port=s_hit
#pragma HLS PIPELINE II=1

  ap_uint<NSTRIP> outL, outR, output;


  /* get data */

  FTOFStrip_s fifo;
  fifo = s_strip.read();
  outL  = fifo.outL;
  outR  = fifo.outR;


  /* check for left-right coincidence */

  for(int i=0; i<NSTRIP; i++)
  {
    output = outL & outR;
  }


  /* send trigger solution */

  FTOFHit fifo1;
  fifo1.output = output;
  s_hit.write(fifo1);

}
