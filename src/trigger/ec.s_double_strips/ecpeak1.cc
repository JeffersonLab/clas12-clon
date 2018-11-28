
/* ecpeak1.c */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include <iostream>
using namespace std;



#ifdef USE_PCAL

#include "../pc.s/pclib.h"
#define ecpeak1 pcpeak1
#define ec_first_and_last pc_first_and_last

#else

#include "eclib.h"

#endif


//#define DEBUG



void
ec_first_and_last(ap_uint<16> strip_threshold, ap_uint<4> strip_dip_factor, 
               ap_uint<13> en_m2, ap_uint<13> en_m1, ap_uint<13> en_0, ap_uint<13> en_p1, ap_uint<1> first_last[2])
{
#pragma HLS INLINE
#pragma HLS PIPELINE
  ap_uint<13> energy0, energy1;

  first_last[0] = 0;
  first_last[1] = 0;

  /* found first */
  energy0 = en_m1;
  energy1 = en_m1 + (en_m1>>strip_dip_factor);
#ifdef DEBUG
    cout<<"      INPUTS: "<<+strip_threshold<<" "<<+en_m2<<" "<<+en_m1<<" "<<+en_0<<" "<<+en_p1<<endl;
#endif
#ifdef DEBUG
    cout<<"   LOOKING_FOR_FIRST: "<<+energy0<<" "<<+energy1<<endl;
#endif
  if((en_m1<=strip_threshold && en_0>strip_threshold) ||
     (energy0>strip_threshold && en_m2>energy0 && energy1<en_0) ||
	 (energy0>strip_threshold && en_m2>energy1 && energy0<en_0))
  {
#ifdef DEBUG
    cout<<"FOUND_FIRST"<<endl;
#endif
    first_last[0] = 1;
  }

  /* found last */
  energy0 = en_0;
  energy1 = en_0 + (en_0>>strip_dip_factor);
#ifdef DEBUG
    cout<<"   LOOKING_FOR_LAST: "<<+energy0<<" "<<+energy1<<endl;
#endif
  if((en_0>strip_threshold && en_p1<=strip_threshold) ||
     (energy0>strip_threshold && en_m1>energy0 && energy1<en_p1) ||
     (energy0>strip_threshold && en_m1>energy1 && energy0<en_p1))
  {
    first_last[1] = 1;
#ifdef DEBUG
    cout<<"FOUND_LAST"<<endl;
#endif
  }

}


/*xc7vx550tffg1158-1*/




/* ECAL: 4.23/14/4/0%/0%/(6062)~0%/(5530)1% II=4 */

/* PCAL: 4.23/30/4/0%/0%/3%/5% II=4 */

