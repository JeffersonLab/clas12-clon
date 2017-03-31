
/* pcal.c */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "pclib.h"

#define THRESHOLD1 ((uint16_t)1)
#define THRESHOLD2 ((uint16_t)1)
#define THRESHOLD3 ((uint16_t)3)

/* 3.60/420/72/1%/5%/13%/33% - parameters ap_fifo, internal arrays complete */
/* 3.48/351/18/11%/7%/23%/54% - same plus params complete */

uint8_t
pcal(PCStrip strip[3][PSTRIP], uint8_t *nhits, PCHit hit[PHIT])
{
#pragma HLS ARRAY_PARTITION variable=hit complete dim=1
#pragma HLS ARRAY_PARTITION variable=strip complete dim=1
#pragma HLS INTERFACE ap_memory port=hit
#pragma HLS INTERFACE ap_memory port=strip
#pragma HLS PIPELINE
  uint8_t ret, sec, uvw, npsble;
  uint8_t npeak[3];
  PCPeak peak[3][PPEAK];
  uint16_t peakfrac[3][PPEAK];
  PCHit hittmp[PHIT];
  PCPeak0 peaktmp1[PPEAKMAX];
  PCPeak0 peaktmp2[PPEAK];
  PCStrip striptmp[PSTRIPMAX];

  *nhits = 0;

  for(uvw=0; uvw<3; uvw++)
  {
    pcstrip(uvw, &strip[uvw][0], &striptmp[0]);
    npeak[uvw] = pcpeak(THRESHOLD1, uvw, &striptmp[0], &peaktmp1[0]);
    pcpeaksort(&peaktmp1[0], &peaktmp2[0]);
    pcpeakcoord(uvw, &peaktmp2[0], &peak[uvw][0]);
  }

  
  npsble = pchit(&npeak[0],&peak[0][0],&peakfrac[0][0],&hittmp[0]);

  if(npsble)
  {
    pccorr(THRESHOLD3,&npeak[0],&peak[0][0],&peakfrac[0][0],npsble,nhits,&hittmp[0],&hit[0]);
  }
  

  return(*nhits);
}
