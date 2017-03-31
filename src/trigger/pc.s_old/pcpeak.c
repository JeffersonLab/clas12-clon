
/* pcpeak.c - looking for peaks in particular layer

  input:  strip[].energy - strip energy (MeV)
          strip[].time   - strip time (ns)
          threshold      - 
          gap            - 
          min            - 
          max            - 

  output: npeak  - the number of peaks obtained
          peak   - peaks information
*/

	


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "pclib.h"

#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)
#define ABS(x)      ((x) < 0 ? -(x) : (x))


/* 3.48/280/89/0%/2%/2%/4% */
/* 3.39/199/ 1/0%/2%/17%/89% - i/o arrays: complete dim=1 */
/* 3.39/292/86/0%/2%/4%/7% - i/o arrays: block factor=2 dim=1 */

#undef DEBUG


#define OPEN_PEAK \
  if(peak_opened == 0) strip1 = ii; \
  energysum4coord += nstrip*energy; \
  nstrip++; \
  energysum += energy; \
  peak_opened = 1

#define CLOSE_PEAK \
  peak0[npeak].strip1   = strip1; \
  peak0[npeak].stripn   = nstrip; \
  peak0[npeak].energy   = energysum; \
  peak0[npeak].energysum4coord = energysum4coord; \
  energysum4coord=0; \
  peak_opened = 0; \
  npeak++; \
  strip1 = 0; \
  nstrip = 0; \
  energysum = 0




int
pcpeak(uint16_t strip_threshold, uint8_t view, PCStrip strip[PSTRIPMAX], PCPeak0 peak0[PPEAKMAX])
{
#pragma HLS ARRAY_PARTITION variable=strip block factor=2 dim=1
#pragma HLS ARRAY_PARTITION variable=peak0 block factor=2 dim=1
#pragma HLS PIPELINE
  uint16_t ii, jj, strip1, nstrip, npeak, peak_opened;
  uint16_t energy;
  uint32_t energysum, energysum4coord;

#ifdef DEBUG
  printf("\n+++ pcpeak +++\n");
  printf("pcpeak: strip_threshold=%d\n",strip_threshold);
#endif


  peak_opened = 0;
  npeak  = 0;
  strip1 = 0;
  nstrip = 0;
  energysum = 0;
  energysum4coord=0;

  for(ii=0; ii<PPEAKMAX; ii++) peak0[ii].energy = 0;

  for(ii=0; ii<PSTRIPMAX; ii++)
  {
    energy = strip[ii].energy;
#ifdef DEBUG
	printf("strip[%d]: energy=%d (peak_opened=%d)\n",ii,energy,peak_opened);
#endif
    if(energy>strip_threshold)
    {
      OPEN_PEAK;
	}
    else if(peak_opened==1)
    {
	  CLOSE_PEAK;
    }
  }

  /* peak still open after the loop - close it */
  if(peak_opened==1)
  {
#ifdef DEBUG
    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
#endif
	CLOSE_PEAK;
  }

#ifdef DEBUG
  printf("npeak=%d\n",npeak);
  for(ii=0; ii<npeak; ii++)
  {
    printf("peak[%2d]: energy=%d, energysum4coord=%d, first strip=%d, number of strips=%d\n",ii,peak[ii].energy,peak[ii].energysum4coord,peak[ii].strip1,peak[ii].stripn);
  }
#endif

  return(npeak);
}
