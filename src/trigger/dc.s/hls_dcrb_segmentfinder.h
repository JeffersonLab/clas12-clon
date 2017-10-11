/* hls_dcrb_segmentfinder.h */

#ifndef hls_dcrb_segmentfinder_h
#define hls_dcrb_segmentfinder_h

#include <ap_int.h>
#include <ap_fixed.h>
#include <hls_stream.h>

typedef struct
{
  ap_uint<32> vals[7];
} vxs_words_t;

typedef ap_uint<64> fiber_word_t;
typedef ap_uint<96> dcrb_hits_t;
typedef ap_uint<112> dc_row_t;


void hls_dcrb_segmentfinder(ap_uint<3> hit_threshold, hls::stream<vxs_words_t> &s_vxs_words, hls::stream<fiber_word_t> &s_fiber_word);

#endif
