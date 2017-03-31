/* eccorr.c - attenuation correction

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

#include "eclib.h"
#include "ecal_atten.h"



#undef DEBUG


#define FACTOR1 256/*128*/
#define SHIFT1  8

#define FACTOR2 128
#define SHIFT2  7

#define SHIFT12 15/*14*/ /* CHANGE IT IF FACTOR1 or FACTOR2 CHANGED: it is shift to get FACTOR1*FACTOR2 */

#define WIN1_LEN  16384 /* max size available for now !!! */

static void
atten_read(int view, uint16_t array[WIN1_LEN])
{
  int i, k, ch, kk, uvw[3];
  uint16_t att[3];

  for(k=0; k<WIN1_LEN; k++) array[k] = 0;

  for(i=0; i<1296; i++)
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

    kk = 74 - (uvw[0]+uvw[1]+uvw[2]);
    k = uvw[0] + (uvw[1]<<6) + (kk<<12);
    array[k] = att[view];
    /*printf("atten_read: %2d %2d %2d -> kk=%2d k=0x%04x -> %4d %4d %4d\n",uvw[0],uvw[1],uvw[2],kk,k,att[0],att[1],att[2]);*/
  }
}



/*xc7vx550tffg1158-1*/


/* 3.48/32/12/1%/2%/0%/0% */



