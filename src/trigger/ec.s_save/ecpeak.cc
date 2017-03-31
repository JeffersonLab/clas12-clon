
/* ecpeak.c - looking for peaks in particular layer

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

#include <iostream>
using namespace std;

#include <ap_int.h>

#include "eclib.h"


#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)
#define ABS(x)      ((x) < 0 ? -(x) : (x))


/* 3.48/47/18/0%/1%/0%/2% */


#define DEBUG

#define OPEN_PEAK \
{ \
  strip1 = i-1; \
}

#define FILL_PEAK \
{ \
  energysum4coord += (nstrip+1)*en;			\
  nstrip++; \
  energysum += en; \
}


#define CLOSE_PEAK \
{ \
  ii=i-1; \
  /*cout << "!!!!===!!!! " << +(i-1) << " " << +ii << " strip1=" << +strip1 << endl;*/ \
  peak_strip1[ii]          = strip1; \
  peak_stripn[ii]          = nstrip; \
  peak_energy[ii]          = energysum; \
  peak_energysum4coord[ii] = energysum4coord; \
  energysum4coord=0; \
  strip1 = 0; \
  nstrip = 0; \
  energysum = 0; \
}

/*xc7vx550tffg1158-1*/


int
ecpeak(uint16_t strip_threshold, ECStrip strip[NSTRIP], ECPeak0 peak[NSTRIP])
{
#pragma HLS PIPELINE
//#pragma HLS ARRAY_PARTITION variable=strip complete dim=1
//#pragma HLS ARRAY_PARTITION variable=peak complete dim=1

  ap_uint<8>  i, ii, ii_prev, strip1, nstrip, npeak;
  ap_uint<1>  peak_opened;
  ap_uint<13> en;
  ap_uint<16> energysum;
  ap_uint<32> energysum4coord;

  ap_uint<16> energy0, energy1;
  ap_uint<13> energy[NSTRIP+2];
#pragma HLS ARRAY_PARTITION variable=energy complete dim=1

  ap_uint<1> first[NSTRIP+2];
#pragma HLS ARRAY_PARTITION variable=first complete dim=1

  ap_uint<1> middle[NSTRIP+2];
#pragma HLS ARRAY_PARTITION variable=middle complete dim=1

  ap_uint<1> last[NSTRIP+2];
#pragma HLS ARRAY_PARTITION variable=last complete dim=1

  ap_uint<8>  peak_strip1[NSTRIP];
#pragma HLS ARRAY_PARTITION variable=peak_strip1 complete dim=1
  ap_uint<8>  peak_stripn[NSTRIP];
#pragma HLS ARRAY_PARTITION variable=peak_stripn complete dim=1
  ap_uint<16> peak_energy[NSTRIP];
#pragma HLS ARRAY_PARTITION variable=peak_energy complete dim=1
  ap_uint<32> peak_energysum4coord[NSTRIP];
#pragma HLS ARRAY_PARTITION variable=peak_energysum4coord complete dim=1


#ifdef DEBUG
  printf("\n+++ ecpeak +++\n");
  printf("ecpeak: strip_threshold=%d\n",strip_threshold);
#endif

  energy[0] = 0;
  for(i=1; i<=NSTRIP; i++)
  {
    energy[i] = strip[i-1].energy;
  }
  energy[NSTRIP+1] = 0;


  for(i=0; i<NSTRIP+2; i++)
  {
    first[i] = 0;
    middle[i] = 0;
    last[i] = 0;
  }

  /* found first */
  for(i=1; i<=NSTRIP; i++)
  {
	energy0 = energy[i-1];
	energy1 = energy[i-1] + (energy[i-1]>>2);
    if((energy[i-1]<=strip_threshold && energy[i]>strip_threshold) ||
       (energy[i-2]>energy0 && energy1<energy[i]) ||
	   (energy[i-2]>energy1 && energy0<energy[i]))
	{
      first[i] = 1;
	}
  }

  /* found last */
  for(i=1; i<=NSTRIP; i++)
  {
	energy0 = energy[i];
	energy1 = energy[i] + (energy[i]>>2);
    if((energy[i]>strip_threshold && energy[i+1]<=strip_threshold) ||
       (energy0>strip_threshold && energy[i-1]>energy0 && energy1<energy[i+1]) ||
	   (energy0>strip_threshold && energy[i-1]>energy1 && energy0<energy[i+1]))
	{
      last[i] = 1;
	}
  }

  /* fill middle */
  peak_opened = 0;
  for(i=1; i<=NSTRIP; i++)
  {
    if(peak_opened)
	{
      middle[i] = 1;
	}

    if(first[i])
	{
      middle[i] = 1;
      peak_opened = 1;
	}

    if(last[i])
	{
      middle[i] = 1;
      peak_opened = 0;
	}
  }  


#ifdef DEBUG
  for(i=1; i<=NSTRIP; i++) std::cout<<"strip="<<+i<<" energy="<<+energy[i]<<" first="<<+first[i]<<" middle="<<+middle[i]<<" last="<<+last[i]<<endl;
#endif


  /* form peaks */
  strip1 = 0;
  nstrip = 0;
  energysum = 0;
  energysum4coord=0;
  for(i=0; i<NSTRIP; i++) peak_energy[i] = 0;
  for(i=1; i<=NSTRIP; i++)
  {
    en = energy[i];

    if(first[i])
	{
      OPEN_PEAK;
	}

    if(middle[i])
	{
      FILL_PEAK;
	}

    if(last[i])
	{
      CLOSE_PEAK;
	}
  }

#ifdef DEBUG
  for(i=0; i<NSTRIP; i++)
  {
	std::cout << "pppp[" << +i << "]: energy=" << +peak_energy[i] << " energysum4coord=" << +peak_energysum4coord[i] << " first strip=" << +peak_strip1[i] << " number of strips=" << +peak_stripn[i] << endl;
  }
#endif

  npeak = 0;
  for(i=0; i<NSTRIP; i++)
  {
    peak[i].strip1   = peak_strip1[i];
    peak[i].stripn   = peak_stripn[i];
    peak[i].energy   = peak_energy[i];
    peak[i].energysum4coord = peak_energysum4coord[i];
    if(peak[i].energy > 0) npeak++;
  }

#ifdef DEBUG
  std::cout << "npeak=" << +npeak << endl;
  for(i=0; i<NSTRIP; i++)
  {
	std::cout << "peak[" << +i << "]: energy=" << +peak[i].energy << " energysum4coord=" << +peak[i].energysum4coord;
	std::cout << " first strip=" << +peak[i].strip1 << " number of strips=" << +peak[i].stripn << endl;
  }
#endif

  return(npeak);
}
