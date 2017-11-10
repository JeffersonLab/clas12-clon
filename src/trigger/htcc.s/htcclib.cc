
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include <iostream>
using namespace std;

#include "evio.h"
#include "evioBankUtil.h"

#include "htcclib.h"
#include "htcctrans.h"

#include "hls_fadc_sum.h"
#include "trigger.h"


//#define DEBUG


#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)

static float ped[21][16]; /* pedestals */
static float tet[21][16]; /* threshold */
static float gain[21][16]; /* gain */
static int nsa[21]; /* NSA */
static int nsb[21]; /* NSB */


void
htcchiteventreader(hls::stream<eventdata_t> &event_stream, HTCCHit &hit, uint32_t *bufout)
{
  eventdata_t eventdata;
  uint32_t data_end, word_first, tag, inst, view, data, *bufptr = bufout + 1;
  while(1)
  {
    if(event_stream.empty()) {bufout[0]=0;printf("htcchiteventreader: EMPTY STREAM ERROR1\n");break;}

    eventdata = event_stream.read();
    if(eventdata.end == 1)
    {
      bufout[0] = bufptr - bufout - 1;
      /*printf("htcchiteventreader: END_OF_DATA\n");*/
      break;
    }

    *bufptr++ = eventdata.data;
    data_end = eventdata.end;           /* 0 for all words except last one when it is 1 */
    word_first = eventdata.data(31,31); /* 1 for the first word in hit, 0 for followings */
    tag = eventdata.data(30,27); /* must be 'HTCCHIT_TAG' */

    if(event_stream.empty()) {printf("htcchiteventreader: EMPTY STREAM ERROR2\n");break;}
    eventdata = event_stream.read();
    *bufptr++ = eventdata.data;
    data_end = eventdata.end;
	hit.output(47,31) = eventdata.data(16,0);
    /*cout<<"htcchiteventreader: output="<<hit.output<<endl;*/

    if(event_stream.empty()) {printf("htcchiteventreader: EMPTY STREAM ERROR2\n");break;}
    eventdata = event_stream.read();
    *bufptr++ = eventdata.data;
    data_end = eventdata.end;
	hit.output(30,0) = eventdata.data(30,0);
    /*cout<<"htcchiteventreader: output="<<hit.output<<endl;*/
  }
}







static trig_t trig; /* assumed to be cleaned up because of 'static' */

void
htcclib(uint32_t *bufptr, uint16_t threshold_[3], uint16_t nframes_)
{
  ap_uint<16> threshold[3] = {threshold_[0], threshold_[1], threshold_[2]};
  nframe_t nframes = nframes_;

  int status, ret;
  int iev;
  unsigned long long timestamp;

  uint32_t bufout[NRAM];

  hls::stream<fadc_16ch_t> s_fadcs[NFADCS];
  hls::stream<fadc_2ch_t> s_fadc_words[NFADCS];
  hls::stream<HTCCHit> s_hits;
  HTCCHit hit_tmp;

  hls::stream<trig_t> trig_stream;
  hit_ram_t buf_ram[NRAM];
  hls::stream<eventdata_t> event_stream;
  HTCCHit hit;

  int detector = HTCC;
  int sec = 0;
  int nslot = 3;


  ret = fadcs(bufptr, threshold[0], sec, detector, s_fadcs, 0, 0, &iev, &timestamp);
  if(ret<=0) return;

  trig.t_stop = trig.t_start + MAXTIMES*NH_READS; /* set readout window MAXTIMES*32ns in 4ns ticks */
#ifdef DEBUG
  cout<<"-htcclib-> t_start="<<trig.t_start<<" t_stop="<<trig.t_stop<<endl;
#endif
  trig_stream.write(trig);

  for(int it=0; it<MAXTIMES; it++)
  {
#ifdef DEBUG
 	printf("htcclib: timing slice = %d\n",it);fflush(stdout);
#endif
    /* adjust to 4ns domain */
    for(int i=0; i<nslot; i++) fadcs_32ns_to_4ns(s_fadcs[i], s_fadc_words[i]); /* 1 read, 8 writes */

    htcc(threshold, nframes, s_fadc_words, s_hits, buf_ram);

    /* read hits to avoid 'leftover' warnings */
    for(int i=0; i<NH_READS; i++) hit_tmp = s_hits.read();
  }

  htcchiteventwriter(trig_stream, event_stream, buf_ram);
  htcchiteventreader(event_stream, hit, bufout);

  if(bufout[0]>0)
  {
    int fragtag = 60093;
    int banktag = 0xe122;
    trigbank_open(bufptr, fragtag, banktag, iev, timestamp);
    trigbank_write(bufout);
    trigbank_close();
  }

  trig.t_start += MAXTIMES*8; /* in preparation for next event, step up MAXTIMES*32ns in 4ns ticks */

  return;
}
