/* ecfrac2.c - energy fractions calculation

  input:  

  output: 
*/


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

using namespace std;
#include <strstream>


#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)
#define ABS(x)      ((x) < 0 ? -(x) : (x))


#ifdef USE_PCAL

#include "../pc.s/pclib.h"

#define ecfrac2 pcfrac2

#else

#include "eclib.h"

#endif


//#define DEBUG

#include <ap_fixed.h>



/*xc7vx550tffg1158-1*/



/* 4.35/9/4/0%/0%/(14136)2%/(5178)1% II=4 */

void
ecfrac2(hls::stream<hitsume_t> s_hitout1[NH_FIFOS], hls::stream<hitsume_t> s_hitout2[NH_FIFOS])
{
#pragma HLS DATA_PACK variable=s_hitout2
#pragma HLS INTERFACE axis register both port=s_hitout2
#pragma HLS DATA_PACK variable=s_hitout1
#pragma HLS INTERFACE axis register both port=s_hitout1
#pragma HLS ARRAY_PARTITION variable=s_hitout1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=s_hitout2 complete dim=1
#pragma HLS PIPELINE II=4


  uint8_t i,j,u,v,w,ind;
  ap_uint<24> peak_sumE[3][NPEAK];
#pragma HLS ARRAY_PARTITION variable=peak_sumE complete dim=1
#pragma HLS ARRAY_PARTITION variable=peak_sumE complete dim=2

  hitsume_t hitouttmp1[NHITMAX];
#pragma HLS ARRAY_PARTITION variable=hitouttmp1 complete dim=1
  hitsume_t hitouttmp2[NHITMAX];
#pragma HLS ARRAY_PARTITION variable=hitouttmp2 complete dim=1

#ifdef DEBUG
  cout<<endl;
  cout <<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!ecfrac2!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<< endl;
  cout <<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!ecfrac2!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<< endl;
#endif


  for(int i=0; i<NH_FIFOS; i++)
  {
    for(int j=0; j<NH_READS; j++)
	{
      hitouttmp1[i*NH_READS+j] = s_hitout1[i].read();
	}
  }


  for(i=0; i<3; i++)
  for(j=0; j<NPEAK; j++)
  {
    peak_sumE[i][j] = 0;
  }

  for(ind=0; ind<NHITMAX; ind++)
  {
    u = U4(ind);
    v = V4(ind);
    w = W4(ind);

    peak_sumE[0][u] += hitouttmp1[ind].peak_sumE[0];
    peak_sumE[1][v] += hitouttmp1[ind].peak_sumE[1];
    peak_sumE[2][w] += hitouttmp1[ind].peak_sumE[2];
  }

#ifdef DEBUG
  for(i=0; i<3; i++)
  for(j=0; j<NPEAK; j++)
  {
    if(peak_sumE[i][j]>0) cout<<"peak_sumE["<<+i<<"]["<<+j<<"]="<<peak_sumE[i][j]<<endl;
  }
#endif

  for(ind=0; ind<NHITMAX; ind++)
  {
    u = U4(ind);
    v = V4(ind);
    w = W4(ind);

    hitouttmp2[ind].sumE         = hitouttmp1[ind].sumE; /* for check below .. */
    hitouttmp2[ind].peak_sumE[0] = peak_sumE[0][u];
    hitouttmp2[ind].peak_sumE[1] = peak_sumE[1][v];
    hitouttmp2[ind].peak_sumE[2] = peak_sumE[2][w];
  }

  for(int i=0; i<NH_FIFOS; i++)
  {
    for(int j=0; j<NH_READS; j++)
	{
      s_hitout2[i].write(hitouttmp2[i*NH_READS+j]);
	}
  }

#ifdef DEBUG
  cout <<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!ecfrac2!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<< endl;
  cout <<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!ecfrac2!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<< endl;
#endif

  return;
}
