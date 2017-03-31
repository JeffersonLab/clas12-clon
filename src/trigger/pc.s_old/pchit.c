/* pchit - looking for hits in pcal

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

#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)
#define ABS(x)      ((x) < 0 ? -(x) : (x))


#define DEBUG


#include "pclib.h"


/* to report debugging info to the main */
int ndalz;
int dalz[MAXDALZ];

#define DALITZ_MIN 140
#define DALITZ_MAX 160

/* 3.39/41/27/0%/0%/0%/1% */

int
pchit(uint8_t npeak[3], PCPeak peak[3][PPEAK], uint16_t peakfrac[3][PPEAK], PCHit hit[PHIT])
{
#pragma HLS ARRAY_PARTITION variable=hit->peakn complete dim=1
#pragma HLS ARRAY_PARTITION variable=hit->peak1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=hit->coord complete dim=1
#pragma HLS PIPELINE
  uint16_t i, j, npsble;
  uint16_t u, v, w, u1, v1, w1;
  uint16_t strip1, nstrip;
  uint16_t sumadc, sumadcU, sumadcV, sumadcW;
  uint16_t coordU, coordV, coordW, dalitz;

  uint16_t peaktmp[3][PPEAK];
#pragma HLS ARRAY_PARTITION variable=peaktmp complete dim=1

  int npeakU, npeakV, npeakW;
  /*PCPeak peakU[PPEAK];*/
  PCPeak peakV[PPEAK];
#pragma HLS ARRAY_PARTITION variable=peakV complete dim=1
  PCPeak peakW[PPEAK];
#pragma HLS ARRAY_PARTITION variable=peakW complete dim=1

#ifdef DEBUG
  printf("\n+++ pchit +++\n");
  ndalz = 0;
#endif


  npeakU = npeak[0];
  npeakV = npeak[1];
  npeakW = npeak[2];

  for(i=0; i<3; i++) for(j=0; j<PPEAK; j++) peaktmp[i][j] = 0;

  npsble = 0;
  for(u=0; u<PPEAK; u++)
  {
    if(u<npeakU)
	{
      sumadcU = peak[0][u].energy;
      coordU = peak[0][u].coord;

      for(v=0; v<PPEAK; v++)
      {
        if(v<npeakV)
	    {
          if(u==0)
          {
            /*peakV[v].strip1 = peak[1][v].strip1;*/
			/*peakV[v].stripn = peak[1][v].stripn;*/
            peakV[v].energy = peak[1][v].energy;          
            peakV[v].coord  = peak[1][v].coord;          
          }

          sumadcV = peakV[v].energy;
          coordV = peakV[v].coord;

	      for(w=0; w<PPEAK; w++)
          {
            if(w<npeakW)
		    {
              if(u==0&&v==0)
              {
                /*peakW[w].strip1 = peak[2][w].strip1;*/
                /*peakW[w].stripn = peak[2][w].stripn;*/
                peakW[w].energy = peak[2][w].energy;          
                peakW[w].coord = peak[2][w].coord;          
              }

              sumadcW = peakW[w].energy;
              coordW = peakW[w].coord;

              u1 = coordU/fview[0];
              v1 = coordV/fview[1];
			  w1 = coordW/fview[2];

              /*dalitz = coordU + coordV + coordW;*/
              dalitz = u1 + v1 + w1;
#ifdef DEBUG
			  printf("pchit: ACCEPTED ??? coordU=%d(%d) coordV=%d(%d) coordW=%d(%d) dalitz=%d (must be from %d to %d)\n",
					 coordU,u1,coordV,v1,coordW,w1,dalitz,DALITZ_MIN,DALITZ_MAX);
#endif
              if(dalitz>=DALITZ_MIN && dalitz<=DALITZ_MAX)
		      {
                sumadc = sumadcU + sumadcV + sumadcW;

				hit[npsble].energy = sumadc;

                hit[npsble].coord[0] = u1/*coordU*/;
                hit[npsble].coord[1] = v1/*coordV*/;
				hit[npsble].coord[2] = w1/*coordW*/;

                /* peak number (from 0) */
                hit[npsble].peak1[0] = u;
                hit[npsble].peak1[1] = v;
                hit[npsble].peak1[2] = w;

                /* the number of peaks */
                hit[npsble].peakn[0] = 1;
                hit[npsble].peakn[1] = 1;
                hit[npsble].peakn[2] = 1;

                peaktmp[0][u] += sumadc;
                peaktmp[1][v] += sumadc;
                peaktmp[2][w] += sumadc;

#ifdef DEBUG
                printf("pchit: ACCEPTED !!! pchit[%d]: iu=%d iv=%d iw=%d energy=%d (%d %d %d) (sumadc's=%d %d %d)\n",
				  npsble,hit[npsble].coord[0],hit[npsble].coord[1],hit[npsble].coord[2],hit[npsble].energy,
					   peaktmp[0][u],peaktmp[1][v],peaktmp[2][w],sumadcU,sumadcV,sumadcW);
                dalz[ndalz++] = dalitz;
#endif
                npsble++;
				goto out; /* keep one peak only !!! */
		      }



            }
          }

	    }
      }

	}
  }

out:

  for(i=0; i<3; i++) for(j=0; j<PPEAK; j++) peakfrac[i][j] = peaktmp[i][j];

#ifdef DEBUG
  printf("\npchit returns %d hits\n\n",npsble);
#endif

  return(npsble);
}


