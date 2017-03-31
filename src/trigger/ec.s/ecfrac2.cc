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


/* 2.81/6/2/0%/0%/1%/1% */




void
ecfrac2(hitsume_t hitouttmp1[NHITMAX], hitsume_t hitouttmp2[NHITMAX])
{
#pragma HLS ARRAY_PARTITION variable=hitouttmp1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=hitouttmp2 complete dim=1
#pragma HLS PIPELINE

  uint8_t i,j,u,v,w,ind;
  ap_uint<24> peak_sumE[3][NPEAK];

#ifdef DEBUG
  cout<<endl;
  cout <<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!ecfrac2!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<< endl;
  cout <<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!ecfrac2!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<< endl;
#endif

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

#ifdef DEBUG
  cout <<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!ecfrac2!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<< endl;
  cout <<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!ecfrac2!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<< endl;
#endif

  return;
}
