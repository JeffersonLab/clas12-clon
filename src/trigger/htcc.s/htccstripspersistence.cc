
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



/* 4.77/6/4/0%/0%/(9086)1%/(4302)1% II=4 */

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


#if 0
void
htccstripspersistence(nframe_t nframes,
		hls::stream<HTCCStrip_s> s_stripin[NSTREAMS1],
		hls::stream<HTCCStrip_s> s_stripout[NSTREAMS1])
{
#pragma HLS INTERFACE ap_stable register port=nframes
#pragma HLS ARRAY_PARTITION variable=s_stripin complete dim=1
#pragma HLS DATA_PACK variable=s_stripin
#pragma HLS INTERFACE axis register both port=s_stripin
#pragma HLS ARRAY_PARTITION variable=s_stripout complete dim=1
#pragma HLS DATA_PACK variable=s_stripout
#pragma HLS INTERFACE axis register both port=s_stripout
#pragma HLS PIPELINE II=1

  HTCCStrip_s fifosum;

  static HTCCStrip_s fifo[NPER][NSTREAMS1];
#pragma HLS ARRAY_PARTITION variable=fifo complete dim=1
#pragma HLS ARRAY_PARTITION variable=fifo complete dim=2

#ifdef DEBUG
  printf("=== htccstripspersistence starts ===\n");
#endif

  for(int j=0; j<NSTREAMS1; j++)
  {
	for(int k=(NPER-1); k>0; k--)
	{
	  fifo[k][j].energy00 = fifo[k-1][j].energy00;
	  fifo[k][j].energy01 = fifo[k-1][j].energy01;
	  fifo[k][j].energy02 = fifo[k-1][j].energy02;
	  fifo[k][j].energy03 = fifo[k-1][j].energy03;
	  fifo[k][j].energy04 = fifo[k-1][j].energy04;
	  fifo[k][j].energy05 = fifo[k-1][j].energy05;
	}

    fifo[0][j] = s_stripin[j].read();

#ifdef DEBUG
    cout<<"s_stripin["<<j<<"]="<<fifo[0][j].energy00<<" "<<fifo[0][j].energy01<<" "<<fifo[0][j].energy02<<" "<<fifo[0][j].energy03<<" "<<fifo[0][j].energy04<<" "<<fifo[0][j].energy05<<endl;
#endif
	/*
  }

  for(int j=0; j<NSTREAMS1; j++)
  {
	*/

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
        fifosum.energy00 += fifo[k][j].energy00;
        fifosum.energy01 += fifo[k][j].energy01;
        fifosum.energy02 += fifo[k][j].energy02;
        fifosum.energy03 += fifo[k][j].energy03;
        fifosum.energy04 += fifo[k][j].energy04;
        fifosum.energy05 += fifo[k][j].energy05;
      }
	}
#ifdef DEBUG
    cout<<"s_stripout["<<j<<"]="<<fifosum.energy00<<" "<<fifosum.energy01<<" "<<fifosum.energy02<<" "<<fifosum.energy03<<" "<<fifosum.energy04<<" "<<fifosum.energy05<<endl;
#endif
	s_stripout[j].write(fifosum);
  }

#ifdef DEBUG
  printf("=== htccstripspersistence starts ===\n");
#endif

}
#endif

