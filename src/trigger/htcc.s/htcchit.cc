
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

static ap_uint<6> cl2d[NCLSTR][4] = {
  0, 4, 1, 5,  /* cluster 00 */
  4, 8, 5, 9,  /* cluster 01 */
  8, 12,9, 13, /* cluster 02 */
  12,16,13,17, /* cluster 03 */
  16,20,17,21, /* cluster 04 */
  20,24,21,25, /* cluster 05 */
  24,28,25,29, /* cluster 06 */
  28,32,29,33, /* cluster 07 */
  32,36,33,37, /* cluster 08 */
  36,40,37,41, /* cluster 09 */
  40,44,41,45, /* cluster 10 */
  44,0, 45,1,  /* cluster 11 */

  1, 5, 2, 6,  /* cluster 12 */
  5, 9, 6, 10, /* cluster 13 */
  9, 13,10,14, /* cluster 14 */
  13,17,14,18, /* cluster 15 */
  17,21,18,22, /* cluster 16 */
  21,25,22,26, /* cluster 17 */
  25,29,26,30, /* cluster 18 */
  29,33,30,34, /* cluster 19 */
  33,37,34,38, /* cluster 20 */
  37,41,38,42, /* cluster 21 */
  41,45,42,46, /* cluster 22 */
  45,1, 46,2,  /* cluster 23 */

  2, 6, 3, 7,  /* cluster 24 */
  6, 10,7, 11, /* cluster 25 */
  10,14,11,15, /* cluster 26 */
  14,18,15,19, /* cluster 27 */
  18,22,19,23, /* cluster 28 */
  22,26,23,27, /* cluster 29 */
  26,30,27,31, /* cluster 30 */
  30,34,31,35, /* cluster 31 */
  34,38,35,39, /* cluster 32 */
  38,42,39,43, /* cluster 33 */
  42,46,43,47, /* cluster 34 */
  46,2 ,47,3   /* cluster 35 */
};

/* High Threshold Cherenkov Counter:

   S1    S2    S3    S4    S5    S6
  0  4  8 12 16 20 24 28 32 36 40 44
  1  5  9 13 17 21 25 29 33 37 41 45
  2  6 10 14 18 22 26 30 34 38 42 46
  3  7 11 15 19 23 27 31 35 39 43 47

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

//#define SINGLES


/* 1.96/39/1/0%/0%/(15739)2%/(8352)2% II=1 */

void
htcchit(ap_uint<16> strip_threshold, ap_uint<16> mult_threshold, ap_uint<16> cluster_threshold, hls::stream<HTCCStrip_s> s_strip[NSTREAMS1],
        hls::stream<HTCCHit> &s_hit)
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


  /* to become output */
  ap_uint<NCHAN> output;
  ap_uint<NCHAN> dmask;
  ap_uint<NCHAN> cmask;

  int i, ii;
  ap_uint<NCLSTR> maskEnergy, maskMult, mask;

  ap_uint<13> d[NCHAN];
#pragma HLS ARRAY_PARTITION variable=d complete dim=1
  ap_uint<3> mult[NCLSTR];
#pragma HLS ARRAY_PARTITION variable=mult complete dim=1
  ap_uint<16> clusters[NCLSTR];
#pragma HLS ARRAY_PARTITION variable=clusters complete dim=1

#ifdef DEBUG
  cout<<endl<<"htcchit reached: strip_threshold="<<strip_threshold<<" mult_threshold="<<mult_threshold<<" cluster_threshold="<<cluster_threshold<<endl;
#endif

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


  /* channel mask */
  dmask = 0;
  for(i=0; i<NCHAN; i++)
  {
	if(d[i] >= strip_threshold)
	{
      dmask(i,i) = 1;
#ifdef DEBUG
      printf("htcchit: chan=%d, energy=%d\n",i,(uint16_t)d[i]);
#endif
	}
  }
#ifdef DEBUG
  cout<<"htcchit: dmask="<<hex<<dmask<<dec<<endl;
#endif





#ifdef SINGLES

  output = dmask;

