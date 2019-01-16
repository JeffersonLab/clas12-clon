
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


/* 2.66/1/1/0%/0%/(135)~0%/(58)~0% II=1 */

/* called every 4ns */

void
htcchiteventfiller(hls::stream<HTCCHit> &s_hitin, hit_ram_t buf_ram[NRAM])
{
#pragma HLS INTERFACE axis register both port=s_hitin
#pragma HLS DATA_PACK variable=s_hitin
#pragma HLS DATA_PACK variable=buf_ram
//#pragma HLS ARRAY_PARTITION variable=buf_ram block factor=4 dim=1
#pragma HLS PIPELINE II=1

  HTCCHit fifo;

  static ap_uint<RAM_BITS> itime = 0;

  fifo = s_hitin.read();
  buf_ram[itime].output = fifo.output;
#ifdef DEBUG
  cout<<"htcchiteventfiller: itime="<<itime<<endl;
#endif
  itime ++;
}
