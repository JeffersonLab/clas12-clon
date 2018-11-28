
/* ftofstrips.cc */

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


#include "ftoftrans.h"
#include "ftoflib.h"

#define TRANSLATE(ch_m) \
  energy = fadcs.fadc[isl].e##ch_m; \
  chan = ch_m; \
  lay    = adclayer[isl][chan] - 1; \
  str    = adcstrip[isl][chan] - 1; \
  lr     = adclr[isl][chan] - 1; \
  timexxx = ((energy >= strip_threshold) ? ((uint8_t)fadcs.fadc[isl].t##ch_m) : 8); /* error in '?' without (uint16_t) ...*/ \
  energyxxx = ((energy >= strip_threshold) ? ((uint8_t)fadcs.fadc[isl].e##ch_m) : 0); /* error in '?' without (uint16_t) ...*/ \
  if(lay==0) \
  { \
    timetmp[lr][str] = timexxx;	\
    energytmp[lr][str] = energyxxx; \
  }
  /*if(lay==0 && energy>0) cout<<"ftofstrips: energy[lay="<<lay<<"][str="<<str<<"][lr="<<lr<<"]="<<energy<<endl; \
	if(lay==0) cout<<"ftofstrips: timetmp[lr="<<lr<<"][str="<<str<<"]="<<timetmp[lr][str]<<endl*/


/* 1.96/16/8/0%/0%/~0%(638)/~0%(519) II=8 */

/* reads one 32-ns timing slice */
void
ftofstrips(ap_uint<16> strip_threshold, hls::stream<fadc_256ch_t> &s_fadcs, FTOFStrip_s s_strip[NH_READS])
{
//#pragma HLS INTERFACE ap_stable port=strip_threshold
//#pragma HLS DATA_PACK variable=s_fadcs
//#pragma HLS INTERFACE axis register both port=s_fadcs
#pragma HLS PIPELINE II=1

  ap_uint<13> energy;
  ap_uint<4> chan; /*0-15*/
  ap_uint<2> lay; /*0-2*/
  ap_uint<6> str; /*0-61*/
  ap_uint<1> lr; /*0-1*/

  fadc_256ch_t fadcs;

  ap_uint<4> timexxx;
  ap_uint<13> energyxxx;

  ap_uint<4> timetmp[2][NSTRIP]; /* 3 low bits for time interval (0-7), high bit to mark missing hits */
#pragma HLS ARRAY_PARTITION variable=timetmp complete dim=1
#pragma HLS ARRAY_PARTITION variable=timetmp complete dim=2
  ap_uint<13> energytmp[2][NSTRIP];
#pragma HLS ARRAY_PARTITION variable=energytmp complete dim=1
#pragma HLS ARRAY_PARTITION variable=energytmp complete dim=2

  ap_uint<13> out[2][NSTRIP];
#pragma HLS ARRAY_PARTITION variable=out complete dim=1
#pragma HLS ARRAY_PARTITION variable=out complete dim=2


#ifdef DEBUG
  printf("== ftofstrips starts: strip_threshold=%d ==\n",(uint16_t)strip_threshold);fflush(stdout);
#endif


  for(int i=0; i<2; i++)
  {
    for(int j=0; j<NSTRIP; j++) timetmp[i][j] = 0;
    for(int j=0; j<NSTRIP; j++) energytmp[i][j] = 0;
    for(int j=0; j<NSTRIP; j++) out[i][j] = 0;
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



  /* filling 4ns-slices using 3 timing bits, and send it over - one slice per stream, all in parallel - we are 32ns domain ! */

  for(int j=0; j<NH_READS; j++)
  {
    for(int k=0; k<NLR; k++)
    {
      for(int i=0; i<NSTRIP; i++)
      {
        if(timetmp[k][i]==j) out[k][i] = energytmp[k][i];
        else                 out[k][i] = 0;
      }
    }


    for(int i=0; i<NSTRIP; i++)
	{
      s_strip[j].enL[i] = out[0][i];
      s_strip[j].enR[i] = out[1][i];
#ifdef DEBUG
      cout<<"ftofstrips: s_strip["<<j<<"].enL["<<i<<"]="<<hex<<s_strip[j].enL[i]<<dec<<endl;
      cout<<"ftofstrips: s_strip["<<j<<"].enR["<<i<<"]="<<hex<<s_strip[j].enR[i]<<dec<<endl;
#endif
	}


  }



#ifdef DEBUG
  printf("== ftofstrips ends ==\n");fflush(stdout);
#endif

}
