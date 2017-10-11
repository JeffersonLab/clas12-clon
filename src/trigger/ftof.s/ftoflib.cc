
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include <iostream>
using namespace std;

#include "evio.h"
#include "evioBankUtil.h"

#include "ftoflib.h"
#include "ftoftrans.h"


#define DEBUG_0
#define DEBUG_3


#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)

static float ped[21][16]; /* pedestals */
static float tet[21][16]; /* threshold */
static float gain[21][16]; /* gain */
static int nsa[21]; /* NSA */
static int nsb[21]; /* NSB */


void
ftofhiteventreader(hls::stream<trig_t> &trig_stream, hls::stream<eventdata_t> &event_stream, FTOFHit &hit)
{
  eventdata_t eventdata;
  uint32_t data_end, word_first, tag, inst, view, data;
  int i = 0;
  while(1)
  {
    if(event_stream.empty()) {printf("ftofhiteventreader: EMPTY STREAM ERROR1\n");break;}
    eventdata = event_stream.read();

    data = eventdata.data;
    if(data == 0xFFFFFFFF) break;

    data_end = eventdata.end;           /* 0 for all words except last one when it is 1 */
    word_first = eventdata.data(31,31); /* 1 for the first word in hit, 0 for followings */
    tag = eventdata.data(30,27); /* must be 'FTOFHIT_TAG' */
    hit.output(35,24) = eventdata.data(11,0);

    if(event_stream.empty()) {printf("ftofhiteventreader: EMPTY STREAM ERROR2\n");break;}
    eventdata = event_stream.read();
    data_end = eventdata.end;
	hit.output(23,0) = eventdata.data(23,0);
  }

}


int
ftoffadcs(unsigned int *bufptr, unsigned short threshold, int sec, int io, hls::stream<fadc_word_t> s_fadc_words[NSLOT], int dtimestamp, int dpulsetime)
{
  int i, j, k, ind, nhits, str, layer, error, ii, jj, nbytes, ind_data, nn, mm, isample, isam1, isam2;
  int summing_in_progress;
  int datasaved[1000];
  int energy, it, ret;
  unsigned short strip_threshold;
  unsigned long long timestamp, timestamp_old, itime, ptime, offset;
  GET_PUT_INIT;

  int tag[6] = {5,11,17,23,29,35}; /* tag for sectors */
  int nslot = NSLOT;

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

  int slot2isl[22] = {-1,-1,-1,0,1,2,3,4,5,6,7,-1,-1,8,9,10,11,-1,-1,-1,-1,-1}; /* FTOF slots: 3-10,13-16 */


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
	  }
	}
  }
  ret = 0;

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
    //printf("cannot find bank 0x%04x num %d (pedestal)\n",banktag, banknum);
    return(0); /* return 0 if cannot get pedestals */
    fflush(stdout);
  }

  /* pedestal and nba */
  /********************/






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
if(slot >=3 && slot <=16) ret = 1; /*select FTOF-related slots only*/
      isl = slot2isl[slot];
      GET32(trig);
      GET64(time); /* time stamp for FADCs 2 counts bigger then for VTPs */
      time = ((time&0xFFFFFF)<<24) | ((time>>24)&0xFFFFFF); /* UNTIL FIXED IN ROL2 !!!!!!!!!!!!!!!!! */
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

        /*find threshold crossing sample*/
        isample = -1;
        pulse_integral = 0;
        for(mm=0; mm<nsamples; mm++)
		{
          if(datasaved[mm] > (int)ped[slot][chan]+tet[slot][chan])
		  {
            isample = mm;
            break;
		  }
		}


		
        printf("data[sl %2d][ch %2d]:",slot,chan);
        for(mm=40; mm<80; mm++)
		{
          printf(" [%2d]%4d,",mm,datasaved[mm]);
		}
        printf("\n");
		


        /*integrate*/
        fsum = 0.0;
        fped = 0.0;
        if(isample>=0)
		{

          isam1 = MAX((isample-nsb[slot]),0);
          isam2 = MIN((isample+nsa[slot]),nsamples);

          for(mm=isam1; mm<isam2; mm++)
		  {
            fsum += (float)datasaved[mm];
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

it-=TIMECORR; /*TEMP !!!!!!!!!!!!!!!!!!!*/
tt-=TIMECORR; /*TEMP !!!!!!!!!!!!!!!!!!!*/

//#ifdef DEBUG_0
          int tttt;
		  tttt = ((int)time) & 0xFFFFFF;
		  printf("isample=%d nsa=%d nsb=%d -> isam1=%d isam2=%d -> data=%d ped=%f -> fsum=%f fped=%f -> pulse_integral=%6d (strip=%2d) -> it=%d tt=%d time=%lld(0x%016x) (tttt=%lld(0x%08x) (%lld %lld))\n\n",
                 isample, nsa[slot], nsb[slot], isam1, isam2, datasaved[mm], ped[slot][chan], fsum, fped, pulse_integral, adcstrip[isl][chan]-1,it,(int)tt,time,time,
				 tttt,tttt,tttt/8,tttt%8);
//#endif
		}
        else
		{
#ifdef DEBUG_0
          printf("no pulse found - will do nothing for that channel\n");
#endif
          pulse_integral = 0;
		}




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
          int ch1 = chan/2;
          int ch2 = chan%2;
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
	    }



      } /*for(nn=0; nn<nchan; nn++)*/


