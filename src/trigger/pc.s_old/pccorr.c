/* pccorr.c - attenuation correction

  input:  
          npeak  - the number of peaks obtained
          peak   - peaks information

  output: hit    - hits information
*/


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)
#define ABS(x)      ((x) < 0 ? -(x) : (x))

#include "pclib.h"
#include "pcal_atten.h"



#define DEBUG


#define FACTOR1 256/*128*/
#define FACTOR2 128
#define SHIFT12 15/*14*/ /* CHANGE IT IF FACTOR1 or FACTOR2 CHANGED: it is shift to get FACTOR1*FACTOR2 */

#define WIN1_LEN  262144/*16384*/ /* max size available for now !!! */

/* u-68, v-62, w-62 */

static void
atten_read(int view, uint16_t array[WIN1_LEN])
{
  int i, k, ch, kk, uvw[3];
  uint16_t att[3];
  uint16_t dalz, dmin=999, dmax=0, kkmax=0;

  for(k=0; k<WIN1_LEN; k++) array[k] = 0;

  for(i=0; i<PCATTEN; i++)
  {
    uvw[0] = filecontent[i][0];
    uvw[1] = filecontent[i][1];
    uvw[2] = filecontent[i][2];

	/* in following do '1000*' because 'filecontent' already multiplied by 1000;
    modify that if attenuation presentation is changed */
#ifdef NO_ATTEN
    att[0] = (uint16_t)FACTOR2;
	att[1] = (uint16_t)FACTOR2;
	att[2] = (uint16_t)FACTOR2;
#else
    att[0] = (uint16_t)(1000.* (float)FACTOR2 / (float)filecontent[i][3]);
	att[1] = (uint16_t)(1000.* (float)FACTOR2 / (float)filecontent[i][4]);
	att[2] = (uint16_t)(1000.* (float)FACTOR2 / (float)filecontent[i][5]);
#endif

    kk = 132 - (uvw[0]+uvw[1]+uvw[2]);
    k = uvw[2] + (uvw[1]<<6) + (kk<<12);
    dalz = uvw[0]+uvw[1]+uvw[2];
    if(k>=0&&k<WIN1_LEN)
	{
      array[k] = att[view];
	}
    else
	{
      printf("atten_read ERROR: %2d %2d %2d (sum=%d) -> kk=%2d k=0x%04x -> %4d %4d %4d\n",uvw[0],uvw[1],uvw[2],dalz,kk,k,att[0],att[1],att[2]);
	}
    if(dalz<dmin) dmin=dalz;
    if(dalz>dmax) dmax=dalz;
    if(kk>kkmax) kkmax=kk;
    /*printf("atten_read: %2d %2d %2d (sum=%d) -> kk=%2d k=0x%04x -> %4d %4d %4d\n",uvw[0],uvw[1],uvw[2],dalz,kk,k,att[0],att[1],att[2]);*/
	if(kk<0||kk>18)
	{
      printf("atten_read: exit\n");
      exit(0);
	}
  }
  /*printf("dmin=%d dmax=%d kkmax=%d\n",dmin,dmax, kkmax);*/
  /*exit(0);*/
}

static void
coeff1_init(uint16_t rom_array[WIN1_LEN])
{
  int i,j,k;

  /* fraction(8bit) = energy(7bit) / energy(7bit) */
  for(k=0; k<WIN1_LEN; k++)
  {
    j = k&0x7F;
    i = (k>>7)&0x7F;
    if(j==0) rom_array[k] = 0;
    else rom_array[k] = (uint16_t)( ((float)(i))*((float)(FACTOR1))/((float)(j)) );
    /*printf("coeff1_init: i=%3d j=%4d k=%7d -> data=%5d\n",i,j,k,rom_array[k]);*/
  }
}


static uint8_t
get_addr_shift(addr)
{
#pragma HLS INLINE
#pragma HLS PIPELINE
  uint8_t nn;

  if(addr&0x8000)      nn = 9;
  else if(addr&0x4000) nn = 8;
  else if(addr&0x2000) nn = 7;
  else if(addr&0x1000) nn = 6;
  else if(addr&0x0800) nn = 5;
  else if(addr&0x0400) nn = 4;
  else if(addr&0x0200) nn = 3;
  else if(addr&0x0100) nn = 2;
  else if(addr&0x0080) nn = 1;
  else                 nn = 0;

  return(nn);
}

static uint16_t
coeff1_get(uint16_t addr1, uint16_t addr2, uint8_t n, uint16_t rom_array[WIN1_LEN])
{
#pragma HLS INLINE
#pragma HLS PIPELINE
  uint16_t ret;
  uint8_t  temp1,temp2;
  uint16_t addr;

  temp1 = addr1>>n;
  temp2 = addr2>>n;

  /*printf("+++ n1=%d n2=%d\n",n1,n2);*/

  addr = (temp1<<7)|temp2;
  ret = rom_array[addr];

  return(ret);
}


