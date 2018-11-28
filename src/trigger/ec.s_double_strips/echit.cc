/* echit - looking for hits in particular part ( inner , outer, whole )

  input:  geom   - geometry parameters
          npeak  - the number of peaks obtained
          peak   - peaks information

  output: hit    - hits information
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

using namespace std;
#include <strstream>


#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)
#define ABS(x)      ((x) < 0 ? -(x) : (x))


//#define DEBUG

#ifdef USE_PCAL

#include "../pc.s/pclib.h"
#define echit pchit

#else

#include "eclib.h"

#endif


/* some stuff for presentation */
#ifdef DEBUG
#ifdef USE_PCAL
#define ndalz ndalzPC
#define dalz  dalzPC
#endif
int ndalz;
int dalz[MAXDALZ];
#endif




/* 4.98/19/4/0%/0%/(16163)2%/(11839)3% II=4 */


void
echit(ap_uint<NBIT_DALITZ> dalitzmin, ap_uint<NBIT_DALITZ> dalitzmax, hls::stream<ECPeak_s> &s_peak_u, hls::stream<ECPeak_s> &s_peak_v, hls::stream<ECPeak_s> &s_peak_w,
      hls::stream<ECStream6_s> s_pcount[NH_FIFOS], hls::stream<ECStream16_s> s_energy[NH_FIFOS], hls::stream<ECStream10_s> s_coord[NH_FIFOS])
{
#pragma HLS INTERFACE ap_stable port=dalitzmax
#pragma HLS INTERFACE ap_stable port=dalitzmin
#pragma HLS DATA_PACK variable=s_coord
#pragma HLS INTERFACE axis register both port=s_coord
#pragma HLS ARRAY_PARTITION variable=s_coord complete dim=1
#pragma HLS DATA_PACK variable=s_energy
#pragma HLS INTERFACE axis register both port=s_energy
#pragma HLS ARRAY_PARTITION variable=s_energy complete dim=1
#pragma HLS DATA_PACK variable=s_pcount
#pragma HLS INTERFACE axis register both port=s_pcount
#pragma HLS ARRAY_PARTITION variable=s_pcount complete dim=1
#pragma HLS DATA_PACK variable=s_peak_w
#pragma HLS INTERFACE axis register both port=s_peak_w
#pragma HLS DATA_PACK variable=s_peak_v
#pragma HLS INTERFACE axis register both port=s_peak_v
#pragma HLS DATA_PACK variable=s_peak_u
#pragma HLS INTERFACE axis register both port=s_peak_u
#pragma HLS PIPELINE II=4

  uint8_t i, j, k;
  uint8_t u, v, w, ind;
  uint16_t sumadc, sumadcU, sumadcV, sumadcW;
  ap_uint<NBIT_COORD> coordU, coordV, coordW;
  uint32_t dalitz;
  ECPeak peak[3][NPEAK];
#pragma HLS ARRAY_PARTITION variable=peak complete dim=1
#pragma HLS ARRAY_PARTITION variable=peak complete dim=2


  ECPeakCount peakcountU[NPEAK];
#pragma HLS ARRAY_PARTITION variable=peakcountU complete dim=1
  ECPeakCount peakcountV[NPEAK];
#pragma HLS ARRAY_PARTITION variable=peakcountV complete dim=1
  ECPeakCount peakcountW[NPEAK];
#pragma HLS ARRAY_PARTITION variable=peakcountW complete dim=1
  ECPeakCount peakcount[NHITMAX][3];
#pragma HLS ARRAY_PARTITION variable=peakcount complete dim=1
#pragma HLS ARRAY_PARTITION variable=peakcount complete dim=2
  ECStream6_s pc_fifo;




  ap_uint<16> energy[NHITMAX][3];
#pragma HLS ARRAY_PARTITION variable=energy complete dim=1
#pragma HLS ARRAY_PARTITION variable=energy complete dim=2
  ECStream16_s en_fifo;

  ap_uint<NBIT_COORD> coord[NHITMAX][3];
#pragma HLS ARRAY_PARTITION variable=coord complete dim=1
#pragma HLS ARRAY_PARTITION variable=coord complete dim=2
  ECStream10_s coord_fifo;





#ifdef DEBUG
  printf("\n+++ echit +++\n");
  ndalz = 0;
#endif

  ECPeak_s fifo[3][NPEAK];
#pragma HLS ARRAY_PARTITION variable=fifo complete dim=1
#pragma HLS ARRAY_PARTITION variable=fifo complete dim=2

  for(int j=0; j<NPEAK; j++)
  {
	fifo[0][j] = s_peak_u.read();
	fifo[1][j] = s_peak_v.read();
	fifo[2][j] = s_peak_w.read();
  }

  for(int i=0; i<3; i++)
  {
    for(int j=0; j<NPEAK; j++)
    {
      peak[i][j].energy = fifo[i][j].energy;
      peak[i][j].coord = fifo[i][j].coord;
#ifndef __SYNTHESIS__
      peak[i][j].strip1 = fifo[i][j].strip1;
      peak[i][j].stripn = fifo[i][j].stripn;
#endif
    }
  }



  for(int i=0; i<NPEAK; i++)
  {
    peakcountU[i] = 0;
    peakcountV[i] = 0;
    peakcountW[i] = 0;
  }

  for(u=0; u<NPEAK; u++)
  {
    for(v=0; v<NPEAK; v++)
    {
	  for(w=0; w<NPEAK; w++)
      {
        sumadcU = peak[0][u].energy;
        coordU = peak[0][u].coord;

        sumadcV = peak[1][v].energy;
        coordV = peak[1][v].coord;

        sumadcW = peak[2][w].energy;
        coordW = peak[2][w].coord;

        dalitz = coordU + coordV + coordW;

#ifdef DEBUG
		if(sumadcU>0&&sumadcV>0&&sumadcW>0) cout<<"ACCEPTED ??? coordU="<<coordU<<"("<<(coordU/fview[0])<<") coordV="<<coordV<<"("<<(coordV/fview[1])<<") coordW="<<coordW<<"("<<(coordW/fview[2])<<") dalitz="<<dalitz<<"("<<(dalitz>>3)<<") (must be from "<<dalitzmin<<" to "<<dalitzmax<<")"<<endl;
#endif

        ind = IND4(u,v,w);

        /* HAVE TO CHECK THAT ENERGY IS NOT ZERO ! */
		if(sumadcU>0&&sumadcV>0&&sumadcW>0 && dalitz>=dalitzmin && dalitz<=dalitzmax)
		{
          sumadc = sumadcU + sumadcV + sumadcW;
		  
          peakcountU[u] ++;
          peakcountV[v] ++;
          peakcountW[w] ++;


          energy[ind][0] = sumadcU;
          energy[ind][1] = sumadcV;
          energy[ind][2] = sumadcW;
          coord[ind][0] = coordU;
          coord[ind][1] = coordV;
          coord[ind][2] = coordW;

#ifdef DEBUG
          cout<<"ACCEPTED !!! (u/v/w="<<+u<<" "<<+v<<" "<<+w<<") hitout: iu="<<+coordU<<" iv="<<+coordV<<" iw="<<+coordW<<" energy: U: "<<+sumadcU<<" V: "<<+sumadcV<<" W: "<<+sumadcW<<")"<<endl;
          cout<<"  peakcount: "<<peakcountU[u]<<" "<<peakcountV[v]<<" "<<peakcountW[w]<<endl;
          dalz[ndalz++] = dalitz;
#endif
		}
		else
		{
		  energy[ind][0] = 0;
		}


      }
    }
  }


  for(int ind=0; ind<64; ind++)
  {
    u = U4(ind);
    v = V4(ind);
    w = W4(ind);
    peakcount[ind][0] = peakcountU[u];
    peakcount[ind][1] = peakcountV[v];
    peakcount[ind][2] = peakcountW[w];
  }

//  hls::stream<ECStream6_s> s_pcount_fifo[NH_FIFOS];
//#pragma HLS STREAM variable=s_pcount_fifo depth=256 dim=1

  for(int i=0; i<NH_FIFOS; i++)
  {
    for(int j=0; j<NH_READS; j++)
    {
      for(int k=0; k<3; k++)
	  {
        pc_fifo.word6[k] = peakcount[i*NH_READS+j][k];
	  }
      s_pcount[i].write(pc_fifo);
//      s_pcount_fifo[i].write(pc_fifo);
    }
  }
/*
  for(int i=0; i<NH_FIFOS; i++)
  {
    if(!s_pcount_fifo[i].empty() && !s_pcount[i].full())
  	  s_pcount[i].write(s_pcount_fifo[i].read());
  }
*/


  for(int i=0; i<NH_FIFOS; i++)
  {
    for(int j=0; j<NH_READS; j++)
    {
      for(int k=0; k<3; k++)
	  {
        en_fifo.word16[k] = energy[i*NH_READS+j][k];
	  }
      s_energy[i].write(en_fifo);
    }
  }

  for(int i=0; i<NH_FIFOS; i++)
  {
    for(int j=0; j<NH_READS; j++)
    {
      for(int k=0; k<3; k++)
	  {
        coord_fifo.word10[k] = coord[i*NH_READS+j][k];
	  }
      s_coord[i].write(coord_fifo);
    }
  }






#ifdef DEBUG
  printf("\n+++ echit done +++\n\n");
#endif

}



