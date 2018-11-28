
/* dclib.cc */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include <iostream>
using namespace std;

#include "evio.h"
#include "evioBankUtil.h"

#include "dclib.h"
#include "dctrans.h"

#include "hls_fadc_sum.h"
#include "trigger.h"


//#define DEBUG


#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)




#if 0 /* dcrb event builder from Ben */
#include <hls_stream.h>
#include <ap_int.h>

typedef struct
{
  ap_uint<11> t_start;
  ap_uint<11> t_stop;
} trig_t;

#define TAG_DC_ROAD   0xC0

typedef ap_uint<71> eventram_t;
typedef ap_uint<33> eventdata_t;

#define S_IDLE    0
#define S_WRITE_0 1
#define S_WRITE_1 2
#define S_WRITE_2 3
#define S_END     4

void dcrbroadfinder_eb_hls(
    eventram_t eventram[512],
    hls::stream<trig_t> &trigger_stream,
    hls::stream<eventdata_t> &eventdata_stream
  )
{
  static ap_uint<9> pos, t;
  static ap_uint<3> state = S_IDLE;
  static trig_t trigger_data;
  static eventram_t eventram_word;
  eventdata_t eventdata;

  switch(state)
  {
    case S_IDLE:
      trigger_data = trigger_stream.read();
      pos = trigger_data.t_start(10,2);
      t = 0;
      state = S_WRITE_0;
      break;

    case S_WRITE_0:
      eventram_word = eventram[pos];

      if(eventram_word)
      {
        // word 0
        eventdata(32,32)  = 0;                    // end-of-event flag
        eventdata(31,31)  = 1;                    // data-defining flag
        eventdata(30,23)  = TAG_DC_ROAD;          // data type (0xC0)
        eventdata(22,17)  = eventram_word(70,65); // sl6,sl5,...,sl1
        eventdata(16,12)  = 0;                    // unused
        eventdata(11,11)  = eventram_word[62];    // road_inbend
        eventdata(10,10)  = eventram_word[63];    // road_outbend
        eventdata(9,9)    = eventram_word[64];    // road_valid
        eventdata(8,0)    = t;                    // time

        eventdata_stream.write(eventdata);
      }

      pos++;
      t++;

      state = S_WRITE_1;
      break;

    case S_WRITE_1:
      if(eventram_word)
      {
        eventdata(32,32)  = 0;                    // end-of-event flag
        eventdata(31,31)  = 0;                    // data-defining flag
        eventdata(30,0)   = eventram_word(61,31); // data type

        eventdata_stream.write(eventdata);
      }

      state = S_WRITE_2;
      break;

    case S_WRITE_2:
      if(eventram_word)
      {
        eventdata(32,32)  = 0;                    // end-of-event flag
        eventdata(31,31)  = 0;                    // data-defining flag
        eventdata(30,0)   = eventram_word(30,0);  // data type

        eventdata_stream.write(eventdata);
      }

      if(pos == trigger_data.t_stop(10,2))
        state = S_END;
      else
        state = S_WRITE_0;

      break;

    case S_END:
      state = S_IDLE;

      eventdata(32,32)        = 1;          // end-of-event flag
      eventdata(31,0)         = 0;          // n/a
      eventdata_stream.write(eventdata);
      break;
  }
}
#endif









