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

#ifdef USE_PCAL
#define DALITZ_MIN (155*80)
#define DALITZ_MAX (165*80)
#else
#define DALITZ_MIN (70<<3)/*(67<<3)*/
#define DALITZ_MAX (73<<3)
#endif



/* 3.49/32/1/0%/0%/~0%/1% */


int
echit(ECPeak peak[3][NPEAK], fp0201S_t peakcount[3][NPEAK], ap_uint<16> energy[NHITMAX][3], ap_uint<16> coord[NHITMAX][3])
{
#pragma HLS ARRAY_PARTITION variable=peak complete dim=1
#pragma HLS ARRAY_PARTITION variable=peak complete dim=2
#pragma HLS ARRAY_PARTITION variable=peakcount complete dim=1
#pragma HLS ARRAY_PARTITION variable=peakcount complete dim=2
#pragma HLS ARRAY_PARTITION variable=energy complete dim=1
#pragma HLS ARRAY_PARTITION variable=energy complete dim=2
#pragma HLS ARRAY_PARTITION variable=coord complete dim=1
#pragma HLS ARRAY_PARTITION variable=coord complete dim=2
#pragma HLS PIPELINE
  uint8_t i, j, k, ret;
  uint8_t u, v, w, ind;
  uint16_t sumadc, sumadcU, sumadcV, sumadcW;
  uint16_t coordU, coordV, coordW;
  uint32_t dalitz;
#ifdef USE_PCAL
  uint16_t u1d, v1d, w1d; /* for dalitz */
  uint16_t u1c, v1c, w1c; /* for coordinates */
#endif

#ifdef DEBUG
  printf("\n+++ echit +++\n");
  ndalz = 0;
#endif

  for(i=0; i<3; i++) for(j=0; j<NPEAK; j++) peakcount[i][j] = 0;

  ret = 0;
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

#ifdef USE_PCAL
        u1d = coordU;
		v1d = coordV;
		w1d = coordW;
        dalitz = u1d + v1d + w1d;

        u1c = coordU;
		v1c = coordV;
		w1c = coordW;
#ifdef DEBUG
		cout<<"ACCEPTED ??? coordU="<<coordU<<"("<<+u1d<<","<<+u1c<<") coordV="<<coordV<<"("<<+v1d<<","<<+v1c<<") coordW="<<coordW<<"("<<+w1d<<","<<+w1c<<") dalitz="<<+dalitz<<" (must be from "<<DALITZ_MIN<<" to "<<DALITZ_MAX<<")"<<endl;
#endif
#else
        dalitz = coordU + coordV + coordW;
#ifdef DEBUG
		cout<<"ACCEPTED ??? coordU="<<coordU<<"("<<(coordU>>3)<<") coordV="<<coordV<<"("<<(coordV>>3)<<") coordW="<<coordW<<"("<<(coordW>>3)<<") dalitz="<<dalitz<<"("<<(dalitz>>3)<<") (must be from "<<DALITZ_MIN<<" to "<<DALITZ_MAX<<")"<<endl;
#endif
#endif

        ind = IND4(u,v,w);

        /* HAVE TO CHECK THAT ENERGY IS NOT ZERO ! */
		if(sumadcU>0&&sumadcV>0&&sumadcW>0 && dalitz>=DALITZ_MIN && dalitz<=DALITZ_MAX)
		{
          sumadc = sumadcU + sumadcV + sumadcW;
		  
          peakcount[0][u] ++;
          peakcount[1][v] ++;
          peakcount[2][w] ++;


          energy[ind][0] = sumadcU;
          energy[ind][1] = sumadcV;
          energy[ind][2] = sumadcW;
          coord[ind][0] = coordU;
          coord[ind][1] = coordV;
          coord[ind][2] = coordW;

          ret = 1;

#ifdef DEBUG
          cout<<"ACCEPTED !!! (u/v/w="<<+u<<" "<<+v<<" "<<+w<<") hitout: iu="<<+coordU<<" iv="<<+coordV<<" iw="<<+coordW<<" energy: U: "<<+sumadcU<<" V: "<<+sumadcV<<" W: "<<+sumadcW<<")"<<endl;
          cout<<"  peakcount: "<<peakcount[0][u]<<" "<<peakcount[1][v]<<" "<<peakcount[2][w]<<endl;
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


#ifdef DEBUG
  printf("\n+++ echit done +++\n\n");
#endif


  return(ret);
}



