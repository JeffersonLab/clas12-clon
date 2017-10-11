/* hls_dcrb_segmentfinder */

#include "hls_dcrb_segmentfinder.h"
#include "sgutil.h"

void
hls_dcrb_segmentfinder(
    ap_uint<3> hit_threshold,
    hls::stream<vxs_words_t> &s_vxs_words,
    hls::stream<fiber_word_t> &s_fiber_word
  )
{
  int layer_map[6] = {1,3,5,0,2,4};
  vxs_words_t vxs_words[4][8];
  vxs_words_t vxs_words_or[4];
  dcrb_hits_t dcrb_hits;
  word112_t dc_hits[6];
  word112_t dc_segs[NOFFSETS];

  // Find segments in 256ns chunks of hits
  for(int iter=0; iter<8; iter++)
  {
    // Read complete DCRB trigger data frame (32ns of hits)
    for(int i=0;i<4;i++) vxs_words[i][iter] = s_vxs_words.read();
  }

  for(int i=0; i<4; i++)
  {
  	for(int j=0; j<7; j++)
  	{
  	  vxs_words_or[i].vals[j] =
  		vxs_words[i][0].vals[j] | vxs_words[i][1].vals[j] |
  		vxs_words[i][2].vals[j] | vxs_words[i][3].vals[j] |
  		vxs_words[i][4].vals[j] | vxs_words[i][5].vals[j] |
  		vxs_words[i][6].vals[j] | vxs_words[i][7].vals[j];
  	}
  }

  for(int i=0; i<7; i++)
  {
    // Extract DCRB hits
    dcrb_hits(23, 0) = vxs_words_or[0].vals[i].range(23,0);
    dcrb_hits(47,24) = vxs_words_or[1].vals[i].range(23,0);
    dcrb_hits(71,48) = vxs_words_or[2].vals[i].range(23,0);
    dcrb_hits(95,72) = vxs_words_or[3].vals[i].range(23,0);

    // Organize DCRB hits into chamber wire/layer format
    for(int j=0; j<96; j++)
    {
      int layer = layer_map[j%6];
      int wire = i*16+j/6;
      dc_hits[layer][wire] = dcrb_hits[j];
    }
  }

  SegmentSearch3(dc_hits, dc_segs, hit_threshold);

  // Send segments to Fiber interface
  for(int i=0;i<16;i++)
  {
    if(i < 15)
    {
      fiber_word_t fiber_word = 0;

      fiber_word(55, 0) = dc_segs[i].range(55,0);
      fiber_word(63,56) = (i<<1)+0;
      s_fiber_word.write(fiber_word);

      fiber_word(55, 0) = dc_segs[i].range(111,56);
      fiber_word(63,56) = (i<<1)+1;
      s_fiber_word.write(fiber_word);
    }
    else
    {
      s_fiber_word.write(0xFFFFFFFFFFFFFFFFLL);
      s_fiber_word.write(0xFFFFFFFFFFFFFFFFLL);
    }
  }
}