void
dceventreader(hls::stream<eventdata5_t> segment_event_stream[NSLS], hls::stream<eventdata5_t> &road_event_stream, DCSegment segm[NSLS], DCRoad &road, uint32_t bufout[NSLS+1][RAMSIZE])
{

  eventdata5_t eventdata;
  uint32_t data_end, word_first, tag, inst, view, data, *bufptr;
  int j, isl, superlayer;


  /* SEGMENTS */

  for(isl=0; isl<NSLS; isl++)
  {
    bufptr = bufout[isl] + 1;


    while(1)
    {
      if(segment_event_stream[isl].empty())
      {
        bufout[isl][0] = 0;
        printf("dcsegmenteventreader: EMPTY STREAM ERROR1\n");
        break;
      }

      eventdata = segment_event_stream[isl].read();
      if(eventdata.end == 1) /* 0 for all words except last one when it is 1 */
      {
        bufout[isl][0] = bufptr - bufout[isl] - 1;
        /*printf("dcsegmenteventreader: END_OF_DATA\n");*/
        break;
      }


      *bufptr++ = eventdata.data[0];
      word_first = eventdata.data[0](31,31); /* 1 for the first word in segment, 0 for followings */
      tag = eventdata.data[0](30,23); /* must be 'DC1SEGM_TAG' */
      j = eventdata.data[0](18,16); /* 3 lowest bits of timing - always zero in our case since we have only one 32ns timing bin !?*/
      superlayer = eventdata.data[0](11,9); /* superlayer number */

      *bufptr++ = eventdata.data[1];
	  segm[isl].output(111,93) = eventdata.data[1](18,0);

      *bufptr++ = eventdata.data[2];
	  segm[isl].output(92,62) = eventdata.data[2](30,0);

      *bufptr++ = eventdata.data[3];
	  segm[isl].output(61,31) = eventdata.data[3](30,0);

      *bufptr++ = eventdata.data[4];
	  segm[isl].output(30,0)  = eventdata.data[4](30,0);
    }

  }


  /* ROADS */

  bufptr = bufout[NSLS] + 1;

  while(1)
  {
    if(road_event_stream.empty())
    {
      bufout[NSLS][0] = 0;
      printf("dcroadeventreader: EMPTY STREAM ERROR1\n");
      break;
    }

    eventdata = road_event_stream.read();
    if(eventdata.end == 1) /* 0 for all words except last one when it is 1 */
    {
      bufout[NSLS][0] = bufptr - bufout[NSLS] - 1;
      /*printf("dcroadeventreader: END_OF_DATA\n");*/
      break;
    }

    *bufptr++ = eventdata.data[0];
    word_first = eventdata.data[0](31,31); /* 1 for the first word in road, 0 for followings */
    tag = eventdata.data[0](30,23); /* must be 'DC1ROAD_TAG' */
    j = eventdata.data[0](18,16); /* 3 lowest bits of timing - always zero in our case since we have only one 32ns timing bin !?*/

    *bufptr++ = eventdata.data[1];
	road.output(111,93) = eventdata.data[1](18,0);

    *bufptr++ = eventdata.data[2];
	road.output(92,62) = eventdata.data[2](30,0);

    *bufptr++ = eventdata.data[3];
	road.output(61,31) = eventdata.data[3](30,0);

    *bufptr++ = eventdata.data[4];
	road.output(30,0)  = eventdata.data[4](30,0);

  }

}














static trig_t trig[NSLS+1]; /* stream to initiate readout, one for every dc..writer() call; assumed to be cleaned up because of 'static' */