void
eccorr(unsigned short threshold, uint8_t npeak[3], ECPeak peak_in[3][NPEAK],
		   ECPeak1 peak1[3][NPEAK], uint8_t npsble,
           uint8_t *nhits, ECHit hit[NHIT], uint32_t hitfrac[3][NHIT], ECHit hitout[NHIT])
{
#pragma HLS PIPELINE
  static uint16_t att_u[WIN1_LEN];
  static uint16_t att_v[WIN1_LEN];
  static uint16_t att_w[WIN1_LEN];
  int i, j, k, kk, l, ith, axis, edge, ihit, ipeak, peakID, NhitsINpeak, peakn, npks, nhit;
  int attn, lat; 
  uint16_t fracenergy;
  uint16_t hitenergy;
  uint32_t energysum;
  uint16_t addr;
  uint16_t dalitz;
  uint16_t uvw[3];
#pragma HLS ARRAY_PARTITION variable=uvw complete dim=1
  uint16_t uvw2[3];
#pragma HLS ARRAY_PARTITION variable=uvw2 complete dim=1

  ECPeak peak[3][NPEAK];
#pragma HLS ARRAY_PARTITION variable=peak complete dim=1

  int peak_ID, hitID;
  uint32_t fractmp;


  /* In order to ensure that 'coeff1' is inferred and properly
  initialized as a ROM, it is recommended that the array initialization
  be done in a sub-function with global (wrt this source file) scope. */
  atten_read(0, att_u);
  atten_read(1, att_v);
  atten_read(2, att_w);

  for(i=0; i<3; i++) for(j=0; j<NPEAK; j++) peak[i][j] = peak_in[i][j];


#ifdef DEBUG
  printf("\n\n\n+++ eccorr +++\n\n\n");
#endif

  /* sorting output array in decreasing energy order */
  /*vivado qsort((void *)hit, npsble, sizeof(ECHit), (int (*) (const void *, const void *))hit_compare);
  nhit = 0;
  while(hit[nhit].energy > threshold && nhit < NHIT && nhit < npsble) nhit++;
*/
  nhit = npsble;

  /* loop for all hits */

#ifdef DEBUG
  printf("entering loop over hits\n");
#endif


  for(ihit=0; ihit<NHIT; ihit++)
  {
    if(ihit>=nhit) continue;

    hitenergy = hit[ihit].energy;
    energysum = 0;
    hitout[ihit].energy = 0;

    for(i=0; i<3; i++) uvw[i] = hit[ihit].coord[i];
#ifdef DEBUG
    printf("\n   hit[%d] coordU=%d, coordV=%d, coordW=%d, hitenergy=%d\n",ihit,hit[ihit].coord[0],hit[ihit].coord[1],hit[ihit].coord[2],hitenergy);
#endif




#ifdef DEBUG
    printf("   entering loop over edge\n");
#endif


    /*************************/
    /* loop for 3 axis/edges */
    for(edge=0; edge<3; edge++)
    {

      peakID = hit[ihit].peak1[edge]; /* peak id, which is participates in this hit in this edge */

      fractmp = peak[edge][peakID].energy * hitfrac[edge][ihit];
#ifdef DEBUG
	  printf(">>>>>>>>> peak[][].energy=%d * hitfrac=%d = fractmp=%d\n",peak[edge][peakID].energy,hitfrac[edge][ihit],fractmp);
#endif	  







      /*********************************************/
      /* extract attenuation factor from the table */
      for(i=0; i<3; i++) uvw2[i] = (uvw[i]>>3)+1; /* convert to strip numbers to access atten table */
      dalitz = uvw2[0]+uvw2[1]+uvw2[2];
#ifdef DEBUG
	  printf("         atten: uvw2=%d %d %d dalitz=%d\n",uvw2[0],uvw2[1],uvw2[2],dalitz);
#endif	  
	 /* HUCK for now */
      if( dalitz!=73 && dalitz!=74)
	  {
        if(dalitz<73)      uvw2[2] = 73 - (uvw2[0]+uvw2[1]);
        else if(dalitz>74) uvw2[2] = 74 - (uvw2[0]+uvw2[1]);
        dalitz = uvw2[0]+uvw2[1]+uvw2[2];
#ifdef DEBUG
	    printf("         atten: HUCK !!! new uvw2=%d, now have %d %d %d, so dalitz=%d\n",uvw2[2],uvw2[0],uvw2[1],uvw2[2],dalitz);
#endif
	  }
	  /* HUCK for now */
      kk = 74 - dalitz;
      addr = uvw2[0] | (uvw2[1]<<6) | (kk<<12);
	  /*printf("uvw=%d %d %d -> addr=0x%04x\n",uvw[0],uvw[1],kk,addr);*/
	  if(edge==0)       attn = att_u[addr];
      else if(edge==1)  attn = att_v[addr];
	  else              attn = att_w[addr];
#ifdef DEBUG
      printf("         atten: u=%2d v=%2d w=%2d -> kk=%d -> addr=0x%08x -> attn=%3d\n",uvw[0],uvw[1],uvw[2],kk,addr,attn);
#endif
      /*********************************************/
      /*********************************************/




      /**********************************/
      /* correct energy for attenuation */

      fracenergy = ((fractmp*attn)>>SHIFT2)>>SHIFT1;
#ifdef DEBUG
      printf(">>>>>>>>> fracenergy=%d\n",fracenergy);
#endif
      /**********************************/
      /**********************************/

      




      energysum += fracenergy;
#ifdef DEBUG
      printf("         corrected energy: peak = %d, so far = %d\n",fracenergy,energysum);
#endif

      hitout[ihit].corren[edge] = fracenergy;
      hitout[ihit].peak1[edge] = peakID;
      hitout[ihit].peakn[edge] = peakn;
    }
#ifdef DEBUG
    printf("\n   exiting loop over edge\n\n");
#endif

    for(i=0; i<3; i++) hitout[ihit].coord[i] = uvw[i];
    hitout[ihit].energy = energysum;

#ifdef DEBUG
    printf("   result: coordU=%d, coordV=%d, coordW=%d -> hitenergy=%d, energy=%d\n",hitout[ihit].coord[0],hitout[ihit].coord[1],hitout[ihit].coord[2],hitenergy,hitout[ihit].energy);
#endif

  }





#ifdef DEBUG
  printf("exiting loop over hits\n\n\n");
#endif

  *nhits = nhit;

  return;
}


