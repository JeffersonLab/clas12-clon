
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

#define NPIPE 24 /* the number on 4ns slices to keep and use in ftofhit logic; will be shifted 8 elements right on every call */

/*xc7vx550tffg1158-1*/


/* 1.96/46/8/0%/0%/(10270)1%/(5734)1% II=8 */

void
ftofhit(nframe_t nframes, hls::stream<FTOFStrip_s> s_strip[NH_READS], hls::stream<FTOFHit> s_hit[NH_READS])
{
#pragma HLS INTERFACE ap_stable port=nframes
#pragma HLS DATA_PACK variable=s_strip
#pragma HLS INTERFACE axis register both port=s_strip
#pragma HLS ARRAY_PARTITION variable=s_strip complete dim=1
#pragma HLS DATA_PACK variable=s_hit
#pragma HLS INTERFACE axis register both port=s_hit
#pragma HLS ARRAY_PARTITION variable=s_hit complete dim=1
#pragma HLS PIPELINE II=1

  static ap_uint<NSTRIP> outL[NPIPE], outR[NPIPE];

  ap_uint<NSTRIP> output[NH_READS];

  /* shift old data 8 elements to the right */
  for(int i=15; i>=0; i--)
  {
	outL[i+8] = outL[i];
	outR[i+8] = outR[i];
  }


  /* get new data */
  for(int j=0; j<NH_READS; j++)
  {
    FTOFStrip_s fifo;
    fifo = s_strip[j].read();
    outL[j]  = fifo.outL;
    outR[j]  = fifo.outR;
  }





  /* check for left-right coincidence withing 'nframes' interval */

  for(int i=8; i<16; i++) /* take middle interval left PMTs, and compare with +-(NPER/2) right PMTs */
  {
    for(int j=i-(NPER/2); j<=i+(NPER/2); j++) output[i-8] = outL[i] & outR[j];
  }


  /* send trigger solution */

  FTOFHit fifo1;
  for(int j=0; j<8; j++)
  {
    fifo1.output[j] = output[j];
    s_hit[j].write(fifo1);
  }
}
