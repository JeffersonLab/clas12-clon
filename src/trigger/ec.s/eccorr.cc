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

#include <iostream>
using namespace std;


#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)
#define ABS(x)      ((x) < 0 ? -(x) : (x))

//#define DEBUG

#ifdef USE_PCAL

#include "../pc.s/pclib.h"
#define eccorr pccorr

#include "../pc.s/pcal_atten.h"

#else

#include "eclib.h"
#include "ecal_atten.h"

#endif


/* some stuff for presentation */
#ifdef DEBUG
#ifdef USE_PCAL
#define ndalz2 ndalz2PC
#define dalz2  dalz2PC
#endif
int ndalz2;
int dalz2[MAXDALZ];
#endif



#ifdef USE_PCAL

typedef ap_uint<16> atten_t;

#define WIN1_LEN  8192 /* 13 bit */
/* u-68, v-62, w-62 */
static void
atten_read(int view, atten_t arrayUV[WIN1_LEN], atten_t arrayUW[WIN1_LEN], atten_t arrayVW[WIN1_LEN])
{
  int i, k, ch, kk, uvw[3];
  uint16_t att[3];
  uint16_t dalz, dmin=999, dmax=0, kkmax=0;

  for(k=0; k<WIN1_LEN; k++) arrayUV[k] = 0;
  for(k=0; k<WIN1_LEN; k++) arrayUW[k] = 0;
  for(k=0; k<WIN1_LEN; k++) arrayVW[k] = 0;

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
    att[0] = (uint16_t)(1000.* (float)FACTOR2 / (float)filecontent[i][3] / 3.0);
	att[1] = (uint16_t)(1000.* (float)FACTOR2 / (float)filecontent[i][4] / 3.0);
	att[2] = (uint16_t)(1000.* (float)FACTOR2 / (float)filecontent[i][5] / 3.0);
#endif

    k = (uvw[0]<<6) + uvw[1];
    arrayUV[k] = att[view];

    k = (uvw[0]<<6) + uvw[2];
    arrayUW[k] = att[view];

    k = (uvw[1]<<6) + uvw[2];
    arrayVW[k] = att[view];
  }
  /*printf("dmin=%d dmax=%d kkmax=%d\n",dmin,dmax, kkmax);*/
  /*exit(0);*/
}

uint16_t
pcal_coord_to_strip(uint8_t view, uint16_t jj)
{
#pragma HLS INLINE
#pragma HLS PIPELINE
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

#else

typedef ap_uint<9> atten_t;

#define WIN1_LEN  16384 /* max size available for now !!! */
static void
atten_read(int view, atten_t array[WIN1_LEN])
{
  int i, k, kk, uvw[3];
  uint16_t att[3];
#ifdef DEBUG
  int min=10000;
  int max=0;
#endif

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
#ifdef DEBUG
    if(array[k]>max) max=array[k];
    if(array[k]<min) min=array[k];
    /*printf("atten_read: %2d %2d %2d -> kk=%2d k=0x%04x -> array[0x%04x]=%d (min=%d max=%d)\n",uvw[0],uvw[1],uvw[2],kk,k,k,array[k],min,max);*/
#endif
  }
}

#endif


/*xc7vx550tffg1158-1*/



/* 3.39/11/1/36%/13%/3%/7% */


