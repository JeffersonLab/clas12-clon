
/* htcchiteventfiller.cc */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

using namespace std;
#include <strstream>


#include "htcclib.h"


//#define DEBUG


/* 2.66/4/4/0%/0%/(194)~0%/(102)~0% II=4 */

void
htcchiteventfiller(hls::stream<HTCCHit> &s_hitin, hit_ram_t buf_ram[256])
{
#pragma HLS INTERFACE axis register both port=s_hitin
#pragma HLS DATA_PACK variable=s_hitin
#pragma HLS DATA_PACK variable=buf_ram
//#pragma HLS ARRAY_PARTITION variable=buf_ram block factor=4 dim=1
#pragma HLS PIPELINE II=1

  HTCCHit fifo;

  static ap_uint<8> itime = 0;

  fifo = s_hitin.read();
  buf_ram[itime].output = fifo.output;

#ifdef __SYNTHESIS__
  itime ++;
#endif
}


