
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



#define DEBUG


#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)
#define ABS(x)      ((x) < 0 ? -(x) : (x))

#define NPIPE 24 /* the number on 4ns slices to keep and use in cndhit logic; will be shifted 8 elements right on every call */

/*xc7vx550tffg1158-1*/


/* 1.96/46/8/0%/0%/(10270)1%/(5734)1% II=8 */

#define THIRD (NSTRIP/3)

void
cndhit(nframe_t nframes, CNDStrip_s s_strip[NH_READS], CNDHit s_hit[NH_READS])
{
//#pragma HLS INTERFACE ap_stable port=nframes
#pragma HLS PIPELINE II=1

  static ap_uint<NSTRIP> outL[NPIPE], outR[NPIPE];
  ap_uint<NSTRIP> output[NH_READS];
  ap_uint<THIRD> coinc12[NH_READS];
  ap_uint<THIRD> coinc23[NH_READS];
  ap_uint<THIRD> coinc123[NH_READS];

#ifdef DEBUG
  printf("== cndhit start ==\n");
  for(int j=0; j<NH_READS; j++)
  {
    if(s_strip[j].outL>0) cout<<"cndhit: s_strip["<<j<<"].outL="<<hex<<s_strip[j].outL<<dec<<endl;
    if(s_strip[j].outR>0) cout<<"cndhit: s_strip["<<j<<"].outR="<<hex<<s_strip[j].outR<<dec<<endl;
  }
#endif

  /* shift old data 8 elements to the right */
  for(int i=15; i>=0; i--)
  {
	outL[i+8] = outL[i];
	outR[i+8] = outR[i];
  }


  /* get new data */
  for(int i=0; i<NH_READS; i++)
  {
    outL[i]  = s_strip[i].outL;
    outR[i]  = s_strip[i].outR;
  }

#ifdef DEBUG
  for(int i=NPIPE-1; i>=0; i--)
  {
    if(outL[i]>0) cout<<"cndhit: outL[pipe="<<i<<"]="<<hex<<outL[i]<<dec<<endl;
    if(outR[i]>0) cout<<"cndhit: outR[pipe="<<i<<"]="<<hex<<outR[i]<<dec<<endl;
  }
#endif

  /* check for left-right coincidence withing 'nframes' interval */

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

  /* coincidence logic */
  for(int i=0; i<8; i++)
  {
    for(int j=0; j<24; j++)
	{
      int k = j*3;
      coinc12[i](j,j) = output[i](k,k) & output[i](k+1,k+1);
      coinc23[i](j,j) = output[i](k+1,k+1) & output[i](k+2,k+2);
      coinc123[i](j,j) = output[i](k,k) & output[i](k+1,k+1) & output[i](k+2,k+2);
	}
    output[i] = (coinc123[i], coinc23[i], coinc12[i]);
#ifdef DEBUG
    if(coinc12[i]>0)  cout<<"cndhit: coinc12[pipe="<<i<<"]="<<hex<<coinc12[i]<<dec<<endl;
    if(coinc23[i]>0)  cout<<"cndhit: coinc23[pipe="<<i<<"]="<<hex<<coinc23[i]<<dec<<endl;
    if(coinc123[i]>0) cout<<"cndhit: coinc123[pipe="<<i<<"]="<<hex<<coinc123[i]<<dec<<endl;
    if(output[i]>0)   cout<<"cndhit: output[pipe="<<i<<"]="<<hex<<output[i]<<dec<<endl;
#endif
  }

  /* send trigger solution */
  for(int j=0; j<NH_READS; j++)
  {
#ifdef DEBUG
    if(output[j]>0) cout<<"cndhit: output["<<j<<"]="<<hex<<output[j]<<dec<<endl;
#endif
    s_hit[j].output = output[j];
  }
}
