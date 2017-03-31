
/* hitpattern.c - generates segments for the CLAS12 Drift Chamber superlayer */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define ABS(x) ((x) < 0 ? -(x) : (x))

/*#define USE_ROOT*/

#define munique 500 /* maximum number of the unique hit pattern matrices */
#define munique6 500 /* maximum number of the unique 6-hit patterns */
#define mstepx0 80 /* number of steps in x at "bottom" */
#define mstepxl 960 /* number of steps in x at "top" */
#define mcombo (mstepx0*mstepxl) /* number of tracks */
#define degperrad 57.29578
#define mwires 21 /* the number of wires we are considering (gorizontally) */
#define cwire 10 /* 'central' wire (will be wire '0' in reported segments) */

static float x0array[mcombo]; /* entry coordinates array */
static float xlarray[mcombo]; /* exit coordinates array */

static float xhitarray[mwires][6]; /* cell centers x-coord array */
static float yhitarray[mwires][6]; /* cell centers y-coord array */
static float xcellarray[mwires][6][7]; /* cell corners y-coord array */
static float ycellarray[mwires][6][7]; /* cell corners y-coord array */
static int ihitarray[mwires][6][mcombo]; /* hit array */
static float dhitarray[mwires][6][mcombo]; /* distance hit array */

#define YSHIFT 16. /* y distance between bottom and upper drawings */


