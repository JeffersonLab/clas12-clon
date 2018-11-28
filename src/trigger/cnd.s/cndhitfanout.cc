
/* cndhitfanout.cc */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

using namespace std;
#include <strstream>



#include "cndlib.h"


//#define DEBUG


/* 0.0/5/4/0%/0%/(306)~0%/(231)~0% II=4 */

void
cndhitfanout(CNDHit hitin[NH_READS], hls::stream<CNDOut_8slices> &s_hits, CNDHit hitout[NH_READS], volatile ap_uint<1> &hit_scaler_inc)
{
//#pragma HLS INTERFACE ap_none port=hit_scaler_inc
//#pragma HLS DATA_PACK variable=s_hits
//#pragma HLS INTERFACE axis register both port=s_hits
#pragma HLS PIPELINE II=1

  CNDOut_8slices out;
  ap_uint<1> scaler[NH_READS];
  ap_uint<1> scaler_tmp = 0;

  static int first = 0;
  if(first==0)
  {
    first = 1;
    return;
  }

#ifdef DEBUG
    cout<<"cndhitfanout =="<<endl;
#endif

  for(int i=0; i<NH_READS; i++)
  {
    /* hitout is 72-bit long, we'll take lower 48-bit and send to out */
    out.out[i](47,0) = hitin[i].output(47,0);
    out.out[i](63,48) = 0;
#ifdef DEBUG
    if(out.out[i]>0) cout<<"cndhitfanout: out.out["<<i<<"]="<<hex<<out.out[i]<<dec<<endl;
#endif

    scaler[i] = 0;
    if(out.out[i] != 0) scaler[i] = 1;

    hitout[i].output = hitin[i].output;
  }

  s_hits.write(out);

  for(int i=0; i<NH_READS; i++)
  {
    scaler_tmp |= scaler[i];
  }

  if(scaler_tmp) hit_scaler_inc = 1;
  else           hit_scaler_inc = 0;
}


