
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

//#define DEBUG



#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)
#define ABS(x)      ((x) < 0 ? -(x) : (x))





/* 4.87/41/ 4/0%/(8)~0%/(9896)1%/(6594)1% II=4 */

void
ecpeakcoord(ap_uint<2> view, hls::stream<ECPeak0_s> &s_peak0, hls::stream<ECPeak_s> &s_peak)
{
#pragma HLS DATA_PACK variable=s_peak
#pragma HLS INTERFACE axis register both port=s_peak
#pragma HLS DATA_PACK variable=s_peak0
#pragma HLS INTERFACE axis register both port=s_peak0
#pragma HLS INTERFACE ap_stable port=view
#pragma HLS PIPELINE II=4

  uint8_t i, j, k, strip1, nstrip, npeak, ipeak, mpeak, peak_opened, iview;
  uint16_t tmp;
  uint16_t energy;

  ECPeak0 peakin;
  ECPeak0_s fifo;

  ECPeak peakout;
  ECPeak_s fifo1;

  iview = (uint8_t)view;

#ifdef DEBUG
  printf("\n\n++ ecpeakcoord ++ (iview=%d)\n",iview);
#endif

  for(int i=0; i<NPEAK; i++)
  {
	fifo = s_peak0.read();

    peakin.energy = fifo.energy;
    peakin.energysum4coord = fifo.energysum4coord;
    peakin.strip1 = fifo.strip1;
    peakin.stripn = fifo.stripn;

#ifdef DEBUG
    printf("BEFOR:\n");
    cout<<"peakin["<<+i<<"]: energy="<<peakin.energy<<", energysum4coord="<<peakin.energysum4coord<<", first strip="<<peakin.strip1<<", number of strips="<<peakin.stripn<<endl;
#endif



    energy = peakin.energy;
    strip1 = peakin.strip1;
    peakout.energy = energy;
	if(energy>0)
	{
	  /* strip1 here from 0 */
	  myfp_t tmp1, tmp2, tmp3;
      tmp1 = ( ((myfp_t)peakin.energysum4coord) * fview[view]);
      tmp2 = tmp1 / (myfp_t)energy;
      tmp3 = ((myfp_t)strip1)*fview[view];
	  peakout.coord = (ap_uint<NBIT_COORD>)(tmp3 - fhalf[view] + tmp2);
	}
#ifndef __SYNTHESIS__
    peakout.strip1 = strip1;
    peakout.stripn = peakin.stripn;
#endif


#ifdef DEBUG
    printf("AFTER:\n");
    cout<<"peakout["<<+i<<"]: energy="<<peakout.energy<<", coord="<<peakout.coord <<"("<<peakout.coord/fview[view]<<")"<<endl;
#endif

    fifo1.energy = peakout.energy;
    fifo1.coord = peakout.coord;
#ifndef __SYNTHESIS__
    fifo1.strip1 = peakout.strip1;
    fifo1.stripn = peakout.stripn;
#endif

	s_peak.write(fifo1);
  }

}
