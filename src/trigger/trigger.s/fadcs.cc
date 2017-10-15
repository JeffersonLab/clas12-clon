
/* fadcs.cc - loading strip info for particular sector

  input:  sector - sector number

  output: strip[][].energy - strip energy (MeV)
          strip[][].time   - strip time (ns)
*/

//#define DEBUG_0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#include <iostream>
using namespace std;

#include <ap_int.h>
/*#include <ap_fixed.h>*/
#include <hls_stream.h>

#include "evio.h"
#include "evioBankUtil.h"

#include "hls_fadc_sum.h"
#include "trigger.h"

#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)

static int nslots[NDET] = {7,7,7/*14*/,12,14,0,0,0,0};
static int slot2isl[NDET][22] = {
  -1,-1,-1, 0, 1, 2, 3, 4, 5, 6,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, /* ECAL_IN slots: 3-9 */
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 0,-1,-1, 1, 2, 3, 4, 5, 6,-1,-1,-1, /* ECAL_OUT slots: 10, 13-18 */
  -1,-1,-1, 0, 1, 2, 3, 4, 5, 6, 0,-1,-1, 1, 2, 3, 4, 5, 6,-1,-1,-1, /* ECAL slots: 3-10, 13-18 */
  -1,-1,-1, 0, 1, 2, 3, 4, 5, 6, 7,-1,-1, 8, 9,10,11,-1,-1,-1,-1,-1, /* PCAL slots: 3-10, 13-16 */
  -1,-1,-1, 0, 1, 2, 3, 4, 5, 6,-1,-1,-1, 7, 8, 9,10,11,12,13,-1,-1, /* DCRB slots: 3-9, 14-20 */
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, /* HTCC slots: */
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, /* FTOF slots: */
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, /* CTOF slots: */
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1  /* CND slots: */
};

static int nfragtags[NDET] = {6,6,6,6,18,1,6,1,1};
static int fragtags[NDET][18] = {
   1, 7,13,19,25,31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*ECIN*/
   1, 7,13,19,25,31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*ECOUT*/
   1, 7,13,19,25,31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*ECAL*/
   3, 9,15,21,27,33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*PCAL*/
  41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58, /*DCRB*/
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*HTCC*/
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*FTOF*/
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*CTOF*/
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  /*CND*/
};

static float ped[NDET][21][16]; /* pedestals */
static float tet[NDET][21][16]; /* threshold */
static float gain[NDET][21][16]; /* gain */
static int nsa[NDET][21]; /* NSA */
static int nsb[NDET][21]; /* NSB */

