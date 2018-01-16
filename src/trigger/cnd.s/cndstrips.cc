
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
      timexxx = ((energy >= strip_threshold) ? ((uint8_t)fadcs.fadc[isl].t##ch_m) : 8); /* error in '?' without (uint16_t) ...*/ \
      timetmp[lr][str] = timexxx


/* 1.96/16/8/0%/0%/~0%(638)/~0%(519) II=8 */

/* reads one 32-ns timing slice */
void
cndstrips(ap_uint<16> strip_threshold, hls::stream<fadc_256ch_t> &s_fadcs, CNDStrip_s s_strip[NH_READS])
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

  ap_uint<4> timexxx;

  ap_uint<4> timetmp[2][NSTRIP]; /* 3 low bits for time interval (0-7), high bit to mark missing hits */
#pragma HLS ARRAY_PARTITION variable=timetmp complete dim=1
#pragma HLS ARRAY_PARTITION variable=timetmp complete dim=2
  ap_uint<NSTRIP> out[2];
#pragma HLS ARRAY_PARTITION variable=out complete dim=1



#ifdef DEBUG
  printf("== cndstrips starts ==\n");fflush(stdout);
#endif


  for(int i=0; i<2; i++)
  {
    for(int j=0; j<NSTRIP; j++) timetmp[i][j] = 0;
    out[i] = 0;
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
        if(timetmp[k][i]==j) out[k](i,i) = 1;
        else                 out[k](i,i) = 0;
      }
    }

    s_strip[j].outL = out[0];
    s_strip[j].outR = out[1];
  }



#ifdef DEBUG
  printf("== cndstrips ends ==\n");fflush(stdout);
#endif

}
