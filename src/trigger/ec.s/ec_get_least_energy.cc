
/* ec_get_least_energy.cc */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include <iostream>
using namespace std;

#include "eclib.h"

#undef DEBUG

/* 3.15/1/1/0%/0%/0%/0% */

uint8_t
ec_get_least_energy(uint16_t en[NPEAK])
{
#pragma HLS PIPELINE
#pragma HLS ARRAY_PARTITION variable=en complete dim=1
  uint8_t j;
  uint16_t energy;

  if     ((en[0]<=en[1])&&(en[0]<=en[2])&&(en[0]<=en[3])) j=0;
  else if((en[1]<=en[0])&&(en[1]<=en[2])&&(en[1]<=en[3])) j=1;
  else if((en[2]<=en[0])&&(en[2]<=en[1])&&(en[2]<=en[3])) j=2;
  else if((en[3]<=en[0])&&(en[3]<=en[1])&&(en[3]<=en[2])) j=3;

  return(j);
}

/*
uint8_t
ec_get_least_energy1(uint16_t en[NPEAK])
{
#pragma HLS PIPELINE
#pragma HLS ARRAY_PARTITION variable=en complete dim=1
  uint8_t i, j;
  uint16_t energy;
  j = 0;
  energy = en[0];
  for(i=1; i<NPEAK; i++)
  {
    if(en[i] < energy)
	{
      energy = en[i];
      j=i;
	}
  }
  return(j);
}
*/

