
/* htcchitfanout.cc */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

using namespace std;
#include <strstream>



#include "htcclib.h"


//#define DEBUG


/* 0.0/5/4/0%/0%/(306)~0%/(231)~0% II=4 */

void
htcchitfanout(HTCCHit hitin[NH_READS], hls::stream<HTCCOut_8slices> &s_hits, HTCCHit hitout[NH_READS], volatile ap_uint<1> &hit_scaler_inc)
{
//#pragma HLS INTERFACE ap_none port=hit_scaler_inc
//#pragma HLS DATA_PACK variable=s_hits
//#pragma HLS INTERFACE axis register both port=s_hits
#pragma HLS PIPELINE II=1

  HTCCOut_8slices out;

  ap_uint<1> scaler[NH_READS];
  ap_uint<1> scaler_tmp = 0;

#ifdef __SYNTHESIS__
  static int first = 0;
  if(first==0)
  {
    first = 1;
    return;
  }
#endif

  for(int i=0; i<NH_READS; i++)
  {
    for(int j=0; j<NBIT_OUT; j++)
	{
	  /*was for 16-bit output when every bit in out.out corresponds to 3 counters
      int k=j*3;
      out.out[i](j,j) = hitin[i].output(k,k) | hitin[i].output(k+1,k+1) | hitin[i].output(k+2,k+2);
	  */
      out.out[i](j,j) = hitin[i].output(j,j);
	}

#ifdef DEBUG
    if(out.out[i]>0) cout<<"htcchitfanout: out.out["<<i<<"]="<<hex<<out.out[i]<<dec<<endl;
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


