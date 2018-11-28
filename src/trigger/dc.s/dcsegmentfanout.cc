
/* dcsegmentfanout.cc */


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
dcsegmentfanout(segment_word_t segment, hls::stream<segment_word_t> &s_segments, segment_word_t &segment2, volatile ap_uint<1> &hit_scaler_inc)
{
//#pragma HLS INTERFACE ap_none port=hit_scaler_inc
//#pragma HLS DATA_PACK variable=s_hits
//#pragma HLS INTERFACE axis register both port=s_hits
#pragma HLS PIPELINE II=1

  segment_word_t segment_tmp;

#ifdef DEBUG
  printf("== dcsegmentfanout starts ==\n");fflush(stdout);
#endif

  segment_tmp = segment;

  hit_scaler_inc = 0;
  if(segment_tmp != 0) hit_scaler_inc = 1;

  segment2 = segment_tmp;
#ifdef DEBUG
  cout<<"segment2="<<hex<<segment2<<dec<<endl;
#endif

  s_segments.write(segment_tmp);

#ifdef DEBUG
  printf("== dcsegmentfanout ends ==\n");fflush(stdout);
#endif

}


