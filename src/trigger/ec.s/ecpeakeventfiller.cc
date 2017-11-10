
/* ecpeakeventfiller.cc - writes peaks to buf_ram[][] */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

using namespace std;
#include <strstream>


#ifdef USE_PCAL

#include "../pc.s/pclib.h"
#define ecpeakeventfiller pcpeakeventfiller
#define ecpeakeventfiller0 pcpeakeventfiller0
#define ecpeakeventfiller1 pcpeakeventfiller1
#define ecpeakeventfiller2 pcpeakeventfiller2

#else

#include "eclib.h"

#endif

//#define DEBUG



void
ecpeakeventfiller(ap_uint<2> view, hls::stream<ECPeak_s> &s_peakin, peak_ram_t buf_ram[NPEAK][256])
{
#pragma HLS INTERFACE ap_stable port=view
#pragma HLS DATA_PACK variable=buf_ram
#pragma HLS INTERFACE axis register both port=s_peakin
#pragma HLS DATA_PACK variable=s_peakin
#pragma HLS ARRAY_PARTITION variable=buf_ram block factor=4 dim=1
#pragma HLS PIPELINE II=4
  ECPeak peakin;
  ECPeak_s fifo;

  static ap_uint<8> addr = 0;

  for(int i=0; i<NPEAK; i++)
  {
	fifo = s_peakin.read();

    peakin.energy = fifo.energy;
    peakin.coord  = fifo.coord;
#ifndef __SYNTHESIS__
    peakin.strip1 = fifo.strip1;
    peakin.stripn = fifo.stripn;
#endif

#ifdef DEBUG
    cout << "-ecfill-> addr="<<addr<<"  energy="<<peakin.energy<<endl;
#endif

    buf_ram[i][addr].energy = peakin.energy;
    buf_ram[i][addr].coord = peakin.coord;
    buf_ram[i][addr].view = view;
#ifndef __SYNTHESIS__
    buf_ram[i][addr].strip1 = peakin.strip1;
    buf_ram[i][addr].stripn = peakin.stripn;
#endif
  }

/*#ifdef __SYNTHESIS__*/
  addr ++; /*increment on every call (every 32ns)*/
/*#endif*/
}




#ifndef __SYNTHESIS__

#define ECPEAKEVENTFILLER(view_macro) \
  ECPeak peakin; \
  ECPeak_s fifo; \
  static ap_uint<8> addr = 0; \
  for(int i=0; i<NPEAK; i++) \
  { \
	fifo = s_peakin.read(); \
    peakin.energy = fifo.energy; \
    peakin.coord  = fifo.coord; \
    peakin.strip1 = fifo.strip1; \
    peakin.stripn = fifo.stripn; \
    /*cout << "-fill-> addr="<<addr<<"  energy="<<peakin.energy<<endl;*/ \
    buf_ram[i][addr].energy = peakin.energy; \
    buf_ram[i][addr].coord = peakin.coord; \
    buf_ram[i][addr].view = view_macro; \
    buf_ram[i][addr].strip1 = peakin.strip1; \
    buf_ram[i][addr].stripn = peakin.stripn; \
  } \
  addr ++;

void
ecpeakeventfiller0(hls::stream<ECPeak_s> &s_peakin, peak_ram_t buf_ram[NPEAK][256])
{
  ECPEAKEVENTFILLER(0);
  /*
#ifdef USE_PCAL
  cout << "-pcfill0-> addr="<<addr-1<<endl;
#else
  cout << "-ecfill0-> addr="<<addr-1<<endl;
#endif
  */
}
void
ecpeakeventfiller1(hls::stream<ECPeak_s> &s_peakin, peak_ram_t buf_ram[NPEAK][256])
{
  ECPEAKEVENTFILLER(1);
  /*
#ifdef USE_PCAL
  cout << "-pcfill1-> addr="<<addr-1<<endl;
#else
  cout << "-ecfill1-> addr="<<addr-1<<endl;
#endif
  */
}
void
ecpeakeventfiller2(hls::stream<ECPeak_s> &s_peakin, peak_ram_t buf_ram[NPEAK][256])
{
  ECPEAKEVENTFILLER(2);
  /*
#ifdef USE_PCAL
  cout << "-pcfill2-> addr="<<addr-1<<endl;
#else
  cout << "-ecfill2-> addr="<<addr-1<<endl;
#endif
  */
}

#endif
