
/* htcchit.c - first stage HTCC trigger

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



#include "htcclib.h"



//#define DEBUG


#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)
#define ABS(x)      ((x) < 0 ? -(x) : (x))



/*xc7vx550tffg1158-1*/



/* High Threshold Cherenkov Counter:

   S1    S2    S3    S4    S5    S6
  0  1  2  3  4  5  6  7  8  9 10 11
 12 13 14 15 16 17 18 19 20 21 22 23
 24 25 26 27 28 29 30 31 32 33 34 35
 36 37 38 39 40 41 42 43 44 45 46 47

1. cluster is formed by 2x2 window in any location - NCLSTR=36 total

2. for every cluster multiplicity and energy sum are calculated

3. trigger formed if at least one cluster exceeds multiplicity OR energy thresholds

input:
  threshold - individual channel energy threshold
  mult_threshold - cluster multiplicity threshold
  cluster_threshold - cluster energy threshold
  d[] - input adc values

output:
  mult[] - multiplicity for every cluster
  clusters[] - energy sum for every cluster

return:
  1 - if at least one cluster exceeds multiplicity OR energy threshold
  0 - otherwise

 */


/* 1.96/39/1/0%/0%/(15739)2%/(8352)2% II=1 */

void
htcchit(ap_uint<16> strip_threshold, ap_uint<16> mult_threshold, ap_uint<16> cluster_threshold, hls::stream<HTCCStrip_s> s_strip[NSTREAMS1], hls::stream<HTCCHit> &s_hit)
{
#pragma HLS INTERFACE ap_stable port=strip_threshold
#pragma HLS INTERFACE ap_stable port=mult_threshold
#pragma HLS INTERFACE ap_stable port=cluster_threshold
#pragma HLS ARRAY_PARTITION variable=s_strip complete dim=1
#pragma HLS DATA_PACK variable=s_strip
#pragma HLS INTERFACE axis register both port=s_strip
#pragma HLS DATA_PACK variable=s_hit
#pragma HLS INTERFACE axis register both port=s_hit
#pragma HLS PIPELINE II=1

  int i;
  ap_uint<NCLSTR> maskEnergy, maskMult, mask;

  ap_uint<13> d[NCHAN];
#pragma HLS ARRAY_PARTITION variable=d complete dim=1
  ap_uint<3> mult[NCLSTR];
#pragma HLS ARRAY_PARTITION variable=mult complete dim=1
  ap_uint<16> clusters[NCLSTR];
#pragma HLS ARRAY_PARTITION variable=clusters complete dim=1

  HTCCStrip_s fifo;

  for(int j=0; j<NSTREAMS1; j++)
  {
	fifo = s_strip[j].read();

	d[j*N1+0]  = fifo.energy00;
	d[j*N1+1]  = fifo.energy01;
	d[j*N1+2]  = fifo.energy02;
	d[j*N1+3]  = fifo.energy03;
	d[j*N1+4]  = fifo.energy04;
	d[j*N1+5]  = fifo.energy05;
  }


  /* clusters energy sums */

  for(i=0; i<11; i++)
  {
	clusters[i]  = d[i];
	clusters[i] += d[i+1];
	clusters[i] += d[i+12];
	clusters[i] += d[i+13];
  }
  clusters[11]  = d[11];
  clusters[11] += d[12];
  clusters[11] += d[23];
  clusters[11] += d[0];

  for(i=12; i<23; i++)
  {
	clusters[i]  = d[i];
	clusters[i] += d[i+1];
	clusters[i] += d[i+12];
	clusters[i] += d[i+13];
  }
  clusters[23]  = d[23];
  clusters[23] += d[24];
  clusters[23] += d[35];
  clusters[23] += d[12];

  for(i=24; i<35; i++)
  {
	clusters[i]  = d[i];
	clusters[i] += d[i+1];
	clusters[i] += d[i+12];
	clusters[i] += d[i+13];
  }
  clusters[35]  = d[35];
  clusters[35] += d[36];
  clusters[35] += d[47];
  clusters[35] += d[24];


  /* clusters multiplicity */

  for(i=0; i<NCLSTR; i++) mult[i]=0;
  for(i=0; i<11; i++)
  {
	if(d[i   ] > strip_threshold) mult[i] ++;
	if(d[i+ 1] > strip_threshold) mult[i] ++;
	if(d[i+12] > strip_threshold) mult[i] ++;
	if(d[i+13] > strip_threshold) mult[i] ++;
  }
  if(d[11] > strip_threshold) mult[11] ++;
  if(d[12] > strip_threshold) mult[11] ++;
  if(d[23] > strip_threshold) mult[11] ++;
  if(d[ 0] > strip_threshold) mult[11] ++;

  for(i=12; i<23; i++)
  {
	if(d[i   ] > strip_threshold) mult[i] ++;
	if(d[i+ 1] > strip_threshold) mult[i] ++;
	if(d[i+12] > strip_threshold) mult[i] ++;
	if(d[i+13] > strip_threshold) mult[i] ++;
  }
  if(d[23] > strip_threshold) mult[23] ++;
  if(d[24] > strip_threshold) mult[23] ++;
  if(d[35] > strip_threshold) mult[23] ++;
  if(d[12] > strip_threshold) mult[23] ++;

  for(i=24; i<35; i++)
  {
	if(d[i   ] > strip_threshold) mult[i] ++;
	if(d[i+ 1] > strip_threshold) mult[i] ++;
	if(d[i+12] > strip_threshold) mult[i] ++;
	if(d[i+13] > strip_threshold) mult[i] ++;
  }
  if(d[35] > strip_threshold) mult[35] ++;
  if(d[36] > strip_threshold) mult[35] ++;
  if(d[47] > strip_threshold) mult[35] ++;
  if(d[24] > strip_threshold) mult[35] ++;


  /* trigger solution */

  maskEnergy = 0;
  maskMult = 0;
  for(i=0; i<NCLSTR; i++)
  {
    if(clusters[i] > cluster_threshold)
	{
      maskEnergy |= (1<<i);
#ifdef DEBUG
      printf("cluster[%d]: energy=%d\n",i,(uint16_t)clusters[i]);
#endif
	}
    if(mult[i] > mult_threshold)
	{
      maskMult |= (1<<i);
#ifdef DEBUG
      printf("cluster[%d]: mult=%d\n",i,(uint16_t)mult[i]);
#endif
	}
  }
  mask = maskEnergy & maskMult;


  HTCCHit fifo1;
  fifo1.output = mask;
  s_hit.write(fifo1);

}
