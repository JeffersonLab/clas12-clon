
/* dcroadeventfiller.cc */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

using namespace std;
#include <strstream>


#include "dclib.h"


//#define DEBUG


/* 2.66/4/4/0%/0%/(194)~0%/(102)~0% II=4 */

void
dcroadeventfiller(road_word_t road, hit_ram_t buf_ram[RAMSIZE])
{
//#pragma HLS DATA_PACK variable=buf_ram
////#pragma HLS ARRAY_PARTITION variable=buf_ram block factor=8 dim=1
#pragma HLS PIPELINE II=1

  static ap_uint<8> itime = 0;

  buf_ram[itime].output(111,0) = road(111,0);
#ifdef DEBUG
  std::cout<<"dcroadeventfiller: itime="<<itime<<", buf_ram.output="<<buf_ram[itime].output<<endl;
#endif

  itime ++;
}


