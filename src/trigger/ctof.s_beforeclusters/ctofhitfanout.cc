
/* ctofhitfanout.cc */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

using namespace std;
#include <strstream>



#include "ctoflib.h"


//#define DEBUG


/* 0.0/5/4/0%/0%/(306)~0%/(231)~0% II=4 */

void
ctofhitfanout(CTOFHit hitin[NH_READS], hls::stream<CTOFOut_8slices> &s_hits, CTOFHit hitout[NH_READS], volatile ap_uint<1> &hit_scaler_inc)
{
//#pragma HLS INTERFACE ap_none port=hit_scaler_inc
//#pragma HLS DATA_PACK variable=s_hits
//#pragma HLS INTERFACE axis register both port=s_hits
#pragma HLS PIPELINE II=1

  CTOFOut_8slices out;

  ap_uint<1> scaler[NH_READS];
  ap_uint<1> scaler_tmp = 0;

  for(int i=0; i<NH_READS; i++)
  {
    /* every bit in out.out corresponds to 3 counters */
    for(int j=0; j<NBIT_OUT; j++)
	{
      int k=j*3;
      out.out[i](j,j) = hitin[i].output(k,k) | hitin[i].output(k+1,k+1) | hitin[i].output(k+2,k+2);
	}

#ifdef DEBUG
    if(out.out[i]>0) cout<<"ctofhitfanout: out.out["<<i<<"]="<<hex<<out.out[i]<<dec<<endl;
#endif

    scaler[i] = 0;
    if(out.out[i] != 0) scaler[i] = 1;

    hitout[i].output = hitin[i].output;
    hitout[i].standalone = hitin[i].standalone;
  }

  s_hits.write(out);

  for(int i=0; i<NH_READS; i++)
  {
    scaler_tmp |= scaler[i];
  }

  if(scaler_tmp) hit_scaler_inc = 1;
  else           hit_scaler_inc = 0;
}


