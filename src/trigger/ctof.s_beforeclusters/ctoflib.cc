
/* ctoflib.cc */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include <iostream>
using namespace std;

#include "evio.h"
#include "evioBankUtil.h"

#include "ctoflib.h"
#include "ctoftrans.h"

#include "hls_fadc_sum.h"
#include "trigger.h"


#define DEBUG


#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)

static int the_number_of_banks = 0;

void
ctofhiteventreader(hls::stream<eventdata3_t> &event_stream, CTOFHit_8slices &hit, uint32_t *bufout)
{
  eventdata3_t eventdata;
  uint32_t data_end, word_first, tag, inst, view, data, *bufptr = bufout + 1;
  int j;
  while(1)
  {
    if(event_stream.empty())
    {
      bufout[0] = 0;
      printf("ctofhiteventreader: EMPTY STREAM ERROR1\n");
      break;
    }

    eventdata = event_stream.read();
    if(eventdata.end == 1) /* 0 for all words except last one when it is 1 */
    {
      bufout[0] = bufptr - bufout - 1;
      //printf("ctofhiteventreader: END_OF_DATA\n");
      break;
    }

    *bufptr++ = eventdata.data[0];  
    word_first = eventdata.data[0](31,31); /* 1 for the first word in hit, 0 for followings */
    tag = eventdata.data[0](30,27); /* must be 'CTOFHIT_TAG' */
    j = eventdata.data[0](18,16); /* 3 lowest bits of timing */

    *bufptr++ = eventdata.data[1];
	hit.output[j](47,31) = eventdata.data[1](16,0);

    *bufptr++ = eventdata.data[2];
	hit.output[j](30,0) = eventdata.data[2](30,0);
  }

}



void
ctof_buf_ram_to_event_buf_ram(hit_ram_t buf_ram[512], event_ram_t event_buf_ram[4096])
{
  for(int i=0; i<512; i++)
  {
    event_buf_ram[i*8+0].output = buf_ram[i].output[0];
    event_buf_ram[i*8+1].output = buf_ram[i].output[1];
    event_buf_ram[i*8+2].output = buf_ram[i].output[2];
    event_buf_ram[i*8+3].output = buf_ram[i].output[3];
    event_buf_ram[i*8+4].output = buf_ram[i].output[4];
    event_buf_ram[i*8+5].output = buf_ram[i].output[5];
    event_buf_ram[i*8+6].output = buf_ram[i].output[6];
    event_buf_ram[i*8+7].output = buf_ram[i].output[7];
  }
}




static trig_t trig; /* assumed to be cleaned up because of 'static' */

void
ctoflib(uint32_t *bufptr, uint16_t threshold_[3], uint16_t nframes_)
{
  int status, ret, sec;
  int iev;
  unsigned long long timestamp;

  ap_uint<16> threshold[3] = {threshold_[0], threshold_[1], threshold_[2]};
  nframe_t nframes = nframes_;

  hls::stream<fadc_16ch_t> s_fadc_words[NSLOT];
  hls::stream<fadc_256ch_t> s_fadcs;
  hls::stream<CTOFOut_8slices> s_hits;
  CTOFOut_8slices hit_tmp;
  volatile ap_uint<1> hit_scaler_inc;

  hls::stream<trig_t> trig_stream;
  hit_ram_t buf_ram[512];
  event_ram_t event_buf_ram[4096];
  hls::stream<eventdata3_t> event_stream;
  CTOFHit_8slices hit;

  uint32_t bufout[2048];

  int detector = CTOF;

  for(sec=0; sec<1/*NSECTOR*/; sec++)
  {
    ret = fadcs(bufptr, threshold[0], sec, detector, s_fadc_words, 0, 0, &iev, &timestamp);
    if(ret<=0) continue;

    trig.t_stop = trig.t_start + MAXTIMES*NH_READS; /* set readout window MAXTIMES*32ns in 4ns ticks */
    trig_stream.write(trig);

    for(int it=0; it<MAXTIMES; it++)
    {
      fadcs_to_onestream(NSLOT, s_fadc_words, s_fadcs);
      ctof(threshold, nframes, s_fadcs, s_hits, hit_scaler_inc, buf_ram);

      /* read hits to avoid 'leftover' warnings */
      if(!s_hits.empty()) hit_tmp = s_hits.read();
    }

    ctof_buf_ram_to_event_buf_ram(buf_ram, event_buf_ram);
    ctofhiteventwriter(trig_stream, event_stream, event_buf_ram);
    ctofhiteventreader(event_stream, hit, bufout);
#ifdef DEBUG
    for(int i=0; i<=bufout[0]; i++) printf("CTOF bufout[%d]=0x%08x\n",i,bufout[i]);
#endif
    if(bufout[0]>0) /*bufout contains something */
	{
#ifdef DEBUG
      printf("CTOF BANK %d\n",the_number_of_banks++);
#endif
      int fragtag = 61093;
      int banktag = 0xe122;
      trigbank_open(bufptr, fragtag, banktag, iev, timestamp);
      trigbank_write(bufout);
      trigbank_close(bufout[0]);
	}


    trig.t_start += MAXTIMES*8; /* in preparation for next event, step up MAXTIMES*32ns in 4ns ticks */
  }


  return;
}
