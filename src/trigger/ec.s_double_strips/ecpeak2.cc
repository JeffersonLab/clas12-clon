
/* ecpeak2.c */

	


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include <iostream>
using namespace std;



#ifdef USE_PCAL

#include "../pc.s/pclib.h"
#define ecpeak2 pcpeak2

#else

#include "eclib.h"

#endif


//#define DEBUG




#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)
#define ABS(x)      ((x) < 0 ? -(x) : (x))



#define OPEN_PEAK \
{ \
  strip1 = i; /* strip number from 0 ! */ \
}

#define FILL_PEAK \
{ \
  energysum += en; \
  nstrip++; \
  energysum4coord += nstrip*en; \
}


#define CLOSE_PEAK \
{ \
  /*ii=i;*/ /* LUT=10% */ \
\
  if(nstrip==1) ii=i; /* to make zero suppression easy -> LUT=14% */ \
  else          ii=i-1; \
\
  /*cout << "!!!!===!!!! " << +(i-1) << " " << +ii << " strip1=" << +strip1 << endl;*/ \
  peak_strip1[ii]          = strip1; \
  peak_stripn[ii]          = nstrip; \
  peak_energy[ii]          = energysum; \
  peak_energysum4coord[ii] = energysum4coord; \
  energysum4coord=0; \
  strip1 = 0; \
  nstrip = 0; \
  energysum = 0; \
}

/*xc7vx550tffg1158-1*/




/* PCAL: 6.99/126/4/0%/2%/10%/14% II=4 */

