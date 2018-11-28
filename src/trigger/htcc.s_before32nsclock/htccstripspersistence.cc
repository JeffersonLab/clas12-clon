
/* htccstripspersistence.c */

	


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include <iostream>
using namespace std;



#include "htcclib.h"



//#define DEBUG


/*xc7vx550tffg1158-1*/



/* 1.68/15/1/0%/0%/(5234)~0%/(1390)~0% II=1 */

/* nframes from 0 to (NPER-1) */


void
htccstripspersistence(nframe_t nframes,
		              hls::stream<HTCCStrip_s> &s_stripin,
					  hls::stream<HTCCStrip_s> &s_stripout, ap_uint<3> jj)
{
#pragma HLS INTERFACE ap_stable register port=nframes
#pragma HLS DATA_PACK variable=s_stripin
#pragma HLS INTERFACE axis register both port=s_stripin
#pragma HLS DATA_PACK variable=s_stripout
#pragma HLS INTERFACE axis register both port=s_stripout
#pragma HLS PIPELINE II=1

  HTCCStrip_s fifosum;

  static HTCCStrip_s fifo[NSTREAMS1][NPER];
#pragma HLS ARRAY_PARTITION variable=fifo complete dim=1
#pragma HLS ARRAY_PARTITION variable=fifo complete dim=2

#ifdef DEBUG
  printf("=== htccstripspersistence starts ===\n");
#endif


#ifdef __SYNTHESIS__
  jj=0;
#endif

	for(int k=(NPER-1); k>0; k--)
	{
	  fifo[jj][k].energy00 = fifo[jj][k-1].energy00;
	  fifo[jj][k].energy01 = fifo[jj][k-1].energy01;
	  fifo[jj][k].energy02 = fifo[jj][k-1].energy02;
	  fifo[jj][k].energy03 = fifo[jj][k-1].energy03;
	  fifo[jj][k].energy04 = fifo[jj][k-1].energy04;
	  fifo[jj][k].energy05 = fifo[jj][k-1].energy05;
	}
#ifdef DEBUG
    cout<<"s_stripold="<<fifo[jj][1].energy00<<" "<<fifo[jj][1].energy01<<" "<<fifo[jj][1].energy02<<" "<<fifo[jj][1].energy03<<" "<<fifo[jj][1].energy04<<" "<<fifo[jj][1].energy05<<endl;
#endif

    fifo[jj][0] = s_stripin.read();
#ifdef DEBUG
    cout<<"s_stripin ="<<fifo[jj][0].energy00<<" "<<fifo[jj][0].energy01<<" "<<fifo[jj][0].energy02<<" "<<fifo[jj][0].energy03<<" "<<fifo[jj][0].energy04<<" "<<fifo[jj][0].energy05<<endl;
#endif

    fifosum.energy00 = 0;
    fifosum.energy01 = 0;
    fifosum.energy02 = 0;
    fifosum.energy03 = 0;
    fifosum.energy04 = 0;
    fifosum.energy05 = 0;
    for(int k=0; k<NPER; k++)
	{
      if(k<=nframes)
      {
        fifosum.energy00 += fifo[jj][k].energy00;
        fifosum.energy01 += fifo[jj][k].energy01;
        fifosum.energy02 += fifo[jj][k].energy02;
        fifosum.energy03 += fifo[jj][k].energy03;
        fifosum.energy04 += fifo[jj][k].energy04;
        fifosum.energy05 += fifo[jj][k].energy05;
      }
	}
#ifdef DEBUG
    cout<<"s_stripout="<<fifosum.energy00<<" "<<fifosum.energy01<<" "<<fifosum.energy02<<" "<<fifosum.energy03<<" "<<fifosum.energy04<<" "<<fifosum.energy05<<endl;
#endif
	s_stripout.write(fifosum);


#ifdef DEBUG
  printf("=== htccstripspersistence ends ===\n");
#endif

}
