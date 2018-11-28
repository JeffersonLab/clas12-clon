
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


inline void
product(ap_uint<16> threshold, ap_uint<13> enL[NSTRIP], ap_uint<13> enR[NSTRIP], ap_uint<NSTRIP> &output)
{
#pragma HLS PIPELINE II=1

  ap_uint<NSTRIP> mask = 0;

  for(int i=0; i<NSTRIP; i++)
  {
    //if( (enL[i]*enR[i]) > threshold) mask(i,i) |= 1;
    //if( (enL[i]+enR[i]) > threshold) mask(i,i) |= 1;
    if( (enL[i]>threshold) &&(enR[i]>threshold) ) mask(i,i) = 1;
  }

  output |= mask;
}


/* 1.96/46/8/0%/0%/(10270)1%/(5734)1% II=8 */

void
ftofhit(ap_uint<16> threshold, nframe_t nframes, FTOFStrip_s s_strip[NH_READS], FTOFHit s_hit[NH_READS])
{
//#pragma HLS INTERFACE ap_stable port=nframes
#pragma HLS PIPELINE II=1
  nframe_t nframes_half;

  static ap_uint<13> enL[NPIPE][NSTRIP], enR[NPIPE][NSTRIP];

  ap_uint<NSTRIP> output[NH_READS];


#ifdef DEBUG
  printf("== ftofhit start ==\n");
  for(int j=0; j<NH_READS; j++)
  {
    for(int i=0; i<NSTRIP; i++)
    {
      if(s_strip[j].enL[i]>0) cout<<"ftofhit: s_strip["<<j<<"].enL["<<i<<"]="<<hex<<s_strip[j].enL[i]<<dec<<endl;
      if(s_strip[j].enR[i]>0) cout<<"ftofhit: s_strip["<<j<<"].enR["<<i<<"]="<<hex<<s_strip[j].enR[i]<<dec<<endl;
    }
  }
#endif

  /* shift old NH_READS*2 timing slices to the right */
  for(int j=(NH_READS*2-1); j>=0; j--)
  {
    for(int i=0; i<NSTRIP; i++)
    {
	  enL[j+8][i] = enL[j][i];
	  enR[j+8][i] = enR[j][i];
	}
  }


  /* get NH_READS timing slices of new data */
  for(int j=0; j<NH_READS; j++)
  {
    for(int i=0; i<NSTRIP; i++)
    {
      enL[j][i] = s_strip[j].enL[i];
      enR[j][i] = s_strip[j].enR[i];
	}
  }


#ifdef DEBUG
  for(int j=NPIPE-1; j>=0; j--)
  {
    for(int i=0; i<NSTRIP; i++)
    {
      if(enL[j][i]>0) cout<<"ftofhit: enL[pipe="<<j<<"]["<<i<<"]="<<hex<<enL[j][i]<<dec<<endl;
      if(enR[j][i]>0) cout<<"ftofhit: enR[pipe="<<j<<"]["<<i<<"]="<<hex<<enR[j][i]<<dec<<endl;
	}
  }
#endif



  /* check for left-right coincidence within 'nframes' interval */

  nframes_half = nframes/2;
  if(nframes_half>(NPER/2)) nframes_half = NPER/2;

  for(int j=NH_READS; j<NH_READS*2; j++) /* take middle interval left PMTs, and compare with +-nframes_half right PMTs */
  {
    output[j-8] = 0;
    for(int k=j-(NPER/2); k<=j+(NPER/2); k++)
	{
      if(k<(j-nframes_half)) continue;
      if(k>(j+nframes_half)) continue;


      ap_uint<NSTRIP> mask = 0;

      for(int i=0; i<NSTRIP; i++)
      {
        //if( (enL[j][i]*enR[k][i]) > threshold) mask(i,i) = 1;
        if( (enL[j][i]+enR[k][i]) > threshold) mask(i,i) = 1;
        //if( (enL[j][i]>threshold) &&(enR[k][i]>threshold) ) mask(i,i) = 1;
      }

      output[j-8] |= mask;

  /*
      product(threshold, enL[j], enR[k], output[j-8]);
  */
	}
  }


  /* send trigger solution */
  for(int j=0; j<NH_READS; j++)
  {
#ifdef DEBUG
    if(output[j]>0) cout<<"ftofhit: output["<<j<<"]="<<hex<<output[j]<<dec<<endl;
#endif
    s_hit[j].output = output[j];
  }
}