uint16_t
pcal_coord_to_strip(uint8_t view, uint16_t jj)
{
  uint16_t ii;

  if(view==0)
  {
    if(jj <= 52) ii = jj;
    if(jj >  52) ii = 52+(jj-52)/2;
  }
  else if(view==1)
  {
    if(jj <= 30) ii = jj/2;
    if(jj >  30) ii = jj-15;
  }
  else if(view==2)
  {
    if(jj <= 30) ii = jj/2;
    if(jj >  30) ii = jj-15;
  }
  return(ii);
}


/*xc7vx550tffg1158-1*/


/* 3.39/60/30/1%/4%/1%/6% */

/* to report debugging info to the main */
int ndalz2;
int dalz2[MAXDALZ];


void
pccorr(unsigned short threshold, uint8_t npeak[3], PCPeak peak_in[3][PPEAK],
		   uint16_t peakfrac_in[3][PPEAK], uint8_t npsble,
           uint8_t *nhits, PCHit hit[PHIT], PCHit hitout[PHIT])
{
#pragma HLS PIPELINE
  static uint16_t coeff1[WIN1_LEN];
  static uint16_t att_u[WIN1_LEN];
  static uint16_t att_v[WIN1_LEN];
  static uint16_t att_w[WIN1_LEN];
  uint16_t temp1, temp2, temp3;
  int i, j, k, kk, l, ith, axis, ihit, peakid, peakn, npks, nhit;
  int attn, lat, fraction[3];
      int u,v,w;
      int uu,vv,ww;
      int uuu,vvv,www;
#pragma HLS ARRAY_PARTITION variable=fraction complete dim=1
  int fracenergy, tmp1, tmp2;
  uint16_t fracen;
  uint16_t energy;
  uint32_t energysum;
  uint16_t addr;
  uint8_t n,n1,n2; /* shift needed for addr1 and addr2 to be fitted into 7 bit */
  uint16_t uvw[3], dalitz, uvw2[3];
#pragma HLS ARRAY_PARTITION variable=uvw complete dim=1

  PCPeak peak[3][PPEAK];
#pragma HLS ARRAY_PARTITION variable=peak complete dim=1
  uint16_t peakfrac[3][PPEAK];
#pragma HLS ARRAY_PARTITION variable=peakfrac complete dim=1


  /* In order to ensure that 'coeff1' is inferred and properly
  initialized as a ROM, it is recommended that the array initialization
  be done in a sub-function with global (wrt this source file) scope. */
  atten_read(0, att_u);
  atten_read(1, att_v);
  atten_read(2, att_w);

  coeff1_init(coeff1);

  for(i=0; i<3; i++) for(j=0; j<PPEAK; j++) peak[i][j] = peak_in[i][j];
  for(i=0; i<3; i++) for(j=0; j<PPEAK; j++) peakfrac[i][j] = peakfrac_in[i][j];

#ifdef DEBUG
  printf("\n+++ pccorr +++\n\n");
  ndalz2 = 0;
#endif

  /* sorting output array in decreasing energy order */
  /*vivado qsort((void *)hit, npsble, sizeof(PCHit), (int (*) (const void *, const void *))hit_compare);
  nhit = 0;
  while(hit[nhit].energy > threshold && nhit < NHIT && nhit < npsble) nhit++;
*/
  nhit = npsble;

  /* loop for all hits */

  for(ihit=0; ihit<PHIT; ihit++)
  {
    if(ihit>=nhit) continue;

    energy = hit[ihit].energy;
    energysum = 0;
    hitout[ihit].energy = 0;
    /*hitout[ihit].time   = 0;*/
    /*hitout[ihit].width  = 0.;*/
    for(i=0; i<3; i++) uvw[i] = hit[ihit].coord[i];
#ifdef DEBUG
    printf("INPUT [%d] coordU=%d, coordV=%d, coordW=%d -> energy=%d\n",ihit,hit[ihit].coord[0],hit[ihit].coord[1],hit[ihit].coord[2],energy);
#endif

    /* loop for 3 axis */

    npks = 0;
    for(axis=0; axis<3; axis++)
    {
      npks++;

      peakid = hit[ihit].peak1[axis];
      peakn = hit[ihit].peakn[axis];








	  /* 'peakfrac' range is [100:1] */
	  
#ifdef DEBUG
	  printf("\nTMP1: hit energy=%d, peakfrac=%d, peak.energy=%d -> tmp1=%d\n",energy,peakfrac[axis][peakid],peak[axis][peakid].energy,(energy / peakfrac[axis][peakid]) * peak[axis][peakid].energy);
#endif
      temp1 = energy;
      temp2 = peakfrac[axis][peakid];
      n1 = get_addr_shift(temp1);
      n2 = get_addr_shift(temp2);
      n = MAX(n1,n2);
      temp3 = coeff1_get(temp1,temp2,n,coeff1);
      tmp1 = temp3 * peak[axis][peakid].energy;
#ifdef DEBUG
      printf("temp1=%d temp2=%d temp3=%d (dec %d, shift %d) => tmp1=%d\n",temp1,temp2,temp3,temp1*100/temp2,(temp1/temp2)<<7, tmp1);
#endif







      /* convert to strip numbers to access atten table (uvw2 from 0) */
	  for(i=0; i<3; i++)
      {

        temp1 = uvw[i] /* /fview[i]*/;

		printf("temp1=%d\n",temp1);
        uvw2[i] = pcal_coord_to_strip(i,temp1)+1; /* +1 because atten table contains strip numbers from 1, we have from 0 */
	  }

      printf("UVW:  %d %d %d\n",uvw[0],uvw[1],uvw[2]);
      printf("UVW2: %d %d %d\n",uvw2[0],uvw2[1],uvw2[2]);






      dalitz = uvw2[0]+uvw2[1]+uvw2[2];
	  
#ifdef DEBUG
	  printf("===> uvw2=%d %d %d dalitz=%d\n",uvw2[0],uvw2[1],uvw2[2],dalitz);
      dalz2[ndalz2++] = dalitz;
#endif	  

	 /* HUCK for now */
     if( dalitz<114 || dalitz>132)
	 {
       if(dalitz<114)      uvw2[2] = 114 - (uvw2[0]+uvw2[1]);
       else if(dalitz>132) uvw2[2] = 132 - (uvw2[0]+uvw2[1]);
       dalitz = uvw2[0]+uvw2[1]+uvw2[2];
#ifdef DEBUG
	   printf("HUCK !!! new uvw2=%d, now have %d %d %d, so dalitz=%d\n",uvw2[2],uvw2[0],uvw2[1],uvw2[2],dalitz);
#endif
	 }
	 /* HUCK for now */


      kk = 132 - dalitz;



      addr = uvw2[2] | (uvw2[1]<<6) | (kk<<12);
#ifdef DEBUG
	  printf("uvw=%d %d %d -> addr=0x%04x\n",uvw[0],uvw[1],kk,addr);
#endif
	  if(axis==0)       attn = att_u[addr];
      else if(axis==1)  attn = att_v[addr];
	  else              attn = att_w[addr];
#ifdef DEBUG
      printf("ATT[%d]: u=%2d v=%2d w=%2d -> kk=%d -> addr=0x%08x -> attn=%3d\n",axis,uvw[0],uvw[1],uvw[2],kk,addr,attn);
#endif









	  /* 'attn' range is [1000:1] */
      /*fracenergy = (tmp1 * 1000) / attn;*/
      fracenergy = (tmp1 * attn);
      fracen = fracenergy >> SHIFT12;
#ifdef DEBUG
	  printf("AAA: tmp1=%d attn=%d -> fracenergy=%d -> fracen=%d\n",tmp1,attn,fracenergy,fracen);
#endif









      energysum += fracen/*ergy*/;
#ifdef DEBUG
      printf("\n4: energy=%d\n",energysum);
#endif

/*hit[ihit].energy = hit[ihit].energy + fracenergy;*/

#ifdef DEBUG_
      printf("AXIS[%d]: lat=%3.0f distance=%3.0f: init_energy=%d, correct_energy=%d\n",axis,lat,path[axis],peak[axis][peakid].energy,fracenergy);
#endif

	  /*
      hit[ihit].time = hit[ihit].time + peak[axis][peakid].time;
	  */

/*width
      hit[ihit].width = hit[ihit].width + peak[axis][peakid].width;
width*/


      hitout[ihit].peak1[axis] = peakid;
      hitout[ihit].peakn[axis] = peakn;
    }

    for(i=0; i<3; i++) hitout[ihit].coord[i] = ( ( pcal_coord_to_strip(i,(uvw[i] /* /fview[i]*/)) ) << 3 ) + 4;
    hitout[ihit].energy = energysum;

/*width*/
    /*hit[ihit].width = hit[ihit].width / 3;*/
/*width*/
#ifdef DEBUG
    printf("OUTPUT [%d] coordU=%d(%d), coordV=%d(%d), coordW=%d(%d) -> energy=%d\n",ihit,hitout[ihit].coord[0],uuu,hitout[ihit].coord[1],vvv,hitout[ihit].coord[2],www,energy);
    printf("pccorr: energy=%d\n\n",hitout[ihit].energy);
#endif

	/*
    npks = MAX(npks,1);
    hit[ihit].time = hit[ihit].time / npks;
	*/

#ifdef DEBUG_
    printf("5: pccorr[%d] sec=%d lay=%d i,j=%f %f\n",ihit+1,hitout[ihit].i,hitout[ihit].j);
    printf("pccorr: energy=%d time=%d\n",hitout[ihit].energy,hitout[ihit].time);
#endif
  }
#ifdef DEBUG
  printf("\n");
#endif

  *nhits = nhit;

  return;
}


