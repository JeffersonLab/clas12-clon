
/* filter1.cc */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#include <iostream>
using namespace std;

#include <ap_int.h>
#include <hls_stream.h>

#include "hls_fadc_sum.h"
//#include "trigger.h"



void
filter1(hls::stream<fadc_hit_t> fADC[100], hls::stream<uint32_t> peak_time[2])
{
  int n;
  int peak = 0;

  for (int i=0; i<2; i++) peak_time[i] = 0;

  for (int i=0; i<90; i++) fADC[i] = fADC[i+10] - fADC[i];

  for (n=0; n<90; n++)
  {
    fADC[n]=fADC[n]+fADC[n+1]+fADC[n+2]+fADC[n+3]+fADC[n+4]+fADC[n+5]+fADC[n+6]+fADC[n+7]+fADC[n+8]+fADC[n+9]+fADC[n+10];
    if (fADC[n]>peak)
    {
      peak_time[0] = fADC[n];
      peak_time[1] = n;
    }
  }
}