#ifdef USE_ROOT
/* draw one segment from generated hit array */
int
drawpattern(int handler, TCanvas *fCanvas, int itrack, float sxy, float x0, float y0, float T0)
{
  int i,j,k,ibgrd,ired,igreen,iblue,ipurp,ilblue;
  float xmin, ymin, xmax, ymax, degrad;
  float x00, y00, xtrack[2], ytrack[2], xcell[7], ycell[7];
  float xpm[2], ypm[2], dpm[2], dlong[2], dshort[2];
  float yin, yout;
  float rcell = 1.; /* cell radius */
  float tracksin, trackcos, trackangle, layershift[6];

  sxy = 0.5;
  printf("drawpattern reached, sxy=%f, x0=%f, y0=%f\n",sxy,x0,y0);
  /* Set up a view box for the convenience of hard_copy output */
  if(fCanvas>0)
  {
    xmin = ymin = 0.;
    xmax = ymax = 20.;
    ibgrd  = 1;
    ired   = 2;
    igreen = 3;
    iblue  = 4;
    ipurp  = 6;
    ilblue = 7;
    degrad = acos(-1.)/180.;

    ISPMCI(ibgrd);
    ISPLCI(ibgrd);
    IGBOX(0.,xmax,0.05,0.77*ymax);


    /***********************/
    /* 'draw 'normal' view */

    x00 = x0;
    y00 = y0;
    yin = 0.;
    yout = 8.5;

    /* draw bottom cells */
    for(i=0; i<mwires; i++)
    {
      for(j=0; j<6; j++)
      {
        for(k=0; k<7; k++)
	{
          xcell[k] = (x00 + xcellarray[i][j][k])*sxy;
          ycell[k] = (y00 + ycellarray[i][j][k])*sxy;
	}
        IPL(7,xcell,ycell);
      }
    }
    
    /* draw track */
    i = itrack;
    printf("track number %5d (x0=%f xl=%f)\n",i+1,x0array[i],xlarray[i]);
    for(j=5; j>=0; j--)
    {
      printf("layer %d>",j+1);
      for(k=0; k<mwires; k++)
      {
        printf("%2d",ihitarray[k][j][i]);
      }
      printf("\n");
    }

    ytrack[0] = (yin+y00)*sxy;
    ytrack[1] = (yout+y00)*sxy;
    xtrack[0] = (x0array[i]+x00)*sxy;
    xtrack[1] = (xlarray[i]+x00)*sxy;
    ISPLCI(ired);
    IPL(2,xtrack,ytrack);
    ISPLCI(ibgrd);

    /* draw circles for hits */
    ISPMCI(ired);
    for(j=0; j<mwires; j++)
    {
      for(k=0; k<6; k++)
      {
        if(ihitarray[j][k][i])
	{
          xpm[0] = (x00 + xhitarray[j][k])*sxy;
          ypm[0] = (y00 + yhitarray[j][k])*sxy;	  
	  dpm[0] = (dhitarray[j][k][i])*sxy;
          IPM_DC(1,xpm,ypm,dpm);
	}
      }
    }
    ISPMCI(ibgrd);



    /*************************************************/
    /* 'shifted' view - draw just above the 'normal' */

    x00 = x0;
    y00 = y0 + YSHIFT;

    /* draw upper cells */
    for(i=0; i<mwires; i++)
    {
      for(j=0; j<6; j++)
      {
        for(k=0; k<7; k++)
	{
          xcell[k] = (x00 + xcellarray[i][j][k])*sxy;
          ycell[k] = (y00 + ycellarray[i][j][k])*sxy;
	}
        IPL(7,xcell,ycell);
      }
    }

    /* draw track */
    i = itrack;
    printf("track number %5d (x0=%f xl=%f)\n",i+1,x0array[i],xlarray[i]);
    for(j=5; j>=0; j--)
    {
      printf("layer %d>",j+1);
      for(k=0; k<mwires; k++)
      {
        printf("%2d",ihitarray[k][j][i]);
      }
      printf("\n");
    }


    /* original track
    ytrack[0] = (yin+y00)*sxy;
    ytrack[1] = (yout+y00)*sxy;
    xtrack[0] = (x0array[i]+x00)*sxy;
    xtrack[1] = (xlarray[i]+x00)*sxy;
    ISPLCI(ired);
    IPL(2,xtrack,ytrack);
    ISPLCI(ibgrd);
*/

    /* track angle */
    trackangle = atan((xtrack[1]-xtrack[0])/(ytrack[1]-ytrack[0]));
    printf("dx=%f %f dy=%f %f angle=%f\n",xtrack[1],xtrack[0],ytrack[1],ytrack[0],
      degperrad*trackangle);

    printf("90=%f\n",90./degperrad);

    /* shifted track */
    ytrack[0] = (yin+y00)*sxy;
    ytrack[1] = (yout+y00)*sxy;
    xtrack[0] = (x0array[i]+x00)*sxy;
    xtrack[1] = (x0array[i]/*xlarray[i]*/+x00)*sxy;
    ISPLCI(iblue);
    IPL(2,xtrack,ytrack);
    ISPLCI(ibgrd);


    /* shift for every layer (distance between layers is 1.5) */
    layershift[0] = 0.5*tan(trackangle);
    layershift[1] = 2.0*tan(trackangle);
    layershift[2] = 3.5*tan(trackangle);
    layershift[3] = 5.0*tan(trackangle);
    layershift[4] = 6.5*tan(trackangle);
    layershift[5] = 8.0*tan(trackangle);

    printf("layershift --> %f %f %f %f %f %f\n",
	   layershift[0],
	   layershift[1],
	   layershift[2],
	   layershift[3],
	   layershift[4],
	   layershift[5]);


    /* circles for shifted hits */
    ISPMCI(ired);
    ISPLCI(iblue);
    for(j=0; j<mwires; j++)
    {
      for(k=0; k<6; k++)
      {
        if(ihitarray[j][k][i])
	{
	  
	  dlong[0] = (dhitarray[j][k][i] / sin(1.570796-trackangle) )*sxy;
	  dshort[0] = dhitarray[j][k][i]*sxy;
          ypm[0] = (y00 + yhitarray[j][k])*sxy;
          xpm[0] = (x00 + xhitarray[j][k] - layershift[k])*sxy;
          IPM_ELLIPSE(1,xpm,ypm,dlong,dshort);
          
	  /*
	  dpm[0] = dhitarray[j][k][i] / sin(1.570796-trackangle));
          ypm[0] = (y00 + yhitarray[j][k])*sxy;
          ypm[1] = ypm[0]; 
          xpm[0] = (x00 + xhitarray[j][k] - layershift[k] - dpm[0])*sxy;
          xpm[1] = (x00 + xhitarray[j][k] - layershift[k] + dpm[0])*sxy;
	  printf("= %d %d - x=%f %f , y=%f %f\n",j,k,xpm[0],xpm[1],ypm[0],ypm[1]);
          IPL(2,xpm,ypm);
	  */
	}
      }
    }
    ISPMCI(ibgrd);
    ISPLCI(ibgrd);




    ISPMCI(ibgrd);
    ISPLCI(ibgrd);
    REDRAW;
  }

  return(0);
}
#endif /*USE_ROOT*/


