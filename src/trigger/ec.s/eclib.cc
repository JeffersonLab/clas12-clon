/************************************************

  eclib.c - forward calorimeter reconstruction

  BUG report: boiarino@jlab.org

  TO DO: 'gap' parameter in ecpeak() not in use -> use it !!! see ecpeakc()

*************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#include <iostream>
using namespace std;

#include "evio.h"
#include "evioBankUtil.h"


#ifdef USE_PCAL
#include "../pc.s/pctrans.h"
#include "../pc.s/pclib.h"
#define ecstrips pcstrips
#define eclib pclib
#define ecpeakeventreader pcpeakeventreader
#define echiteventreader pchiteventreader
#define ectrig pctrig
#else
#include "ectrans.h"
#include "eclib.h"
#endif

#include "hls_fadc_sum.h"
#include "trigger.h"


//#define DEBUG
//#define DEBUG_0
//#define DEBUG_1
//#define DEBUG_2 /* hardware trigger info */
//#define DEBUG_3


#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)
#define ABS(x)      ((x) < 0 ? -(x) : (x))


/* some functions for simulation only */

void
ecstrip_in(hls::stream<ECStrip_s> s_strip[NF1], ECStrip strip[NSTRIP])
{
  ECStrip_s fifo[NF1*NH_READS];

  for(int j=0; j<NH_READS; j++)
  {
    for(int i=0; i<NF1; i++)
    {
      fifo[j*NF1+i] = s_strip[i].read();
    }
  }

  for(int i=0; i<NSTRIP/2; i++)
  {
	strip[i*2+0].energy = fifo[i].energy0; 
	strip[i*2+1].energy = fifo[i].energy1;
  }
}


void
ecstrip_out(ECStrip strip[NSTRIP], hls::stream<ECStrip_s> s_strip[NF1])
{
  ECStrip_s fifo[NF1*NH_READS];

  for(int i=0; i<NSTRIP/2; i++)
  {
	fifo[i].energy0 = strip[i*2+0].energy;
	fifo[i].energy1 = strip[i*2+1].energy;
  }

  for(int j=0; j<NH_READS; j++)
  {
    for(int i=0; i<NF1; i++)
    {
      s_strip[i].write(fifo[j*NF1+i]);
    }
  }
}


void
ecpeak_in(hls::stream<ECPeak_s> &s_peak, ECPeak peak[NPEAK])
{
  ECPeak_s fifo[NPEAK];

  for(int j=0; j<NPEAK; j++)
  {
	fifo[j] = s_peak.read();
  }

  for(int i=0; i<NPEAK; i++)
  {
    peak[i].energy = fifo[i].energy;
    peak[i].coord = fifo[i].coord;
#ifndef __SYNTHESIS__
    peak[i].strip1 = fifo[i].strip1;
    peak[i].stripn = fifo[i].stripn;
#endif
  }

}

void
echit_in(hls::stream<ECHit> &s_hits, ECHit hit[NHIT])
{
  for(int i=0; i<NHIT; i++) hit[i] = s_hits.read();
}











void
ecpeakeventreader(hls::stream<eventdata_t> &event_stream, ECPeak peak[NPEAK], uint32_t *bufout)
{
  eventdata_t eventdata;
  uint32_t data_end=0, word_first=0, tag=0, inst=0, view=0, it=0, *bufptr = bufout+1; /* reserve length word */
  int i = 0;
  while(1)
  {
    if(event_stream.empty()) {bufout[0]=0;printf("ecpeakeventreader: EMPTY STREAM 1\n"); break;}

    eventdata = event_stream.read();
    if(eventdata.end == 1)
    {
      bufout[0] = bufptr - bufout - 1; /* do not count end_of_data */
      /*printf("ecpeakeventreader: END_OF_DATA\n");*/
      break;
    }

    *bufptr++ = eventdata.data;
    data_end = eventdata.end;              /* 0 for all words except last one when it is 1 */
    word_first = eventdata.data(31,31);    /* 1 for the first word in peak, 0 for followings */
    tag = eventdata.data(30,27);           /* must be 'ECPEAK_TAG' */
	/*printf("ecpeakeventreader[it=%d i=%d]: tag=%d\n",it,i,tag);*/
    inst = eventdata.data(26,26);          /* instance */
    view = eventdata.data(25,24);          /* view */
    peak[i].strip1 = eventdata.data(13,7); /* first strip in peak (not in hardware) */
    peak[i].stripn = eventdata.data(6,0);  /* the number of strips in peak (not in hardware) */

    if(event_stream.empty()) {printf("ecpeakeventreader: EMPTY STREAM 2\n");break;}
    eventdata = event_stream.read();
    *bufptr++ = eventdata.data;
    data_end = eventdata.end;
    peak[i].coord = eventdata.data(25,16);
	peak[i].energy = eventdata.data(15,0);

    i++;
  }

  for(int j=i; j<NPEAK; j++) peak[j].energy = 0;
}



