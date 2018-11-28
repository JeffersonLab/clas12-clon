
/* pcustrips.cc */

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


#include "pcutrans.h"
#include "pculib.h"

#define TRANSLATE(ch_m) \
  energy = fadcs.fadc[isl].e##ch_m; \
  chan = ch_m; \
  str    = adcstrip[isl][chan] - 1; \
  timexxx = ((energy >= strip_threshold) ? ((uint8_t)fadcs.fadc[isl].t##ch_m) : 0); /* error in '?' without (uint16_t) ...*/ \
  energyxxx = ((energy >= strip_threshold) ? ((uint8_t)fadcs.fadc[isl].e##ch_m) : 0); /* error in '?' without (uint16_t) ...*/ \
  timetmp[str] = timexxx;	\
  energytmp[str] = energyxxx

  /*if(lay==0 && energy>0) cout<<"pcustrips: energy[lay="<<lay<<"][str="<<str<<"][lr="<<lr<<"]="<<energy<<endl; \
	if(lay==0) cout<<"pcustrips: timetmp[lr="<<lr<<"][str="<<str<<"]="<<timetmp[lr][str]<<endl*/


/* 1.96/16/8/0%/0%/~0%(638)/~0%(519) II=8 */

/* reads one 32-ns timing slice */
void
pcustrips(ap_uint<16> strip_threshold, hls::stream<fadc_256ch_t> &s_fadcs, PCUStrip_s &s_strip)
{
//#pragma HLS INTERFACE ap_stable port=strip_threshold
//#pragma HLS DATA_PACK variable=s_fadcs
//#pragma HLS INTERFACE axis register both port=s_fadcs
//#pragma HLS DATA_PACK variable=s_strip
//#pragma HLS INTERFACE axis register both port=s_strip
#pragma HLS PIPELINE II=1

  ap_uint<13> energy;
  ap_uint<4> chan; /*0-15*/
  ap_uint<2> lay; /*0-2*/
  ap_uint<6> str; /*0-61*/

  fadc_256ch_t fadcs;

  ap_uint<3> timexxx;
  ap_uint<13> energyxxx;

  ap_uint<3> timetmp[NSTRIP]; /* 3 low bits for time interval (0-7) */
#pragma HLS ARRAY_PARTITION variable=timetmp complete dim=1
  ap_uint<13> energytmp[NSTRIP];
#pragma HLS ARRAY_PARTITION variable=energytmp complete dim=1

  ap_uint<13> out[NSTRIP];
#pragma HLS ARRAY_PARTITION variable=out complete dim=1


#ifdef DEBUG
  printf("== pcustrips starts: strip_threshold=%d ==\n",(uint16_t)strip_threshold);fflush(stdout);
#endif


  for(int j=0; j<NSTRIP; j++) timetmp[j] = 0;
  for(int j=0; j<NSTRIP; j++) energytmp[j] = 0;
  for(int j=0; j<NSTRIP; j++) out[j] = 0;

  for(int j=0; j<NSTRIP; j++)
  {
    s_strip.en[j] = 0;
    s_strip.tm[j] = 0;
  }

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
    s_strip.en[i] = energytmp[i];
    s_strip.tm[i] = timetmp[i];
#ifdef DEBUG
    cout<<"pcustrips: s_strip.en["<<i<<"]="<<s_strip.en[i]<<", s_strip.tm["<<i<<"]="<<s_strip.tm[i]<<endl;
#endif
  }


#ifdef DEBUG
  printf("== pcustrips ends ==\n");fflush(stdout);
#endif

}
