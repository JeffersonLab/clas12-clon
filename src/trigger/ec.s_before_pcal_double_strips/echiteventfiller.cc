
/* echiteventfiller.cc */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

using namespace std;
#include <strstream>


#ifdef USE_PCAL

#include "../pc.s/pclib.h"
#define echiteventfiller pchiteventfiller

#else

#include "eclib.h"

#endif

//#define DEBUG


/* 2.66/4/4/0%/0%/(194)~0%/(102)~0% II=4 */

void
echiteventfiller(hls::stream<ECHit> &s_hitin, hit_ram_t buf_ram[NHIT][256])
{
#pragma HLS INTERFACE axis register both port=s_hitin
#pragma HLS DATA_PACK variable=s_hitin
#pragma HLS DATA_PACK variable=buf_ram
#pragma HLS ARRAY_PARTITION variable=buf_ram block factor=4 dim=1
#pragma HLS PIPELINE II=4
  ECHit hitin;
  ECHit fifo;

  static ap_uint<8> itime = 0;

  for(int i=0; i<NHIT; i++)
  {
	fifo = s_hitin.read();

    hitin.energy = fifo.energy;
    for(int j=0; j<3; j++) hitin.coord[j] = fifo.coord[j];
#ifndef __SYNTHESIS__
    hitin.ind = fifo.ind;
    for(int j=0; j<3; j++) hitin.enpeak[j] = fifo.enpeak[j];
#endif

    buf_ram[i][itime].energy = hitin.energy;
    buf_ram[i][itime].coordU = hitin.coord[0];
    buf_ram[i][itime].coordV = hitin.coord[1];
    buf_ram[i][itime].coordW = hitin.coord[2];
#ifndef __SYNTHESIS__
    buf_ram[i][itime].ind = hitin.ind;
    buf_ram[i][itime].enpeakU = hitin.enpeak[0];
    buf_ram[i][itime].enpeakV = hitin.enpeak[1];
    buf_ram[i][itime].enpeakW = hitin.enpeak[2];
#endif
  }

/*#ifdef __SYNTHESIS__*/
  itime ++;
/*#endif*/
}
