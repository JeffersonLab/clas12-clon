
/* ecpeakcoord.c */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

using namespace std;
#include <strstream>

#include "eclib.h"


#include <ap_fixed.h>
#define NUMBER_OF_BITS				24
#define NUMBER_OF_DECIMAL_DIGITS	3
typedef ap_fixed<NUMBER_OF_BITS,(NUMBER_OF_BITS-NUMBER_OF_DECIMAL_DIGITS)> fp2403_t;


#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)
#define ABS(x)      ((x) < 0 ? -(x) : (x))


/*NPEAK=3*/
/* 3.88/40/ 1/0%/0%/1%/1% */

#undef DEBUG


int
ecpeakcoord(ECPeak0 peakin[NPEAK], ECPeak peakout[NPEAK])
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
    printf("peakin[%2d]: energy=%d, energysum4coord=%d, first strip=%d, number of strips=%d\n",
      i,peakin[i].energy,peakin[i].energysum4coord,peakin[i].strip1,peakin[i].stripn);
  }
#endif


  for(i=0; i<NPEAK; i++)
  {
    energy = peakin[i].energy;
    strip1 = peakin[i].strip1;

    peakout[i].energy = energy;
    /*peakout[i].time = peakin[i].time;*/
    peakout[i].strip1 = strip1;
    peakout[i].stripn = peakin[i].stripn;
	if(energy>0)
	{
	  /*
      fp2403_t temp;
      temp = ( (fp2403_t)(peakin[i].energysum4coord<<3) / (fp2403_t)energy );
	  printf("##1## %d\n",((peakin[i].energysum4coord)<<3)/energy);
	  cout << "temp= " << temp << endl;
	  */
      tmp = (uint16_t)( (fp2403_t)(peakin[i].energysum4coord<<3) / (fp2403_t)energy );
      peakout[i].coord = (strip1<<3) - 4 + tmp;
	}
  }



#ifdef DEBUG
  printf("AFTER:\n");
  for(i=0; i<NPEAK; i++)
  {
    printf("peakout[%2d]: energy=%d, coord=%d, first strip=%d, number of strips=%d\n",
      i,peakout[i].energy,peakout[i].coord,peakout[i].strip1,peakout[i].stripn);
  }
#endif

  return(0);
}