static int
segm6compare(const void *segm1, const void *segm2)
{
  int *p1 = (int *)segm1;
  int *p2 = (int *)segm2;
  int k, itmp;


  
  for(k=0; k<6; k++)
  {
    if(p1[k]>p2[k]) return(1);
    if(p1[k]<p2[k]) return(-1);
  }
  

  /*
  itmp = 0;
  for(k=0; k<6; k++) itmp += (p1[k]-p2[k]);
  if(itmp>0)  return(1);
  if(itmp<0)  return(-1);
  */

  return(0);
}


static int ihitunique[mwires][6][munique];

int
hitpattern()
{
  int match, matrixmatch, delta;
  int itrkx0, itrkxl; /*iteration counters for track x-position at "bottom" and "top"*/
  int itrkno, icombo, iunique, nunique; /* track number counter */
  int isenselayer, isensewire; /* counters for sense layers and sense wires in each layer */
  int icnt[munique], nhits[munique];
  int n1[6], nn[6], nnn, l1, l2, l3, l4, l5, l6;
  int exist, nsegment6, segment6[munique6][6];
  /* track positions at bottom,top, and above and below each layer */
  float x0, xl, y0, yl, ylower, yupper, xlower, xupper, x00, y00;
  /* hexcell parameters */
  float rcell, dely, delx;
  /* temporary hit wire integer */
  int isenselower, isenseupper;
  /* utility integer counters */
  int i, j, k;
  /* utility numbers */
  float thdeg[mcombo];
  float thdegunique[munique];
  float degrad;

  degrad = acos(-1.)/180.;

  /* hexcell parameters */
  rcell = 1.; /*cell radius (from center to corner) */
  dely = 1.5; /*1.+sin(30deg)*/
  delx = 1.732050808; /*2.*cos(30deg)*/

  /* zero track number and initialize starting and final y-values */
  itrkno = 0;
  y0 = 0.;
  yl = 8.5;

  /* calculate and fill cell corners and centers arrays */
  for(j=0; j<6; j++)
  {
    y00 = - rcell*sin(30.*degrad) + j*rcell*(1.+sin(30.*degrad));
    for(i=0; i<mwires; i++)
    {
      x00 = rcell*cos(30.*degrad) + i*2*rcell*cos(30.*degrad);
      if(j%2) x00 -= rcell*cos(30.*degrad);
      xhitarray[i][j] = x00;
      yhitarray[i][j] = y00+rcell;

      xcellarray[i][j][0] = xcellarray[i][j][6] = x00;
      ycellarray[i][j][0] = ycellarray[i][j][6] = y00;
      xcellarray[i][j][1] = x00 + rcell*cos(30.*degrad);
      ycellarray[i][j][1] = y00 + rcell*sin(30.*degrad);
      xcellarray[i][j][2] = xcellarray[i][j][1];
      ycellarray[i][j][2] = ycellarray[i][j][1] + rcell;
      xcellarray[i][j][3] = x00;
      ycellarray[i][j][3] = y00 + 2.*rcell;
      xcellarray[i][j][4] = x00 - rcell*cos(30.*degrad);
      ycellarray[i][j][4] = ycellarray[i][j][2];
      xcellarray[i][j][5] = xcellarray[i][j][4];
      ycellarray[i][j][5] = ycellarray[i][j][1];
    }
  }

  /******************************/
  /* loop over input x position */
  for(itrkx0=1; itrkx0<=mstepx0; itrkx0++)
  {
    /* calculate x starting position of track at "y=0" */
    x0 = cwire*delx + ((float)(itrkx0-1))/((float)(mstepx0))*delx;
      
    /* loop over output x position */   
    for(itrkxl=1; itrkxl<=mstepxl; itrkxl++)
    {
      xl = x0 - 6.*delx + ((float)(itrkxl-1))/((float)(mstepxl))*12.*delx;

      /* iterate track number */
      itrkno ++;

      /* remember track coordinates */
      x0array[itrkno-1] = x0;
      xlarray[itrkno-1] = xl;
       
      /* initialize the ihitarray to all zero's */
      for(i=0; i<mwires; i++)
      {
        for(j=0; j<6; j++)
	    {
	      ihitarray[i][j][itrkno-1] = 0;
	      dhitarray[i][j][itrkno-1] = 0.0;
	    }
      }

      /* calculate the angle of the track */
      thdeg[itrkno-1] = degperrad*atanf((xl-x0)/(yl-y0));



      /* loop over layers; calc. x,y of track at lower and upper layer boundary */
      for(isenselayer=1; isenselayer<=6; isenselayer++)
      {
        ylower = 0.+((float)(isenselayer-1))*1.5;
        yupper = ylower+1.;
        xlower = x0+(ylower-y0)/yl*(xl-x0);
        xupper = x0+(yupper-y0)/yl*(xl-x0);

	    /* now account for 'brick-wall' structure */
	    if (isenselayer==2||isenselayer==4||isenselayer==6)
	    {
	      xlower = xlower + delx/2.;
	      xupper = xupper + delx/2.;
	    }

        /* calculate which sense wire cell the track is in at the lower boundary */
	    isenselower = (int)(xlower/delx) + 1;
        /* calculate which sense wire cell the track is in at the upper boundary */
	    isenseupper = (int)(xupper/delx) + 1;
        /* update the hit array for this track with the wires hit */
	    /*printf("---------> %d %d\n",isenselower,isenseupper);*/
	    if(isenselower>(cwire-7) && isenselower<(cwire+7))
	    {
	      if(isenseupper>(cwire-7) && isenseupper<(cwire+7))
	      {
	        ihitarray[isenselower-1][isenselayer-1][itrkno-1] = 1;
            x00 = xhitarray[isenselower-1][isenselayer-1]; /* wire X coord */
            y00 = yhitarray[isenselower-1][isenselayer-1]; /* wire Y coord */
            dhitarray[isenselower-1][isenselayer-1][itrkno-1] =
                   ABS((xl-x0)*(y0-y00)-(yl-y0)*(x0-x00))/
                   sqrt((xl-x0)*(xl-x0)+(yl-y0)*(yl-y0));
	    
	        ihitarray[isenseupper-1][isenselayer-1][itrkno-1] = 1;
            x00 = xhitarray[isenseupper-1][isenselayer-1];
            y00 = yhitarray[isenseupper-1][isenselayer-1];
            dhitarray[isenseupper-1][isenselayer-1][itrkno-1] =
                   ABS((xl-x0)*(y0-y00)-(yl-y0)*(x0-x00))/
                   sqrt((xl-x0)*(xl-x0)+(yl-y0)*(yl-y0));
	      }
	    }
      } /* loop over layers to find track-wire intersection */



    } /* loop over track exit point */
  } /* loop over track entrance point */

  /*
  printf("Found %4d patterns\n",mcombo);
  for(i=0; i<mcombo; i++)
  {
    printf("track number %5d (x0=%f xl=%f)\n",i+1,x0array[i],xlarray[i]);
    for(j=5; j>=0; j--)
    {
      printf("layer %d>",j+1);
      for(k=0; k<mwires; k++)
      {
        printf("%2d",ihitarray[k][j][i]);
      }
      printf("\n");
    }
  }
  */






  /***************************************************************************/
  /* now look for unique combinations and count number of occurences of each */

  /* first, zero out the counters */
  for(i=0; i<munique; i++) icnt[i] = 0;
  nunique = 1;

  /* fill the first unique entry "by hand" */
  for(i=0; i<mwires; i++)
  {
    for(j=0; j<6; j++)
    {
      ihitunique[i][j][nunique-1] = 0;
      if(i==cwire) ihitunique[i][j][nunique-1] = 1;
    }
  }

  /* fill in the number of hits "by hand" for the 1st entry */
  nhits[nunique-1] = 6;

  /* now loop over all track combo's, keeping all unique combo's */
  for(icombo=1; icombo<=mcombo; icombo++)
  {
    /* assume match is false unless we find a match */
    match = 0;

    for(iunique=1; iunique<=nunique; iunique++)
    {
      /* compare element by element; assume the matrix does match */
      matrixmatch = 1;
      for(i=0; i<mwires; i++)
      {
        for(j=0; j<6; j++)
        {
	  /* matrixmatch is false if anything disagrees */
	  if(ihitarray[i][j][icombo-1] != ihitunique[i][j][iunique-1]) matrixmatch = 0;
        }
      }

      /* we found a match for this value of icombo and iunique */
      if(matrixmatch == 1)
      {
	    match = 1;
	    icnt[iunique-1]=icnt[iunique-1]+1;
      }
    }  /* loop over all unique combos */

    /* if no matches we found a new unique candidate; fill it in */
    if(match == 0)
    {
      nunique ++;

      /* assign theta value for this unique combo */
      thdegunique[nunique-1]=thdeg[icombo-1];

      nhits[nunique-1] = 0;
      for(i=0; i<mwires; i++)
      {
        for(j=0; j<6; j++)
        {
	      ihitunique[i][j][nunique-1] = ihitarray[i][j][icombo-1];
	      nhits[nunique-1] = nhits[nunique-1] + ihitarray[i][j][icombo-1];
	    }
      }
    }

  } /* loop over all track combos */

  
  printf("Found %4d patterns\n",nunique);
  for(i=0; i<nunique; i++)
  {
    printf("pattern #%6d # entries%6d angle%6.2f # hits %4d\n",
       i+1,icnt[i],thdegunique[i],nhits[i]);
    for(j=5; j>=0; j--)
    {
      printf("layer %d>",j+1);
      for(k=0; k<mwires; k++)
      {
        printf("%2d",ihitunique[k][j][i]);
      }
      printf("\n");
    }
  }
  


  /**************************/
  /* extract 6-hit patterns */

  nsegment6 = 0;
  for(i=0; i<nunique; i++)
  {
    printf("pattern #%6d, #hits %4d\n",i+1,nhits[i]);
    for(j=5; j>=0; j--)
    {
      nn[j]=0;
      n1[j]=-1;
      printf("layer %1d, hits: ",j+1);
      for(k=0; k<mwires; k++)
      {
        if(ihitunique[k][j][i])
	{
          if(n1[j]<0) n1[j] = k;
          nn[j]++;
	}
        printf("%2d",ihitunique[k][j][i]);
      }
      printf(" -> %1d hits, 1st hit is %d\n",nn[j],n1[j]);
    }
    printf("\n");

    /**/
    printf("6-hit patterns:\n");
    nnn=0;
    for(l1=0; l1<nn[0]; l1++)
    {
      for(l2=0; l2<nn[1]; l2++)
      {
        for(l3=0; l3<nn[2]; l3++)
	    {
          for(l4=0; l4<nn[3]; l4++)
	      {
            for(l5=0; l5<nn[4]; l5++)
	        {
              for(l6=0; l6<nn[5]; l6++)
              {
                printf("[%2d] %2d %2d %2d %2d %2d %2d\n",nnn,
                  n1[0]+l1,n1[1]+l2,n1[2]+l3,n1[3]+l4,n1[4]+l5,n1[5]+l6);

                /*if segment is unique, add it to the list*/
                exist = 0;
                for(j=0;j<nsegment6; j++)
		        {
                  if(segment6[j][0]==n1[0]+l1 &&
                     segment6[j][1]==n1[1]+l2 &&
                     segment6[j][2]==n1[2]+l3 &&
                     segment6[j][3]==n1[3]+l4 &&
                     segment6[j][4]==n1[4]+l5 &&
                     segment6[j][5]==n1[5]+l6)
                  {
                    printf("Segment exist already\n");
                    exist = 1;
                    break;
                  }
		        }
                if(!exist)
		        {
                  segment6[nsegment6][0]=n1[0]+l1;
                  segment6[nsegment6][1]=n1[1]+l2;
                  segment6[nsegment6][2]=n1[2]+l3;
                  segment6[nsegment6][3]=n1[3]+l4;
                  segment6[nsegment6][4]=n1[4]+l5;
                  segment6[nsegment6][5]=n1[5]+l6;
		          nsegment6++;
		        }

                nnn++;
              }
	        }
	      }
	    }
      }
    }
    printf("\n\n");
  }

  /* normalize segments to cwire */
  for(j=0; j<nsegment6; j++)
  {
    for(k=0; k<6; k++) segment6[j][k] -= cwire;
  }

  /* normalize to zero in first layer */
  for(j=0; j<nsegment6; j++)
  {
    delta = segment6[j][0];
    for(k=0; k<6; k++) segment6[j][k] -= delta;
  }

  printf("\n\nList of 6-hit segments:\n");
  for(j=0; j<nsegment6; j++)
  {
    for(k=0; k<6; k++) printf("%2d",segment6[j][k]);
    printf("  # segm %2d\n",j);
  }
  printf("\n\n");

  /* sort segments */
  qsort((void *)segment6, nsegment6, 6*sizeof(int), segm6compare);
  printf("\n\nList of 6-hit segments (sorted):\n");
  for(j=0; j<nsegment6; j++)
  {
    for(k=0; k<6; k++) printf("%2d",segment6[j][k]);
    printf("  # segm %2d\n",j);
  }
  printf("\n\n");

  /*remove duplicate segments*/
  for(j=1; j<nsegment6; j++)
  {
    if(segment6[j][0]==segment6[j-1][0]&&
       segment6[j][1]==segment6[j-1][1]&&
       segment6[j][2]==segment6[j-1][2]&&
       segment6[j][3]==segment6[j-1][3]&&
       segment6[j][4]==segment6[j-1][4]&&
       segment6[j][5]==segment6[j-1][5])
	{
      printf(" [%d] duplicate segment\n",j);
      for(k=j; k<nsegment6-1; k++)
        for(i=0; i<6; i++)
          segment6[k][i]=segment6[k+1][i];
      nsegment6 --;
      j--; /* look again on the same segment, it can be 3 or more identical segments ! */
	}
  }
  printf("\n\nList of 6-hit segments (duplicates removed):\n");
  for(j=0; j<nsegment6; j++)
  {
    for(k=0; k<6; k++) printf("%2d",segment6[j][k]);
    printf("  # segm %2d\n",j);
  }
  printf("\n\n");


  /*file for *.c
  {
    FILE *ff = fopen("segmdict_hitpattern.txt","w");
    fprintf(ff,"SEGM_DICT %d\n",nsegment6);
    for(k=0; k<nsegment6; k++)
	{
      fprintf(ff,"%2d %2d %2d %2d %2d %2d\n",
        segment6[k][0],segment6[k][1],segment6[k][2],
        segment6[k][3],segment6[k][4],segment6[k][5]);
	}
    fclose(ff);
  }
  */

  /*header file*/
  {
    FILE *ff = fopen("segmdict.h","w");
    fprintf(ff,"#define MAXSEGM %d\n",nsegment6);
    fprintf(ff,"int8_t segment6[MAXSEGM][6] = {\n");
    for(k=0; k<nsegment6; k++)
	{
      fprintf(ff,"/*%3d*/ %2d, %2d, %2d, %2d, %2d, %2d,\n",k,
        segment6[k][0],segment6[k][1],segment6[k][2],
        segment6[k][3],segment6[k][4],segment6[k][5]);
	}
    fprintf(ff,"};\n");
    fclose(ff);
  }

  return(0);
}