void
echiteventreader(hls::stream<eventdata_t> &event_stream, ECHit hit[NHIT], uint32_t *bufout)
{
  eventdata_t eventdata;
  uint32_t data_end=0, word_first=0, tag=0, inst=0, view=0, it=0, *bufptr = bufout+1, nw;
  int i = 0;
  while(1)
  {
    if(event_stream.empty()) {printf("echiteventreader: EMPTY STREAM 1\n");break;}

    eventdata = event_stream.read();
    if(eventdata.end == 1)
    {
      bufout[0] = bufptr - bufout - 1;
      /*printf("echiteventreader: END_OF_DATA\n");*/
      break;
    }

    *bufptr++ = eventdata.data;
    data_end = eventdata.end;           /* 0 for all words except last one when it is 1 */
    word_first = eventdata.data(31,31); /* 1 for the first word in hit, 0 for followings */
    tag = eventdata.data(30,27); /* must be 'ECHIT_TAG' */
	/*printf("echiteventreader[it=%d i=%d]: tag=%d\n",it,i,tag);*/
    inst = eventdata.data(26,26); /* instance */
    hit[i].energy = eventdata.data(15,0);

    if(event_stream.empty()) {printf("echiteventreader: EMPTY STREAM 2\n");break;}
    eventdata = event_stream.read();
    *bufptr++ = eventdata.data;
    data_end = eventdata.end;
    hit[i].coord[2] = eventdata.data(29,20);
    hit[i].coord[1] = eventdata.data(19,10);
	hit[i].coord[0] = eventdata.data(9,0);

    if(event_stream.empty()) {printf("echiteventreader: EMPTY STREAM 3\n");break;}
    eventdata = event_stream.read();
    *bufptr++ = eventdata.data;
    data_end = eventdata.end;
    hit[i].ind       = eventdata.data(21,16); /* not in hardware */
    hit[i].enpeak[2] = eventdata.data(15,0); /* not in hardware */

    if(event_stream.empty()) {printf("echiteventreader: EMPTY STREAM 4\n");break;}
    eventdata = event_stream.read();
    *bufptr++ = eventdata.data;
    data_end = eventdata.end;
    hit[i].enpeak[1] = eventdata.data(31,16); /* not in hardware */
    hit[i].enpeak[0] = eventdata.data(15,0); /* not in hardware */

    i++;
  }

  for(int j=i; j<NHIT; j++) hit[j].energy = 0;
}







/*********************************************************
                some useful utilities
**********************************************************/


