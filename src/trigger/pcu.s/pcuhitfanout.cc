
/* pcuhitfanout.cc */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

using namespace std;
#include <strstream>



#include "pculib.h"


//#define DEBUG


/* 0.0/5/4/0%/0%/(306)~0%/(231)~0% II=4 */

void
pcuhitfanout(ap_uint<4> intime[NSTRIP], hls::stream<PCUHit_out> &s_hits, PCUHit hitout[NH_READS], volatile ap_uint<1> &hit_scaler_inc)
{
//#pragma HLS INTERFACE ap_none port=hit_scaler_inc
//#pragma HLS DATA_PACK variable=s_hits
//#pragma HLS INTERFACE axis register both port=s_hits
#pragma HLS PIPELINE II=1

  int it, is;
  ap_uint<1> scaler[NSTRIP];
  ap_uint<1> scaler_tmp = 0;
  PCUHit_out hit;

  for(int i=0; i<NH_READS; i++) hitout[i].output = 0;

  for(int i=0; i<NSTRIP; i++)
  {
    is = intime[i](3,3);
    it = intime[i](2,0);

    scaler[i] = 0;
    if(is>0) scaler[i] = 1;

    if(is>0) hitout[it].output(i,i) = 1;
  }



#ifdef DEBUG
  for(int i=0; i<NH_READS; i++) cout<<"pcuhitfanout: hitout["<<i<<"].output="<<hex<<hitout[i].output<<dec<<endl;
#endif


  for(int i=0; i<NSTRIP; i++)
  {
    hit.output[i] = intime[i];
#ifdef DEBUG
    if(hit.output[i]>0) cout<<"pcuhitfanout: hit.output["<<i<<"]="<<hit.output[i]<<endl;
#endif
  }
  s_hits.write(hit);
  

  for(int i=0; i<NSTRIP; i++)
  {
    scaler_tmp |= scaler[i];
  }

  if(scaler_tmp) hit_scaler_inc = 1;
  else           hit_scaler_inc = 0;
}


