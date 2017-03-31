/* ecfrac.c - energy fractions calculation

  input:  

  output: 
*/


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



#undef DEBUG


#define FACTOR1 256/*128*/
#define SHIFT1  8


/*xc7vx550tffg1158-1*/


/* 3.92/950/4/0%/0%/40%/32% */





void
ecfrac(uint8_t npeak[3], ECPeak1 peak1[3][NPEAK], uint8_t npsble, ECHit hit[NHIT], uint32_t hitfrac[3][NHIT])
{
#pragma HLS PIPELINE
#pragma HLS ARRAY_PARTITION variable=npeak complete dim=1
#pragma HLS ARRAY_PARTITION variable=peak1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=peak1 complete dim=2
#pragma HLS ARRAY_PARTITION variable=hit complete dim=1
#pragma HLS ARRAY_PARTITION variable=hitfrac complete dim=1
#pragma HLS ARRAY_PARTITION variable=hitfrac complete dim=2

  uint8_t i, j, k, kk, l, ith, edge, ihit, ipeak, peakID, NhitsINpeak, peakn, npks, nhit, peak_ID;

  uint8_t peak_id[3][NHIT];
#pragma HLS ARRAY_PARTITION variable=peak_id complete dim=1
#pragma HLS ARRAY_PARTITION variable=peak_id complete dim=2

  uint16_t energy[3][NPEAK];
#pragma HLS ARRAY_PARTITION variable=energy complete dim=1
#pragma HLS ARRAY_PARTITION variable=energy complete dim=2

  uint8_t nhits[3][NPEAK];
#pragma HLS ARRAY_PARTITION variable=nhits complete dim=1
#pragma HLS ARRAY_PARTITION variable=nhits complete dim=2

  uint8_t hitid[3][NPEAK][NHIT];
#pragma HLS ARRAY_PARTITION variable=hitid complete dim=1
#pragma HLS ARRAY_PARTITION variable=hitid complete dim=2
#pragma HLS ARRAY_PARTITION variable=hitid complete dim=3

  uint32_t frac[3][NHIT];
#pragma HLS ARRAY_PARTITION variable=frac complete dim=1
#pragma HLS ARRAY_PARTITION variable=frac complete dim=2



#ifdef DEBUG
  printf("\n\n\n+++ eccorr +++\n\n\n");
#endif

  nhit = npsble;

  /* loop for all hits */

#ifdef DEBUG
  printf("entering loop over hits\n");
#endif


  /*
Registered: \u200e10-24-2013
Re: Unable to enforce a carried dependency constraint?
Options

\u200e04-08-2014 12:49 PM

The problem is that the array is stored in a single large memory.  The code is attempting to do two writes and a read on a two port memory.

The fix is pretty easy.  You need to partition the array into multiple sub-arrays with pragmas like this:

#pragma HLS ARRAY_PARTITION variable=lines complete dim=1

#pragma HLS ARRAY_PARTITION variable=lines block factor=10 dim=2

 This will make the 2x1024 single array into 20 1x102 arrays (some are 1x103).  That way, each block of memory is on a separate BRAM so access can happen simultaneously.
  */



  for(i=0; i<3; i++) for(j=0; j<NHIT;  j++) peak_id[i][j] = hit[j].peak1[i];
  for(i=0; i<3; i++) for(j=0; j<NPEAK; j++) energy[i][j] = peak1[i][j].energy;
  for(i=0; i<3; i++) for(j=0; j<NPEAK; j++) nhits[i][j] = peak1[i][j].nhits;
  for(i=0; i<3; i++) for(j=0; j<NPEAK; j++) for(k=0; k<NHIT; k++) hitid[i][j][k] = peak1[i][j].hitid[k];


  /********************/
  /* loop for 3 edges */
  for(edge=0; edge<3; edge++)
  {

	/*********************************/
    /* loop over all peaks in 'edge' */
    for(peakID=0; peakID<NPEAK; peakID++)
    {
      if(peakID>=npeak[edge]) continue;
      NhitsINpeak = nhits[edge][peakID]; /* the number of hits where this peak participates */

#ifdef DEBUG
	  printf("      !!! edge=%d peakID=%d: NhitsINpeak=%d\n",edge,peakID,NhitsINpeak);
#endif

      if(NhitsINpeak > 1) /***** if the number of participating hits in current peak more then 1 *****/
	  {
        ecfracratio(edge, peakID, NhitsINpeak, peak_id, energy, nhits, hitid[edge][peakID], frac[edge]);
	  } /***** if the number of participating hits in current peak more then 1 *****/

	}
    /* loop over all peaks in 'edge' */
	/*********************************/

  }
  /* loop for 3 edges */
  /********************/




  /* fill output array */
  for(ihit=0; ihit<NHIT; ihit++)
  {
    if(ihit>=nhit) continue;

    for(edge=0; edge<3; edge++)
    {
      peak_ID = hit[ihit].peak1[edge];
      if(peak1[edge][peak_ID].nhits == 1)
	  {
        hitfrac[edge][ihit] = 1 * FACTOR1;
#ifdef DEBUG
		printf("      !!! 1: hitfrac=%d\n",hitfrac[edge][ihit]);
#endif
	  }
      else
	  {
        hitfrac[edge][ihit] = frac[edge][ihit];
#ifdef DEBUG
		printf("      !!! 2: hitfrac=%d\n",hitfrac[edge][ihit]);
#endif
	  }
    }
  }  






  return;
}


