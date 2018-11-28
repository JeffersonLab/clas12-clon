
/* ftofhiteventfiller.cc */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

using namespace std;
#include <strstream>


#include "ftoflib.h"


//#define DEBUG


/* 2.66/4/4/0%/0%/(194)~0%/(102)~0% II=4 */

void
ftofhiteventfiller(hls::stream<FTOFHit> s_hitin[NH_READS], hit_ram_t buf_ram[256])
{
#pragma HLS DATA_PACK variable=s_hitin
#pragma HLS INTERFACE axis register both port=s_hitin
#pragma HLS ARRAY_PARTITION variable=s_hitin complete dim=1
#pragma HLS DATA_PACK variable=buf_ram
//#pragma HLS ARRAY_PARTITION variable=buf_ram block factor=8 dim=1
#pragma HLS PIPELINE II=1

  FTOFHit fifo;

  static ap_uint<8> itime = 0;

  for(int i=0; i<NH_READS; i++)
  {
    fifo = s_hitin[i].read();

    buf_ram[itime].output[i] = fifo.output;
  }

  itime ++;
}


