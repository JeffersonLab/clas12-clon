
/* ecfracratio.cc */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

using namespace std;
#include <strstream>

#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)
#define ABS(x)      ((x) < 0 ? -(x) : (x))

#include "eclib.h"
#include "ecal_atten.h"

#include <ap_fixed.h>
#define NUMBER_OF_BITS_DECIMAL	 18
#define NUMBER_OF_BITS_FRACTIONAL 4
typedef ap_ufixed<(NUMBER_OF_BITS_DECIMAL+NUMBER_OF_BITS_FRACTIONAL),NUMBER_OF_BITS_DECIMAL> fp2403_t;


#undef DEBUG


#define FACTOR1 256/*128*/
#define SHIFT1  8


/*xc7vx550tffg1158-1*/

/* 3.92/401/1/0%/0%/8%/9% */


void
ecfracratio(uint8_t edge, uint8_t peakID, uint8_t NhitsINpeak, uint8_t peak_id[3][NHIT], uint16_t energy[3][NPEAK], uint8_t nhits[3][NPEAK], uint8_t hitid[NHIT], uint32_t frac[NHIT])
{
#pragma HLS PIPELINE
#pragma HLS ARRAY_PARTITION variable=peak_id complete dim=1
#pragma HLS ARRAY_PARTITION variable=peak_id complete dim=2
#pragma HLS ARRAY_PARTITION variable=energy complete dim=1
#pragma HLS ARRAY_PARTITION variable=energy complete dim=2
#pragma HLS ARRAY_PARTITION variable=nhits complete dim=1
#pragma HLS ARRAY_PARTITION variable=nhits complete dim=2
#pragma HLS ARRAY_PARTITION variable=hitid complete dim=1
#pragma HLS ARRAY_PARTITION variable=frac complete dim=1
  uint8_t ihit, axis;
  uint8_t hitID, peak_ID;

  uint8_t Nvalid;
  fp2403_t fractmp;
  fp2403_t sumE0;
  fp2403_t sumE[NHIT];
#pragma HLS ARRAY_PARTITION variable=sumE complete dim=1

  sumE0 = 0.0;

  /* loop over hits, contributed into current peak */
  for(ihit=0; ihit<NHIT; ihit++)
  {
    if(ihit>=NhitsINpeak) continue;
	hitID = hitid[ihit];       /* hit ID */
#ifdef DEBUG
	printf("      !!!    edge=%d peakID=%d ihit=%d hitID=%d\n",edge,peakID,ihit,hitID);
#endif
    sumE[ihit] = 0;
    Nvalid = 0;

	/* find peaks on other two edges that are involved in this hit only */
    for(axis = 0; axis<3; axis++)
	{
      if(axis != edge)                 /* considering 2 other views */
  	  {
		peak_ID = peak_id[axis][hitID];   /* peak ID for that hit in that view */

#ifdef DEBUG
	    printf("      !!!       -> axis=%d peak_ID=%d nhits=%d\n",axis,peak_ID,nhits[axis][peak_ID]);
#endif
        if(nhits[axis][peak_ID] == 1)
		{
          Nvalid = Nvalid + 1;
		  sumE[ihit] += energy[axis][peak_ID];
#ifdef DEBUG
          cout << "      !!!          -> energy=" << energy[axis][peak_ID] << " -> sumE=" << sumE[ihit] << endl;
#endif
        }
      }
    }

    /* calculate energy sum for normalization */
    /*if(Nvalid > 0.0) sumE[ihit] /= Nvalid;*/
    if(Nvalid == 2) sumE[ihit] /= (fp2403_t)2.0;

    sumE0 += sumE[ihit];
#ifdef DEBUG
	cout << "      !!!    edge=" << edge << " Nvalid=" << Nvalid <<" sumE0=" << sumE0 << endl;
#endif
  }


  /* loop over hits again normalizing energy */
  if(sumE0 > 0)
  {
    for(ihit=0; ihit<NHIT; ihit++)
	{
      if(ihit>=NhitsINpeak) continue;
      hitID = hitid[ihit];       /* hit ID */
      fractmp = sumE[ihit] / sumE0;
      frac[hitID] = (uint32_t)(fractmp * FACTOR1);
#ifdef DEBUG
	  cout << "      !!!       -> sumE=" << sumE[ihit] << " sumE0=" << sumE0 << " -> fractmp=" << fractmp << endl;
#endif
	}
  }

  return;
}
