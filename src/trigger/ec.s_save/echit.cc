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

#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)
#define ABS(x)      ((x) < 0 ? -(x) : (x))


#undef DEBUG

#include "eclib.h"

		  
#define DALITZ_MIN (70<<3)
#define DALITZ_MAX (73<<3)

/* 3.39/41/27/0%/0%/0%/1% */

int
echit(uint8_t npeak[3], ECPeak peak[3][NPEAK], ECPeak1 peak1[3][NPEAK], ECHit hit[NHIT])
{
#pragma HLS ARRAY_PARTITION variable=hit->peakn complete dim=1
#pragma HLS ARRAY_PARTITION variable=hit->peak1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=hit->coord complete dim=1
#pragma HLS PIPELINE
  uint8_t i, j, npsble;
  uint8_t u, v, w;
  uint8_t strip1, nstrip;
  uint16_t sumadc, sumadcU, sumadcV, sumadcW;
  uint16_t coordU, coordV, coordW, dalitz;

  uint8_t peakN[3][NPEAK];
#pragma HLS ARRAY_PARTITION variable=peakN complete dim=1
  uint16_t peakE[3][NPEAK];
#pragma HLS ARRAY_PARTITION variable=peakE complete dim=1

  int npeakU, npeakV, npeakW;
  /*ECPeak peakU[NPEAK];*/
  ECPeak peakV[NPEAK];
#pragma HLS ARRAY_PARTITION variable=peakV complete dim=1
  ECPeak peakW[NPEAK];
#pragma HLS ARRAY_PARTITION variable=peakW complete dim=1

#ifdef DEBUG
  printf("\n+++ echit +++\n");
#endif


  npeakU = npeak[0];
  npeakV = npeak[1];
  npeakW = npeak[2];

  for(i=0; i<3; i++) for(j=0; j<NPEAK; j++) {peakN[i][j] = 0; peakE[i][j] = 0;}

  npsble = 0;
  for(u=0; u<NPEAK; u++)
  {
    if(u<npeakU)
	{
      sumadcU = peak[0][u].energy;
      coordU = peak[0][u].coord;

      for(v=0; v<NPEAK; v++)
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

	      for(w=0; w<NPEAK; w++)
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

              dalitz = coordU + coordV + coordW;
#ifdef DEBUG
			  printf("ACCEPTED ??? coordU=%d(%d) coordV=%d(%d) coordW=%d(%d) dalitz=%d(%d) (must be from %d to %d)\n",
					 coordU,(coordU>>3),coordV,(coordV>>3),coordW,(coordW>>3),dalitz,(dalitz>>3),DALITZ_MIN,DALITZ_MAX);
#endif
              if(dalitz>=DALITZ_MIN && dalitz<=DALITZ_MAX)
		      {
                sumadc = sumadcU + sumadcV + sumadcW;

				hit[npsble].energy = sumadc;

                hit[npsble].coord[0] = coordU;
                hit[npsble].coord[1] = coordV;
				hit[npsble].coord[2] = coordW;

                hit[npsble].peak1[0] = u;
                hit[npsble].peak1[1] = v;
                hit[npsble].peak1[2] = w;

                hit[npsble].peakn[0] = 1;
                hit[npsble].peakn[1] = 1;
                hit[npsble].peakn[2] = 1;





				/*
                peakE[0][u] += sumadc;
                peakE[1][v] += sumadc;
                peakE[2][w] += sumadc;
				*/

                peak1[0][u].hitid[ peakN[0][u] ] = npsble;
                peak1[1][v].hitid[ peakN[1][v] ] = npsble;
                peak1[2][w].hitid[ peakN[2][w] ] = npsble;

                peakN[0][u] ++;
                peakN[1][v] ++;
                peakN[2][w] ++;



#ifdef DEBUG
                printf("ACCEPTED !!! echit[%d]: iu=%d iv=%d iw=%d energy=%d (Uen=%d Ven=%d Wen=%d) (peakN %d %d %d, peakE %d %d %d)\n",
					   npsble,hit[npsble].coord[0],hit[npsble].coord[1],hit[npsble].coord[2],hit[npsble].energy,sumadcU,sumadcV,sumadcW,
					   peakN[0][u],peakN[1][v],peakN[2][w],peakE[0][u],peakE[1][v],peakE[2][w]);
#endif
                npsble++;
		      }



            }
          }

	    }
      }

	}
  }


  for(i=0; i<3; i++)
  {
    for(j=0; j<NPEAK; j++)
	{
      peak1[i][j].nhits = peakN[i][j];
      peak1[i][j].energy = peak[i][j].energy;
	}
  }

  return(npsble);
}