void
dclib(uint32_t *bufptr, uint16_t threshold_[3], uint16_t nframes_)
{
  int status, ret, sec, crate, isl;
  int iev, nw;
  unsigned long long timestamp;

  int banktag = 0xe122; /* vtp bank tag */
  int fragtag;

  ap_uint<16> threshold[3] = {threshold_[0], threshold_[1], threshold_[2]};
  nframe_t nframes = nframes_;

  hls::stream<fadc_16ch_t> s_fadc_words[NFADCS];
  hls::stream<dcrb_96ch_t> s_dcrb_words[NSLOT];

  hls::stream<segment_word_t> s_segments[NSLS];
  hls::stream<road_word_t> s_roads;

  segment_word_t segment_tmp;
  road_word_t road_tmp;

  volatile ap_uint<1> segment_scaler_inc;
  volatile ap_uint<1> road_scaler_inc;

  hls::stream<trig_t> trig_stream[NSLS+1]; /* one stream for each event writer */
  hit_ram_t segm_buf_ram[NSLS][RAMSIZE]; /* always fill it by dcsegmenteventfiller.cc, index run over */
  hit_ram_t road_buf_ram[RAMSIZE]; /* always fill it by dcroadeventfiller.cc, index run over */


  hls::stream<eventdata5_t> segment_event_stream[NSLS];
  hls::stream<eventdata5_t> road_event_stream;
  DCSegment segm[2];
  DCRoad road;

  int roads_empty;
  uint32_t bufout[NSLS+1][RAMSIZE];

  int detector = DC;



  /***********/
  /* stage 1 */

  for(sec=0; sec<NSECTOR; sec++) /* loop over sectors */
  {

    for(int i=0; i<NSLS+1; i++) trig[i].t_stop = trig[i].t_start + DCRBTIMES * EBTICKS; /* set readout window DCRBTIMES*32ns in 8ns ticks */
#ifdef DEBUG
    cout<<"-dclib-> t_start="<<trig[0].t_start<<" t_stop="<<trig[0].t_stop<<endl;
#endif
    for(int i=0; i<NSLS+1; i++) trig_stream[i].write(trig[i]); /* one write per stream */


    for(crate=0; crate<3; crate++) /* loop over crates inside sector */
	{
      ret = fadcs(bufptr, threshold[0], (sec*3)+crate, detector, s_fadc_words, 0, 0, &iev, &timestamp); /* 3rd parameter here is crate number from 0 to 17; s_fadc_words has dimension 16 to be compatible with fadcs, we use 14 */
      if(ret<=0)
	  {
        /*break*/;
	  }
#ifdef DEBUG
      printf("dclib: segment search sec=%d crate=%d\n",sec,crate);
#endif

	  //printf("dclib 0\n");fflush(stdout);
      for(int it=0; it<DCRBTIMES; it++)
      {
		//printf("dclib 1 (crate=%d it=%d)\n",crate,it);fflush(stdout);
        /* first 7 slots */
        for(int i=0; i<NSLOT; i++) fadcs_32ns_to_dcrb_32ns(s_fadc_words[i], s_dcrb_words[i]);
        isl = crate*2;
        dc1(threshold, nframes, s_dcrb_words, s_segments[isl], segment_scaler_inc, segm_buf_ram[isl]);
        if(s_segments[isl].empty()) printf("ERROR: EMPTY0 isl=%d crate=%d sec=%d\n",isl,crate,sec);


        //printf("dclib 2\n");fflush(stdout);
        /* last 7 slots */
        for(int i=0; i<NSLOT; i++) fadcs_32ns_to_dcrb_32ns(s_fadc_words[i+7], s_dcrb_words[i]);
        isl = crate*2+1;
        dc1(threshold, nframes, s_dcrb_words, s_segments[isl], segment_scaler_inc, segm_buf_ram[isl]);
        if(s_segments[isl].empty()) printf("ERROR: EMPTY1 isl=%d crate=%d sec=%d\n",isl,crate,sec);

        //printf("dclib 3\n");fflush(stdout);
	  }

	  //printf("dclib 4\n");fflush(stdout);

	} /* loop over 3 crates inside sector */


    /***********/
    /* stage 2 */

    for(int it=0; it<DCRBTIMES; it++)
    {
	  //printf("dclib 41 it=%d\n",it);fflush(stdout);
      dc2(threshold, nframes, s_segments, s_roads, road_scaler_inc, road_buf_ram);
	  //printf("dclib 42\n");fflush(stdout);

      if(s_roads.empty()) roads_empty = 1;
      else                roads_empty = 0;
      if(s_roads.empty()) printf("ERROR: EMPTY2 roads_empty=%d\n",roads_empty);fflush(stdout);

      /* read roads to avoid 'leftover' warnings */
      if(!roads_empty)
	  {
        road_tmp = s_roads.read();
	  }
	  //printf("dclib 44\n");fflush(stdout);
    }


	//printf("dclib 5\n");fflush(stdout);



    /* activate event builder */
    for(int isl=0; isl<NSLS; isl++) dcsegmenteventwriter(trig_stream[isl], segment_event_stream[isl], segm_buf_ram[isl]);
    dcroadeventwriter(trig_stream[NSLS], road_event_stream, road_buf_ram);


    /* get data from event builder streams and create evio banks */
    dceventreader(segment_event_stream, road_event_stream, segm, road, bufout);
#ifdef DEBUG
    for(int isl=0; isl<=NSLS; isl++) for(int i=0; i<=bufout[isl][0]; i++) printf("DC bufout[%d][%d]=0x%08x\n",isl,i,bufout[isl][i]);
#endif




    /* create evio banks */

    /* region 1 */
    nw = bufout[0][0]+bufout[1][0];
    if(nw>0)
	{
      fragtag = 60119+sec; /* dc vtp rocid range for region1 is 119-124 */
      trigbank_open(bufptr, fragtag, banktag, iev, timestamp);
      if(bufout[0][0]>0) trigbank_write(bufout[0]);
      if(bufout[1][0]>0) trigbank_write(bufout[1]);
      trigbank_close(nw);
	}

    /* region 2 */
    nw = bufout[2][0]+bufout[3][0];
    if(nw>0)
	{
      fragtag = 60086+sec; /* dc vtp rocid range for region2 is 86-91 */
      trigbank_open(bufptr, fragtag, banktag, iev, timestamp);
      if(bufout[2][0]>0) trigbank_write(bufout[2]);
      if(bufout[3][0]>0) trigbank_write(bufout[3]);
      trigbank_close(nw);
	}

    /* region 3 */
    nw = bufout[4][0]+bufout[5][0]+bufout[6][0];
    if(nw>0)
	{
      fragtag = 60113+sec; /* dc vtp rocid range for region3 is 113-118 */
      trigbank_open(bufptr, fragtag, banktag, iev, timestamp);
      if(bufout[4][0]>0) trigbank_write(bufout[4]);
      if(bufout[5][0]>0) trigbank_write(bufout[5]);
      if(bufout[6][0]>0) trigbank_write(bufout[6]);
      trigbank_close(nw);
    }



	//printf("dclib 7\n");fflush(stdout);



    for(int i=0; i<NSLS+1; i++) trig[i].t_start += DCRBTIMES * EBTICKS; /* in preparation for next event, step up DCRBTIMES*32ns in 8ns ticks */

  } /* loop over sectors */


  return;
}