int
ectrig(unsigned int *bufptr, int sec, int npeak[NVIEW], TrigECPeak peak[NVIEW][NPEAKMAX], int *nhit, TrigECHit hit[NHIT])
{
  int i, j, k, ind, nhits, npeaks, instance, view, str, layer, error, ii, nbytes, ind_data, nn, mm;
  int energy;
  long long timestamp, timestamp_old;
  GET_PUT_INIT;
#ifdef USE_PCAL
  int tag[6] = {3,9,15,21,27,33}; /* tag for sectors */
#else
  int tag[6] = {1,7,13,19,25,31}; /* tag for sectors */
#endif
  int fragtag, fragnum, banktag, banknum;
  unsigned short pulse_time, pulse_min, pulse_max;
  unsigned int pulse_integral;
  unsigned char *end;
  unsigned long long time;
  int crate,slot,trig,nchan,chan,npulses,notvalid,edge,data,count,ncol1,nrow1;
  int ndata0[22], data0[21][8];
  int baseline, sum, channel;

  uint32_t flag, index, val, last_val, v, coord, coordU, coordV, coordW;
  char view_array[4] = {'U', 'V', 'W', '?'};

#ifdef DEBUG_2
  printf("ectrig reached, sector=%d\n",sec);
#endif


  bzero((char *)peak,NVIEW*NPEAKMAX*sizeof(TrigECPeak));
  bzero((char *)npeak,NVIEW*sizeof(int));
  bzero((char *)hit,NHIT*sizeof(TrigECPeak));
  *nhit = 0;
  npeaks = 0;
  nhits = 0;


#ifdef USE_PCAL
  fragtag = 60107+sec;
#else
  fragtag = 60101+sec;
#endif
  fragnum = -1;
  banktag = 0xe122;
  banknum = 255;

  if((ind = evLinkBank(bufptr, fragtag, fragnum, banktag, banknum, &nbytes, &ind_data)) <= 0)
  {
    printf("cannot find bank tag=0x%04x num=%d in fragtag=%d fragnum=%d (trigger)\n",banktag,banknum,fragtag,fragnum);
    return(0);
  }
#ifdef DEBUG_2
  printf("\n\n== ecal/pcal trigger bank: ind=%d ind_data=%d\n",ind, ind_data);
#endif

#ifdef DEBUG_2
  printf("ind=%d, nbytes=%d\n",ind,nbytes);
#endif
  b08 = (unsigned char *) &bufptr[ind_data];
  end = b08 + nbytes;
#ifdef DEBUG_2
  printf("ind=%d, nbytes=%d (from 0x%08x to 0x%08x)\n",ind,nbytes,b08,end);
#endif
  val = 0;
  while(b08<end)
  {
    last_val = val;
    GET32(val);
#ifdef DEBUG_2
    printf("data=0x%08x\n",data);
#endif
    
    if(val & 0x80000000)
    {
      index = 0;
      flag = (val>>27) & 0xF;
    }
    else
	{
      index++;
    }
    
#ifdef DEBUG_2
    printf("%08X: ", val);
#endif
    
    switch(flag)
    {
      case 0:  // Block Header
#ifdef DEBUG_2
        printf("Block Header:");
        printf(" Block Count = %d,", (val>>8) & 0x3FF);
        printf(" Block Size = %d\n", (val>>0) & 0xFF);
#endif
        break;
        
      case 1:  // Block Trailer
#ifdef DEBUG_2
        printf("Block Trailer:");
        printf(" Word Count = %d\n", (val>>0) & 0x3FFFFF);
#endif
        break;
        
      case 2:  // Event Header
#ifdef DEBUG_2
        printf("Event Header:");
        printf(" Event Number: %d\n", (val>>0) & 0x3FFFFF);
#endif
        break;
        
      case 3:  // Trigger Time
        if(index == 1)
        {
#ifdef DEBUG_2
          printf("Trigger Time:");
          printf(" 23:0: %d,", (last_val>>0) & 0xFFFFFF);
          printf(" 47:24: %d\n", (val>>0) & 0xFFFFFF);
#endif
        }
        break;

      case 4:  // ECtrig Peak
        if(index == 1)
        {
          instance = (last_val>>26) & 0x1; /* always 0 */
          view = (last_val>>24) & 0x3;
          time = (last_val>>16) & 0xff;

          coord = (val>>16) & 0x3ff;
          energy = (val>>0) & 0xffff;

		  layer = view;
          
#ifdef DEBUG_2
          printf("ECtrig Peak: ");
          printf(" Inst = %1d", instance);
          printf(" View = %c", view_array[view]);
          printf(" (layer=%d)",layer);
          printf(" Coord = %d (%2.2f)", coord, ((float)(coord) / (float)fview[view]) );
          printf(" Energy = %d", energy);
          printf(" Time = %d\n", time);
#endif
		  npeaks = npeak[layer];
		  peak[layer][npeaks].coord = coord;
		  peak[layer][npeaks].energy = energy;
		  peak[layer][npeaks].time = time;
		  npeak[layer] ++;
        }
        break;
        
      case 5:  // ECtrig Cluster
        if(index == 1)
        {
          instance = (last_val>>26) & 0x1; /* always 0 */
          time =     (last_val>>16) & 0xff;
          energy =   (last_val>>0) & 0xffff;

          coordW = (val>>20) & 0x3ff;
          coordV = (val>>10) & 0x3ff;
		  coordU = (val>>0) & 0x3ff;
#ifdef DEBUG_2
          printf("ECtrig Cluster:");
          printf(" Inst = %1d", instance);
          printf(" Coord W = %d (%2.2f)", coordW, ((float)(coordW) / (float)fview[2]) );
          printf(" Coord V = %d (%2.2f)", coordV, ((float)(coordV) / (float)fview[1]) );
		  printf(" Coord U = %d (%2.2f)", coordU, ((float)(coordU) / (float)fview[0]) );
          printf(" Energy = %4d", energy);
          printf(" Time = %4d\n", time);
#endif
          if(nhits < NHIT)
		  {
		    hit[nhits].coord[0] = coordU;
		    hit[nhits].coord[1] = coordV;
		    hit[nhits].coord[2] = coordW;
		    hit[nhits].energy = energy;
		    hit[nhits].time = time;
            nhits ++;
		    *nhit = nhits;
		  }
          else
		  {
            printf("WARN: nhits=%d >= NHIT=%d - ignore that hit\n",nhits,NHIT);
		  }
        }
        break;
        
      case 6:  // Trigger bit
#ifdef DEBUG_2
        printf("Trigger bit:");
        printf(" Inst = %1d", (val>>16)&0x1);
        printf(" Lane = %2d", (val>>11)&0x1f);
        printf(" Time = %4d\n", (val>>0)&0x7ff);
#endif
        break;
        
      default:
#ifdef DEBUG_2
        printf("*** UNKNOWN FLAG/WORD *** 0x%08X\n", val);
#endif
        break;
    }
#ifdef DEBUG_2
        printf("\n\n");
#endif
  }



return(ind);



  /* get VTP board timestamp */
  banktag = 0xe10a;
  banknum = 0;

  if((ind = evLinkBank(bufptr, fragtag, fragnum, banktag, banknum, &nbytes, &ind_data)) <= 0)
  {
    printf("cannot find bank tag=0x%04x num=%d (trigger)\n",banktag,banknum);
    return(0);
  }
#ifdef DEBUG_2
  printf("trigger bank: ind=%d ind_data=%d\n",ind, ind_data);
#endif

#ifdef DEBUG_2
  printf("ind=%d, nbytes=%d\n",ind,nbytes);
#endif
  b08 = (unsigned char *) &bufptr[ind_data];
  end = b08 + nbytes;
#ifdef DEBUG_2
  printf("ind=%d, nbytes=%d (from 0x%08x to 0x%08x)\n",ind,nbytes,b08,end);
#endif
  val = 0;
  while(b08<end)
  {
    GET32(val);
    printf("VTP TI header 0x%08x (%d)\n",val,val);
    GET32(val);
    printf("VTP TI event 0x%08x (%d)\n",val,val);

    GET32(val);
    printf("VTP TI timestamp(low) 0x%08x (%d)\n",val,val);
    timestamp = (long long)(val&0xFFFFFF);
    printf("VTP TI %lld (%lld %lld)\n",timestamp,timestamp/8,timestamp%8);

    GET32(val);
    printf("VTP TI whatever %d\n",val);
  }

  return(ind);
}





