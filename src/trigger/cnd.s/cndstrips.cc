
/* cndstrips.cc */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

//#define DEBUG

#ifdef DEBUG
#include <iostream>
using namespace std;
#endif


#include "cndtrans.h"
#include "cndlib.h"

#define TRANSLATE(ch_m) \
  energy = fadcs.fadc[isl].e##ch_m; \
  chan = ch_m; \
  str    = adcstrip[isl][chan] - 1; \
  lr     = adclr[isl][chan] - 1; \
  timexxx = ((energy >= strip_threshold) ? ((uint16_t)fadcs.fadc[isl].t##ch_m) : 0); /* error in '?' without (uint16_t) ...*/ \
  energyxxx = ((energy >= strip_threshold) ? ((uint16_t)fadcs.fadc[isl].e##ch_m) : 0); /* error in '?' without (uint16_t) ...*/ \
  timetmp[lr][str] = timexxx; \
  energytmp[lr][str] = energyxxx


/* 1.96/16/8/0%/0%/~0%(638)/~0%(519) II=8 */

/* reads one 32-ns timing slice */
void
cndstrips(ap_uint<16> strip_threshold, hls::stream<fadc_256ch_t> &s_fadcs, CNDStrip_s strip[NSTRIP])
{
//#pragma HLS INTERFACE ap_stable port=strip_threshold
//#pragma HLS DATA_PACK variable=s_fadcs
//#pragma HLS INTERFACE axis register both port=s_fadcs
#pragma HLS PIPELINE II=1

  ap_uint<13> energy;
  ap_uint<4> chan; /*0-15*/
  ap_uint<7> str; /*0-71*/
  ap_uint<1> lr; /*0-1*/

  fadc_256ch_t fadcs;

  ap_uint<3> timexxx;
  ap_uint<13> energyxxx;

  ap_uint<3> timetmp[2][NSTRIP]; /* 3 low bits for time interval (0-7), high bit to mark missing hits */
#pragma HLS ARRAY_PARTITION variable=timetmp complete dim=1
#pragma HLS ARRAY_PARTITION variable=timetmp complete dim=2
  ap_uint<13> energytmp[2][NSTRIP];
#pragma HLS ARRAY_PARTITION variable=energytmp complete dim=1
#pragma HLS ARRAY_PARTITION variable=energytmp complete dim=2

  ap_uint<13> out[2][NSTRIP];
#pragma HLS ARRAY_PARTITION variable=out complete dim=1
#pragma HLS ARRAY_PARTITION variable=out complete dim=2



#ifdef DEBUG
  printf("== cndstrips starts ==\n");fflush(stdout);
#endif


  for(int i=0; i<2; i++)
  {
    for(int j=0; j<NSTRIP; j++) timetmp[i][j] = 0;
    for(int j=0; j<NSTRIP; j++) energytmp[i][j] = 0;
    for(int j=0; j<NSTRIP; j++) out[i][j] = 0;
  }
/*
  for(int j=0; j<NSTRIP; j++)
  {
    strip[j].enL = 0;
    strip[j].tmL = 0;
    strip[j].enR = 0;
    strip[j].tmR = 0;
  }
*/

  /********************************************************************/
  /* get FADC data for 32-ns interval, and fill timetmp[2][NSTRIP] array */

  fadcs = s_fadcs.read();
  for(int isl=0; isl<NSLOT; isl++)
  {
    TRANSLATE(0);
    TRANSLATE(1);
    TRANSLATE(2);
    TRANSLATE(3);
    TRANSLATE(4);
    TRANSLATE(5);
    TRANSLATE(6);
    TRANSLATE(7);
    TRANSLATE(8);
    TRANSLATE(9);
    TRANSLATE(10);
    TRANSLATE(11);
    TRANSLATE(12);
    TRANSLATE(13);
    TRANSLATE(14);
    TRANSLATE(15);
  }

  /************************************/
  /************************************/


  /* sending translated signals */

  for(int i=0; i<NSTRIP; i++)
  {
    strip[i].enL = energytmp[0][i];
    strip[i].tmL = timetmp[0][i];
    strip[i].enR = energytmp[1][i];
    strip[i].tmR = timetmp[1][i];
#ifdef DEBUG
    if(strip[i].enL>0) cout<<"cndstrips: strip["<<i<<"].enL="<<strip[i].enL<<", strip["<<i<<"].tmL="<<strip[i].tmL<<endl;
    if(strip[i].enR>0) cout<<"cndstrips: strip["<<i<<"].enR="<<strip[i].enR<<", strip["<<i<<"].tmR="<<strip[i].tmR<<endl;
#endif
  }



#ifdef DEBUG
  printf("== cndstrips ends ==\n");fflush(stdout);
#endif

}