void
ecpeak1(ap_uint<16> strip_threshold, ap_uint<4> strip_dip_factor, ap_uint<4> nstrip_max, hls::stream<ECStrip_s> s_strip1[NF1],
        hls::stream<ECfml_t> &s_first, hls::stream<ECfml_t> &s_middle, hls::stream<ECfml_t> &s_last)
{
#pragma HLS DATA_PACK variable=s_last
#pragma HLS INTERFACE axis register both port=s_last
#pragma HLS DATA_PACK variable=s_middle
#pragma HLS INTERFACE axis register both port=s_middle
#pragma HLS DATA_PACK variable=s_first
#pragma HLS INTERFACE axis register both port=s_first
#pragma HLS DATA_PACK variable=s_strip1
#pragma HLS INTERFACE axis register both port=s_strip1
#pragma HLS INTERFACE ap_stable port=nstrip_max
#pragma HLS INTERFACE ap_stable port=strip_dip_factor
#pragma HLS INTERFACE ap_stable port=strip_threshold
#pragma HLS ARRAY_PARTITION variable=s_strip1 complete dim=1
#pragma HLS PIPELINE II=4

  ap_uint<1>  peak_opened;
  ap_uint<13> en;
  ap_uint<16> energysum;

  ECPeak0 peak[NSTRIP];
#pragma HLS ARRAY_PARTITION variable=peak complete dim=1

#ifdef USE_PCAL
  ap_uint<13> energy[NSTRIP+3+4+1]; /* 2 extra below, one above, 4 extra to round up last read from streams */
#else
  ap_uint<13> energy[NSTRIP+3+4]; /* 2 extra below, one above, 4 extra to round up last read from streams */
#endif

  #pragma HLS ARRAY_PARTITION variable=energy complete dim=1

  ap_uint<NSTRIP+2> first;
  ap_uint<NSTRIP+2> middle;
  ap_uint<NSTRIP+2> last;
  ap_uint<1> first_last[2];

  ECStrip_s fifo;

#ifdef DEBUG
  printf("\n+++ ecpeak1 +++\n");
  cout<<"  strip_threshold="<<strip_threshold<<endl;
#endif

  energy[0] = 0;
  energy[1] = 0;
  energy[NSTRIP+2] = 0; /* done in ecstrips ? will be overwritten anyway below .. */
  first = 0;
  middle = 0;
  last = 0;


  /* if doing .read/.write, limits have to be even to stream dimansion ! */
#ifdef USE_PCAL
  /*NF1=11 for PCAL, so we are reading 22 strips every 'j' iteration*/

  /*temporary assumed NSTRIP=42*/
  const int b1[4] = {  0,  6, 12, 18};
  const int e1[4] = {  6, 12, 18, 24};

  /*temporary assumed NSTRIP=42*/
  const int b2[4] = {  2, 13, 24, 35};
  const int e2[4] = { 12, 23, 34, 42/*45 bad*/};

#else
  /*NF1=5 for ECAL, so we are reading 10 strips every 'j' iteration*/

  /*temporary assumed NSTRIP=36*/
  const int b1[4] = {  0,  5, 10, 15};
  const int e1[4] = {  5, 10, 15, 20/*18*/};

  /*temporary assumed NSTRIP=36*/
  const int b2[4] = {  2, 12, 22, 32};
  const int e2[4] = { 11, 21, 31, 37};

#endif


  for(int j=0; j<NH_READS; j++)
  {

    for(int i=b1[j]; i<e1[j]; i++)
    {
	  fifo = s_strip1[i-b1[j]].read();
      energy[i*2+2] = fifo.energy0;
      energy[i*2+3] = fifo.energy1;
#ifdef DEBUG
      cout<<"-> energy["<<i*2+2<<"]="<<energy[i*2+2]<<endl;
      cout<<"-> energy["<<i*2+3<<"]="<<energy[i*2+3]<<endl;
#endif
    }

#if 1
  }
  for(int j=0; j<NH_READS; j++)
  {
#endif

    for(int i=b2[j]; i<=e2[j]; i++)
    {
#ifdef DEBUG
      cout<<"=> energy["<<i-2<<"]="<<energy[i-2]<<endl;
      cout<<"=> energy["<<i-1<<"]="<<energy[i-1]<<endl;
      cout<<"=> energy["<<i<<"]="<<energy[i]<<endl;
      cout<<"=> energy["<<i+1<<"]="<<energy[i+1]<<endl;
#endif
	  /*sergey: using strip_threshold=0 here, because it is applied already in ecstrips.cc*/
      ec_first_and_last(0/*strip_threshold*/, strip_dip_factor, energy[i-2], energy[i-1], energy[i], energy[i+1], first_last);
      first[i] = first_last[0];
      last[i]  = first_last[1];
    }
  }

  /* fill middle */
  peak_opened = 0;
  for(int i=2; i<(NSTRIP+2); i++)
  {
    middle[i] = peak_opened;

    if(first[i])
	{
      middle[i] = 1;
      peak_opened = 1;
	}

    if(last[i])
	{
      middle[i] = 1;
      peak_opened = 0;
	}
  }


#ifdef DEBUG
  for(int i=2; i<=NSTRIP+1; i++) std::cout<<"strip="<<+i<<" energy="<<+energy[i]<<" first="<<+first[i]<<" middle="<<+middle[i]<<" last="<<+last[i]<<endl;
#endif


  /* check peak width in according to 'nstrip_max' */
  if(nstrip_max)
  {
    ap_uint<NSTRIP+2+16> mask;
    ap_uint<NSTRIP+2> mask1;
    mask = 0;
    mask1 = 0;
    for(int i=0; i<16; i++)
	{
      if(i<nstrip_max) mask = mask | (first<<i);
	}
    mask1 = mask(NSTRIP+2,0) & middle;
#ifdef DEBUG
    cout<<"masks: "<<first<<" "<<mask<<" "<<middle<<" "<<mask1<<" "<<(mask1^middle)<<endl;
#endif
    if( (mask1^middle) == 0 ) {first=0; middle=0; last=0;}
  }




  /* output */
  s_first.write(first>>2);
  s_middle.write(middle>>2);
  s_last.write(last>>2);


#ifdef DEBUG
  printf("\n+++ ecpeak1 +++\n");
#endif

}
