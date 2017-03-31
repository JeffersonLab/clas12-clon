
/* pcstrip.c - 'normalize' strips */ 

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "pclib.h"

#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)
#define ABS(x)      ((x) < 0 ? -(x) : (x))


/* 1.38/0/1/0%/0%/0%/0% */



#undef DEBUG



int
pcstrip(uint8_t view, PCStrip stripin[PSTRIP], PCStrip stripout[PSTRIPMAX])
{
#pragma HLS ARRAY_PARTITION variable=stripin complete dim=1
#pragma HLS ARRAY_PARTITION variable=stripout complete dim=1
#pragma HLS PIPELINE
  uint16_t ii, jj;
  uint16_t energy;

  if(view==0)
  {
    for(jj=0; jj<=52; jj++)
	{
      ii = jj;
      stripout[jj].energy = stripin[ii].energy;
	}
    for(jj=54; jj<PSTRIPMAX; jj+=2)
	{
      ii = 52+(jj-52)/2;
      energy = stripin[ii].energy / 2;
      stripout[jj-1].energy = energy;
      stripout[jj].energy = energy;
    }
  }
  else
  {
    for(jj=0; jj<=30; jj+=2)
    {
      ii = jj/2;
      energy = stripin[ii].energy / 2;
      stripout[jj].energy = energy;
      stripout[jj+1].energy = energy;
	}
    for(jj=31; jj<PSTRIPMAX-1; jj++)
	{
      ii = jj-15;
      stripout[jj].energy = stripin[ii].energy;
    }
  }


  return(0);
}