void
ecpeak2(ap_uint<16> peak_threshold, hls::stream<ECStrip_s> s_strip2[NF1],
        hls::stream<ECfml_t> &s_first, hls::stream<ECfml_t> &s_middle, hls::stream<ECfml_t> &s_last,
		hls::stream<ECPeak0_s> s_peak0strip[NF2])
{
#pragma HLS INTERFACE axis register both port=s_peak0strip
#pragma HLS DATA_PACK variable=s_peak0strip
#pragma HLS INTERFACE axis register both port=s_last
#pragma HLS DATA_PACK variable=s_last
#pragma HLS INTERFACE axis register both port=s_middle
#pragma HLS DATA_PACK variable=s_middle
#pragma HLS INTERFACE axis register both port=s_first
#pragma HLS DATA_PACK variable=s_first
#pragma HLS INTERFACE axis register both port=s_strip2
#pragma HLS DATA_PACK variable=s_strip2
#pragma HLS INTERFACE ap_stable port=peak_threshold
#pragma HLS ARRAY_PARTITION variable=s_strip2 complete dim=1
#pragma HLS ARRAY_PARTITION variable=s_peak0strip complete dim=1
#pragma HLS PIPELINE II=4

  int ii;
  ap_uint<8>  strip1;
  ap_uint<8>  nstrip;
  ap_uint<13> en;
  ap_uint<16> energysum;
  ap_uint<24> energysum4coord;

  ECPeak0 peak[NSTRIP];
#pragma HLS ARRAY_PARTITION variable=peak complete dim=1

#ifdef USE_PCAL
  ap_uint<13> energy[NSTRIP+4+2];
#else
  ap_uint<13> energy[NSTRIP+4];
#endif

  #pragma HLS ARRAY_PARTITION variable=energy complete dim=1

  ap_uint<NSTRIP> first;
  ap_uint<NSTRIP> middle;
  ap_uint<NSTRIP> last;

  ap_uint<8>  peak_strip1[NSTRIP];
#pragma HLS ARRAY_PARTITION variable=peak_strip1 complete dim=1
  ap_uint<8>  peak_stripn[NSTRIP];
#pragma HLS ARRAY_PARTITION variable=peak_stripn complete dim=1
  ap_uint<16> peak_energy[NSTRIP];
#pragma HLS ARRAY_PARTITION variable=peak_energy complete dim=1
  ap_uint<24> peak_energysum4coord[NSTRIP];
#pragma HLS ARRAY_PARTITION variable=peak_energysum4coord complete dim=1


#ifdef DEBUG
  printf("\n+++ ecpeak2 +++\n");
  cout<<"  peak_threshold="<<peak_threshold<<endl;
#endif


  ECStrip_s fifo;
  ECPeak0_s fifo1;


  strip1 = 0;
  nstrip = 0;
  energysum = 0;
  energysum4coord=0;
  for(int i=0; i<NSTRIP; i++) peak_energy[i] = 0;

  first = s_first.read();
  middle = s_middle.read();
  last = s_last.read();

  /* if doing .read/.write, limits have to be even to stream dimansion ! */
#ifdef USE_PCAL
  /*NF1=6 for PCAL, so we are reading 12 strips every 'j' iteration*/
  /*assumed NSTRIP=42*/
  const int b1[4] = {  0,  6, 12, 18};
  const int e1[4] = {  6, 12, 18, 24};

  /*NF2=11 for PCAL, so we are writing 11 peaks every 'j' iteration*/
  /*assumed NSTRIP=42*/
  const int b2[4] = {  0, 11, 22, 33};
  const int e2[4] = { 11, 22, 33, 44};
#else
  /*NF1=5 for ECAL, so we are reading 10 strips every 'j' iteration*/
  /*assumed NSTRIP=36*/
  const int b1[4] = {  0,  5, 10, 15};
  const int e1[4] = {  5, 10, 15, 20/*18*/};

  /*NF2=9 for ECAL, so we are writing 9 peaks every 'j' iteration*/
  /*assumed NSTRIP=36*/
  const int b2[4] = {  0,  9, 18, 27};
  const int e2[4] = {  9, 18, 27, 36};

#endif


  for(int j=0; j<NH_READS; j++)
  {
    for(int i=b1[j]; i<e1[j]; i++)
    {
	  fifo = s_strip2[i-b1[j]].read();
      energy[i*2]   = fifo.energy0;
      energy[i*2+1] = fifo.energy1;
	}
  }





  ii = 0;
  for(int i=0; i<NSTRIP; i++)
  {
    en = energy[i];
    if(first[i])
	{
      OPEN_PEAK;
#ifdef DEBUG
cout<<"OPEN_PEAK: first["<<i<<"] = "<<first[i]<<endl;
#endif
	}
    if(middle[i])
	{
      FILL_PEAK;
#ifdef DEBUG
cout<<"FILL_PEAK: middle["<<i<<"] = "<<middle[i]<<endl;
#endif
	}
    if(last[i])
	{
      CLOSE_PEAK;
#ifdef DEBUG
cout<<"CLOSE_PEAK: last["<<i<<"] = "<<last[i]<<endl;
#endif
	}
  }





  for(int i=0; i<NSTRIP; i++)
  {
    if(peak_energy[i]<peak_threshold) peak[i].energy = 0;
    else                              peak[i].energy = peak_energy[i];

    peak[i].strip1   = peak_strip1[i];
    peak[i].stripn   = peak_stripn[i];
    peak[i].energysum4coord = peak_energysum4coord[i];
  }

  for(int j=0; j<NH_READS; j++)
  {
    for(int i=b2[j]; i<e2[j]; i++)
    {
#ifdef USE_PCAL
      if(i<42)
      {
#endif
        fifo1.energy = peak[i].energy;
        fifo1.energysum4coord = peak[i].energysum4coord;
        fifo1.strip1 = peak[i].strip1;
        fifo1.stripn = peak[i].stripn;
#ifdef USE_PCAL
      }
      else
      {
    	fifo1.energy = 0;
      }
#endif
	  s_peak0strip[i-b2[j]].write(fifo1);
    }
  }

#ifdef DEBUG
  for(int i=0; i<NSTRIP; i++) cout<<" peak["<<i<<"]: energy="<<peak[i].energy<<" energysum4coord="<<peak[i].energysum4coord<<" strip1="<<peak[i].strip1<<" stripn="<<peak[i].stripn<<endl;
  printf("\n+++ ecpeak2 +++\n");
#endif

}