#else

  /* clusters energy sums */

  for(i=0; i<11; i++)
  {
    ii = i*4;
	clusters[i]  = d[ii];
	clusters[i] += d[ii+4];
	clusters[i] += d[ii+1];
	clusters[i] += d[ii+5];
  }
  clusters[11]  = d[0];
  clusters[11] += d[1];
  clusters[11] += d[44];
  clusters[11] += d[45];

  for(i=12; i<23; i++)
  {
    ii = (i-12)*4 + 1;
	clusters[i]  = d[ii];
	clusters[i] += d[ii+4];
	clusters[i] += d[ii+1];
	clusters[i] += d[ii+5];
  }
  clusters[23]  = d[1];
  clusters[23] += d[2];
  clusters[23] += d[45];
  clusters[23] += d[46];

  for(i=24; i<35; i++)
  {
    ii = (i-24)*4 + 2;
	clusters[i]  = d[ii];
	clusters[i] += d[ii+4];
	clusters[i] += d[ii+1];
	clusters[i] += d[ii+5];
  }
  clusters[35]  = d[2];
  clusters[35] += d[3];
  clusters[35] += d[46];
  clusters[35] += d[47];


  /* clusters multiplicity */

  for(i=0; i<NCLSTR; i++) mult[i]=0;
  for(i=0; i<11; i++)
  {
    ii = i*4;
	if(d[ii  ] >= strip_threshold) mult[i] ++;
	if(d[ii+4] >= strip_threshold) mult[i] ++;
	if(d[ii+1] >= strip_threshold) mult[i] ++;
	if(d[ii+5] >= strip_threshold) mult[i] ++;
  }
  if(d[ 0] >= strip_threshold) mult[11] ++;
  if(d[ 1] >= strip_threshold) mult[11] ++;
  if(d[44] >= strip_threshold) mult[11] ++;
  if(d[45] >= strip_threshold) mult[11] ++;

  for(i=12; i<23; i++)
  {
    ii = (i-12)*4 + 1;
	if(d[ii  ] >= strip_threshold) mult[i] ++;
	if(d[ii+4] >= strip_threshold) mult[i] ++;
	if(d[ii+1] >= strip_threshold) mult[i] ++;
	if(d[ii+5] >= strip_threshold) mult[i] ++;
  }
  if(d[ 1] >= strip_threshold) mult[23] ++;
  if(d[ 2] >= strip_threshold) mult[23] ++;
  if(d[45] >= strip_threshold) mult[23] ++;
  if(d[46] >= strip_threshold) mult[23] ++;

  for(i=24; i<35; i++)
  {
    ii = (i-24)*4 + 2;
	if(d[ii ] >= strip_threshold) mult[i] ++;
	if(d[ii+4] >= strip_threshold) mult[i] ++;
	if(d[ii+1] >= strip_threshold) mult[i] ++;
	if(d[ii+5] >= strip_threshold) mult[i] ++;
  }
  if(d[ 2] >= strip_threshold) mult[35] ++;
  if(d[ 3] >= strip_threshold) mult[35] ++;
  if(d[46] >= strip_threshold) mult[35] ++;
  if(d[47] >= strip_threshold) mult[35] ++;


  /* trigger solution */
  maskEnergy = 0;
  maskMult = 0;
  for(i=0; i<NCLSTR; i++)
  {
    if(clusters[i] >= cluster_threshold)
	{
      maskEnergy |= (1<<i);
#ifdef DEBUG
      printf("htcchit: cluster[%d]: energy=%d\n",i,(uint16_t)clusters[i]);
#endif
	}
    if(mult[i] >= mult_threshold)
	{
      maskMult |= (1<<i);
#ifdef DEBUG
      printf("htcchit: cluster[%d]: mult=%d\n",i,(uint16_t)mult[i]);
#endif
	}
  }
  mask = maskEnergy & maskMult;
#ifdef DEBUG
  cout<<"htcchit: maskEnergy="<<hex<<maskEnergy<<dec<<endl;
  cout<<"htcchit:   maskMult="<<hex<<maskMult<<dec<<endl;
  cout<<"htcchit:       mask="<<hex<<mask<<dec<<endl;
#endif

  cmask = 0;
#ifdef DEBUG
  cout<<"htcchit: CMASK BEFOR="<<hex<<cmask<<dec<<endl;
#endif
  for(int i=0; i<NCLSTR; i++)
  {
    if( (mask>>i)&0x1 )
	{
      for(int j=0; j<4; j++)
	  {
        cmask(cl2d[i][j],cl2d[i][j]) = 1;
#ifdef DEBUG
        cout<<"FOUND CLUSTER "<<i<<" -> ADD BIT " << cl2d[i][j] <<" to CMASK" << endl;
#endif
	  }
	}
  }
#ifdef DEBUG
  cout<<"htcchit: CMASK AFTER="<<hex<<cmask<<dec<<endl;
#endif

  output = dmask & cmask;


#endif /*SINGLES*/





#ifdef DEBUG
  cout<<endl;
  cout<<"htcchit:  dmask="<<hex<<dmask<<dec<<endl;
  cout<<"htcchit:  cmask="<<hex<<cmask<<dec<<endl;
  cout<<"htcchit: output="<<hex<<output<<dec<<endl<<endl;
#endif

  /* output stream */

  HTCCHit fifo1;
  fifo1.output = output;
  s_hit.write(fifo1);

}
