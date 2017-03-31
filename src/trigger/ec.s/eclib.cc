
/************************************************

  eclib.c - forward calorimeter reconstruction

  BUG report: boiarino@jlab.org

  TO DO: 'gap' parameter in ecpeak() not in use -> use it !!! see ecpeakc()

*************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <iostream>
using namespace std;

#include "evio.h"
#include "evioBankUtil.h"




#ifdef USE_PCAL

#include "../pc.s/pctrans.h"
#include "../pc.s/pclib.h"
#define ecinit pcinit
#define ecstrips pcstrips
#define ecl3 pcl3

#else

#include "ectrans.h"
#include "eclib.h"

#endif


#include "prlib.h"


//#define DEBUG
//#define DEBUG_1
//#define DEBUG_2


#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)
#define ABS(x)      ((x) < 0 ? -(x) : (x))



/* variables global for present file */

static int runnum = 0;
static float ped[NSECTOR][NLAYER][NSTRIP]; /* pedestals (channels) */
static float nba[NSECTOR][NLAYER][NSTRIP]; /* nsa+nsb (bins) */
static float ech[NSECTOR][NLAYER][NSTRIP];
static float t0 [NSECTOR][NLAYER][NSTRIP]; /* delays (nsec) */
static float tch[NSECTOR][NLAYER][NSTRIP];




/*********************************************************
                some useful utilities
**********************************************************/






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
        ped[i][j][k] = 0.0;
        nba[i][j][k] = 0.0;
        ech[i][j][k] = 1.0/*0.0001*/;
        t0 [i][j][k] = 0.0;
        tch[i][j][k] = 1.0/*0.015625*/; /* divide by 64 to get nanoseconds */
      }
    }
  }

  return(0);
}


/* ecstrips.c - loading strip info for particular sector

  input:  sector - sector number

  output: strip[][].energy - strip energy (MeV)
          strip[][].time   - strip time (ns)
*/



