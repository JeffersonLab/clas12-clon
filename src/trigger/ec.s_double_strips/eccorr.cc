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
#define eccorr_1 pccorr_1

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
atten_read(uint8_t view, atten_t arrayUV[WIN1_LEN], atten_t arrayUW[WIN1_LEN], atten_t arrayVW[WIN1_LEN])
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


/* take one of two strips */
uint16_t
pcal_coord_to_strip(ap_uint<2> view, uint16_t jj)
{
#pragma HLS INLINE
#pragma HLS PIPELINE
 uint16_t ii;

  if(view==0)
  {
    if(jj <= 25) ii = jj*2;
    if(jj >  25) ii = jj+26;
  }
  else if(view==1)
  {
    if(jj <= 15) ii = jj;
    if(jj >  15) ii = jj + (jj-15);
  }
  else if(view==2)
  {
    if(jj <= 15) ii = jj;
    if(jj >  15) ii = jj + (jj-15);
  }
  return(ii);
}

#else

typedef ap_uint<9> atten_t;

#define WIN1_LEN  16384 /* max size available for now !!! */
static void
atten_read(uint8_t view, atten_t array[WIN1_LEN])
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




#ifndef __SYNTHESIS__
  static int first = 1;
#endif


/* 3.39/11/1/1%/~0%/~0%/~0% II=1 */

void
eccorr_1(ap_uint<16> hit_threshold, ap_uint<16> energy[3], ap_uint<NBIT_COORD> coord[3], uint16_t frac[3], uint16_t enpeak[3])
{

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
  uint16_t hitenergy, fracenergy;
  uint32_t frac32;
  uint16_t addr, addr1, addr2, addr3;
  uint16_t dalitz;
  ap_uint<NBIT_COORD> uvw[3];
#pragma HLS ARRAY_PARTITION variable=uvw complete dim=1
  uint16_t uvw2[3];
#pragma HLS ARRAY_PARTITION variable=uvw2 complete dim=1

  int peak_ID, hitID;
  uint16_t fracfrac;
  uint32_t fractmp;

#ifndef __SYNTHESIS__
  if(first)
  {
	first = 0; 
#endif

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

#ifndef __SYNTHESIS__
  }
#endif

#ifdef DEBUG
  printf("\n\n+++ eccorr +++\n\n");
  ndalz2 = 0;
#endif


  if(energy[0]==0 || energy[1]==0 || energy[2]==0)
  {
    enpeak[0] = 0;
    enpeak[1] = 0;
    enpeak[2] = 0;
  }
  else
  {
    for(i=0; i<3; i++) uvw[i] = coord[i];
#ifdef DEBUG
    cout<<"coord["<<+ind<<"] coordU="<<coord[0]<<", coordV="<<coord[1]<<", coordW="<<coord[2]<<endl;fflush(stdout);
#endif

    /*************************/
    /* loop for 3 axis/edges */
    hitenergy = 0;
    for(edge=0; edge<3; edge++)
    {

#ifdef DEBUG
 	  cout<<"edge="<<edge<<" ind="<<+ind<<endl;
	  cout<<"energy="<<energy[edge]<<endl;
#endif
      fracfrac = /*256*/frac[edge]; /* sergey: use 256 if frac1/2/3 does not work ! */

#ifdef DEBUG
      cout<<"FRACFRAC["<<+edge<<"]="<<fracfrac<<endl;
#endif

      fractmp = energy[edge] * fracfrac;

#ifdef DEBUG
	  cout<<">>>>>>>>> peak[][].energy="<<energy[edge]<<" * fracfrac="<<fracfrac<<" = fractmp="<<fractmp<<endl;fflush(stdout);
#endif	  



      /*********************************************/
      /* extract attenuation factor from the table */

      /* convert to strip numbers to access atten table (uvw2 from 0) */
#ifdef USE_PCAL
	  for(i=0; i<3; i++) uvw2[i] = pcal_coord_to_strip(i, ((uint32_t)uvw[i])/fview[i] ) + 1;
#else
      for(i=0; i<3; i++) uvw2[i] = (uvw[i]>>3) + 1;
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

      enpeak[edge] = fracenergy;
      hitenergy += fracenergy;
#ifdef DEBUG
      cout<<"         corrected energy: peak = "<<fracenergy<<endl;fflush(stdout);
#endif

    }

    if(hitenergy<hit_threshold)
	{
      enpeak[0] = 0;
      enpeak[1] = 0;
      enpeak[2] = 0;
	}

#ifdef DEBUG
    cout<<"   output: coordU="<<coord[0]<<", coordV="<<coord[1]<<", coordW="<<coord[2]<<" -> energy="<<enpeak[0]<<" "<<enpeak[1]<<" "<<enpeak[2]<<endl;fflush(stdout);
#endif

  }


}





/* 5.59/15/ 4/(432)18%/(144)5%/(29080)4%/(14488)4% II=4 */

void
eccorr(ap_uint<16> hit_threshold, hls::stream<ECStream16_s> s_energy2[NH_FIFOS], hls::stream<ECStream10_s> s_coord1[NH_FIFOS],
	   hls::stream<ECStream16_s> s_frac[NH_FIFOS], hls::stream<ECStream16_s> s_enpeak[NH_FIFOS])
{
#pragma HLS DATA_PACK variable=s_enpeak
#pragma HLS INTERFACE axis register both port=s_enpeak
#pragma HLS DATA_PACK variable=s_frac
#pragma HLS INTERFACE axis register both port=s_frac
#pragma HLS DATA_PACK variable=s_coord1
#pragma HLS INTERFACE axis register both port=s_coord1
#pragma HLS DATA_PACK variable=s_energy2
#pragma HLS INTERFACE axis register both port=s_energy2
#pragma HLS INTERFACE ap_stable port=hit_threshold
#pragma HLS ARRAY_PARTITION variable=s_energy2 complete dim=1
#pragma HLS ARRAY_PARTITION variable=s_coord1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=s_frac complete dim=1
#pragma HLS ARRAY_PARTITION variable=s_enpeak complete dim=1
#pragma HLS PIPELINE II=4

  int ind;
  ap_uint<16> energy[3];
  ap_uint<NBIT_COORD> coord[3];
  uint16_t frac[3];
  uint16_t enpeak[3];

  ECStream16_s en_fifo;
  ECStream10_s coord_fifo;
  ECStream16_s frac_fifo;
  ECStream16_s enpeak_fifo;


  for(int i=0; i<NH_FIFOS; i++) /* read each fifo's 8 times */
  {
    for(int j=0; j<NH_READS; j++) /* every fifo slice contains 8 hits */
    {
	  en_fifo = s_energy2[i].read();
      coord_fifo = s_coord1[i].read();
      frac_fifo = s_frac[i].read();

	  for(int k=0; k<3; k++) energy[k] = en_fifo.word16[k];
      for(int k=0; k<3; k++) coord[k] = coord_fifo.word10[k];
      for(int k=0; k<3; k++) frac[k] = frac_fifo.word16[k];

      eccorr_1(hit_threshold, energy, coord, frac, enpeak);
      /*cout<<"eccorr["<<ind<<"]: enpeak="<<enpeak[ind][0]<<" "<<enpeak[ind][1]<<" "<<enpeak[ind][2]<<endl;*/

      for(int k=0; k<3; k++) enpeak_fifo.word16[k] = enpeak[k];
      s_enpeak[i].write(enpeak_fifo);
    }
  }


}
