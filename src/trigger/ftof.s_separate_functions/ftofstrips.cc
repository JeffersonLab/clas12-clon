
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
      energy = fadcs.e##ch_m; \
      chan = ch_m; \
      lay    = adclayer[isl][chan] - 1; \
      str    = adcstrip[isl][chan] - 1; \
      lr     = adclr[isl][chan] - 1; \
      timexxx = ((energy >= strip_threshold) ? ((uint8_t)fadcs.t##ch_m) : 8); /* error in '?' without (uint16_t) ...*/ \
      if(lay==0) timetmp[lr][str] = timexxx


/* 1.96/16/8/0%/0%/~0%(638)/~0%(519) II=8 */

/* reads one 32-ns timing slice */
void
ftofstrips(ap_uint<16> strip_threshold, hls::stream<fadc_16ch_t> s_fadc_words[NSLOT], hls::stream<FTOFStrip_s> s_strip[NH_READS])
{
#pragma HLS INTERFACE ap_stable port=strip_threshold
#pragma HLS DATA_PACK variable=s_fadc_words
#pragma HLS INTERFACE axis register both port=s_fadc_words
#pragma HLS ARRAY_PARTITION variable=s_fadc_words complete dim=1
#pragma HLS DATA_PACK variable=s_strip
#pragma HLS INTERFACE axis register both port=s_strip
#pragma HLS ARRAY_PARTITION variable=s_strip complete dim=1
#pragma HLS PIPELINE II=1

  ap_uint<13> energy;
  ap_uint<4> chan; /*0-15*/
  ap_uint<2> lay; /*0-2*/
  ap_uint<6> str; /*0-61*/
  ap_uint<1> lr; /*0-1*/

  fadc_16ch_t fadcs;

  ap_uint<4> timexxx;

  ap_uint<4> timetmp[2][NSTRIP]; /* 3 low bits for time interval (0-7), high bit to mark missing hits */
#pragma HLS ARRAY_PARTITION variable=timetmp complete dim=1
#pragma HLS ARRAY_PARTITION variable=timetmp complete dim=2
  ap_uint<NSTRIP> out[2];
#pragma HLS ARRAY_PARTITION variable=out complete dim=1



#ifdef DEBUG
  printf("== ftofstrips starts ==\n");fflush(stdout);
#endif


  for(int i=0; i<2; i++)
  {
    for(int j=0; j<NSTRIP; j++) timetmp[i][j] = 0;
    out[i] = 0;
  }



  /********************************************************************/
  /* get FADC data for 32-ns interval, and fill timetmp[2][NSTRIP] array */

  for(int isl=0; isl<NSLOT; isl++)
  {
	fadcs = s_fadc_words[isl].read();
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

  FTOFStrip_s fifo;

  for(int j=0; j<NH_READS; j++)
  {
    for(int k=0; k<NLR; k++)
    {
      for(int i=0; i<NSTRIP; i++)
      {
        if(timetmp[k][i]==j) out[k](i,i) = 1;
        else                 out[k](i,i) = 0;
      }
    }

    fifo.outL = out[0];
    fifo.outR = out[1];
    s_strip[j].write(fifo);
  }



#ifdef DEBUG
  printf("== ftofstrips ends ==\n");fflush(stdout);
#endif

}
