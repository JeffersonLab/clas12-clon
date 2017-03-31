
/* ecpeakcoord.c */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

using namespace std;
#include <strstream>


#ifdef USE_PCAL

#include "../pc.s/pclib.h"
#define ecpeakcoord pcpeakcoord

#else

#include "eclib.h"

#endif

#define DEBUG


#define NUMBER_OF_BITS				32
#define NUMBER_OF_DECIMAL_DIGITS	6
typedef ap_fixed<NUMBER_OF_BITS,(NUMBER_OF_BITS-NUMBER_OF_DECIMAL_DIGITS)> fp2403_t;


#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)
#define ABS(x)      ((x) < 0 ? -(x) : (x))


/*NPEAK=3*/
/* 3.88/40/ 1/0%/0%/1%/1% */



int
ecpeakcoord(uint8_t view, ECPeak0 peakin[NPEAK], ECPeak peakout[NPEAK])
{
#pragma HLS PIPELINE
#pragma HLS ARRAY_PARTITION variable=peakout complete dim=1
#pragma HLS ARRAY_PARTITION variable=peakin complete dim=1
  uint8_t i, j, k, strip1, nstrip, npeak, ipeak, mpeak, peak_opened;
  uint16_t tmp;
  uint16_t energy;

#ifdef DEBUG
  printf("\n\n++ ecpeakcoord ++\n");
  printf("BEFOR:\n");
  for(i=0; i<NPEAK; i++)
  {
    cout<<"peakin["<<+i<<"]: energy="<<peakin[i].energy<<", energysum4coord="<<peakin[i].energysum4coord<<", first strip="<<peakin[i].strip1<<", number of strips="<<peakin[i].stripn<<endl;
  }
#endif


  for(i=0; i<NPEAK; i++)
  {
    energy = peakin[i].energy;
    strip1 = peakin[i].strip1;

    peakout[i].energy = energy;
    peakout[i].strip1 = strip1;
    peakout[i].stripn = peakin[i].stripn;
	if(energy>0)
	{
#ifdef USE_PCAL
	  /* strip1 here from 0 */
      tmp = (uint16_t)( (fp2403_t)(peakin[i].energysum4coord * fview[view]) / (fp2403_t)energy );
      peakout[i].coord = strip1*fview[view] - UVWADD + tmp;
#else
      tmp = (uint16_t)( (fp2403_t)(peakin[i].energysum4coord<<3) / (fp2403_t)energy );
      peakout[i].coord = (strip1<<3) - 4 + tmp;
#endif
	}
  }



#ifdef DEBUG
#ifdef USE_PCAL
  printf("AFTER:\n");
  for(i=0; i<NPEAK; i++)
  {
    cout<<"peakout["<<+i<<"]: energy="<<peakout[i].energy<<", coord="<<peakout[i].coord <<"("<<peakout[i].coord/80<<","<<peakout[i].coord/fview[view]<<")"<<", first strip="<<peakout[i].strip1<<", number of strips="<<peakout[i].stripn<<endl;
  }
#endif
#endif

  return(0);
}