#ifdef USE_PCAL
#define ecal pcal
#define ecstrips pcstrips
#define ecstrip pcstrip
#define ecpeak pcpeak
#define ecpeaksort pcpeaksort
#define ecpeakcoord pcpeakcoord
#define echit pchit
#define ecfrac pcfrac
#define eccorr pccorr
#define echitsort pchitsort
#endif




static ECStrip strall[6][MAXTIMES][3][NSTRIP];
void
ec_get_str_all(int sec, ECStrip strip[MAXTIMES][3][NSTRIP])
{
  for(int i=0; i<MAXTIMES; i++) for(int j=0; j<3; j++) for(int k=0; k<NSTRIP; k++) {strip[i][j][k] = strall[sec][i][j][k]; /*printf("STRALL[%d][%d][%d]=%d\n",i,j,k,(int16_t)strall[sec][i][j][k].energy);*/}
}


static trig_t trig[4]; /* assumed to be cleaned up because of 'static' */

/* to be used by analysis software: add peaks and hits to current evio event in form of evio banks */
void
eclib(uint32_t *bufptr, uint16_t threshold_[3], uint16_t nframes_, uint16_t dipfactor_, uint16_t dalitzmin_, uint16_t dalitzmax_, uint16_t nstripmax_)
{
  GET_PUT_INIT;
  int ii, it, ret, sec, uvw, npsble;
  uint8_t nhits;
  uint32_t bufout0[256], bufout1[256], bufout2[256], bufout3[256];
  int iev;
  unsigned long long timestamp;

  ap_uint<16> threshold[3] = {threshold_[0], threshold_[1], threshold_[2]};

  nframe_t    nframes = nframes_;
  ap_uint<4>  dipfactor = dipfactor_;
  ap_uint<12> dalitzmin = dalitzmin_;;
  ap_uint<12> dalitzmax = dalitzmax_;
  ap_uint<4>  nstripmax = nstripmax_;

  hls::stream<fadc_16ch_t> s_fadcs[NFADCS];
  hls::stream<fadc_4ch_t> s_fadc_words[NFADCS];
  hls::stream<ECHit> s_hits;

  ECPeak peak[3][NPEAK];
  ECHit hit[NHIT];
  ECHit hit_tmp;

  peak_ram_t buf_ram_u[NPEAK][256];
  peak_ram_t buf_ram_v[NPEAK][256];
  peak_ram_t buf_ram_w[NPEAK][256];
  hit_ram_t buf_ram[NHIT][256];
  hls::stream<trig_t> trig_stream[4];
  hls::stream<eventdata_t> event_stream[4];

#ifdef USE_PCAL
  int detector = PCAL;
  int nslot = 12;
#else
  int detector = /*ECIN*/ECAL;
  int nslot = /*7*/14;
#endif

  for(sec=0; sec<NSECTOR; sec++)
  {
#ifdef DEBUG_0
#ifdef USE_PCAL
	cout<<"pclib: sec1="<<sec<<endl;
	cout<<"pclib: threshold="<<threshold[0]<<" "<<threshold[1]<<" "<<threshold[2]<<endl;
#else
	cout<<"eclib: sec1="<<sec<<endl;
	cout<<"eclib: threshold="<<threshold[0]<<" "<<threshold[1]<<" "<<threshold[2]<<endl;
#endif
#endif

    ret = fadcs(bufptr, threshold[0], sec, detector, s_fadcs, 0, 0, &iev, &timestamp);
	if(ret<=0) continue;

    /* trig will be incremented for every sector, because 'static addr' in ...eventfill is common for all sectors ..*/
    for(int i=0; i<4; i++) trig[i].t_stop = trig[i].t_start + MAXTIMES*8; /* set readout window MAXTIMES*32ns in 4ns ticks */
#ifdef DEBUG_0
#ifdef USE_PCAL
    cout<<"-pclib-> t_start="<<trig[0].t_start<<" t_stop="<<trig[0].t_stop<<endl;
#else
    cout<<"-eclib-> t_start="<<trig[0].t_start<<" t_stop="<<trig[0].t_stop<<endl;
#endif
#endif
    for(int i=0; i<4; i++)
    {
      trig_stream[i].write(trig[i]);
    }

    for(it=0; it<MAXTIMES; it++) /*loop over timing slices */
	{
#ifdef DEBUG_0
#ifdef USE_PCAL
 	  printf("pclib: sec = %d, timing slice = %d\n",sec,it);fflush(stdout);
#else
 	  printf("eclib: sec = %d, timing slice = %d\n",sec,it);fflush(stdout);
#endif
#endif

	  /* adjust to 8ns domain */
      for(int i=0; i<nslot; i++) fadcs_32ns_to_8ns(s_fadcs[i], s_fadc_words[i]);

      /* FPGA section */
      ecal(threshold, nframes, dipfactor, dalitzmin, dalitzmax, nstripmax, s_fadc_words, s_hits, buf_ram_u, buf_ram_v, buf_ram_w, buf_ram);
      /* FPGA section */

      /* read hits to avoid 'leftover' warnings */
      for(ii=0; ii<NHIT; ii++) hit_tmp = s_hits.read();

      /*save strips*/
      ec_get_str(strall[sec][it]);

    } /* it */


    /* done processing data, now we will extract results */

    /* move data from buf_ram to event_stream in according to trig_stream */
    ecpeakeventwriter(0, 0, trig_stream[0], event_stream[0], buf_ram_u);
    ecpeakeventwriter(1, 0, trig_stream[1], event_stream[1], buf_ram_v);
    ecpeakeventwriter(2, 0, trig_stream[2], event_stream[2], buf_ram_w);
    echiteventwriter(0, trig_stream[3], event_stream[3], buf_ram);



    /* extract peaks */
	ecpeakeventreader(event_stream[0], peak[0], bufout0);
    ecpeakeventreader(event_stream[1], peak[1], bufout1);
    ecpeakeventreader(event_stream[2], peak[2], bufout2);
#ifdef DEBUG_0
    for(ii=0; ii<NPEAK; ii++) if(peak[0][ii].energy>0) printf("U peak[%d]\n",ii);
    for(ii=0; ii<NPEAK; ii++) if(peak[1][ii].energy>0) printf("V peak[%d]\n",ii);
    for(ii=0; ii<NPEAK; ii++) if(peak[2][ii].energy>0) printf("W peak[%d]\n",ii);
#endif

    /* extract hits */
    nhits=0;
    echiteventreader(event_stream[3], hit, bufout3);
    for(ii=0; ii<NHIT; ii++)
	{
      if(hit[ii].energy>0)
	  {
        nhits++;
#ifdef DEBUG_0
        cout<<"eclib/pcal: hit["<<ii<<"]: energy="<<+hit[ii].energy;
        cout<<" coordU="<<+hit[ii].coord[0]<<"("<<((uint32_t)hit[ii].coord[0])/fview[0]<<"/"<<pcal_coord_to_strip(0, ((uint32_t)hit[ii].coord[0])/fview[0] )<<")";
        cout<<" coordV="<<+hit[ii].coord[1]<<"("<<((uint32_t)hit[ii].coord[1])/fview[1]<<"/"<<pcal_coord_to_strip(1, ((uint32_t)hit[ii].coord[1])/fview[1] )<<")";
        cout<<" coordW="<<+hit[ii].coord[2]<<"("<<((uint32_t)hit[ii].coord[2])/fview[2]<<"/"<<pcal_coord_to_strip(2, ((uint32_t)hit[ii].coord[2])/fview[2] )<<")";
        cout<<" ind="<<+hit[ii].ind<<" enU="<<+hit[ii].enpeak[0]<<" enV="<<+hit[ii].enpeak[1]<<" enW="<<+hit[ii].enpeak[2]<<endl;
#endif
	  }
	}



    /* done extracting results, now we will create trigger bank(s) if we have at least one peak */

    if(bufout0[0]>0 || bufout1[0]>0 || bufout2[0]>0 || bufout3[0]>0)
	{
#ifdef USE_PCAL
      int fragtag = 60107+sec;
#else
      int fragtag = 60101+sec;
#endif
      int banktag = 0xe122;
      trigbank_open(bufptr, fragtag, banktag, iev, timestamp);
      if(bufout0[0]>0) {printf("writing U\n");trigbank_write(bufout0);}
      if(bufout1[0]>0) {printf("writing V\n");trigbank_write(bufout1);}
      if(bufout2[0]>0) {printf("writing W\n");trigbank_write(bufout2);}
      if(bufout3[0]>0) {printf("writing H\n");trigbank_write(bufout3);}
      trigbank_close(bufout0[0]+bufout1[0]+bufout2[0]+bufout3[0]);
	}


    for(int i=0; i<4; i++) trig[i].t_start += MAXTIMES*8; /* in preparation for next event, step up MAXTIMES*32ns in 4ns ticks */

  } /* sec */

  return;
}
