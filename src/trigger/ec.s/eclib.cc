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
#define ecinit pcinit
#define ecfadcs pcfadcs
#define ecstrips pcstrips
#define ecl3 pcl3
#define eclib pclib
#define ecpeakeventreader pcpeakeventreader
#define echiteventreader pchiteventreader

#else

#include "ectrans.h"
#include "eclib.h"

#endif


//#include "prlib.h"


//#define DEBUG
//#define DEBUG_0
//#define DEBUG_1
//#define DEBUG_2 /* hardware trigger info */
//#define DEBUG_3


#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)
#define ABS(x)      ((x) < 0 ? -(x) : (x))



/* variables global for present file */

static int runnum = 0;
static float nba[NSECTOR][NLAYER][NSTRIP]; /* nsa+nsb (bins) */
static float ech[NSECTOR][NLAYER][NSTRIP];
static float t0 [NSECTOR][NLAYER][NSTRIP]; /* delays (nsec) */
static float tch[NSECTOR][NLAYER][NSTRIP];


static float ped[21][16]; /* pedestals */
static float tet[21][16]; /* threshold */
static float gain[21][16]; /* gain */
static int nsa[21]; /* NSA */
static int nsb[21]; /* NSB */





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
ecpeakeventreader(hls::stream<trig_t> &trig_stream, hls::stream<eventdata_t> &event_stream, ECPeak peak[NPEAK], uint32_t *bufout)
{
  eventdata_t eventdata;
  uint32_t data_end=0, word_first=0, tag=0, inst=0, view=0, data=0, it=0, *bufptr = bufout;
  int i = 0;
  while(1)
  {
    if(event_stream.empty()) {printf("ecpeakeventreader: EMPTY STREAM 1\n"); break;}
    eventdata = event_stream.read();
    *bufptr++ = eventdata.data;

    data = eventdata.data;
    if(data == 0xFFFFFFFF)
    {
      printf("ecpeakeventreader: END_OF_DATA\n");
      break;
    }

    data_end = eventdata.end;              /* 0 for all words except last one when it is 1 */
    word_first = eventdata.data(31,31);    /* 1 for the first word in peak, 0 for followings */
    tag = eventdata.data(30,27);           /* must be 'ECPEAK_TAG' */
	printf("ecpeakeventreader[it=%d i=%d]: tag=%d\n",it,i,tag);
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
echiteventreader(hls::stream<trig_t> &trig_stream, hls::stream<eventdata_t> &event_stream, ECHit hit[NHIT], uint32_t *bufout)
{
  eventdata_t eventdata;
  uint32_t data_end=0, word_first=0, tag=0, inst=0, view=0, data=0, it=0, *bufptr = bufout;
  int i = 0;
  while(1)
  {
    if(event_stream.empty()) {printf("echiteventreader: EMPTY STREAM 1\n");break;}
    eventdata = event_stream.read();
    *bufptr++ = eventdata.data;

    data = eventdata.data;
    if(data == 0xFFFFFFFF)
    {
      /*printf("echiteventreader: END_OF_DATA\n");*/
      break;
    }

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
ectrig(unsigned int *bufptr, int sec, int npeak[NLAYER], TrigECPeak peak[NLAYER][NPEAKMAX], int nhit[2], TrigECHit hit[2][NHIT])
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

#ifdef DEBUG
  printf("ectrig reached, sector=%d\n",sec);
#endif


  bzero((char *)peak,NLAYER*NPEAKMAX*sizeof(TrigECPeak));
  bzero((char *)npeak,NLAYER*sizeof(int));
  bzero((char *)hit,2*NHIT*sizeof(TrigECPeak));
  bzero((char *)nhit,2*sizeof(int));
  npeaks = 0;
  nhits = 0;


#ifdef USE_PCAL
  fragtag = 107+sec;
#else
  fragtag = 101+sec;
#endif
  fragnum = -1;
  banktag = 0xe122;
  banknum = 0/*101+sec*/;

  if((ind = evLinkBank(bufptr, fragtag, fragnum, banktag, banknum, &nbytes, &ind_data)) <= 0)
  {
    printf("cannot find bank tag=0x%04x num=%d (trigger)\n",banktag,banknum);
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
          instance = (last_val>>26) & 0x1;
          view = (last_val>>24) & 0x3;
          time = (last_val>>16) & 0xff;

          coord = (val>>16) & 0x3ff;
          energy = (val>>0) & 0xffff;

		  layer = instance*3 + view;
          
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
          instance = (last_val>>26) & 0x1;
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
		  nhits = nhit[instance];
          if(nhits < NHIT)
		  {
		    hit[instance][nhits].coord[0] = coordU;
		    hit[instance][nhits].coord[1] = coordV;
		    hit[instance][nhits].coord[2] = coordW;
		    hit[instance][nhits].energy = energy;
		    hit[instance][nhits].time = time;
		    nhit[instance] ++;
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




/* ecinit, ecinitgeom, ecinitcalib - EC initialization */

int
ecinit(int runnum, int def_adc, int def_tdc, int def_atten)
{
  int i,j,k,l;

#ifdef DEBUG
  printf("eclib: def_adc=%d    def_tdc=%d\n",def_adc,def_tdc);fflush(stdout);
#endif

  for(i=0; i<NSECTOR; i++)
  {
    for(j=0; j<NLAYER; j++)
    {
      for(k=0; k<NSTRIP; k++)
      {
        //ped[i][j][k] = 0.0;
        nba[i][j][k] = 0.0;
        ech[i][j][k] = 1.0/*0.0001*/;
        t0 [i][j][k] = 0.0;
        tch[i][j][k] = 1.0/*0.015625*/; /* divide by 64 to get nanoseconds */
      }
    }
  }

  for(int slot=0; slot<21; slot++)
  {
    nsa[slot] = 0;
    nsb[slot] = 0;
	for(int chan=0; chan<16; chan++)
	{
      ped[slot][chan] = 0.0;
      gain[slot][chan] = 0.0;
      tet[slot][chan] = 0;
	}
  }

  return(0);
}




/*************************************************************************************************************************/
/*************************************************************************************************************************/
/*************************************************************************************************************************/


/* ecfadcs.c - loading strip info for particular sector

  input:  sector - sector number

  output: strip[][].energy - strip energy (MeV)
          strip[][].time   - strip time (ns)
*/



int
ecfadcs(unsigned int *bufptr, unsigned short threshold, int sec, int io, hls::stream<fadc_word_t> s_fadc_words[NSLOT], int dtimestamp, int dpulsetime,
        int *iev, unsigned long long *timestamp)
{
  int i, j, k, ind, nhits, str, layer, error, ii, jj, nbytes, ind_data, nn, mm, isample, isam1, isam2;
  int summing_in_progress;
  int datasaved[1000];
  int energy, it;
  unsigned short strip_threshold;
  unsigned long long itime, ptime, offset;
  GET_PUT_INIT;
#ifdef USE_PCAL
  int tag[6] = {3,9,15,21,27,33}; /* tag for sectors */
  int nslot = NSLOT;
#else
  int tag[6] = {1,7,13,19,25,31}; /* tag for sectors */
  int nslot = NSLOT;
#endif
  int fragtag, fragnum, banktag, banknum;
  unsigned short pulse_time, pulse_min, pulse_max, dat16;
  unsigned int pulse_integral;
  unsigned char *end;
  unsigned long long time;
  int crate,slot,trig,nchan,chan,npulses,nsamples,notvalid,edge,data,count,ncol1,nrow1;
  int ndata0[22], data0[21][8];
  int baseline, sum, channel;
  char *ch, str_tmp[256];
  int dd[16];
  float fsum, fped, ff[16];
  int bank_not_found = 1;

  fadc_word_t fadcs[MAXTIMES][NSLOT][NH_READS];
  int isl;
  ap_uint<13> ee;
  ap_uint<3>  tt;
#ifdef USE_PCAL
  int slot2isl[22] = {-1,-1,-1,0,1,2,3,4,5,6,7,-1,-1,8,9,10,11,-1,-1,-1,-1,-1}; /* PCAL slots: 3-10, 13-16 */
#else
  int slot2isl[22] = {-1,-1,-1,0,1,2,3,4,5,6,0,-1,-1,1,2,3,4,5,6,-1,-1,-1}; /* ECAL_IN slots: 3-9, ECAL_OUT slots: 10, 13-18 */
#endif


#ifdef DEBUG
  printf("ecfadcs reached, sector=%d\n",sec);fflush(stdout);
#endif


  /* cleanup output array */
  for(i=0; i<MAXTIMES; i++)
  {
	for(isl=0; isl<NSLOT; isl++)
	{
	  for(j=0; j<NH_READS; j++)
	  {
        fadcs[i][isl][j].e0 = 0;
        fadcs[i][isl][j].t0 = 0;
        fadcs[i][isl][j].e1 = 0;
        fadcs[i][isl][j].t1 = 0;
        fadcs[i][isl][j].e2 = 0;
        fadcs[i][isl][j].t2 = 0;
        fadcs[i][isl][j].e3 = 0;
        fadcs[i][isl][j].t3 = 0;
	  }
	}
  }


  /********************/
  /* pedestal and nba */

  fragtag = tag[sec];
  fragnum = -1;
  banktag = 0xe10e;
  banknum = tag[sec];
  if((ind = evLinkBank(bufptr, fragtag, fragnum, banktag, banknum, &nbytes, &ind_data)) > 0)
  {
#ifdef DEBUG_3
    printf("evLinkBank: ind=%d ind_data=%d\n",ind, ind_data);fflush(stdout);
    printf("ind=%d, nbytes=%d\n",ind,nbytes);fflush(stdout);
#endif
    b08 = (unsigned char *) &bufptr[ind_data];
    end = b08 + nbytes;
#ifdef DEBUG_3
    printf("ind=%d, nbytes=%d (from 0x%08x to 0x%08x)\n",ind,nbytes,b08,end);fflush(stdout);
#endif
    while(b08<end)
	{
      ch = (char *)b08;
      ii=0;
#ifdef DEBUG_3
      printf("CH\n");fflush(stdout);
#endif
      while( (isalnum(ch[ii])||(ch[ii]==' ')||(ch[ii]=='_')||(ch[ii]=='.')) && (ii<500))
      {
#ifdef DEBUG_3
        printf("%c",ch[ii]);fflush(stdout);
#endif
        ii++;
      }
#ifdef DEBUG_3
      if((ch[ii]=='\0')||(ch[ii]=='\n')) printf("\n");fflush(stdout);
#endif
      if(!strncmp(ch,"FADC250_SLOT",12))
      {
        slot=atoi((char *)&ch[12]);
#ifdef DEBUG_3
        printf("===> slot=%d\n",slot);fflush(stdout);
#endif
      }

      if(!strncmp(ch,"FADC250_NSB",11))
      {
        nsb[slot] = atoi((char *)&ch[11]) / 4;
#ifdef DEBUG_3
        printf("===> nsb=%d ticks\n",nsb[slot]);fflush(stdout);
#endif
      }

      if(!strncmp(ch,"FADC250_NSA",11))
      {
        nsa[slot] = atoi((char *)&ch[11]) / 4;
#ifdef DEBUG_3
        printf("===> nsa=%d ticks\n",nsa[slot]);fflush(stdout);
#endif
      }

      if(!strncmp(ch,"FADC250_ALLCH_PED",17))
      {
#ifdef DEBUG_3
        printf("===> peds\n");fflush(stdout);
#endif
        sscanf (ch, "%*s %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
                &ff[0],&ff[1],&ff[2],&ff[3],&ff[4],&ff[5],&ff[6],&ff[7],
                &ff[8],&ff[9],&ff[10],&ff[11],&ff[12],&ff[13],&ff[14],&ff[15]);

        for(chan=0; chan<16; chan++)
	    {
          ped[slot][chan] = ff[chan];
#ifdef DEBUG_3
          printf("ped[%d][%d]=%f\n",slot,chan,ped[slot][chan]);fflush(stdout);
#endif

	    }
      }

      if(!strncmp(ch,"FADC250_ALLCH_GAIN",18))
      {
#ifdef DEBUG_3
        printf("===> peds\n");fflush(stdout);
#endif
        sscanf (ch, "%*s %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
                &ff[0],&ff[1],&ff[2],&ff[3],&ff[4],&ff[5],&ff[6],&ff[7],
                &ff[8],&ff[9],&ff[10],&ff[11],&ff[12],&ff[13],&ff[14],&ff[15]);

        for(chan=0; chan<16; chan++)
	    {
          gain[slot][chan] = ff[chan];
#ifdef DEBUG_3
          printf("gain[%d][%d]=%f\n",slot,chan,gain[slot][chan]);fflush(stdout);
#endif

	    }
      }

      if(!strncmp(ch,"FADC250_ALLCH_TET",17))
      {
#ifdef DEBUG_3
        printf("===> tet\n");fflush(stdout);
#endif
        sscanf (ch, "%*s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
                &dd[0],&dd[1],&dd[2],&dd[3],&dd[4],&dd[5],&dd[6],&dd[7],
                &dd[8],&dd[9],&dd[10],&dd[11],&dd[12],&dd[13],&dd[14],&dd[15]);

        for(chan=0; chan<16; chan++)
	    {
          tet[slot][chan] = dd[chan];
#ifdef DEBUG_3
          printf("tet[%d][%d]=%f\n",slot,chan,tet[slot][chan]);fflush(stdout);
#endif

	    }
      }

      ii++;
      b08 += ii;
	}


  }
  else
  {
    /*printf("cannot find bank 0x%04x num %d (pedestal)\n",banktag, banknum);*/
    fflush(stdout);
  }

  /* pedestal and nba */
  /********************/




  /*************************/
  /* integrated pulse data */

  strip_threshold = threshold;
  nhits = 0;

  fragtag = tag[sec];
  fragnum = -1;
  banktag = 0xe102;

  if(bank_not_found)
  {
  for(banknum=0; banknum<40; banknum++)
  {
  if((ind = evLinkBank(bufptr, fragtag, fragnum, banktag, banknum, &nbytes, &ind_data)) > 0)
  {
#ifdef DEBUG_1
    printf("evLinkBank: ind=%d ind_data=%d\n",ind, ind_data);fflush(stdout);
#endif


#ifdef DEBUG_1
    printf("ind=%d, nbytes=%d\n",ind,nbytes);fflush(stdout);
#endif
    b08 = (unsigned char *) &bufptr[ind_data];
    end = b08 + nbytes;
#ifdef DEBUG_1
    printf("ind=%d, nbytes=%d (from 0x%08x to 0x%08x)\n",ind,nbytes,b08,end);fflush(stdout);
#endif
    while(b08<end)
    {
#ifdef DEBUG_0
      printf("ecfadcs: begin while: b08=0x%08x\n",b08);fflush(stdout);
#endif


      GET8(slot);
      isl = slot2isl[slot];
      GET32(trig);
      GET64(time);
      GET32(nchan);
#ifdef DEBUG_0
      printf("slot=%d, trig=%d, time=%lld nchan=%d\n",slot,trig,time,nchan);fflush(stdout);
#endif
      for(nn=0; nn<nchan; nn++)
	  {
        GET8(chan);
        GET32(npulses);
#ifdef DEBUG_0
        printf("  chan=%d, npulses=%d\n",chan,npulses);fflush(stdout);
#endif
        for(mm=0; mm<npulses; mm++)
	    {
          GET16(pulse_time);
          GET32(pulse_integral);
          GET16(pulse_min);
          GET16(pulse_max);
#ifdef DEBUG_0
          printf(" b32=0x%08x:  pulse_time=%d ns (%d ticks), pulse_integral=%d\n",b32,pulse_time,pulse_time/TIME2TICKS,pulse_integral);fflush(stdout);
#endif

#ifndef USE_PCAL
          if((io==0&&slot<=9) || (io==1&&slot>=10)) /* INNER comes from slots 3<=slot<=9, OUTER - from slots 10<=slot<=18 */
#endif
		  {
            ee = pulse_integral & 0x1FFF;
            itime = 0/*(pulse_time/TIME2TICKS)*/; /* convert time to ticks */
            it = itime / NTICKS;
            tt = itime % NTICKS;
#ifdef DEBUG_0
		    cout<<"ecfadcs: ee="<<ee<<", itime="<<itime<<", isl="<<isl<<", it="<<it<<", tt="<<tt<<endl;
#endif
            if(it>=MAXTIMES)
		    {
              printf("ecfadcs: WARN: itime=%d too big - ignore the hit\n",itime);
		    }
            else
		    {
              int ch1 = chan/NH_READS;
              int ch2 = chan%NH_READS;
              if(ch2==0)
              {
                fadcs[it][isl][ch1].e0 = ee;
                fadcs[it][isl][ch1].t0 = tt;
			  }
              else if(ch2==1)
			  {
                fadcs[it][isl][ch1].e1 = ee;
                fadcs[it][isl][ch1].t1 = tt;
			  }
              else if(ch2==2)
			  {
                fadcs[it][isl][ch1].e2 = ee;
                fadcs[it][isl][ch1].t2 = tt;
			  }
              else if(ch2==3)
			  {
                fadcs[it][isl][ch1].e3 = ee;
                fadcs[it][isl][ch1].t3 = tt;
			  }
		    }
		  }

        } /*for(mm=0; mm<npulses; mm++)*/

      } /*for(nn=0; nn<nchan; nn++)*/


#ifdef DEBUG_1
      printf("ecfadcs: end loop: b08=0x%08x\n",b08);fflush(stdout);
#endif

    }


    /* fills output streams */
	for(i=0; i<MAXTIMES; i++)
	{
	  for(isl=0; isl<NSLOT; isl++)
	  {
		for(j=0; j<NH_READS; j++)
		{
		  /*
#ifdef DEBUG_0
          printf("ecfadcs: writing to stream %d (i=%d j=%d)\n",isl,i,j);fflush(stdout);
#endif
		  */
          s_fadc_words[isl].write(fadcs[i][isl][j]);
		}
	  }
	}

    bank_not_found = 0;
    break; /* if we found bank, exit from 'banknum' loop */
  }
  else
  {
    //printf("cannot find bank 0x%04x num %d (mode 7, fragtag=%d fragnum=%d)\n",banktag,banknum,fragtag,fragnum);fflush(stdout);
    ind = 0;
  }
  }
  }

  /* integrated pulse data */
  /*************************/





















  /*****************/
  /* waveform data */

  strip_threshold = threshold;
  nhits = 0;

  fragtag = tag[sec];
  fragnum = -1;
  banktag = 0xe101;

  if(bank_not_found)
  {
  for(banknum=0; banknum<40; banknum++)
  {
  if((ind = evLinkBank(bufptr, fragtag, fragnum, banktag, banknum, &nbytes, &ind_data)) > 0)
  {
#ifdef DEBUG_0
    printf("\n\nevLinkBank: ind=%d ind_data=%d\n",ind, ind_data);fflush(stdout);
#endif


#ifdef DEBUG_0
    printf("ind=%d, nbytes=%d\n",ind,nbytes);fflush(stdout);
#endif
    b08 = (unsigned char *) &bufptr[ind_data];
    end = b08 + nbytes;
#ifdef DEBUG_0
    printf("ind=%d, nbytes=%d (from 0x%08x to 0x%08x)\n",ind,nbytes,b08,end);fflush(stdout);
#endif
    while(b08<end)
    {
#ifdef DEBUG_0
      printf("ecfadcs: begin while: b08=0x%08x\n",b08);fflush(stdout);
#endif


      GET8(slot);
      isl = slot2isl[slot];
      GET32(trig);
      GET64(time); /* time stamp for FADCs 2 counts bigger then for VTPs */
      time = ((time&0xFFFFFF)<<24) | ((time>>24)&0xFFFFFF); /* UNTILL FIXED IN ROL2 !!!!!!!!!!!!!!!!! */

      *iev = trig;
      *timestamp = time/*0x12345678abcd*/;

      GET32(nchan);
#ifdef DEBUG_0
      printf("slot=%d, trig=%d, time=%lld(0x%016x) nchan=%d\n",slot,trig,time,time,nchan);fflush(stdout);
#endif
      for(nn=0; nn<nchan; nn++)
	  {
        GET8(chan);
        GET32(nsamples);
#ifdef DEBUG_0
        printf("==> slot=%d, chan=%d, nsamples=%d\n",slot,chan,nsamples);fflush(stdout);
#endif

		
        /*save data*/
        for(mm=0; mm<nsamples; mm++)
		{
          GET16(dat16);
          datasaved[mm] = dat16;
		}

#ifdef DEBUG_0
        printf("data[sl=%2d][ch=%2d]:",slot,chan);
        for(mm=40; mm<100/*69*/; mm++)
		{
          printf(" [%2d]%4d,",mm,datasaved[mm]);
		}
        printf("\n");
#endif		

























        /*find threshold crossing sample; first start from mm=0, then proceed with mm=[beginning of next 'it']*/
        mm = 1;

search_for_next_pulse:

        isample = -1;
        pulse_integral = 0;
        while(mm<nsamples)
		{
          if( (datasaved[mm]>(int)ped[slot][chan]+tet[slot][chan]) && (datasaved[mm-1]<=(int)ped[slot][chan]+tet[slot][chan]) )
		  {
            isample = mm;
            break;
		  }
          mm++;
		}
		if(mm>=nsamples)
		{
#ifdef DEBUG_0
          printf("done_with_pulse_search\n");
#endif
          goto done_with_pulse_search;
		}

        /*integrate*/
        fsum = 0.0;
        fped = 0.0;
        if(isample>=0)
		{
          isam1 = MAX((isample-nsb[slot]),0);
          isam2 = MIN((isample+nsa[slot]),nsamples);

          for(int m=isam1; m<isam2; m++)
		  {
            fsum += (float)datasaved[m];
            fped += ped[slot][chan];
		  }

          pulse_integral = (int)roundf(fsum-fped);
          
		  /*offset: 7900/4=1975*/
          offset = 1975;
          ptime = isample;

/*MANUALLY SET SHIFT - CHANGE IF NEEDED !!!*/
		  ptime+=2; /* correspond to the difference in timestamps between VTP and FADCs (FADC is bigger) */
/*MANUALLY SET SHIFT - CHANGE IF NEEDED !!!*/


offset += dtimestamp;
ptime += dpulsetime;

          itime = ((time-offset)+ptime)/8 - (time-offset)/8;
          it = itime;
          tt = ((time-offset)+ptime) - ((((time-offset)+ptime)/8)*8);

/*TEMP for GEMC !!!
it=0;
tt=0;
TEMP for GEMC !!!*/

#ifdef DEBUG_0
          int tttt;
		  tttt = ((int)time) & 0xFFFFFF;
		  printf("isample=%d nsa=%d nsb=%d -> isam1=%d isam2=%d -> data=%d ped=%f -> fsum=%f fped=%f -> pulse_integral=%6d (layer=%d strip=%2d) -> it=%d tt=%d time=%lld(0x%016x) (tttt=%lld(0x%08x) (%lld %lld))\n\n",
                 isample, nsa[slot], nsb[slot], isam1, isam2, datasaved[mm], ped[slot][chan], fsum, fped, pulse_integral, adclayerecal[isl][chan]-1,adcstripecal[isl][chan]-1,it,(int)tt,time,time,
				 tttt,tttt,tttt/8,tttt%8);
#endif
		}
        else
		{
#ifdef DEBUG_0
          printf("no pulse found - will do nothing for that channel\n");
#endif
          pulse_integral = 0;
		}



#ifndef USE_PCAL
        if((io==0&&slot<=9) || (io==1&&slot>=10)) /* INNER comes from slots 3<=slot<=9, OUTER - from slots 10<=slot<=18 */
#endif
		{
          ee = pulse_integral & 0x1FFF;
#ifdef DEBUG_0
		  cout<<"ecfadcs: ee="<<ee<<", itime="<<itime<<", isl="<<isl<<", it="<<it<<", tt="<<tt<<endl;
#endif
          if(ee<=0)
		  {
            ;
#ifdef DEBUG_0
            cout<<"ecfadcs: ee="<<ee<<" - ignore the hit"<<endl;
#endif
		  }
          else if(it>=MAXTIMES)
		  {
            printf("ecfadcs: WARN: itime=%d too big - ignore the hit\n",itime);
		  }
          else
		  {
            int ch1 = chan/NH_READS;
            int ch2 = chan%NH_READS;
            if(ch2==0)
            {
              fadcs[it][isl][ch1].e0 = ee;
              fadcs[it][isl][ch1].t0 = tt;
  		    }
            else if(ch2==1)
			{
              fadcs[it][isl][ch1].e1 = ee;
              fadcs[it][isl][ch1].t1 = tt;
		    }
            else if(ch2==2)
			{
              fadcs[it][isl][ch1].e2 = ee;
              fadcs[it][isl][ch1].t2 = tt;
	  	    }
            else if(ch2==3)
			{
              fadcs[it][isl][ch1].e3 = ee;
              fadcs[it][isl][ch1].t3 = tt;
		    }
		  }
	    }



        /*mm = isample + (8-tt);*/ /* search for more pulses starting from the beginning of next 'it' */
        mm = isample + 8; /* search for more pulses starting 8 pulses from previois crossing */
        printf("search for next pulse starting from mm=%d\n",mm);
		goto search_for_next_pulse;



done_with_pulse_search:
		;



      } /*for(nn=0; nn<nchan; nn++)*/


#ifdef DEBUG_0
      printf("ecfadcs: end loop: b08=0x%08x\n",b08);fflush(stdout);
#endif
    }




    /* fills output streams */
	for(i=0; i<MAXTIMES; i++)
	{
	  for(isl=0; isl<NSLOT; isl++)
	  {
		for(j=0; j<NH_READS; j++)
		{
          s_fadc_words[isl].write(fadcs[i][isl][j]);
		}
	  }
	}

    bank_not_found = 0;
    break; /* if we found bank, exit from 'banknum' loop */
  }
  else
  {
    /*printf("cannot find bank 0x%04x num %d (mode 1, fragtag=%d fragnum=%d)\n",banktag,banknum,fragtag,fragnum);fflush(stdout);*/
    ind = 0;
  }
  }
  }

  /* waveform data */
  /*****************/




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

#define THRESHOLD1 ((uint16_t)1)
#define THRESHOLD2 ((uint16_t)1)
#define THRESHOLD3 ((uint16_t)3)



#if 0

/* search for clusters in one view (ECAL_IN, ECAL_OUT or PCAL) */
int
ecl3(unsigned int *bufptr, ap_uint<16> threshold[3], int io, ECHit hitshits[NHIT])
{
  int ii, it, ret, sec, uvw, npsble;
  uint8_t nhits, nret;

  hls::stream<fadc_word_t> s_fadc_words[NSLOT];
  hls::stream<ECHit> s_hits;

  ECHit hit[NHIT];

  /*for(sec=0; sec<NSECTOR; sec++)*/
#ifdef USE_PCAL
  for(sec=1; sec<2; sec++)
#else
  for(sec=0; sec<1; sec++)
#endif
  {
	cout<<"sec1="<<sec<<" threshold="<<threshold[0]<<" "<<threshold[2]<<" "<<threshold[3]<<" io="<<io<<endl;

    ret = ecfadcs(bufptr, threshold[0], sec, io, s_fadc_words, 0, 0);
	if(ret<=0) continue;

    for(it=0; it<MAXTIMES; it++) /*loop over timing slices */
	{
 	  /*printf("ecl3: sec = %d, timing slice = %d\n",sec,it);fflush(stdout);*/

      /* FPGA section */
      ecal(threshold, s_fadc_words, s_hits);

      nhits=0;
      for(ii=0; ii<NHIT; ii++)
	  {
        hit[ii] = s_hits.read();
        if(hit[ii].energy>0) nhits++;
	  }
      if(nhits<=0) continue; /* goto next timing slice */
      cout<<"nhits="<<+nhits<<endl;fflush(stdout);

      for(ii=0; ii<nhits; ii++) hitshits[ii] = hit[ii];
	  if(io==0) printf("INNER\n");
	  else if(io==1) printf("OUTER\n");
      nret = 0;
      for(ii=0; ii<nhits; ii++)
	  {
        if(hit[ii].energy>0) nret++;
        if(hit[ii].energy>0)
		{
          cout<<"ecl3: hit["<<ii<<"]: energy="<<+hit[ii].energy;
          cout<<" coordU="<<+hit[ii].coord[0]<<"("<<((uint32_t)hit[ii].coord[0])/fview[0]<<"/"<<pcal_coord_to_strip(0, ((uint32_t)hit[ii].coord[0])/fview[0] )<<")";
          cout<<" coordV="<<+hit[ii].coord[1]<<"("<<((uint32_t)hit[ii].coord[1])/fview[1]<<"/"<<pcal_coord_to_strip(1, ((uint32_t)hit[ii].coord[1])/fview[1] )<<")";
          cout<<" coordW="<<+hit[ii].coord[2]<<"("<<((uint32_t)hit[ii].coord[2])/fview[2]<<"/"<<pcal_coord_to_strip(2, ((uint32_t)hit[ii].coord[2])/fview[2] )<<")";
          cout<<" ind="<<+hit[ii].ind<<" enU="<<+hit[ii].enpeak[0]<<" enV="<<+hit[ii].enpeak[1]<<" enW="<<+hit[ii].enpeak[2]<<endl;
		}
	  }
	  cout<<"ecl3: nret="<<+nret<<endl;
      /* FPGA section */

	}

  }

  return(nret);
}

#endif



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
  //ap_uint<16> threshold[3];
  //for(int i=0; i<3; i++) threshold[i] = threshold_[i];

  nframe_t    nframes = nframes_;
  ap_uint<4>  dipfactor = dipfactor_;
  ap_uint<12> dalitzmin = dalitzmin_;;
  ap_uint<12> dalitzmax = dalitzmax_;
  ap_uint<4>  nstripmax = nstripmax_;

  hls::stream<fadc_word_t> s_fadc_words[NSLOT];
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

  for(sec=0; sec<NSECTOR; sec++)
  {
#ifdef USE_PCAL
	cout<<"pclib: sec1="<<sec<<endl;
	cout<<"pclib: threshold="<<threshold[0]<<" "<<threshold[1]<<" "<<threshold[2]<<endl;
#else
	cout<<"eclib: sec1="<<sec<<endl;
	cout<<"eclib: threshold="<<threshold[0]<<" "<<threshold[1]<<" "<<threshold[2]<<endl;
#endif

    ret = ecfadcs(bufptr, threshold[0], sec, 0, s_fadc_words, 0, 0, &iev, &timestamp);
	if(ret<=0) continue;

    /* trig will be incremented for every sector, because 'static addr' in ...eventfill is common for all sectors ..*/
    for(int i=0; i<4; i++) trig[i].t_stop = trig[i].t_start + MAXTIMES*8; /* set readout window MAXTIMES*32ns in 4ns ticks */
    cout<<"-eclib-> t_start="<<trig[0].t_start<<" t_stop="<<trig[0].t_stop<<endl;
    for(int i=0; i<4; i++)
    {
      trig_stream[i].write(trig[i]);
    }

    for(it=0; it<MAXTIMES; it++) /*loop over timing slices */
	{
 	  printf("eclib: sec = %d, timing slice = %d\n",sec,it);fflush(stdout);

      /* FPGA section */
      ecal(threshold, nframes, dipfactor, dalitzmin, dalitzmax, nstripmax, s_fadc_words, s_hits, buf_ram_u, buf_ram_v, buf_ram_w, buf_ram);
      /* FPGA section */

      /* read hits to avoid 'leftover' warnings */
      for(ii=0; ii<NHIT; ii++) hit_tmp = s_hits.read();

    } /* it */


    /* done processing data, now we will extract results */

    /* move data from buf_ram to event_stream in according to trig_stream */
    ecpeakeventwriter(0, 0, trig_stream[0], event_stream[0], buf_ram_u);
    ecpeakeventwriter(1, 0, trig_stream[1], event_stream[1], buf_ram_v);
    ecpeakeventwriter(2, 0, trig_stream[2], event_stream[2], buf_ram_w);
    echiteventwriter(0, trig_stream[3], event_stream[3], buf_ram);



    /* extract peaks */
	ecpeakeventreader(trig_stream[0], event_stream[0], peak[0], bufout0);
    ecpeakeventreader(trig_stream[1], event_stream[1], peak[1], bufout1);
    ecpeakeventreader(trig_stream[2], event_stream[2], peak[2], bufout2);
    for(ii=0; ii<NPEAK; ii++) if(peak[0][ii].energy>0) printf("U peak[%d]\n",ii);
    for(ii=0; ii<NPEAK; ii++) if(peak[1][ii].energy>0) printf("V peak[%d]\n",ii);
    for(ii=0; ii<NPEAK; ii++) if(peak[2][ii].energy>0) printf("W peak[%d]\n",ii);


    /* extract hits */
    nhits=0;
    echiteventreader(trig_stream[3], event_stream[3], hit, bufout3);
    for(ii=0; ii<NHIT; ii++)
	{
      if(hit[ii].energy>0)
	  {
        nhits++;
        cout<<"eclib: hit["<<ii<<"]: energy="<<+hit[ii].energy;
        cout<<" coordU="<<+hit[ii].coord[0]<<"("<<((uint32_t)hit[ii].coord[0])/fview[0]<<"/"<<pcal_coord_to_strip(0, ((uint32_t)hit[ii].coord[0])/fview[0] )<<")";
        cout<<" coordV="<<+hit[ii].coord[1]<<"("<<((uint32_t)hit[ii].coord[1])/fview[1]<<"/"<<pcal_coord_to_strip(1, ((uint32_t)hit[ii].coord[1])/fview[1] )<<")";
        cout<<" coordW="<<+hit[ii].coord[2]<<"("<<((uint32_t)hit[ii].coord[2])/fview[2]<<"/"<<pcal_coord_to_strip(2, ((uint32_t)hit[ii].coord[2])/fview[2] )<<")";
        cout<<" ind="<<+hit[ii].ind<<" enU="<<+hit[ii].enpeak[0]<<" enV="<<+hit[ii].enpeak[1]<<" enW="<<+hit[ii].enpeak[2]<<endl;
	  }
	}



      /* done extracting results, now we will create trigger bank(s) if we have at least one peak */

      if(peak[0][0].energy>0 || peak[1][0].energy>0 || peak[2][0].energy>0)
	  {
        int ind, ind_data;
        uint32_t word;
        int a_instance = 0;
        int a_view, a_time, a_coord, a_energy;

	    int fragtag;
#ifdef USE_PCAL
        fragtag = 107+sec;
#else
        fragtag = 101+sec;
#endif
        int fragnum = 0;
        int banktyp = 1;

        int banktag = 0xe122;
        int banknum = 255; /*real data have event number in block, usually from 0 to 39*/

        ind = evLinkFrag(bufptr, fragtag, fragnum);
        if(ind<=0)
		{
          printf("Fragment %d does not exist - create one\n",fragtag);
          ind = evOpenFrag(bufptr, fragtag, fragnum);
          if(ind<=0) {printf("ERROR: cannot create fragment %d - exit\n",fragtag); exit(0);}
          else printf("Created fragment fragtag=%d fragnum=%d\n",fragtag, fragnum);
		}

        ret = evOpenBank(bufptr, fragtag, fragnum, banktag, banknum, banktyp, "", &ind_data);
        printf("evOpenBank returns = %d, ind_data=%d (fragtag=%d, fragnum=%d, banktag=%d, banknum=%d)\n",ret,ind_data, fragtag, fragnum, banktag, banknum);
        b08out = (unsigned char *)&bufptr[ind_data];

        /*0x12 - event header*/
		word = (0x12<<27) + (iev&0x3FFFFF);
        PUT32(word);

        /*0x13 - time stamp*/
        word = (timestamp>>24)&0xFFFFFF; /* OR OPPOSITE ??? */
        printf("word1=0x%06x\n",word);fflush(stdout);
        word = (0x13<<27) + word;;
        PUT32(word);

        word = timestamp&0xFFFFFF; /* OR OPPOSITE ??? */
        printf("word2=0x%06x\n",word);fflush(stdout);
        PUT32(word);

        /*0x14 - peak */
        if(peak[0][0].energy>0)
		{
          for(int i=0; i<256; i++)
		  {
            if(bufout0[i]==0xFFFFFFFF) break;
			printf("bufout0[%d]=0x%08x\n",i,bufout0[i]);fflush(stdout);
            PUT32(bufout0[i]);
		  }
		}
        
        /*0x14 - peak */
        if(peak[1][0].energy>0)
		{
          for(int i=0; i<256; i++)
		  {
            if(bufout1[i]==0xFFFFFFFF) break;
			printf("bufout1[%d]=0x%08x\n",i,bufout1[i]);fflush(stdout);
            PUT32(bufout1[i]);
		  }
		}
        
        /*0x14 - peak */
        if(peak[2][0].energy>0)
		{
          for(int i=0; i<256; i++)
		  {
            if(bufout2[i]==0xFFFFFFFF) break;
			printf("bufout2[%d]=0x%08x\n",i,bufout2[i]);fflush(stdout);
            PUT32(bufout2[i]);
		  }
		}
        
        /*0x15 - hit */
        if(hit[0].energy>0)
		{
          for(int i=0; i<256; i++)
		  {
            if(bufout3[i]==0xFFFFFFFF) break;
			printf("bufout3[%d]=0x%08x\n",i,bufout3[i]);fflush(stdout);
            PUT32(bufout3[i]);
		  }
		}
		printf("evClose() reached\n");fflush(stdout);
        evCloseBank(bufptr, fragtag, fragnum, banktag, banknum, b08out);
	  }



    for(int i=0; i<4; i++) trig[i].t_start += MAXTIMES*8; /* in preparation for next event, step up MAXTIMES*32ns in 4ns ticks */

  } /* sec */

  return;
}
