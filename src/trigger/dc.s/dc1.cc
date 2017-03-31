
/* dc1.c - first stage DC trigger

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



#include "dclib.h"



//#define DEBUG


#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)
#define ABS(x)      ((x) < 0 ? -(x) : (x))



/*xc7vx550tffg1158-1*/

/* 3.48/4/1/0%/0%/~0%/~0% */


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

ap_uint<6>
dc1(ap_uint<13> threshold, ap_uint<3> mult_threshold, ap_uint<16> cluster_threshold, ap_uint<13> d[NCHAN],
	  ap_uint<3> mult[NCLSTR], ap_uint<16> clusters[NCLSTR])
{
#pragma HLS ARRAY_PARTITION variable=clusters complete dim=1
#pragma HLS ARRAY_PARTITION variable=d complete dim=1
#pragma HLS ARRAY_PARTITION variable=mult complete dim=1
#pragma HLS PIPELINE

  int i;
  ap_uint<6> ret;


  /* clusters energy sums */

  for(i=0; i<11; i++) clusters[i] = d[i] + d[i+1] + d[i+12] + d[i+13];
  clusters[11] = d[11] + d[12] + d[23] + d[0];

  for(i=12; i<23; i++) clusters[i] = d[i] + d[i+1] + d[i+12] + d[i+13];
  clusters[23] = d[23] + d[24] + d[35] + d[12];

  for(i=24; i<35; i++) clusters[i] = d[i] + d[i+1] + d[i+12] + d[i+13];
  clusters[35] = d[35] + d[36] + d[47] + d[24];


  /* clusters multiplicity */

  for(i=0; i<NCLSTR; i++) mult[i]=0;
  for(i=0; i<11; i++)
  {
	if(d[i   ] > threshold) mult[i] ++;
	if(d[i+ 1] > threshold) mult[i] ++;
	if(d[i+12] > threshold) mult[i] ++;
	if(d[i+13] > threshold) mult[i] ++;
  }
  if(d[11] > threshold) mult[11] ++;
  if(d[12] > threshold) mult[11] ++;
  if(d[23] > threshold) mult[11] ++;
  if(d[ 0] > threshold) mult[11] ++;

  for(i=12; i<23; i++)
  {
	if(d[i   ] > threshold) mult[i] ++;
	if(d[i+ 1] > threshold) mult[i] ++;
	if(d[i+12] > threshold) mult[i] ++;
	if(d[i+13] > threshold) mult[i] ++;
  }
  if(d[23] > threshold) mult[23] ++;
  if(d[24] > threshold) mult[23] ++;
  if(d[35] > threshold) mult[23] ++;
  if(d[12] > threshold) mult[23] ++;

  for(i=24; i<35; i++)
  {
	if(d[i   ] > threshold) mult[i] ++;
	if(d[i+ 1] > threshold) mult[i] ++;
	if(d[i+12] > threshold) mult[i] ++;
	if(d[i+13] > threshold) mult[i] ++;
  }
  if(d[35] > threshold) mult[35] ++;
  if(d[36] > threshold) mult[35] ++;
  if(d[47] > threshold) mult[35] ++;
  if(d[24] > threshold) mult[35] ++;


  /* trigger solution */

  ret = 0;
  for(i=0; i<NCLSTR; i++)
  {
    if(clusters[i] > cluster_threshold) ret = 1;
    if(mult[i] > mult_threshold) ret = 1;
  }

  return(ret);
}
