
/* dcroadfanout.cc */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

using namespace std;
#include <strstream>



#include "dclib.h"


//#define DEBUG


/* 0.0/5/4/0%/0%/(306)~0%/(231)~0% II=4 */

void
dcroadfanout(road_word_t road, hls::stream<road_word_t> &s_roads, road_word_t &road2, volatile ap_uint<1> &hit_scaler_inc)
{
//#pragma HLS INTERFACE ap_none port=hit_scaler_inc
//#pragma HLS DATA_PACK variable=s_hits
//#pragma HLS INTERFACE axis register both port=s_hits
#pragma HLS PIPELINE II=1

  road_word_t road_tmp;

  road_tmp = road;

  hit_scaler_inc = 0;
  if(road_tmp != 0) hit_scaler_inc = 1;

  road2 = road_tmp;

  s_roads.write(road_tmp);

}


