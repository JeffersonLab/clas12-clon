
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
ftofhit(ap_uint<16> threshold, nframe_t nframes, FTOFStrip_s s_strip[NH_READS], FTOFHit s_hit[NH_READS])
{
//#pragma HLS INTERFACE ap_stable port=nframes
#pragma HLS PIPELINE II=1

  static ap_uint<NSTRIP> outL[NPIPE], outR[NPIPE];

  ap_uint<NSTRIP> output[NH_READS];

#ifdef DEBUG
  printf("== ftofhit start ==\n");
  for(int j=0; j<NH_READS; j++)
  {
    if(s_strip[j].outL>0) cout<<"ftofhit: s_strip["<<j<<"].outL="<<hex<<s_strip[j].outL<<dec<<endl;
    if(s_strip[j].outR>0) cout<<"ftofhit: s_strip["<<j<<"].outR="<<hex<<s_strip[j].outR<<dec<<endl;
  }
#endif

  /* shift old data 8 elements to the right */
  for(int i=15; i>=0; i--)
  {
	outL[i+8] = outL[i];
	outR[i+8] = outR[i];
  }


  /* get new data */
  for(int j=0; j<NH_READS; j++)
  {
    outL[j]  = s_strip[j].outL;
    outR[j]  = s_strip[j].outR;
  }


#ifdef DEBUG
  for(int i=NPIPE-1; i>=0; i--)
  {
    if(outL[i]>0) cout<<"ftofhit: outL[pipe="<<i<<"]="<<hex<<outL[i]<<dec<<endl;
    if(outR[i]>0) cout<<"ftofhit: outR[pipe="<<i<<"]="<<hex<<outR[i]<<dec<<endl;
  }
#endif



  /* check for left-right coincidence within 'nframes' interval */

  if(nframes>NPER) nframes = NPER;
  for(int i=8; i<16; i++) /* take middle interval left PMTs, and compare with +-nframes right PMTs */
  {
    output[i-8] = 0;
    for(int j=i-NPER; j<=i+NPER; j++)
	{
      if(j<(i-nframes)) continue;
      if(j>(i+nframes)) continue;
      output[i-8] |= outL[i] & outR[j];
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