int
ecstrips(unsigned int *bufptr, unsigned short threshold, int sec, int io, ECStrip strip[3][NSTRIP])
{
  int i, j, k, ind, nhits, str, layer, error, ii, jj, nbytes, ind_data, nn, mm;
  int energy;
  unsigned short strip_threshold;
  unsigned long long timestamp, timestamp_old;
  GET_PUT_INIT;
#ifdef USE_PCAL
  int tag[6] = {2,8,14,20,26,32}; /* tag for sectors */
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
  char *ch, str_tmp[256];
  float ff[16];
  int nsa, nsb;

#ifdef DEBUG
  printf("ecstrips reached, sector=%d\n",sec);fflush(stdout);
#endif


  /********************/
  /* pedestal and nba */

  fragtag = tag[sec];
  fragnum = -1;
  banktag = 0xe10e;
  banknum = tag[sec];
  if((ind = evLinkBank(bufptr, fragtag, fragnum, banktag, banknum, &nbytes, &ind_data)) > 0)
  {
#ifdef DEBUG_1
    printf("evLinkBank: ind=%d ind_data=%d\n",ind, ind_data);fflush(stdout);
    printf("ind=%d, nbytes=%d\n",ind,nbytes);fflush(stdout);
#endif
    b08 = (unsigned char *) &bufptr[ind_data];
    end = b08 + nbytes;
#ifdef DEBUG_1
    printf("ind=%d, nbytes=%d (from 0x%08x to 0x%08x)\n",ind,nbytes,b08,end);fflush(stdout);
#endif
    while(b08<end)
	{
      ch = (char *)b08;
      ii=0;
#ifdef DEBUG_1
      printf("CH\n");fflush(stdout);
#endif
      while( (isalnum(ch[ii])||(ch[ii]==' ')||(ch[ii]=='_')||(ch[ii]=='.')) && (ii<500))
      {
#ifdef DEBUG_1
        printf("%c",ch[ii]);fflush(stdout);
#endif
        ii++;
      }
#ifdef DEBUG_1
      if((ch[ii]=='\0')||(ch[ii]=='\n')) printf("\n");fflush(stdout);
#endif
      if(!strncmp(ch,"FADC250_SLOT",12))
      {
        slot=atoi((char *)&ch[12]);
#ifdef DEBUG_1
        printf("===> slot=%d\n",slot);fflush(stdout);
#endif
      }

      if(!strncmp(ch,"FADC250_NSB",11))
      {
        nsb=atoi((char *)&ch[11]);
#ifdef DEBUG_1
        printf("===> nsb=%d\n",nsb);fflush(stdout);
#endif
      }

      if(!strncmp(ch,"FADC250_NSA",11))
      {
        nsa=atoi((char *)&ch[11]);
#ifdef DEBUG_1
        printf("===> nsa=%d\n",nsa);fflush(stdout);
#endif
      }

      if(!strncmp(ch,"FADC250_ALLCH_PED",17))
      {
#ifdef DEBUG_1
        printf("===> peds\n");fflush(stdout);
#endif
        sscanf (ch, "%*s %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
                &ff[0],&ff[1],&ff[2],&ff[3],&ff[4],&ff[5],&ff[6],&ff[7],
                &ff[8],&ff[9],&ff[10],&ff[11],&ff[12],&ff[13],&ff[14],&ff[15]);

        for(chan=0; chan<16; chan++)
	    {
#ifdef DEBUG_1
          printf("ff[%d]=%f\n",chan,ff[chan]);fflush(stdout);
#endif
#ifdef USE_PCAL
          layer = adclayerpcal[slot][chan] - 1;
          str   = adcstrippcal[slot][chan] - 1;
#else
          layer = adclayerecal[slot][chan] - 1;
          str   = adcstripecal[slot][chan] - 1;
#endif
          ped[sec][layer][str] = ff[chan];
          nba[sec][layer][str] = (float)((nsb+nsa)/4);
	    }
      }

      ii++;
      b08 += ii;
	}


#ifdef DEBUG_1
    for(layer=0; layer<NLAYER; layer++)
    {
	  for(str=0; str<NSTRIP; str++)
	  {
	    printf("==ped==[%d][%d]==%f, nba=%f\n",layer,str,ped[sec][layer][str],nba[sec][layer][str]);fflush(stdout);
	  }
    }
#endif


  }
  else
  {
    printf("cannot find bank 0x%04x num %d\n",banktag, banknum);fflush(stdout);
  }

  /* pedestal and nba */
  /********************/



  strip_threshold = threshold;
  bzero((char *)strip,3*NSTRIP*sizeof(ECStrip));
  nhits = 0;

  fragtag = tag[sec];
  fragnum = -1;
  banktag = 0xe102;
  banknum = 0;

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
#ifdef DEBUG_1
      printf("begin while: b08=0x%08x\n",b08);fflush(stdout);
#endif
      GET8(slot);
      GET32(trig);
      GET64(time);
      GET32(nchan);
#ifdef DEBUG_1
      printf("slot=%d, trig=%d, time=%lld nchan=%d\n",slot,trig,time,nchan);fflush(stdout);
#endif
      for(nn=0; nn<nchan; nn++)
	  {
        GET8(chan);
        /*chan++;*/
        GET32(npulses);
#ifdef DEBUG_1
        printf("  chan=%d, npulses=%d\n",chan,npulses);fflush(stdout);
#endif
        for(mm=0; mm<npulses; mm++)
	    {
          GET16(pulse_time);
          GET32(pulse_integral);
          GET16(pulse_min);
          GET16(pulse_max);
#ifdef DEBUG_1
          printf(" b32=0x%08x:  pulse_time=%d pulse_integral=%d\n",b32,pulse_time,pulse_integral);fflush(stdout);
#endif
#ifdef USE_PCAL

#ifdef SIMULTION
          layer = adclayerpcal[slot][chan-1] - 1; /* from 0 */
          str   = adcstrippcal[slot][chan-1] - 1; /* from 0 */
#endif
          layer = adclayerpcal[slot][chan] - 1; /* from 0 */
          str   = adcstrippcal[slot][chan] - 1; /* from 0 */
#else


#ifdef SIMULTION
          layer = adclayerecal[slot][chan-1] - 1; /* from 0 */
          str   = adcstripecal[slot][chan-1] - 1; /* from 0 */
#endif
          layer = adclayerecal[slot][chan] - 1; /* from 0 */
          str   = adcstripecal[slot][chan] - 1; /* from 0 */



#ifdef DEBUG_1
          printf("### slot %d chan %d ---> sec %d, layer %d, strip %d ---> ped=%f\n",
                 slot,chan,sec,layer,str,ped[sec][layer][str]);fflush(stdout);
#endif
          if(str>=NSTRIP||str<0||layer>=NLAYER||layer<0)
		  {
            printf("ERROR: ### slot %d chan %d ---> sec %d, layer %d, strip %d ---> ped=%f\n",
                 slot,chan,sec,layer,str,ped[sec][layer][str]);fflush(stdout);
            exit(0);
		  }

#endif
#ifdef DEBUG_1
          printf("### slot %d chan %d ---> sec %d, layer %d, strip %d\n",slot,chan,sec,layer,str);fflush(stdout);
#endif
          if(layer<0 || layer>=NLAYER) continue; /*check layer number range*/
          if(str<0 || str>=NSTRIP) continue; /*check strip number range*/

		  /*if(layer>=3) continue;*/ /*sergey: only INNER for now */

		  energy = (pulse_integral - ped[sec][layer][str]*nba[sec][layer][str]) * ech[sec][layer][str];
#ifdef DEBUG_1
          printf("### energy=%d (ped=%f, nba=%f, ech=%f, strip_threshold=%d)\n",
			   energy,ped[sec][layer][str],nba[sec][layer][str],ech[sec][layer][str],strip_threshold);fflush(stdout);
#endif



if(io==0&&layer<3)
{
          if(energy >= strip_threshold)
          {
            strip[layer][str].energy = energy;
            /*strip[layer][str].time = pulse_time * tch[sec][layer][str] - t0[sec][layer][str];*/
#ifdef DEBUG_1
            cout<<"ecstrips["<<nhits<<"]: sec "<<sec+1<<", layer "<<layer+1<<", str "<<str+1<<", energy "<<strip[layer][str].energy<<", time "<<0<<endl;
#endif
          }
	      nhits ++;
}
else if(io==1&&layer>=3)
{
          if(energy >= strip_threshold)
          {
            strip[layer-3][str].energy = energy;
            /*strip[layer-3][str].time = pulse_time * tch[sec][layer][str] - t0[sec][layer][str];*/
#ifdef DEBUG_1
            cout<<"ecstrips["<<nhits<<"]: sec "<<sec+1<<", layer "<<layer+1<<", str "<<str+1<<", energy "<<strip[layer-3][str].energy<<", time "<<0<<endl;
#endif
          }
	      nhits ++;
}


	    }
#ifdef DEBUG_1
        printf("end loop: b08=0x%08x\n",b08);fflush(stdout);
#endif
      }
    }
  }
  else
  {
    printf("cannot find bank 0x%04x num %d\n",banktag, banknum);fflush(stdout);
  }


  return(ind);
}



