
/* ecal.c */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "eclib.h"

#define THRESHOLD1 ((uint16_t)1)
#define THRESHOLD2 ((uint16_t)1)
#define THRESHOLD3 ((uint16_t)3)

/* 3.60/420/72/1%/5%/13%/33% - parameters ap_fifo, internal arrays complete */
/* 3.48/351/18/11%/7%/23%/54% - same plus params complete */

uint8_t
ecal(ECStrip strip[3][36], uint8_t *nhits, ECHit hit[NHIT])
{
#pragma HLS ARRAY_PARTITION variable=hit complete dim=1
#pragma HLS ARRAY_PARTITION variable=strip complete dim=1
#pragma HLS INTERFACE ap_memory port=hit
#pragma HLS INTERFACE ap_memory port=strip
#pragma HLS PIPELINE
  uint8_t ret, sec, uvw, npsble;
  uint8_t npeak[3];
  ECPeak0 peaktmp1[NSTRIP];
  ECPeak0 peaktmp2[NPEAKMAX];
  ECPeak peak[3][NPEAK];
  ECPeak1 peak1[3][NPEAK];
  uint32_t hitfrac[3][NHIT];

  ECHit hittmp[NHIT];

  *nhits = 0;

  for(uvw=0; uvw<3; uvw++)
  {
    npeak[uvw] = ecpeak(THRESHOLD1, &strip[uvw][0], &peaktmp1[0]);
    ecpeaksort(&peaktmp1[0], &peaktmp2[0]);
    ecpeakcoord(&peaktmp2[0], &peak[uvw][0]);
  }

  npsble = echit(npeak,peak,peak1,&hittmp[0]);
  if(npsble)
  {
    ecfrac(&npeak[0], peak1, npsble, &hittmp[0], hitfrac);
    eccorr(THRESHOLD3,npeak,peak,peak1,npsble,nhits,&hittmp[0],hitfrac,&hit[0]);
  }

  return(*nhits);
}
