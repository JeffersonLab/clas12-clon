
/* htcchit.c - 

strip_pipe[] - single counters

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

#define NPIPE 3 /* the number of 32ns slices to keep and use in htcchit logic; will be shifted 1 element right on every call */

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
  d[][] - input adc values

output:
  mult[] - multiplicity for every cluster
  clusters[] - energy sum for every cluster

return:
  1 - if at least one cluster exceeds multiplicity OR energy threshold
  0 - otherwise

 */


/* 1.96/46/8/0%/0%/(10270)1%/(5734)1% II=8 */

void
htcchit(ap_uint<16> strip_threshold, ap_uint<16> mult_threshold, ap_uint<16> cluster_threshold, nframe_t nframes, HTCCStrip_s s_strip, HTCCHit s_hit[NH_READS])
{
//#pragma HLS INTERFACE ap_stable port=nframes
#pragma HLS PIPELINE II=1

  int i, j, ii, it, it1, it2, tdif;

  static HTCCStrip_s strip_pipe[NPIPE];
#pragma HLS ARRAY_PARTITION variable=strip_pipe complete dim=1

  /* to become output */
  ap_uint<1> output[NCHAN];
#pragma HLS ARRAY_PARTITION variable=output complete dim=1
  ap_uint<1> dmask[NCHAN];
#pragma HLS ARRAY_PARTITION variable=dmask complete dim=1
  ap_uint<1> cmask[NCHAN];
#pragma HLS ARRAY_PARTITION variable=cmask complete dim=1

  ap_uint<1> maskEnergy[NCLSTR];
#pragma HLS ARRAY_PARTITION variable=maskEnergy complete dim=1
  ap_uint<1> maskMult[NCLSTR];
#pragma HLS ARRAY_PARTITION variable=maskMult complete dim=1
  ap_uint<1> mask[NCLSTR];
#pragma HLS ARRAY_PARTITION variable=mask complete dim=1

  ap_uint<13> d[NPER][NCHAN];
#pragma HLS ARRAY_PARTITION variable=d complete dim=1
#pragma HLS ARRAY_PARTITION variable=d complete dim=2
  ap_uint<3> mult[NCLSTR];
#pragma HLS ARRAY_PARTITION variable=mult complete dim=1
  ap_uint<16> clusters[NCLSTR];
#pragma HLS ARRAY_PARTITION variable=clusters complete dim=1


  if(nframes>NPER) nframes = NPER;

#ifdef DEBUG
  printf("== htcchit start ==\n");
  cout<<" strip_threshold="<<strip_threshold<<", mult_threshold="<<mult_threshold<<", cluster_threshold="<<cluster_threshold<<endl;
  for(int i=0; i<NSTRIP; i++)
  {
    if(s_strip.en[i]>0) cout<<"htcchit: s_strip.en["<<i<<"]="<<s_strip.en[i]<<", s_strip.tm["<<i<<"]="<<s_strip.tm[i]<<endl;
  }
#endif


  /* shift whole pipe to the right ([1]->[2], [0]->[1]) */
  for(int j=(NPIPE-1); j>0; j--)
  {
    for(int i=0; i<NSTRIP; i++)
    {
	  strip_pipe[j].en[i] = strip_pipe[j-1].en[i];
	  strip_pipe[j].tm[i] = strip_pipe[j-1].tm[i];
	}
  }


  /* get NH_READS timing slices of new data and place in first interval of the pipe */
  for(int i=0; i<NSTRIP; i++)
  {
    strip_pipe[0].en[i] = s_strip.en[i];
    strip_pipe[0].tm[i] = s_strip.tm[i];
  }


#ifdef DEBUG
  for(int j=NPIPE-1; j>=0; j--)
  {
    for(int i=0; i<NSTRIP; i++)
    {
      if(strip_pipe[j].en[i]>0) cout<<"htcchit: strip_pipe["<<j<<"].en["<<i<<"]="<<strip_pipe[j].en[i]<<", strip_pipe["<<j<<"].tm["<<i<<"]="<<strip_pipe[j].tm[i]<<endl;
	}
  }
#endif





  /**************/
  /* fill d[][] */
  
  for(int i=0; i<NSTRIP; i++)
  {
    const int add[NPIPE] = {16, 8, 0}; /* add 16 to interval [0] and 8 to interval [1], to get consequative numbering from 0 to 23 */
    for(it=0; it<NPER; it++) d[it][i] = 0;

    /* get hit from every interval (can be only one per interval) and persist over 'nframes' 4ns ticks */
    for(ii=0; ii<NPIPE; ii++)
	{
      /* check if there is hit in 'ii'interval */
      if( (strip_pipe[ii].en[i]) > 0)
	  {
        /* add hit from interval [1] to the sum */
        it = strip_pipe[ii].tm[i];
        it1 = it + add[ii];      /* first 4ns bin to fill (bins are numnbered from 23 to 0 ) */
        it2 = it1 + nframes + 1; /* last 4ns bin to fill (bins are numnbered from 23 to 0 ); nframes=0 means one interval */
#ifdef DEBUG
        cout<<"=> i="<<i<<", it="<<it<<", ii="<<ii<<" -> it1="<<it1<<", it2="<<it2<<endl;
#endif



#if 1
        /* fill bins overlaping with middle interval */
        for(int k=add[1]; k<add[0]; k++)
		{
          if( (k>=it1) && (k<it2) )
		  {
            d[k-add[1]][i] = strip_pipe[ii].en[i];
#ifdef DEBUG
            cout<<" d["<<k-add[1]<<"]["<<i<<"] = "<<d[k-add[1]][i]<<endl;
#endif
		  }
		}
#endif



#if 0 /* compatible with old version - using interval [0] instead of [1] */
        for(int k=add[0]; k<24; k++)
		{
          if( (k>=it1) && (k<it2) )
		  {
            d[k-add[0]][i] = strip_pipe[ii].en[i];
#ifdef DEBUG
            cout<<" d["<<k-add[0]<<"]["<<i<<"] = "<<d[k-add[0]][i]<<endl;
#endif
		  }
		}
#endif



	  }
	}
  }



  /****************************************/
  /* go through NH_READS(=NPER) intervals */

  for(int jj=0; jj<NH_READS; jj++)
  {
#ifdef DEBUG
    printf(" Processing %d-th 4ns interval\n",jj);
    int print = 0;
#endif

    /* channel mask */
    for(i=0; i<NCHAN; i++)
    {
      dmask[i] = 0;
	  if(d[jj][i] >= strip_threshold)
	  {
        dmask[i] = 1;
#ifdef DEBUG
        printf(" d[%d][%d] = %d\n",jj,i,(uint16_t)d[jj][i]);
        print=1;
#endif
	  }
    }
#ifdef DEBUG
    if(print) {printf("     dmask=");for(i=NCHAN-1; i>=0; i--) printf("%1d",(uint8_t)dmask[i]);printf("\n");}
#endif





    /* clusters energy sums */

    for(i=0; i<NCLSTR; i++) clusters[i] = 0;
    for(i=0; i<NCLSTR; i++)
    {
      for(ii=0; ii<4; ii++)
	  {
        clusters[i] += d[jj][ cl2d[i][ii] ];
	  }
    }


    /* clusters multiplicity */

    for(i=0; i<NCLSTR; i++) mult[i] = 0;
    for(i=0; i<NCLSTR; i++)
    {
      for(ii=0; ii<4; ii++)
	  {
	    if(d[jj][cl2d[i][ii]] >= strip_threshold) mult[i] ++;
	  }
    }


    /* trigger solution */
#ifdef DEBUG
    print = 0;
#endif
    for(i=0; i<NCLSTR; i++)
    {
      maskEnergy[i] = 0;
      maskMult[i] = 0;
      if(clusters[i] >= cluster_threshold)
	  {
        maskEnergy[i] = 1;
#ifdef DEBUG
        printf("  cluster[%d]: energy=%d\n",i,(uint16_t)clusters[i]);
        print=1;
#endif
	  }
      if(mult[i] >= mult_threshold)
	  {
        maskMult[i] = 1;
#ifdef DEBUG
        printf("  cluster[%d]: mult=%d\n",i,(uint16_t)mult[i]);
        print=1;
#endif
	  }
      mask[i] = maskEnergy[i] & maskMult[i];
    }
#ifdef DEBUG
    if(print)
	{
      printf("maskEnergy=");for(i=NCLSTR-1; i>=0; i--) printf("%1d",(uint8_t)maskEnergy[i]);printf("\n");
      printf("  maskMult=");for(i=NCLSTR-1; i>=0; i--) printf("%1d",(uint8_t)maskMult[i]);printf("\n");
      printf("      mask=");for(i=NCLSTR-1; i>=0; i--) printf("%1d",(uint8_t)mask[i]);printf("\n");
	}
#endif

    for(i=0; i<NCHAN; i++) cmask[i] = 0;
#ifdef DEBUG
    //printf("CMASK befor=");for(i=NCHAN-1; i>=0; i--) printf("%1d",(uint8_t)cmask[i]);printf("\n");
    print=0;
#endif
    for(int i=0; i<NCLSTR; i++)
    {
      if(mask[i]==1)
	  {
        for(int j=0; j<4; j++)
	    {
          cmask[ cl2d[i][j] ] = 1;
#ifdef DEBUG
          cout<<"FOUND CLUSTER "<<i<<" -> ADD BIT " << cl2d[i][j] <<" to CMASK" << endl;
          print=1;
#endif
	    }
	  }
    }
#ifdef DEBUG
    if(print) {printf("CMASK after=");for(i=NCHAN-1; i>=0; i--) printf("%1d",(uint8_t)cmask[i]);printf("\n");}
#endif

    for(i=0; i<NCHAN; i++) output[i] = dmask[i] & cmask[i];







#ifdef DEBUG
	if(print)
	{
      cout<<endl;
      printf("   dmask=");for(i=NCHAN-1; i>=0; i--) printf("%1d",(uint8_t)dmask[i]);printf("\n");
      printf("   cmask=");for(i=NCHAN-1; i>=0; i--) printf("%1d",(uint8_t)cmask[i]);printf("\n");
      printf("  output=");for(i=NCHAN-1; i>=0; i--) printf("%1d",(uint8_t)output[i]);printf("\n");
	}
#endif


    for(i=0; i<NCHAN; i++) s_hit[jj].output[i] = output[i];

  } /* NH_READS */


}