int
fadcs(unsigned int *bufptr, unsigned short threshold, int sec, int det, hls::stream<fadc_word_t> s_fadc_words[NFADCS], int dtimestamp, int dpulsetime,
      int *iev, unsigned long long *timestamp)
{
  int i, j, k, ind, nhits, error, ii, jj, nbytes, ind_data, nn, mm, isample, isam1, isam2;
  int summing_in_progress;
  int datasaved[1000];
  int energy, it;
  unsigned short strip_threshold;
  unsigned long long itime, ptime, offset;
  GET_PUT_INIT;
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
  fadc_word_t fadcs[MAXTIMES][NFADCS][NH_READS];
  int isl;
  ap_uint<13> ee;
  ap_uint<3>  tt;
  int nslot;

  nslot = nslots[det];

#ifdef DEBUG
  printf("fadcs reached, sector=%d\n",sec);fflush(stdout);
#endif


  /* cleanup output array */
  for(i=0; i<MAXTIMES; i++)
  {
	for(isl=0; isl<NFADCS; isl++)
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


  /*************************/
  /* pedestal nsa, nsb etc */

  fragtag = fragtags[det][sec];
  fragnum = -1;
  banktag = 0xe10e;
  banknum = fragtags[det][sec];
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
        nsb[det][slot] = atoi((char *)&ch[11]) / 4;
#ifdef DEBUG_3
        printf("===> nsb=%d ticks\n",nsb[det][slot]);fflush(stdout);
#endif
      }

      if(!strncmp(ch,"FADC250_NSA",11))
      {
        nsa[det][slot] = atoi((char *)&ch[11]) / 4;
#ifdef DEBUG_3
        printf("===> nsa=%d ticks\n",nsa[det][slot]);fflush(stdout);
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
          ped[det][slot][chan] = ff[chan];
#ifdef DEBUG_3
          printf("ped[%d][%d][%d]=%f\n",det,slot,chan,ped[det][slot][chan]);fflush(stdout);
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
          gain[det][slot][chan] = ff[chan];
#ifdef DEBUG_3
          printf("gain[%d][%d][%d]=%f\n",det,slot,chan,gain[det][slot][chan]);fflush(stdout);
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
          tet[det][slot][chan] = dd[chan];
#ifdef DEBUG_3
          printf("tet[%d][%d][%d]=%f\n",det,slot,chan,tet[det][slot][chan]);fflush(stdout);
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




  /*****************/
  /* waveform data */

  strip_threshold = threshold;
  nhits = 0;

  fragtag = fragtags[det][sec];
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
      printf("fadcs: begin while: b08=0x%08x\n",b08);fflush(stdout);
#endif


      GET8(slot);
      isl = slot2isl[det][slot];
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
          if( (datasaved[mm]>(int)ped[det][slot][chan]+tet[det][slot][chan]) && (datasaved[mm-1]<=(int)ped[det][slot][chan]+tet[det][slot][chan]) )
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
          isam1 = MAX((isample-nsb[det][slot]),0);
          isam2 = MIN((isample+nsa[det][slot]),nsamples);

          for(int m=isam1; m<isam2; m++)
		  {
            fsum += (float)datasaved[m];
            fped += ped[det][slot][chan];
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
		  printf("isample=%d nsa=%d nsb=%d -> isam1=%d isam2=%d -> data=%d ped=%f -> fsum=%f fped=%f -> pulse_integral=%6d -> it=%d tt=%d time=%lld(0x%016x) (tttt=%lld(0x%08x) (%lld %lld))\n\n",
                 isample, nsa[det][slot], nsb[det][slot], isam1, isam2, datasaved[mm], ped[det][slot][chan], fsum, fped, pulse_integral,it,(int)tt,time,time,
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



        /* now we have pulse - put it in array fadcs[time_slice][slot][channel] */
        ee = pulse_integral & 0x1FFF;
#ifdef DEBUG_0
		cout<<"fadcs: ee="<<ee<<", itime="<<itime<<", isl="<<isl<<", it="<<it<<", tt="<<tt<<endl;
#endif
        if(ee<=0)
	    {
          ;
#ifdef DEBUG_0
            cout<<"fadcs: ee="<<ee<<" - ignore the hit"<<endl;
#endif
		}
        else if(it>=MAXTIMES)
		{
          printf("fadcs: WARN: itime=%d too big - ignore the hit\n",itime);
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




        /*mm = isample + (8-tt);*/ /* search for more pulses starting from the beginning of next 'it' */
        mm = isample + 8; /* search for more pulses starting 8 pulses from previois crossing */
        printf("search for next pulse starting from mm=%d\n",mm);
		goto search_for_next_pulse;



done_with_pulse_search:
		;



      } /*for(nn=0; nn<nchan; nn++)*/


#ifdef DEBUG_0
      printf("fadcs: end loop: b08=0x%08x\n",b08);fflush(stdout);
#endif
    }




    /* write into output streams (one stream per slot) */
	for(i=0; i<MAXTIMES; i++)
	{
	  for(isl=0; isl<nslot; isl++)
	  {
		for(j=0; j<NH_READS; j++)
		{
#ifdef DEBUG_0
          cout<<"FADCS: WRITING TO STREAM ["<<i<<"]["<<isl<<"]["<<j<<"] adcs="<<fadcs[i][isl][j].e0<<" "<<fadcs[i][isl][j].e1<<" "<<fadcs[i][isl][j].e2<<" "<<fadcs[i][isl][j].e3<<endl;
#endif
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
