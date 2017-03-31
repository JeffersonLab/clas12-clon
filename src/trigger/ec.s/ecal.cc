
/* ecal.c */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>


#ifdef USE_PCAL

#include "../pc.s/pclib.h"
#define ecal pcal
#define ecstrip pcstrip
#define ecpeak pcpeak
#define ecpeaksort pcpeaksort
#define ecpeakcoord pcpeakcoord
#define echit pchit
#define ecfrac pcfrac
#define eccorr pccorr
#define echitsort pchitsort

#else

#include "eclib.h"

#endif


#define THRESHOLD1 ((uint16_t)1)
#define THRESHOLD2 ((uint16_t)1)
#define THRESHOLD3 ((uint16_t)3)


/* 3.48/150/2/69%/15%/21%/50% - just copy in echitsort, NHIT=NHITMAX */

uint8_t
ecal(ECStrip strip[3][NSTRIP], ECHit hit[NHIT])
{
#pragma HLS PIPELINE
#pragma HLS ARRAY_PARTITION variable=hit complete dim=1
#pragma HLS ARRAY_PARTITION variable=strip complete dim=1
//#pragma HLS INTERFACE ap_memory port=hit
//#pragma HLS INTERFACE ap_memory port=strip

  uint8_t ii, nhits, uvw, npsble;
  ECPeak0 peaktmp1[NSTRIP];
  ECPeak0 peaktmp2[NPEAKMAX];
  ECPeak peak[3][NPEAK];
  ECStrip striptmp[NSTRIP];

  ap_uint<16> energy[NHITMAX][3];
  ap_uint<16> coord[NHITMAX][3];

  fp0201S_t peakcount[3][NPEAK];

  ap_uint<16> enhit[NHITMAX];
  uint16_t frac[NHITMAX][3];

  nhits = 0;
  for(uvw=0; uvw<3; uvw++)
  {
    ecstrip(uvw, strip[uvw], striptmp);
    ecpeak(THRESHOLD1, striptmp, peaktmp1);
    ecpeaksort(peaktmp1, peaktmp2);
    ecpeakcoord(uvw, peaktmp2, peak[uvw]);
  }

  npsble = echit(peak, peakcount, energy, coord);
  if(npsble>0)
  {
    ecfrac(peakcount, energy, frac);
    eccorr(THRESHOLD3, energy, coord, frac, enhit);
    nhits = echitsort(enhit, coord, hit);

	/* nhits may not be returned ! 
    nhits = 0;
    for(ii=0; ii<NHIT; ii++)
    {
      if(hit[ii].energy>0) nhits++;
    }
	*/
  }

  return(nhits);
}
