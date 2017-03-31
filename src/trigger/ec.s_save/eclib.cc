
/************************************************

  eclib.c - forward calorimeter reconstruction

  BUG report: boiarino@jlab.org

  TO DO: 'gap' parameter in ecpeak() not in use -> use it !!! see ecpeakc()

*************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "evio.h"
#include "evioBankUtil.h"

#include "ectrans.h"
#include "eclib.h"

#include "prlib.h"


#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)
#define ABS(x)      ((x) < 0 ? -(x) : (x))

#undef DEBUG_1


/* variables global for present file */

static int runnum = 0;
unsigned short strip_threshold;
static float e0 [NSECTOR][NLAYER][NSTRIP]; /* pedestals (channels) */
static float ech[NSECTOR][NLAYER][NSTRIP];
static float t0 [NSECTOR][NLAYER][NSTRIP]; /* delays (nsec) */
static float tch[NSECTOR][NLAYER][NSTRIP];
/*static*/ float atten[NSECTOR][3][3][NSTRIP]; /* NSECTOR sectors, 3 parts(inner,outer,whole), 3 views, NSTRIP strips */




/*********************************************************
                some useful utilities
**********************************************************/






/* ecinit, ecinitgeom, ecinitcalib - EC initialization */

int
ecinit(int runnum, int def_adc, int def_tdc, int def_atten)
{
  int i,j,k,l;

  printf("eclib: def_adc=%d    def_tdc=%d\n",def_adc,def_tdc);fflush(stdout);

  for(i=0; i<NSECTOR; i++)
  {
    for(j=0; j<NLAYER; j++)
    {
      for(k=0; k<NSTRIP; k++)
      {
        e0 [i][j][k] = 0.0;
        ech[i][j][k] = 1.0/*0.0001*/;
        t0 [i][j][k] = 0.0;
        tch[i][j][k] = 1.0/*0.015625*/; /* devide by 64 to get nanoseconds */
      }
    }
  }


  printf("eclib: def_atten =%d\n",def_atten);fflush(stdout);
  for(j=0; j<3; j++)
  {
    for(l=0; l<3; l++)
    {
      for(i=0; i<NSECTOR; i++)
      {
        for(k=0; k<NSTRIP; k++)
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


/* ecstrips.c - loading strip info for particular sector

  input:  sector - sector number

  output: strip[][].energy - strip energy (MeV)
          strip[][].time   - strip time (ns)
*/


int
ecstrips(unsigned int *bufptr, unsigned short threshold, int sec, ECStrip strip[NLAYER][NSTRIP])
{
  int i, j, k, ind, nhits, str, layer, error, ii, nbytes, ind_data, nn, mm;
  int energy;
  unsigned long long timestamp, timestamp_old;
  GET_PUT_INIT;
  int tag[6] = {1,7,13,19,25,31}; /* tag for sectors */
  int fragtag, fragnum, banktag, banknum;
  unsigned short pulse_time, pulse_min, pulse_max;
  unsigned int pulse_integral;
  unsigned char *end;
  unsigned long long time;
  int crate,slot,trig,nchan,chan,npulses,notvalid,edge,data,count,ncol1,nrow1;
  int ndata0[22], data0[21][8];
  int baseline, sum, channel;

#ifdef DEBUG_1
  printf("ecstrips reached, sector=%d\n",sec);
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
  bzero((char *)strip,NLAYER*NSTRIP*sizeof(ECStrip));

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
        /*printf(">>> layer %d, strip %d -> adc %d\n",adclayerecal[slot][chan],adcstripecal[slot][chan],pulse_integral);*/
        layer = adclayerecal[slot][chan-1] - 1;
        str   = adcstripecal[slot][chan-1] - 1;
#ifdef DEBUG_1
        printf("### slot %d chan %d ---> sec %d, layer %d, strip %d\n",slot,chan,sec,layer,str);
#endif
        if(str<0 || str>=NSTRIP) continue; /*check strip number range*/
        if(layer<0 || layer>=NLAYER) continue; /*check layer number range*/

		/*if(layer>=3) continue;*/ /*sergey: only INNER for now */

		energy = (pulse_integral - e0[sec][layer][str]) * ech[sec][layer][str];
#ifdef DEBUG_1
        printf("### energy=%d (e0=%d, ech=%d, strip_threshold=%d)\n",
			   energy,e0[sec][layer][str],ech[sec][layer][str],strip_threshold);
#endif
        if(energy >= strip_threshold)
        {
          strip[layer][str].energy = energy;
          /*strip[layer][str].time = pulse_time * tch[sec][layer][str] - t0[sec][layer][str];*/
#ifdef DEBUG_1
          printf("ecstrips[%2d]: sec %d, layer %d, str %d, energy %d, time %d\n",
				 nhits,sec+1,layer+1,str+1,strip[layer][str].energy,0/*strip[layer][str].time*/);
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



/* search for clusters in one ECAL view */
int
ecl3(unsigned int *bufptr, const unsigned short threshold[3], int io, ECHit hitout[NHIT])
{
  int ii, ret, sec, uvw, npsble;
  ECStrip strip[2][3][36];
  int npeak[3][3];
  uint8_t nhits;
  ECPeak peak[3][3][NPEAK];
  ECHit hit[1000];
  uint16_t peakfrac[3][NPEAK];

  ret = 0;

  for(sec=0; sec<NSECTOR; sec++)
  {
    if(ecstrips(bufptr, threshold[0], sec, strip[io]) > 0)
    {
      ret = ecal(strip[io], &nhits, &hit[0]);
      for(ii=0; ii<nhits; ii++) hitout[ii] = hit[ii];
	  if(io==0) printf("INNER\n");
	  else if(io==1) printf("OUTER\n");
	  printf("ecl3: ret=%d nhits=%d\n",ret,nhits);
      for(ii=0; ii<nhits; ii++)
	  {
        printf("ecl3: hit[%d]: energy=%d coordU=%d coordV=%d coordW=%d\n",ii,
              hit[ii].energy,hit[ii].coord[0],hit[ii].coord[1],hit[ii].coord[2]);
	  }
    }
  }

  return(ret);
}
