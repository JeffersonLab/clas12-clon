
/************************************************

  pclib.c - preshower calorimeter reconstruction

  BUG report: boiarino@jlab.org

*************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "evio.h"
#include "evioBankUtil.h"

#include "pctrans.h"
#include "pclib.h"

#include "prlib.h"


#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)
#define ABS(x)      ((x) < 0 ? -(x) : (x))

#define DEBUG_1


/* variables global for present file */

static int runnum = 0;
unsigned short strip_threshold;
static float e0 [PSECTOR][PLAYER][PSTRIP]; /* pedestals (channels) */
static float ech[PSECTOR][PLAYER][PSTRIP];
static float t0 [PSECTOR][PLAYER][PSTRIP]; /* delays (nsec) */
static float tch[PSECTOR][PLAYER][PSTRIP];
/*static*/ float atten[PSECTOR][3][3][PSTRIP]; /* NSECTOR sectors, 3 parts(inner,outer,whole), 3 views, NSTRIP strips */




/*********************************************************
                some useful utilities
**********************************************************/






/* pcinit */

int
pcinit(int runnum, int def_adc, int def_tdc, int def_atten)
{
  int i,j,k,l;

  printf("pclib: def_adc=%d    def_tdc=%d\n",def_adc,def_tdc);fflush(stdout);

  for(i=0; i<PSECTOR; i++)
  {
    for(j=0; j<PLAYER; j++)
    {
      for(k=0; k<PSTRIP; k++)
      {
        e0 [i][j][k] = 0.0;
        ech[i][j][k] = 1.0/*0.0001*/;
        t0 [i][j][k] = 0.0;
        tch[i][j][k] = 1.0/*0.015625*/; /* devide by 64 to get nanoseconds */
      }
    }
  }


  printf("pclib: def_atten =%d\n",def_atten);fflush(stdout);
  for(j=0; j<3; j++)
  {
    for(l=0; l<3; l++)
    {
      for(i=0; i<PSECTOR; i++)
      {
        for(k=0; k<PSTRIP; k++)
        {
          atten[i][j][l][k] = 376.0;
        }
      }
    }
  }

  if(!def_atten)
  {
    for(j=0; j<3; j++) /* inner,outer,whole */
    {
      for(l=0; l<3; l++) /* u,v,w */
      {
		;
      }
    }
  }


  return(0);
}


/* pcstrips.c - loading strip info for particular sector

  input:  sector - sector number

  output: strip[][].energy - strip energy (MeV)
          strip[][].time   - strip time (ns)
*/