#ifdef DEBUG_0
      printf("ecfadcs: end loop: b08=0x%08x\n",b08);fflush(stdout);
#endif
    }


	/* inject artificial hit(s) for testing 
	if(fadcs[0][2][4].e0==1914)
	{
      fadcs[0][0][0].e0 = 100;
      fadcs[0][0][0].t0 = 7;
      fadcs[1][0][0].e0 = 100;
      fadcs[1][0][0].t0 = 0;
	}
	*/


    /* fills output streams */
	for(i=0; i<MAXTIMES; i++)
	{
	  for(isl=0; isl<NSLOT; isl++)
	  {
		for(j=0; j<NH_READS; j++)
		{

		  if(fadcs[i][isl][j].e0>0 || fadcs[i][isl][j].e1>0) printf("[time=%d][isl=%d][write=%d] e0=%5d t0=%d, e1=%5d t1=%d\n",
                                                                    i,isl,j,(uint16_t)fadcs[i][isl][j].e0,(uint16_t)fadcs[i][isl][j].t0,(uint16_t)fadcs[i][isl][j].e1,(uint16_t)fadcs[i][isl][j].t1);

          s_fadc_words[isl].write(fadcs[i][isl][j]);
		}
	  }
	}

    bank_not_found = 0;
    break; /* if we found bank, exit from 'banknum' loop */
  }
  else
  {
    //printf("cannot find bank 0x%04x num %d (fragtag=%d fragnum=%d)\n",banktag,banknum,fragtag,fragnum);fflush(stdout);
    ind = 0;
  }
  }
  }

  /* waveform data */
  /*****************/




  return(ret);
}










static int iev = 0;
#define MAXBUF 1000000
static unsigned int buf[MAXBUF];


/* use it for time scan
#define TIMESTAMP1 0
#define TIMESTAMP2 8
#define PULSETIME1 0
#define PULSETIME2 8
*/
#define TIMESTAMP1 0
#define TIMESTAMP2 1
#define PULSETIME1 0
#define PULSETIME2 1


int
ftoflib(int handler)
{
  int status, ret;
  unsigned int *bufptr;
  ap_uint<16> strip_threshold = 1;
  ap_uint<16> mult_threshold = 1;
  ap_uint<16> cluster_threshold = 1;
  nframe_t nframes = 1;
  hls::stream<fadc_word_t> s_fadc_words[NSLOT];
  hls::stream<FTOFStrip_s> s_strip0;
  hls::stream<FTOFStrip_s> s_strip;
  hls::stream<FTOFHit> s_hit;
  hls::stream<FTOFHit> s_hit1;
  hls::stream<FTOFHit> s_hit2;
  volatile ap_uint<1> hit_scaler_inc;

  hls::stream<trig_t> trig_stream;
  hit_ram_t buf_ram[256];
  hls::stream<eventdata_t> event_stream;
  FTOFHit hit;

  int io = 0;
  int sec = 0;

  status = evRead(handler, buf, MAXBUF);
  if(status < 0)
  {
    if(status==EOF)
	{
      printf("end of file after %d events - exit\n",iev);
      return(EOF);
	}
    else
	{
      printf("evRead error=%d after %d events - exit\n",status,iev);
	}
    return(-1);
  }
  iev ++;
  printf("\nEVENT %5d\n\n",iev);


  bufptr = (unsigned int *) buf;



  for(int dtimestamp = TIMESTAMP1; dtimestamp<TIMESTAMP2; dtimestamp++)
  {
	for(int dpulsetime = PULSETIME1; dpulsetime<PULSETIME2; dpulsetime++)
	{
      ret = ftoffadcs(bufptr, strip_threshold, sec, io, s_fadc_words, dtimestamp, dpulsetime);

#ifdef TEST_BENCH
      if(1)
#else
      if(ret > 0)
#endif
      {
        for(int it=0; it<MAXTIMES; it++)
        {
	      /*FPGA*/
          ftofstrips(strip_threshold, s_fadc_words, s_strip0); /* sends data NH_READS=8 times every 4ns */
		  for(int ii=0; ii<NH_READS; ii++) /* process NH_READS time slices */
		  {
            ftofstripspersistence(nframes, s_strip0, s_strip);
            ftofhit(strip_threshold, mult_threshold, cluster_threshold, s_strip, s_hit);
            ftofhitfanout(s_hit, s_hit1, s_hit2, hit_scaler_inc);
          /*FPGA*/

            ftofhiteventfiller(s_hit1, buf_ram); /* do not need to read that stream, but do it to avoid leftover data */
            ftofhiteventfiller(s_hit2, buf_ram);
            ftofhiteventwriter(trig_stream, event_stream, buf_ram);
            ftofhiteventreader(trig_stream, event_stream, hit);
		  }
        }
	  }
	}
  }

  return(0);
}
