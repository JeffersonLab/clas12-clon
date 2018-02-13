
/* pcuhit.c - 

  input:  

  output: 

*/

	
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include <iostream>
using namespace std;



#include "pculib.h"



//#define DEBUG


#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)
#define ABS(x)      ((x) < 0 ? -(x) : (x))


/*xc7vx550tffg1158-1*/


/* 1.96/46/8/0%/0%/(10270)1%/(5734)1% II=8 */

/* nframes here means half of acceptance interval */
void
pcuhit(ap_uint<16> thresholdmin, ap_uint<16> thresholdmax, nframe_t nframes, PCUStrip strip[NSTRIP], ap_uint<4> outtime[NSTRIP])
{
//#pragma HLS INTERFACE ap_stable port=nframes
//#pragma HLS ARRAY_PARTITION variable=outtime complete dim=1
#pragma HLS PIPELINE II=1

  int it;

#ifdef DEBUG
  printf("== pcuhit start ==\n");
  for(int i=0; i<NSTRIP; i++)
  {
    if(strip[i].en>0) cout<<"pcuhit: strip["<<i<<"].en="<<strip[i].en<<", strip["<<i<<"].tm="<<strip[i].tm<<endl;
  }
#endif


  for(int i=0; i<NSTRIP; i++) /* loop over all counters */
  {
    outtime[i] = 0;
    if( (strip[i].en>=thresholdmin) && (strip[i].en<=thresholdmax) )
	{
      it = strip[i].tm;
      outtime[i] = it + 8;
#ifdef DEBUG
      cout << "pcuhit: it="<<it<<" -> outtime="<<outtime[i]<<endl;
#endif
	}
  }



#ifdef DEBUG
  printf("== pcuhit end ==\n");
#endif
}

