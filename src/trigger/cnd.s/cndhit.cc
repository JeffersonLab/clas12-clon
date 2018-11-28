
/* cndhit.c - 

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



#include "cndlib.h"



//#define DEBUG


#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)
#define ABS(x)      ((x) < 0 ? -(x) : (x))

#define NPIPE 3 /* the number on 32ns slices to keep and use in cndhit logic; will be shifted 1 element right on every call */

/*xc7vx550tffg1158-1*/


/* 1.96/46/8/0%/0%/(10270)1%/(5734)1% II=8 */

#define THIRD (NSTRIP/3)

void
cndhit(ap_uint<32> threshold, nframe_t nframes, CNDStrip_s strip[NSTRIP], CNDHit s_hit[NH_READS])
{
//#pragma HLS INTERFACE ap_stable port=nframes
#pragma HLS PIPELINE II=1

  int tdif;
  static CNDStrip4_s strip_pipe[NPIPE][NSTRIP];
#pragma HLS ARRAY_PARTITION variable=strip_pipe complete dim=1
#pragma HLS ARRAY_PARTITION variable=strip_pipe complete dim=2

  ap_uint<NPER> output[NSTRIP];
#pragma HLS ARRAY_PARTITION variable=strip_pipe complete dim=1

  ap_uint<32> thresh;


  if(nframes>NPER) nframes = NPER;
  thresh = threshold*threshold;


#ifdef DEBUG
  printf("== cndhit start, nframes=%d ==\n",(uint8_t)nframes);
  for(int i=0; i<NSTRIP; i++)
  {
    if(strip[i].enL>0) cout<<"cndhit: strip["<<i<<"].enL="<<strip[i].enL<<", strip["<<i<<"].tmL="<<strip[i].tmL<<endl;
    if(strip[i].enR>0) cout<<"cndhit: strip["<<i<<"].enR="<<strip[i].enR<<", strip["<<i<<"].tmR="<<strip[i].tmR<<endl;
  }
#endif

  /* shift whole pipe to the right ([1]->[2], [0]->[1]) */
  for(int j=(NPIPE-1); j>0; j--)
  {
    for(int i=0; i<NSTRIP; i++)
    {
	  strip_pipe[j][i].enL = strip_pipe[j-1][i].enL;
	  strip_pipe[j][i].tmL = strip_pipe[j-1][i].tmL;
	  strip_pipe[j][i].enR = strip_pipe[j-1][i].enR;
	  strip_pipe[j][i].tmR = strip_pipe[j-1][i].tmR;
	}
  }


  /* get NH_READS timing slices of new data and place in first interval of the pipe */
  for(int i=0; i<NSTRIP; i++)
  {
    strip_pipe[0][i].enL = strip[i].enL;
    strip_pipe[0][i].tmL = strip[i].tmL;
    strip_pipe[0][i].enR = strip[i].enR;
    strip_pipe[0][i].tmR = strip[i].tmR;
  }


#ifdef DEBUG
  for(int j=NPIPE-1; j>=0; j--)
  {
    for(int i=0; i<NSTRIP; i++)
    {
      if(strip_pipe[j][i].enL>0) cout<<"cndhit: strip_pipe["<<j<<"]["<<i<<"].enL="<<strip_pipe[j][i].enL<<", strip_pipe["<<j<<"]["<<i<<"].tmL="<<strip_pipe[j][i].tmL<<endl;
      if(strip_pipe[j][i].enR>0) cout<<"cndhit: strip_pipe["<<j<<"]["<<i<<"].enR="<<strip_pipe[j][i].enR<<", strip_pipe["<<j<<"]["<<i<<"].tmR="<<strip_pipe[j][i].tmR<<endl;
	}
  }
#endif



  /* check for left-right coincidence within 'nframes' interval */

  for(int i=0; i<NH_READS; i++) output[i] = 0; /* cleanup output mask */


#if 0 /* left-right coincidence */

  for(int i=0; i<NSTRIP; i++) /* loop over all counters */
  {
    ap_uint<NPER> mask = 0;
    const int add[NPIPE] = {16, 8, 0}; /* add 16 to interval [0] and 8 to interval [1], to get consequative numbering from 0 to 23 */
    for(int j=0; j<NPIPE; j++) /* on left side, look one interval before, one current and one after, and match with middle on right side */
    {
      if( (strip_pipe[1][i].enR*strip_pipe[j][i].enL) > thresh)
	  {
        tdif = (strip_pipe[j][i].tmL+add[j]) - (strip_pipe[1][i].tmR+add[1]);
#ifdef DEBUG
        cout<<"=> tdif1="<<tdif<<endl;
#endif
        if(tdif < 0) tdif = -tdif;
#ifdef DEBUG
        cout<<"=> tdif2="<<tdif<<",nframes="<<nframes<<endl;
#endif
        if(tdif <= nframes)
		{
          int it = strip_pipe[1][i].tmR; /* take timing from right, because it will match PCAL-U which was PMTs on the same side */
          mask(it,it) = 1;
#ifdef DEBUG
          cout<<"=> it="<<it<<", j="<<j<<", i="<<i<<", mask="<<hex<<mask<<dec<<endl;
#endif
		}
	  }
    }
    output[i] = mask;
#ifdef DEBUG
    if(output[i]>0)  cout<<"cndhit: output[nstrip="<<i<<"]="<<hex<<output[i]<<dec<<endl;
#endif
  }

  /* send trigger solution */
  for(int j=0; j<NH_READS; j++)
  {
    for(int i=0; i<NSTRIP; i++) /* loop over all counters */
    {
      s_hit[j].output(i,i) = output[i](j,j);
    }
  }

#endif



#if 1 /* L1_left x L1_right x L2_left x L2_right */

  for(int k=0; k<THIRD; k++) /* loop over all counters */
  {
    ap_uint<NPER> mask1 = 0;
    ap_uint<NPER> mask2 = 0;
    ap_uint<NPER> mask3 = 0;
    int i;
    const int add[NPIPE] = {16, 8, 0}; /* add 16 to interval [0] and 8 to interval [1], to get consequative numbering from 0 to 23 */

    /* layer 1 */
    i = k*3;
    for(int j=0; j<NPIPE; j++) /* on left side, look one interval before, one current and one after, and match with middle on right side */
    {
      if( (strip_pipe[1][i].enR*strip_pipe[j][i].enL) > thresh)
	  {
        tdif = (strip_pipe[j][i].tmL+add[j]) - (strip_pipe[1][i].tmR+add[1]);
        if(tdif < 0) tdif = -tdif;
        if(tdif <= nframes)
		{
          int it = strip_pipe[1][i].tmR; /* take timing from right, because it will match PCAL-U which was PMTs on the same side */
          mask1(it,it) = 1;
#ifdef DEBUG
          cout<<"L1=> it="<<it<<", j="<<j<<", i="<<i<<", mask1="<<hex<<mask1<<dec<<endl;
#endif
		}
	  }
    }

    /* layer 2 */
    i = k*3+1;
    for(int j=0; j<NPIPE; j++) /* on left side, look one interval before, one current and one after, and match with middle on right side */
    {
      if( (strip_pipe[1][i].enR*strip_pipe[j][i].enL) > thresh)
	  {
        tdif = (strip_pipe[j][i].tmL+add[j]) - (strip_pipe[1][i].tmR+add[1]);
        if(tdif < 0) tdif = -tdif;
        if(tdif <= nframes)
		{
          int it = strip_pipe[1][i].tmR; /* take timing from right, because it will match PCAL-U which was PMTs on the same side */
          mask2(it,it) = 1;
#ifdef DEBUG
          cout<<"L2=> it="<<it<<", j="<<j<<", i="<<i<<", mask2="<<hex<<mask2<<dec<<endl;
#endif
		}
	  }
    }

    /* layer 3 */
    i = k*3+2;
    for(int j=0; j<NPIPE; j++) /* on left side, look one interval before, one current and one after, and match with middle on right side */
    {
      if( (strip_pipe[1][i].enR*strip_pipe[j][i].enL) > thresh)
	  {
        tdif = (strip_pipe[j][i].tmL+add[j]) - (strip_pipe[1][i].tmR+add[1]);
        if(tdif < 0) tdif = -tdif;
        if(tdif <= nframes)
		{
          int it = strip_pipe[1][i].tmR; /* take timing from right, because it will match PCAL-U which was PMTs on the same side */
          mask3(it,it) = 1;
#ifdef DEBUG
          cout<<"L3=> it="<<it<<", j="<<j<<", i="<<i<<", mask3="<<hex<<mask3<<dec<<endl;
#endif
		}
	  }
    }





#if 0

    /* L1 x L2 */
    output[k*3]   = (mask1 & mask2) | (mask1 & (mask2<<1)) | (mask1 & (mask2>>1));

    /* L2 x L3 */
    output[k*3+1] = (mask2 & mask3) | (mask2 & (mask3<<1)) | (mask2 & (mask3>>1));

    /* L1 x L2 x L3 */
    output[k*3+2] = mask1 & mask2 & mask3;

#else /* report single layers */

    output[k*3]   = mask1; /* L1 only */
    output[k*3+1] = mask2; /* L2 only */
	output[k*3+2] = mask3; /* L3 only */

#endif




#ifdef DEBUG
    if(output[k*3]>0)    cout<<"cndhit: output[ncoin="<<k*3<<"]="<<hex<<output[k*3]<<dec<<endl;
    if(output[k*3+1]>0)  cout<<"cndhit: output[ncoin="<<k*3+1<<"]="<<hex<<output[k*3+1]<<dec<<endl;
    if(output[k*3+2]>0)  cout<<"cndhit: output[ncoin="<<k*3+2<<"]="<<hex<<output[k*3+2]<<dec<<endl;
#endif
  }


  /* send trigger solution */
  for(int j=0; j<NH_READS; j++)
  {
    for(int i=0; i<THIRD; i++)
    {
      s_hit[j].output(i,i) = output[i*3](j,j);
      s_hit[j].output(i+24,i+24) = output[i*3+1](j,j);
      s_hit[j].output(i+48,i+48) = output[i*3+2](j,j);
    }
#ifdef DEBUG
    if(s_hit[j].output>0) cout<<"cndhit: s_hit[time="<<j<<"].output="<<hex<<s_hit[j].output<<dec<<endl;
#endif
  }

#endif

}