uint8_t
eccorr(unsigned short threshold, ap_uint<16> energy[NHITMAX][3], ap_uint<16> coord[NHITMAX][3], uint16_t frac[NHITMAX][3], ap_uint<16> enout[NHITMAX])
{
#pragma HLS ARRAY_PARTITION variable=energy complete dim=1
#pragma HLS ARRAY_PARTITION variable=energy complete dim=2
#pragma HLS ARRAY_PARTITION variable=coord complete dim=1
#pragma HLS ARRAY_PARTITION variable=coord complete dim=2
#pragma HLS ARRAY_PARTITION variable=frac complete dim=1
#pragma HLS ARRAY_PARTITION variable=frac complete dim=2
#pragma HLS ARRAY_PARTITION variable=enout complete dim=1
#pragma HLS PIPELINE

#ifdef USE_PCAL
  static atten_t att_u_uv[WIN1_LEN];
  static atten_t att_u_uw[WIN1_LEN];
  static atten_t att_u_vw[WIN1_LEN];
  static atten_t att_v_uv[WIN1_LEN];
  static atten_t att_v_uw[WIN1_LEN];
  static atten_t att_v_vw[WIN1_LEN];
  static atten_t att_w_uv[WIN1_LEN];
  static atten_t att_w_uw[WIN1_LEN];
  static atten_t att_w_vw[WIN1_LEN];
#else
  static atten_t att_u[WIN1_LEN];
  static atten_t att_v[WIN1_LEN];
  static atten_t att_w[WIN1_LEN];
#endif
  int i, j, kk, axis, edge, peakID, peakn;
  uint8_t ind;
  ap_uint<9> attn;
  uint16_t fracenergy;
  uint32_t frac32;
  uint16_t hitenergy;
  uint32_t energysum;
  uint16_t addr, addr1, addr2, addr3;
  uint16_t dalitz;
  uint16_t uvw[3];
#pragma HLS ARRAY_PARTITION variable=uvw complete dim=1
  uint16_t uvw2[3];
#pragma HLS ARRAY_PARTITION variable=uvw2 complete dim=1

  int peak_ID, hitID;
  uint16_t fracfrac;
  uint32_t fractmp;

  /* In order to ensure that 'coeff1' is inferred and properly
  initialized as a ROM, it is recommended that the array initialization
  be done in a sub-function with global (wrt this source file) scope. */
#ifdef USE_PCAL
  atten_read(0, att_u_uv, att_u_uw, att_u_vw);
  atten_read(1, att_v_uv, att_v_uw, att_v_vw);
  atten_read(2, att_w_uv, att_w_uw, att_w_vw);
#else
  atten_read(0, att_u);
  atten_read(1, att_v);
  atten_read(2, att_w);
#endif


#ifdef DEBUG
  printf("\n\n\n+++ eccorr +++\n\n\n");
  ndalz2 = 0;
#endif



  /* loop for all hits */

#ifdef DEBUG
  printf("entering loop over hits\n");fflush(stdout);
#endif


  for(ind=0; ind<NHITMAX; ind++)
  {

    if(energy[ind][0]==0 || energy[ind][1]==0 || energy[ind][2]==0)
	{
      enout[ind] = 0;
	}
    else
	{

      for(i=0; i<3; i++) uvw[i] = coord[ind][i];

#ifdef DEBUG
      cout<<"coord["<<+u<<"]["<<+v<<"]["<<+w<<"] coordU="<<coord[ind][0]<<", coordV="<<coord[ind][1]<<", coordW="<<coord[ind][2]<<endl;fflush(stdout);
#endif




#ifdef DEBUG
      printf("   entering loop over edge\n");fflush(stdout);
#endif



      /*************************/
      /* loop for 3 axis/edges */
      energysum = 0;
      for(edge=0; edge<3; edge++)
      {

#ifdef DEBUG
 	    cout<<"edge="<<edge<<" u/v/w="<<u<<" "<<v<<" "<<w<<endl;
	    cout<<"energy="<<energy[ind][edge]<<endl;
#endif
        fracfrac = frac[ind][edge];

#ifdef DEBUG
        cout<<"FRACFRAC["<<+edge<<"}="<<fracfrac<<endl;
#endif

        fractmp = energy[ind][edge] * fracfrac;

#ifdef DEBUG
	    cout<<">>>>>>>>> peak[][].energy="<<energy[ind][edge]<<" * fracfrac="<<fracfrac<<" = fractmp="<<fractmp<<endl;fflush(stdout);
#endif	  







        /*********************************************/
        /* extract attenuation factor from the table */

        /* convert to strip numbers to access atten table (uvw2 from 0) */
#ifdef USE_PCAL
	    for(i=0; i<3; i++) uvw2[i] = pcal_coord_to_strip(i,uvw[i]/fview[i])+1;
#else
        for(i=0; i<3; i++) uvw2[i] = (uvw[i]>>3)+1;
#endif
        dalitz = uvw2[0]+uvw2[1]+uvw2[2];
#ifdef DEBUG
	    cout<<"         atten: uvw2="<<uvw2[0]<<" "<<uvw2[1]<<" "<<uvw2[2]<<"  dalitz="<<dalitz<<endl;fflush(stdout);
        dalz2[ndalz2++] = dalitz;
#endif	  

#ifdef USE_PCAL
#else
	    /* HUCK for now */
        if( dalitz!=73 && dalitz!=74)
	    {
          if(dalitz<73)      uvw2[2] = 73 - (uvw2[0]+uvw2[1]);
          else if(dalitz>74) uvw2[2] = 74 - (uvw2[0]+uvw2[1]);
          dalitz = uvw2[0]+uvw2[1]+uvw2[2];
	    }
	    /* HUCK for now */
#endif

#ifdef USE_PCAL
        addr1 = (uvw2[0]<<6) + uvw2[1];
        addr2 = (uvw2[0]<<6) + uvw2[2];
        addr3 = (uvw2[1]<<6) + uvw2[2];
	    if(edge==0)       attn = att_u_uv[addr1] + att_u_uw[addr2] + att_u_vw[addr3];
        else if(edge==1)  attn = att_v_uv[addr1] + att_v_uw[addr2] + att_v_vw[addr3];
	    else              attn = att_w_uv[addr1] + att_w_uw[addr2] + att_w_vw[addr3];
#else
        kk = 74 - dalitz;
        addr = uvw2[0] | (uvw2[1]<<6) | (kk<<12);

	    if(edge==0)       attn = att_u[addr];
        else if(edge==1)  attn = att_v[addr];
	    else              attn = att_w[addr];
#endif
	    /*printf("uvw=%d %d %d -> addr=0x%04x\n",uvw[0],uvw[1],kk,addr);*/
#ifdef DEBUG
        cout<<"         atten: u="<<uvw[0]<<" v="<<uvw[1]<<" w="<<uvw[2]<<" -> kk="<<kk<<" -> addr="<<addr<<" -> attn="<<attn<<endl;fflush(stdout);
#endif
        /*********************************************/
        /*********************************************/




        /**********************************/
        /* correct energy for attenuation */

        frac32 = fractmp * attn;
        fracenergy = frac32 >> SHIFT1_SHIFT2;
#ifdef DEBUG
        cout<<">>>>>>>>> fracenergy="<<fracenergy<<endl;fflush(stdout);
#endif
        /**********************************/
        /**********************************/

      
        energysum += fracenergy;
#ifdef DEBUG
        cout<<"         corrected energy: peak = "<<fracenergy<<", so far = "<<energysum<<endl;fflush(stdout);
#endif

      }
#ifdef DEBUG
      printf("\n   exiting loop over edge\n\n");fflush(stdout);
#endif



      if(energysum>0)
	  {
        enout[ind] = energysum;
	  }
	  else
	  {
        enout[ind] = 0;
	  }

#ifdef DEBUG
      cout<<"   output: coordU="<<coord[ind][0]<<", coordV="<<coord[ind][1]<<", coordW="<<coord[ind][2]<<" -> energy="<<enout[ind]<<endl;fflush(stdout);
#endif


	}


  }


#ifdef DEBUG
  printf("exiting loop over hits\n\n\n");fflush(stdout);
#endif


  return(0);
}


