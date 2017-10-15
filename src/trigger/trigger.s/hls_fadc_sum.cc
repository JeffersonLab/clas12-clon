
/* hls_fadc_sum.cc - reading from the VXS stream. So for EC inner you would need 7 streams to read
 from 7 FADCs and then transform it to work with the inputs to you function. For the fiber it is just a hls::stream of type ap_uint<64>.
 For EC you have to share this with inner and outer.

 It would be interesting to see how this code runs if you feed streaming FADC hits every 32ns into it and make sure it behaves like you want.
*/

#include "hls_fadc_sum.h"

esum_t
fadc_sum(fadc_hits_t &fadc_hits, ap_uint<3> t)
{
  ap_uint<16> sum = 0;

  for(int i=0; i<16; i++)
  {
	sum += (fadc_hits.hits[i].t == t) ? fadc_hits.hits[i].e : (ap_uint<13>)0;
  }

  return( (sum(15,14) == 0) ? sum(13,0) : (esum_t)0x3FFF );
}


/* read one FADC board:
     input:
        esum_en - 16-bit mask showing channels with or without hits
        s_fadc_words - 32-bit stream (2 X (13-bit adcs and 3-bit times))
     output:
        s_esum - 
*/
void
hls_fadc_sum(ap_uint<16> esum_en,  hls::stream<fadc_word_t> &s_fadc_words,  hls::stream<esum_t> &s_esum)
{
  fadc_word_t fadc_words;
  fadc_hits_t fadc_hits;

  /* read one FADC board from stream with format 'fadc_word_t', produces 16 'fadc_hits_t' hits */
  for(int i=0; i<8; i++)
  {
	fadc_words = s_fadc_words.read();

	fadc_hits.hits[2*i+0].t = fadc_words.t0;
	fadc_hits.hits[2*i+0].e = esum_en[2*i+0] ? fadc_words.e0 : (ap_uint<13>)0;

	fadc_hits.hits[2*i+1].t = fadc_words.t1;
	fadc_hits.hits[2*i+1].e = esum_en[2*i+1] ? fadc_words.e1 : (ap_uint<13>)0;
  }

  for(int i=0; i<8; i++)
  {
	esum_t esum = fadc_sum(fadc_hits, i);
	s_esum.write(esum);
  }
}