int
pcstrips(int *bufptr, unsigned short threshold, int sec, PCStrip strip[PLAYER][PSTRIP])
{
  int i, j, k, ind, nhits, str, layer, error, ii, nbytes, ind_data, nn, mm;
  int energy;
  unsigned long long timestamp, timestamp_old;
  GET_PUT_INIT;
  int tag[6] = {2,8,14,20,26,32}; /* tag for sectors */
  int fragtag, fragnum, banktag, banknum;
  unsigned short pulse_time, pulse_min, pulse_max;
  unsigned int pulse_integral;
  unsigned char *end;
  unsigned long long time;
  int crate,slot,trig,nchan,chan,npulses,notvalid,edge,data,count,ncol1,nrow1;
  int ndata0[22], data0[21][8];
  int baseline, sum, channel;

#ifdef DEBUG_1
  printf("pcstrips reached, sector=%d\n",sec);
#endif

  fragtag = tag[sec];
  fragnum = -1;
  banktag = 0xe102;
  banknum = 0;

  if((ind = evLinkBank(bufptr, fragtag, fragnum, banktag, banknum, &nbytes, &ind_data)) <= 0) return(0);
#ifdef DEBUG_1
  printf("evLinkBank: ind=%d ind_data=%d\n",ind, ind_data);
#endif

  strip_threshold = threshold;
  bzero((char *)strip,PLAYER*PSTRIP*sizeof(PCStrip));

  nhits = 0;

#ifdef DEBUG_1
  printf("ind=%d, nbytes=%d\n",ind,nbytes);
#endif
  b08 = (unsigned char *) &bufptr[ind_data];
  end = b08 + nbytes;
#ifdef DEBUG_1
  printf("ind=%d, nbytes=%d (from 0x%08x to 0x%08x)\n",ind,nbytes,b08,end);
#endif
  while(b08<end)
  {
#ifdef DEBUG_1
    printf("begin while: b08=0x%08x\n",b08);
#endif
    GET8(slot);
    GET32(trig);
    GET64(time);
    GET32(nchan);
#ifdef DEBUG_1
    printf("slot=%d, trig=%d, time=%lld nchan=%d\n",slot,trig,time,nchan);
#endif
    for(nn=0; nn<nchan; nn++)
	{
      GET8(chan);
      /*chan++;*/
      GET32(npulses);
#ifdef DEBUG_1
      printf("  chan=%d, npulses=%d\n",chan,npulses);
#endif
      for(mm=0; mm<npulses; mm++)
	  {
        GET16(pulse_time);
        GET32(pulse_integral);
        GET16(pulse_min);
        GET16(pulse_max);
#ifdef DEBUG_1
        printf(" b32=0x%08x:  pulse_time=%d pulse_integral=%d\n",b32,pulse_time,pulse_integral);
#endif
        /*printf(">>> layer %d, strip %d -> adc %d\n",adclayerpcal[slot][chan],adcstrippcal[slot][chan],pulse_integral);*/
        layer = adclayerpcal[slot][chan-1] - 1;
        str   = adcstrippcal[slot][chan-1] - 1;
#ifdef DEBUG_1
        printf("### slot %d chan %d ---> sec %d, layer %d, strip %d\n",slot,chan,sec,layer,str);
#endif
        if(layer<0 || layer>=PLAYER) continue; /*check layer number range*/
        if(str<0 || str>=PSTRIP) continue; /*check strip number range*/

		/*if(layer>=3) continue;*/ /*sergey: only INNER for now */

		printf("111111111111 %d, %f %f -> %f, %d\n",pulse_integral,e0[sec][layer][str],ech[sec][layer][str],  energy     ,strip_threshold);

        if((energy = (pulse_integral - e0[sec][layer][str]) * ech[sec][layer][str]) >= strip_threshold)
        {
          strip[layer][str].energy = energy;
          /*strip[layer][str].time = pulse_time * tch[sec][layer][str] - t0[sec][layer][str] ;*/
#ifdef DEBUG_1
          printf("pcstrips[%2d]: sec %d, layer %d, str %d, energy %d, time %d\n",nhits,sec+1,layer+1,str+1,strip[layer][str].energy/*,strip[layer][str].time*/);
#endif
        }

	    nhits ++;
	  }
#ifdef DEBUG_1
      printf("end loop: b08=0x%08x\n",b08);
#endif
    }
  }

  return(ind);
}



/* search for clusters in one PCAL view */
int
pcl3(unsigned int *bufptr, const unsigned short threshold[3], int io, PCHit hitout[PHIT])
{
  int ii, ret, sec, uvw, npsble;
  PCStrip strip[2][3][PSTRIP];
  int npeak[3][3];
  uint8_t nhits;
  PCPeak peak[3][3][PPEAK];
  PCHit hit[1000];
  uint16_t peakfrac[3][PPEAK];

  ret = 0;

  for(sec=0; sec<PSECTOR; sec++)
  {
    if(pcstrips(bufptr, threshold[0], sec, strip) > 0)
    {
      ret = pcal(&strip[io][0][0], &nhits, &hit[0]);
      for(ii=0; ii<nhits; ii++) hitout[ii] = hit[ii];

	  printf("PCAL\n");
	  printf("pcl3: ret=%d nhits=%d\n",ret,nhits);
      for(ii=0; ii<nhits; ii++)
	  {
        printf("pcl3: hit[%d]: energy=%d time=%d coordU=%d coordV=%d coordW=%d\n",ii,
			   hit[ii].energy,  /*hit[ii].time*/0,  hit[ii].coord[0],hit[ii].coord[1],hit[ii].coord[2]);
	  }
    }
  }

  return(ret);
}