int
ectrig(unsigned int *bufptr, int sec, int npeak[NLAYER], TrigECPeak peak[NLAYER][NPEAKMAX], int nhit[2], TrigECHit hit[2][NHITMAX])
{
  int i, j, k, ind, nhits, npeaks, instance, view, str, layer, error, ii, nbytes, ind_data, nn, mm;
  int energy;
  unsigned long long timestamp, timestamp_old;
  GET_PUT_INIT;
#ifdef USE_PCAL
  int tag[6] = {2,8,14,20,26,32}; /* tag for sectors */
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
  printf("ecstrips reached, sector=%d\n",sec);
#endif


  bzero((char *)peak,NLAYER*NPEAKMAX*sizeof(TrigECPeak));
  bzero((char *)npeak,NLAYER*sizeof(int));
  bzero((char *)hit,2*NHITMAX*sizeof(TrigECPeak));
  bzero((char *)nhit,2*sizeof(int));
  npeaks = 0;
  nhits = 0;


  fragtag = 101+sec;
  fragnum = -1;
  banktag = 0xe122;
  banknum = 0/*101+sec*/;

  if((ind = evLinkBank(bufptr, fragtag, fragnum, banktag, banknum, &nbytes, &ind_data)) <= 0)
  {
    printf("cannot find bank tag=0x%04x num=%d\n",banktag,banknum);
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
#ifdef DEBUG_1
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
		  layer = instance*3 + view;
          coord = (last_val>>15) & 0x1ff;
          energy = (last_val>>2) & 0x1fff;
          time = (val>>0) & 0x7ff;
#ifdef DEBUG_2
          printf("ECtrig Peak: ");
          printf(" Inst = %1d", instance);
          printf(" View = %c", view_array[view]);
          printf(" (layer=%d)",layer);
          printf(" Coord = %2.2f (%d)", ((float)(coord) / 8.0f), coord );
          printf(" Energy = %4d", energy);
          printf(" Time = %4d\n", time);
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
          v = ((last_val<<1)&0x1fe) | ((val>>30)&0x1);
          instance = (last_val>>26) & 0x1;
          coordW = (last_val>>17) & 0x1ff;
          coordV = (last_val>>8) & 0x1ff;
		  coordU = v;
          energy = (val>>17) & 0x1fff;
          time = (val>>0) & 0x7ff;
#ifdef DEBUG_2
          printf("ECtrig Cluster:");
          printf(" Inst = %1d", instance);
          printf(" Coord W = %2.2f (%d)", ((float)(coordW)) / 8.0f, coordW );
          printf(" Coord V = %2.2f (%d)", ((float)(coordV)) / 8.0f, coordV );
		  printf(" Coord U = %2.2f (%d)", ((float)(coordU)) / 8.0f, coordU );
          printf(" Energy = %4d", energy);
          printf(" Time = %4d\n", time);
#endif
		  nhits = nhit[instance];
		  hit[instance][nhits].coord[0] = coordU;
		  hit[instance][nhits].coord[1] = coordV;
		  hit[instance][nhits].coord[2] = coordW;
		  hit[instance][nhits].energy = energy;
		  hit[instance][nhits].time = time;
		  nhit[instance] ++;
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
  }


  return(ind);
}




