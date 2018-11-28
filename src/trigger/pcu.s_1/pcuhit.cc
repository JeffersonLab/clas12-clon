
/* pcuhit.c - 

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



#include "pculib.h"



//#define DEBUG


#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)
#define ABS(x)      ((x) < 0 ? -(x) : (x))

#define NPIPE 3 /* the number on 32ns slices to keep and use in pcuhit logic; will be shifted 1 element right on every call */

/*xc7vx550tffg1158-1*/


/* 1.96/46/8/0%/0%/(10270)1%/(5734)1% II=8 */

/* nframes here means half of acceptance interval */
void
pcuhit(ap_uint<16> thresholdmin, ap_uint<16> thresholdmax, nframe_t nframes, PCUStrip_s s_strip, PCUHit s_hit[NH_READS])
{
//#pragma HLS INTERFACE ap_stable port=nframes
#pragma HLS PIPELINE II=1

  int it, ic;

  static PCUStrip_s strip_pipe[NPIPE];
#pragma HLS DATA_PACK variable=strip_pipe
#pragma HLS ARRAY_PARTITION variable=strip_pipe complete dim=1

  ap_uint<NPER> output[NSTRIP];
#pragma HLS ARRAY_PARTITION variable=output complete dim=1


#ifdef DEBUG
  printf("== pcuhit start ==\n");
    for(int i=0; i<NSTRIP; i++)
    {
      if(s_strip.en[i]>0) cout<<"pcuhit: s_strip.en["<<i<<"]="<<s_strip.en[i]<<", s_strip.tm["<<i<<"]="<<s_strip.tm[i]<<endl;
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
      if(strip_pipe[j].en[i]>0) cout<<"pcuhit: strip_pipe["<<j<<"].en["<<i<<"]="<<strip_pipe[j].en[i]<<", strip_pipe["<<j<<"].tm["<<i<<"]="<<strip_pipe[j].tm[i]<<endl;
	}
  }
#endif


  /* check for left-right coincidence within 'nframes' interval */
  for(int i=0; i<NH_READS; i++) output[i] = 0; /* cleanup output mask */

  for(int i=0; i<NSTRIP; i++) /* loop over all counters */
  {
    ap_uint<NPER> mask = 0;
    for(int j=0; j<NPIPE; j++) /* loop over all 3 intervals of fadc data currently in pipe */
    {
      if( (strip_pipe[j].en[i]>=thresholdmin) && (strip_pipe[j].en[i]<=thresholdmax) )
	  {

          mask(j,j) = 1;
          //mask(j,j) = 1;

        //it = strip_pipe[j].tm[i];
        //mask(it,it) = 1;

	  }
    }
    output[i] = mask;
  }


  /*
        for(int ii=0; ii<24; ii++)
		{
          it = j*8 + strip_pipe[j].tm[i];
          
          if(ii<(it-nframes)) continue;
          if(ii>(it+nframes)) continue;
          mask(ii,ii) = 1;
		}
  */


  /*
          int it = strip_pipe[1].tm[i];
          mask(it,it) = 1;
*/


  /* send trigger solution */
  for(int j=0; j<NH_READS; j++)
  {
    for(int i=0; i<NSTRIP; i++) /* loop over all counters */
    {
      s_hit[j].output(i,i) = output[i](j,j);
    }
  }
}

/*
#ifdef DEBUG
    if(output[j]>0) cout<<"pcuhit: output["<<j<<"]="<<hex<<output[j]<<dec<<endl;
#endif
*/
