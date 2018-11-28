
/* dcsegments.cc */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

//#define DEBUG

#ifdef DEBUG
#include <iostream>
using namespace std;
#endif

#include "dclib.h"
#include "sgutil.h"


void
dcsegments(ap_uint<3> hit_threshold, DCSL_s superlayer, segment_word_t &segment_word)
{
  int layer_map[6] = {1,3,5,0,2,4};
  DCSL_s vxs_words[4][8];
  DCSL_s vxs_words_or[4];
  dcrb_hits_t dcrb_hits;
  word112_t dc_hits[6];
  word112_t dc_segs[NOFFSETS];

#ifdef DEBUG
  printf("== dcsegments starts ==\n");fflush(stdout);
#endif

#ifdef DEBUG
  cout<<"superlayer="<<hex<<superlayer.layer[5]<<" "<<superlayer.layer[4]<<" "<<superlayer.layer[3]<<" "<<superlayer.layer[2]<<" "<<superlayer.layer[1]<<" "<<superlayer.layer[0]<<dec<<endl;
#endif

  for(int layer=0; layer<NLAYERS; layer++)
  {
    for(int wire=0; wire<NWIRES; wire++)
    {
      dc_hits[layer][wire] = superlayer.layer[layer](wire,wire);
	}
  }


  SegmentSearch3(dc_hits, dc_segs, hit_threshold);

  /* Send segments to 512-bit Fiber interface */
  /*for(int i=0;i<16;i++)*/
  int i = 7; /* send only middle segment for now */
  {
    segment_word(55, 0)   = dc_segs[i].range(55,0);
    segment_word(111, 56) = dc_segs[i].range(111,56);
    segment_word(511,112) = 0;
  }

#ifdef DEBUG
  cout<<"dc_segs="<<hex<<dc_segs[i]<<dec<<endl;
#endif
#ifdef DEBUG
  cout<<"segment="<<hex<<segment_word<<dec<<endl;
#endif

#ifdef DEBUG
  printf("== dcsegments ends ==\n");fflush(stdout);
#endif

}