#ifdef USE_PCAL
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

/* search for clusters in one ECAL view */
int
ecl3(unsigned int *bufptr, unsigned short threshold[3], int io, ECHit hitshits[NHIT])
{
  int ii, ret, sec, uvw, npsble;
  uint8_t nhits;
  ECStrip strip[3][NSTRIP];
  ECHit hit[NHIT];

  for(sec=0; sec<NSECTOR; sec++)
  {
	printf("sec1=%d threshold[0]=%d io=%d\n",sec,threshold[0],io);fflush(stdout);
    ret = ecstrips(bufptr, threshold[0], sec, io, strip);
 	printf("sec2=%d\n",sec);fflush(stdout);
    if(ret > 0)
    {
	  
#ifdef USE_PCAL
      nhits = pcal(strip, &hit[0]);
#else
      nhits = ecal(strip, &hit[0]);
#endif

      //cout<<"nhits="<<+nhits<<endl;fflush(stdout);

      for(ii=0; ii<nhits; ii++) hitshits[ii] = hit[ii];
	  if(io==0) printf("INNER\n");
	  else if(io==1) printf("OUTER\n");
	  cout<<"ecl3: nhits="<<+nhits<<endl;
      for(ii=0; ii<nhits; ii++)
	  {
        cout<<"ecl3: hit["<<+ii<<"]: energy="<<+hit[ii].energy<<" coordU="<<+hit[ii].coord[0]<<" coordV="<<+hit[ii].coord[1]<<" coordW="<<+hit[ii].coord[2]<<endl;
	  }
    }
  }

  return(nhits);
}

