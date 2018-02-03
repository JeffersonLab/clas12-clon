
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
pcuhit(ap_uint<16> thresholdmin, ap_uint<16> thresholdmax, nframe_t nframes, PCUStrip strip[NSTRIP], PCUHit s_hit[NH_READS])
{
//#pragma HLS INTERFACE ap_stable port=nframes
#pragma HLS PIPELINE II=1

  int it, ic;

  static PCUStrip strip_pipe[NPIPE][NSTRIP];
#pragma HLS DATA_PACK variable=strip_pipe
#pragma HLS ARRAY_PARTITION variable=strip_pipe complete dim=1
#pragma HLS ARRAY_PARTITION variable=strip_pipe complete dim=2

  ap_uint<NPER> output[NSTRIP];
#pragma HLS ARRAY_PARTITION variable=output complete dim=1


#ifdef DEBUG
  printf("== pcuhit start ==\n");
    for(int i=0; i<NSTRIP; i++)
    {
      if(strip[i].en>0) cout<<"pcuhit: strip["<<i<<"].en="<<strip[i].en<<", strip["<<i<<"].tm="<<strip[i].tm<<endl;
    }
#endif



  /* shift whole pipe to the right ([1]->[2], [0]->[1]) */
  for(int j=(NPIPE-1); j>0; j--)
  {
    for(int i=0; i<NSTRIP; i++)
    {
	  strip_pipe[j][i].en = strip_pipe[j-1][i].en;
	  strip_pipe[j][i].tm = strip_pipe[j-1][i].tm;
	}
  }

  /* get NH_READS timing slices of new data and place in first interval of the pipe */
  for(int i=0; i<NSTRIP; i++)
  {
    strip_pipe[0][i].en = strip[i].en;
    strip_pipe[0][i].tm = strip[i].tm;
  }



#ifdef DEBUG
  for(int j=NPIPE-1; j>=0; j--)
  {
    for(int i=0; i<NSTRIP; i++)
    {
      if(strip_pipe[j][i].en>0) cout<<"pcuhit: strip_pipe["<<j<<"]["<<i<<"].en="<<strip_pipe[j][i].en<<", strip_pipe["<<j<<"]["<<i<<"].tm="<<strip_pipe[j][i].tm<<endl;
	}
  }
#endif


  /* check for thresholds satisfaction */
  for(int i=0; i<NH_READS; i++) output[i] = 0; /* cleanup output mask */

  for(int i=0; i<NSTRIP; i++) /* loop over all counters */
  {
    ap_uint<NPER> mask = 0;
    for(int j=0; j<NPIPE; j++) /* loop over all 3 intervals of fadc data currently in pipe */
    {
#ifdef DEBUG
      if(strip_pipe[j][i].en>0)
	  {
        cout << "pcuhit: strip_pipe["<<j<<"]["<<i<<"].en="<<strip_pipe[j][i].en;
        cout << ", thresholdmin="<< thresholdmin<<", thresholdmax="<< thresholdmax<<endl; 
	  }
#endif
      if( (strip_pipe[j][i].en>=thresholdmin) && (strip_pipe[j][i].en<=thresholdmax) )
	  {
        it = strip_pipe[j][i].tm;
        mask(it,it) = 1;
#ifdef DEBUG
        cout << "pcuhit: it="<<it<<" -> mask="<<hex<<mask<<dec<<endl;
#endif
	  }
    }

    output[i] = mask;
  }


  /* send trigger solution */
  for(int j=0; j<NH_READS; j++)
  {
    for(int i=0; i<NSTRIP; i++) /* loop over all counters */
    {
      s_hit[j].output(i,i) = output[i](j,j);
    }
  }

#ifdef DEBUG
  printf("== pcuhit end ==\n");
#endif
}

/*
#ifdef DEBUG
    if(output[j]>0) cout<<"pcuhit: output["<<j<<"]="<<hex<<output[j]<<dec<<endl;
#endif
*/
