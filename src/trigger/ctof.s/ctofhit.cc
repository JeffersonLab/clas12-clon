
/* ctofhit.c - 

strip_pipe[] - single counters
clust_pipe[] - overlaping pairs of counters (0-1, 1-2, 2-3, 3-4, ..., 47-0)

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



#include "ctoflib.h"



//#define DEBUG


#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)
#define ABS(x)      ((x) < 0 ? -(x) : (x))

#define NPIPE 3 /* the number on 32ns slices to keep and use in ctofhit logic; will be shifted 1 element right on every call */

/*xc7vx550tffg1158-1*/



/* 1.96/46/8/0%/0%/(10270)1%/(5734)1% II=8 */

void
ctofhit(ap_uint<16> threshold1, ap_uint<16> threshold2, nframe_t nframes, CTOFStrip_s s_strip, CTOFStrip_s s_clust, CTOFHit s_hit[NH_READS])
{
//#pragma HLS INTERFACE ap_stable port=nframes
#pragma HLS PIPELINE II=1

  int tdif;

  static CTOFStrip_s strip_pipe[NPIPE];
  static CTOFStrip_s clust_pipe[NPIPE];

  ap_uint<NPER> out[NSTRIP]; /* cluster mask */
  ap_uint<NPER> output[NSTRIP]; /* strip mask */
  ap_uint<32> thresh1, thresh2;

  ap_uint<16> energy;

  if(nframes>NPER) nframes = NPER;
  thresh1 = threshold1*threshold1;
  thresh2 = threshold2*threshold2;

#ifdef DEBUG
  printf("== ctofhit start ==\n");
    for(int i=0; i<NSTRIP; i++)
    {
      if(s_strip.enL[i]>0) cout<<"ctofhit: s_strip.enL["<<i<<"]="<<s_strip.enL[i]<<", s_strip.tmL["<<i<<"]="<<s_strip.tmL[i]<<endl;
      if(s_strip.enR[i]>0) cout<<"ctofhit: s_strip.enR["<<i<<"]="<<s_strip.enR[i]<<", s_strip.tmR["<<i<<"]="<<s_strip.tmR[i]<<endl;
    }
#endif


  /**********/
  /* STRIPS */

  /* shift whole pipe to the right ([1]->[2], [0]->[1]) */
  for(int j=(NPIPE-1); j>0; j--)
  {
    for(int i=0; i<NSTRIP; i++)
    {
	  strip_pipe[j].enL[i] = strip_pipe[j-1].enL[i];
	  strip_pipe[j].tmL[i] = strip_pipe[j-1].tmL[i];
	  strip_pipe[j].enR[i] = strip_pipe[j-1].enR[i];
	  strip_pipe[j].tmR[i] = strip_pipe[j-1].tmR[i];
	}
  }


  /* get NH_READS timing slices of new data and place in first interval of the pipe */
  for(int i=0; i<NSTRIP; i++)
  {
    strip_pipe[0].enL[i] = s_strip.enL[i];
    strip_pipe[0].tmL[i] = s_strip.tmL[i];
    strip_pipe[0].enR[i] = s_strip.enR[i];
    strip_pipe[0].tmR[i] = s_strip.tmR[i];
  }


#ifdef DEBUG
  for(int j=NPIPE-1; j>=0; j--)
  {
    for(int i=0; i<NSTRIP; i++)
    {
      if(strip_pipe[j].enL[i]>0) cout<<"ctofhit: strip_pipe["<<j<<"].enL["<<i<<"]="<<strip_pipe[j].enL[i]<<", strip_pipe["<<j<<"].tmL["<<i<<"]="<<strip_pipe[j].tmL[i]<<endl;
      if(strip_pipe[j].enR[i]>0) cout<<"ctofhit: strip_pipe["<<j<<"].enR["<<i<<"]="<<strip_pipe[j].enR[i]<<", strip_pipe["<<j<<"].tmR["<<i<<"]="<<strip_pipe[j].tmR[i]<<endl;
	}
  }
#endif

  /* STRIPS */
  /**********/





  /************/
  /* CLUSTERS */

  /* shift whole pipe to the right ([1]->[2], [0]->[1]) */
  for(int j=(NPIPE-1); j>0; j--)
  {
    for(int i=0; i<NSTRIP; i++)
    {
	  clust_pipe[j].enL[i] = clust_pipe[j-1].enL[i];
	  clust_pipe[j].tmL[i] = clust_pipe[j-1].tmL[i];
	  clust_pipe[j].enR[i] = clust_pipe[j-1].enR[i];
	  clust_pipe[j].tmR[i] = clust_pipe[j-1].tmR[i];
	}
  }


  /*  */
  for(int i=0; i<NSTRIP; i++)
  {
    clust_pipe[0].enL[i] = s_clust.enL[i];
    clust_pipe[0].tmL[i] = s_clust.tmL[i];
    clust_pipe[0].enR[i] = s_clust.enR[i];
    clust_pipe[0].tmR[i] = s_clust.tmR[i];
  }

#ifdef DEBUG
  for(int j=NPIPE-1; j>=0; j--)
  {
    for(int i=0; i<NSTRIP; i++)
    {
      if(clust_pipe[j].enL[i]>0) cout<<"ctofhit: clust_pipe["<<j<<"].enL["<<i<<"]="<<clust_pipe[j].enL[i]<<", clust_pipe["<<j<<"].tmL["<<i<<"]="<<clust_pipe[j].tmL[i]<<endl;
      if(clust_pipe[j].enR[i]>0) cout<<"ctofhit: clust_pipe["<<j<<"].enR["<<i<<"]="<<clust_pipe[j].enR[i]<<", clust_pipe["<<j<<"].tmR["<<i<<"]="<<clust_pipe[j].tmR[i]<<endl;
	}
  }
#endif

  /* CLUSTERS */
  /************/









  /* CLUSTERS: check for left-right coincidence within 'nframes' interval */

  for(int i=0; i<NSTRIP; i++) /* loop over all counters */
  {
    ap_uint<NPER> mask = 0;
    const int add[NPIPE] = {16, 8, 0}; /* add 16 to interval [0] and 8 to interval [1], to get consequative numbering from 0 to 23 */

    for(int j=0; j<NPIPE; j++) /* on left side, look one interval before, one current and one after, and match with middle on right side */
    {
      if( (clust_pipe[1].enR[i]*clust_pipe[j].enL[i]) > thresh2)
	  {
        tdif = (clust_pipe[j].tmL[i]+add[j])-(clust_pipe[1].tmR[i]+add[1]);
#ifdef DEBUG
        cout<<"=> tdif1="<<tdif<<endl;
#endif
        if(tdif < 0) tdif = -tdif;
#ifdef DEBUG
        cout<<"=> tdif2="<<tdif<<endl;
#endif
        if(tdif <= nframes)
		{
          int it = clust_pipe[1].tmR[i]; /* take timing from right, because it will match PCAL-U which was PMTs on the same side */
          mask(it,it) = 1;
#ifdef DEBUG
          cout<<"=> it="<<it<<", j="<<j<<", i="<<i<<endl;
#endif
		}
	  }
    }
    out[i] = mask;
  }



  /* STRIPS: check for left-right coincidence within 'nframes' interval */

  for(int i=0; i<NSTRIP; i++) /* loop over all counters */
  {
    ap_uint<NPER> mask = 0;
    const int add[NPIPE] = {16, 8, 0}; /* add 16 to interval [0] and 8 to interval [1], to get consequative numbering from 0 to 23 */

    for(int j=0; j<NPIPE; j++) /* on left side, look one interval before, one current and one after, and match with middle on right side */
    {
      if( (strip_pipe[1].enR[i]*strip_pipe[j].enL[i]) > thresh1)
	  {
        tdif = (strip_pipe[j].tmL[i]+add[j])-(strip_pipe[1].tmR[i]+add[1]);
#ifdef DEBUG
        cout<<"=> tdif1="<<tdif<<endl;
#endif
        if(tdif < 0) tdif = -tdif;
#ifdef DEBUG
        cout<<"=> tdif2="<<tdif<<endl;
#endif
        if(tdif <= nframes)
		{
          int it = strip_pipe[1].tmR[i]; /* take timing from right, because it will match PCAL-U which was PMTs on the same side */
          mask(it,it) = 1;
#ifdef DEBUG
          cout<<"=> it="<<it<<", j="<<j<<", i="<<i<<endl;
#endif
		}
	  }
    }
    output[i] = mask;
  }




  /* send trigger solution */
  /*
  for(int j=0; j<NH_READS; j++)
  {
    for(int i=0; i<NSTRIP; i++)
    {
      s_hit[j].output(i,i) = output[i](j,j);
    }
  }
  */

  for(int j=0; j<NH_READS; j++)
  {
    s_hit[j].output(0,0) = (output[0](j,j) & out[NSTRIP-1](j,j)) | (output[0](j,j) & out[0](j,j)); /* 0th counter */
    for(int i=1; i<NSTRIP; i++) /* loop over all counters starting from 1st */
    {
      s_hit[j].output(i,i) = (output[i](j,j) & out[i-1](j,j)) | (output[i](j,j) & out[i](j,j));
    }
#ifdef DEBUG
    cout<<"s_hit["<<j<<"].output="<<hex<<s_hit[j].output<<dec<<endl;
#endif
  }

#ifdef DEBUG
  printf("== ctofhit start ==\n");
#endif

}
