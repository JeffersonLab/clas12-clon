/*
   dcdsectEEL.cc - draw sector geometry, tracks and hits

   Inputs  : isec  - sector number
             x0,y0 - position in HIGZ window of size 20x20 cm2

   usage:
    cd $CLON/src/trigger/cmroot/
    ./Linux_x86_64/bin/segmdict /work/boiarino/data/clas_005662.evio.00001

*/


#define DEBUG


#define NOFFSETS 16
#define NWIRES 112

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <termios.h>
#include <unistd.h>


#include "TROOT.h"
#include "TApplication.h"
#include "TGTableLayout.h"
#include "TGFrame.h"
#include "TList.h"
#include "TGWindow.h"
#include "TRootEmbeddedCanvas.h"
#include "TArc.h"
#include "TEllipse.h"
#include "TBox.h"
#include "TArrow.h"
#include "TPolyLine.h"
#include "TPolyMarker.h"
#include "TGButton.h"
#include "TCanvas.h"
#include "TRandom.h"
#include "TGDoubleSlider.h"
#include "TGaxis.h"
#include "Riostream.h"
#include "TText.h"
#include "segmdict.h"

#include "evio.h"
#include "evioBankUtil.h"
#include "trigger.h"

#include "prlib.h"
#include "dclib.h"
#include "sdageom.h"

#include "sgutilold.h"

#define ABS(x) ((x) < 0 ? -(x) : (x))

#define nsup          6
#define nlay          6
#define max_nwire   194
#define max_nhit 36*192
#define USE_ROOT


static TText t(0,0,"a");
static float ticksize = 0.0;
static int polyline_color_index = 0;
static int polymarker_color_index = 0;
static int marker_type = 0;

#define IGAXIS(x0,x1,y0,y1,wmin,wmax,ndiv,chopt) \
{ \
  TGaxis *fAxis = new TGaxis(x0,y0, x1,y1, wmin,wmax, ndiv, chopt); \
  if(ticksize>0.0) fAxis->SetTickSize(ticksize); /*set by IGSET*/ \
  fAxis->Draw();											  \
}

/*polyline color index*/
#define ISPLCI(icoli) \
{ \
  polyline_color_index = icoli; \
}

/*polymarker color index*/
#define ISPMCI(icoli) \
{ \
  polymarker_color_index = icoli; \
}

/*marker type*/
#define ISMK(mtype) \
{ \
  marker_type = mtype; \
}

#define IGBOX(x_min, x_max, y_min, y_max) \
{ \
  Float_t x1 = x_max; \
  Float_t y1 = y_min; \
  Float_t x2 = x_min; \
  Float_t y2 = y_max; \
  TBox* box = new TBox(x1,y1,x2,y2); \
  box->Draw();					 \
}

#define IGTEXT(x,y,chars,size,angle,chopt) \
{ \
  t.SetTextFont(62); \
  t.SetTextSize(size/10.); \
  t.SetTextAlign(12); \
  t.DrawText(x,y,chars);					\
}

#define IPL(n, x, y) \
{ \
  TPolyLine *pl = new TPolyLine(n,x,y); \
  pl->SetLineColor(polyline_color_index); \
  pl->Draw();						  \
}

/*draw polymarker*/
#define IPM(n, x, y) \
{ \
  TPolyMarker *pm = new TPolyMarker(n);	  \
  pm->SetMarkerColor(polymarker_color_index); \
  pm->SetMarkerStyle(marker_type); \
  for(Int_t i=0; i<n; i++) pm->SetPoint(i,x[i],y[i]); \
  pm->Draw();										  \
}

#define IPM_DC(n, x, y, t)			\
{ \
  /*TArc *circle = new TArc();*/				\
  circle->SetLineColor(polymarker_color_index); \
  for(Int_t i=0; i<n; i++) \
  { \
    circle->DrawArc(x[i],y[i],t[i]);		\
  } \
}

#define IPM_ELLIPSE(n, x, y, tlong, tshort) \
{ \
  TEllipse *ellipse = new TEllipse(); \
  ellipse->SetLineColor(polymarker_color_index); \
  for(Int_t i=0; i<n; i++) \
  { \
    ellipse->DrawEllipse(x[i],y[i],tlong[i],tshort[i],0.,360.,0.); \
  } \
}

/*set parameter (here always "TMSI" - Tick Marks Size for axis (in world coordinates))*/
#define IGSET(chname, val) \
{ \
  ticksize = val; \
}



static int
mygetch (void) 
{
  int ch;
  struct termios oldt, newt;

  tcgetattr ( STDIN_FILENO, &oldt );
  newt = oldt;
  newt.c_lflag &= ~( ICANON | ECHO );
  tcsetattr ( STDIN_FILENO, TCSANOW, &newt );
  ch = getchar();
  tcsetattr ( STDIN_FILENO, TCSANOW, &oldt );

  return ch;
}







/* ST <-> AX*/

static int board_layer[96] = {
  2, 4, 6, 1, 3, 5,
  2, 4, 6, 1, 3, 5,
  2, 4, 6, 1, 3, 5,
  2, 4, 6, 1, 3, 5,
  2, 4, 6, 1, 3, 5,  
  2, 4, 6, 1, 3, 5,
  2, 4, 6, 1, 3, 5,
  2, 4, 6, 1, 3, 5,
  2, 4, 6, 1, 3, 5,
  2, 4, 6, 1, 3, 5,
  2, 4, 6, 1, 3, 5,
  2, 4, 6, 1, 3, 5,
  2, 4, 6, 1, 3, 5,
  2, 4, 6, 1, 3, 5,
  2, 4, 6, 1, 3, 5,
  2, 4, 6, 1, 3, 5
};

static int board_wire[96] = {
  1, 1, 1, 1, 1, 1,
  2, 2, 2, 2, 2, 2,
  3, 3, 3, 3, 3, 3,
  4, 4, 4, 4, 4, 4,
  5, 5, 5, 5, 5, 5,
  6, 6, 6, 6, 6, 6,
  7, 7, 7, 7, 7, 7,
  8, 8, 8, 8, 8, 8,
  9, 9, 9, 9, 9, 9,
 10,10,10,10,10,10,
 11,11,11,11,11,11,
 12,12,12,12,12,12,
 13,13,13,13,13,13,
 14,14,14,14,14,14,
 15,15,15,15,15,15,
 16,16,16,16,16,16
};

static int superlayer[21][96];
static int layer[21][96];
static int wire[21][96];


#define ABS(x) ((x) < 0 ? -(x) : (x))

#define NCHAN 256 /*how many channels to draw*/


static int goodevent;

#define MAXBUF 1000000
static unsigned int buf[MAXBUF];


int
dcgetevent(int handler, int dcndig[36], int dcdigi[36][128][2], unsigned char sl[2][NOFFSETS][112])
{
  GET_PUT_INIT;
  unsigned long long timestamp;
  int fragtag, fragnum, banktag, banknum, ind_data, ind, nchan, reg;
  int tag[6][3] = {
    41,42,43,
    44,45,46,
    47,48,49,
    50,51,52,
    53,54,55,
    56,57,58
  };



  int status, ifpga, nchannels, nogoodevents;

  int trig,chan,fpga,apv,hybrid;
  int i1, type, timestamp_flag;
  float f1,f2;
  unsigned int word, *gsegm;

  //int nhitp;
  int *bufptr;
  //const unsigned short threshold[3] = {1,1,3};
  //int io = 0;
  //PCHit hitout[PHIT];

  int ii, ret, sec, uvw, npsble;
  //PCStrip strip[3][PSTRIP];
  //int npeak[3][3];
  uint8_t nhits;
  //PCPeak peak[3][3][PPEAK];
  //PCHit hit[1000];
  //uint16_t peakfrac[3][PPEAK];

  int nr,nl,ncol,nrow,i,j, k, jj,kk,l,l1,l2,ichan,nn,iev,nbytes, ind1;
  int nwpawc,lun,lrec,istat,icycle,idn,nbins,nbins1,igood,offset,ih,il;
  float x1,x2,y1,y2,ww,tmpx,tmpy,ttt,ref;

#ifdef DEBUG
   printf("dcgetevent reached\n");
#endif

  nogoodevents = 1;
  while(nogoodevents)
  {

    for(i=0; i<36; i++) dcndig[i] = 0;

    status = evRead(handler, buf, MAXBUF);
    if(status < 0)
    {
      if(status==EOF)
	  {
        printf("end of file after %d events - exit\n",iev);
        return(EOF);
	  }
      else
	  {
        printf("evRead error=%d after %d events - exit\n",status,iev);
	  }
      return(-1);
    }

    bufptr = (int *) buf;



    sec = 1; /* from 0 */
    for(reg=0; reg<3; reg++)
    {
      fragtag = tag[sec][reg];
      fragnum = -1;
      banktag = 0xe116;
      banknum = 0;

      ind = 0;
      for(banknum==0; banknum<40; banknum++)
	  {
        ind = evLinkBank(buf, fragtag, fragnum, banktag, banknum, &nbytes, &ind_data);
        if(ind>0) break;
	  }
#ifdef DEBUG
      printf("evLinkBank() returns ind=%d (banknum=%d)\n",ind,banknum);
#endif
      if(ind > 0)
      {
        int dcrb_check[21][96];
        int half,chip,chan,bco,tdc,tdcref,chan1,edge,nw;
        unsigned char *end, *start;
        unsigned int tmp;
        float tmpx0, tmpx2, dcrbref;
        unsigned int temp[6];
        unsigned sample[6];
        int slot;
        int ndata0[22], data0[21][8];
        int baseline, sum, channel, ch1;
#ifdef DEBUG
        printf("ind=%d, nbytes=%d\n",ind,nbytes);fflush(stdout);
#endif
        start = b08 = (unsigned char *) &bufptr[ind_data];
        end = b08 + nbytes;
#ifdef DEBUG
        printf("ind=%d, nbytes=%d (from 0x%08x to 0x%08x)\n",ind,nbytes,b08,end);fflush(stdout);
#endif

        goodevent = 0;
        nhits = 0;
        for(i=0; i<21; i++) for(j=0; j<96; j++) dcrb_check[i][j] = 0;


	    dcrbref = -870;


#ifdef DEBUG
        printf("dcrbref=%d\n",dcrbref);
#endif

        tdcref = 0;
        timestamp_flag = 0;
        while(b08<end)
        {
#ifdef DEBUG
          printf("begin while: b08=0x%08x\n",b08);
#endif
          GET8(slot);
          GET32(trig);
          GET64(timestamp);
          GET32(nchan);
#ifdef DEBUG
          printf("slot=%d, trig=%d, timestamp=%lld nchan=%d\n",slot,trig,timestamp,nchan);fflush(stdout);
#endif

          for(nn=0; nn<nchan; nn++)
	      {
            GET8(chan);
            GET16(tdc);

	        if(1/*slot<10 &&*/ /*tdc>140*/ /*&& tdc<1650*/ )
	        {
              nhits ++;

              il = reg*12 + layer[slot][chan] + 6*superlayer[slot][chan];
              if(dcrb_check[slot][chan]==0)
			  {
                dcrb_check[slot][chan]=1;
                dcndig[il] ++;
                ih = dcndig[il] - 1;
                dcdigi[il][ih][0] = wire[slot][chan];
                dcdigi[il][ih][1] = /*200 -*/ tdc /*- 140*/;	    /*inverted tdcs*/

      	        printf("nhits=%d slot=%d chan=%2d (layer=%1d wire=%2d) tdc=%5d\n",
				   nhits,slot,chan,il+1,dcdigi[il][ih][0],dcdigi[il][ih][1]);

                /*events selection*/
                if(nhits>3) goodevent = 1;
                if(nhits>100) goodevent = 0;
			    goodevent = 1;
			  }
              else
			  {
                printf("ignore second hit in slot %d channel %d (tdc=%d)\n",slot,chan,tdc);
			  }
	        }
	      }
        }
#ifdef DEBUG
        printf("end loop: b08=0x%08x\n",b08);
#endif
	  }
    } /*loop over 3 regions*/


    nogoodevents = 0;

  } /*while(nogoodevents)*/

  /*
  for(il=0; il<36; il++)
  {
    if(dcndig[il]>0) printf("== layer %d, nhits=%d\n",il,dcndig[il]);
    for(ih=0; ih<dcndig[il]; ih++)
    {
      printf("== [%3d] wire=%d tdc=%d\n",ih,dcdigi[il][ih][0],dcdigi[il][ih][1]);
    }
  }
  */

#ifdef DEBUG
   printf("dcgetevent reached\n");
#endif

  return(0);
}






static int dcndig[36], dcdigi[36][128][2];
static int ifirst = 1;

static unsigned char sl_trigger[2][NOFFSETS][NWIRES];
static unsigned char sl_offline[2][NOFFSETS][NWIRES];



#define STRLEN 128
#define mxstep 10000
#define npl_dc 36
#define npln   46


#define max_draw 10
typedef struct sdadraw
{
  int   ntrdraw;
  int   draw_isec[max_draw];
  int   draw_nstep[max_draw];
  float draw_xyz[max_draw][1000][3];
  float draw_hit[max_draw][9][3];

  int   ntrdraw0;
  int   draw0_isec[max_draw];
  int   draw0_nstep[max_draw];
  float draw0_xyz[max_draw][1000][3];
  float draw0_hit[max_draw][9][3];

  float sda_dv[2];
  float sda_vc[4];
  float sda_wc[4];
} SDAdraw;
SDAdraw sdadraw_;








int
dcdsectEEL(int handler, Viewer *viewer, int isec, float T0)
{
  TArc *circle = new TArc();

  FILE *fd;
  char *ch, str[STRLEN];
  int iplane,iwire,ilayer,iplane1,iplane2,ret;
  float midx,midy,midz,cosx,cosy,cosz;
  float shift, dt, dh, dy[6];
  unsigned char sum[128];
  PRTRACK *road;
  int i,j,k,nd,il,ip,iw,it,isc,ind,ig, nw,lmax,nwire,nwmin,nwmax,
    Nplane,j_plane, nstepm, npoint, nhit, indb, /*ind_sc[57][2],*/
    ibgrd,ired,igreen,iblue,ipurp,ilblue, /*nslab[6],*/ la, itdc;

  float *xmax, *ymax, deltaR, guardr, phi, degrad,
        phi_plane, x_plane, y_plane, delta_pl, xlast,ylast, xfirst,
        yfirst, phimin,phimax, phi25,d_out,dphi,Ndiv, phiort,
        x,y, x1,y1,x2,y2,x3,y3,x4,y4, dist,din,dout,d20, x00[3],
        f_s, fox, rad, angle,angle1,angle2, sas,sac;

  float xmin0, ymin0, xmax0, ymax0;

  float x_hit[mxstep], y_hit[mxstep], x_bgrd[max_nhit],
    y_bgrd[max_nhit], gwire_x[max_nwire], gwire_y[max_nwire],
    t_bgrd[max_nwire];

  /* for superlayer-based fit */
  int nnn[36]; /* the number of hits in layers 1..6 */
  int iww[36][112]; /* wire number */
  float xxx[36][112]; /* x-coords */
  float yyy[36][112]; /* y-coords */
  float ttt[36][112]; /* time */

  char legend[3];
  int slot, chan;

  printf("dcdsectEEL reached, handler=0x%08x\n",handler);fflush(stdout);

  /*should do it once*/
  if(ifirst)
  {
    ifirst = 0;

    for(isec=0; isec<1/*6*/; isec++)
    {
      cminit_geom(sdageom_.dc_wpmid[isec],sdageom_.dc_wdir[isec],sdageom_.dc_wlen[isec]);
    }

	xmin0 = 1000.;
    ymin0 = 1000.;
    xmax0 = -1000.;
    ymax0 = -1000.;
    for(ilayer=0; ilayer<36; ilayer++)
    {
      for(iwire=0; iwire<112; iwire++)
      {
        if(xmin0 > sdageom_.dc_wpmid[0][ilayer][iwire][0]) xmin0 = sdageom_.dc_wpmid[0][ilayer][iwire][0];
        if(xmax0 < sdageom_.dc_wpmid[0][ilayer][iwire][0]) xmax0 = sdageom_.dc_wpmid[0][ilayer][iwire][0];
        if(ymin0 > sdageom_.dc_wpmid[0][ilayer][iwire][2]) ymin0 = sdageom_.dc_wpmid[0][ilayer][iwire][2];
		if(ymax0 < sdageom_.dc_wpmid[0][ilayer][iwire][2]) ymax0 = sdageom_.dc_wpmid[0][ilayer][iwire][2];
		/*
        printf("KIM: ilayer=%d iwire=%d x=%f y=%f z=%f\n",ilayer,iwire,sdageom_.dc_wpmid[0][ilayer][iwire][0],
			   sdageom_.dc_wpmid[0][ilayer][iwire][1], sdageom_.dc_wpmid[0][ilayer][iwire][2]);
		*/
	  }
    }
    printf("\n  xmin0=%f xmax0=%f ymin0=%f ymax0=%f\n",xmin0,xmax0,ymin0,ymax0);
    xmin0 = xmin0 - ABS(xmin0*0.05);
    xmax0 = xmax0 + ABS(xmax0*0.05);
    ymin0 = ymin0 - ABS(ymin0*0.05);
    ymax0 = ymax0 + ABS(ymax0*0.05);
    printf("\n  xmin0=%f xmax0=%f ymin0=%f ymax0=%f\n",xmin0,xmax0,ymin0,ymax0);

	if(viewer>0)
    {
      viewer->SetRange(xmin0,ymin0,xmax0,ymax0,kTRUE);
      viewer->GetRange(xmin0,ymin0,xmax0,ymax0);
      printf("RANGE(DEFAULT): xmin0=%f, xmax0=%f, ymin0=%f, ymax0=%f\n",xmin0,xmax0,ymin0,ymax0);
    }


    AxialPlusStereoInitOld(); /* should be done once */

    for(slot=3; slot<10; slot++)
    {
      for(chan=0; chan<96; chan++)
      {
        superlayer[slot][chan] = 0;
        layer[slot][chan] = board_layer[chan]-1;
        wire[slot][chan] = (board_wire[chan]-1)+(slot-3)*16;
		/*
  	  printf("translation table: slot %d, chan %d -> sl=%d la=%d w=%d\n",
          slot,chan,superlayer[slot][chan],layer[slot][chan],wire[slot][chan]);
		*/
  	  }
    }
    for(slot=14; slot<21; slot++)
    {
      for(chan=0; chan<96; chan++)
      {
        superlayer[slot][chan] = 1;
        layer[slot][chan] = board_layer[chan]-1;
        wire[slot][chan] = (board_wire[chan]-1)+(slot-14)*16;
		/*
  	  printf("translation table: slot %d, chan %d -> sl=%d la=%d w=%d\n",
          slot,chan,superlayer[slot][chan],layer[slot][chan],wire[slot][chan]);
		*/
  	  }
    }

  } /*if(first)*/



  /*HIGZ world coordinates*/
  sdadraw_.sda_wc[0] = 0.;
  sdadraw_.sda_wc[1] = 20.;
  sdadraw_.sda_wc[2] = 0.;
  sdadraw_.sda_wc[3] = 20.;
  xmax = &sdadraw_.sda_wc[1];
  ymax = &sdadraw_.sda_wc[3];
  ibgrd  = 1;
  ired   = 2;
  igreen = 3;
  iblue  = 4;
  ipurp  = 6;
  ilblue = 7;
  degrad = acos(-1.)/180.;

  /* Set up a view box for superlayers */
  if(viewer>0)
  {
    ISPMCI(ibgrd);
    ISPLCI(ibgrd);
  }

  /* Draw geometry of DC */
  if(viewer>0)
  {

	/* draw 6 superlayers */
    /*draw boxes around wires areas*/

    for(iplane1=0; iplane1<36; iplane1+=6) /*0,6,12,18,24,30*/
	{
      iplane = iplane1;
      iwire = 0;
      gwire_x[0] = sdageom_.dc_wpmid[0][iplane][iwire][0];
      gwire_y[0] = sdageom_.dc_wpmid[0][iplane][iwire][2];


      iplane = iplane1;
      iwire = 111;
      gwire_x[1] = sdageom_.dc_wpmid[0][iplane][iwire][0];
      gwire_y[1] = sdageom_.dc_wpmid[0][iplane][iwire][2];

      iplane = iplane1+1;
      iwire = 111;
      gwire_x[2] = sdageom_.dc_wpmid[0][iplane][iwire][0];
      gwire_y[2] = sdageom_.dc_wpmid[0][iplane][iwire][2];

      iplane = iplane1+2;
      iwire = 111;
      gwire_x[3] = sdageom_.dc_wpmid[0][iplane][iwire][0];
      gwire_y[3] = sdageom_.dc_wpmid[0][iplane][iwire][2];

      iplane = iplane1+3;
      iwire = 111;
      gwire_x[4] = sdageom_.dc_wpmid[0][iplane][iwire][0];
      gwire_y[4] = sdageom_.dc_wpmid[0][iplane][iwire][2];

      iplane = iplane1+4;
      iwire = 111;
      gwire_x[5] = sdageom_.dc_wpmid[0][iplane][iwire][0];
      gwire_y[5] = sdageom_.dc_wpmid[0][iplane][iwire][2];

      iplane = iplane1+5;
      iwire = 111;
      gwire_x[6] = sdageom_.dc_wpmid[0][iplane][iwire][0];
      gwire_y[6] = sdageom_.dc_wpmid[0][iplane][iwire][2];

	  dy[iplane1/6] = (gwire_y[6] - gwire_y[0]) / 5.0; /* distance between layers in this superlayer */

      iplane = iplane1+5;
      iwire = 0;
      gwire_x[7] = sdageom_.dc_wpmid[0][iplane][iwire][0];
      gwire_y[7] = sdageom_.dc_wpmid[0][iplane][iwire][2];

      iplane = iplane1+4;
      iwire = 0;
      gwire_x[8] = sdageom_.dc_wpmid[0][iplane][iwire][0];
      gwire_y[8] = sdageom_.dc_wpmid[0][iplane][iwire][2];

      iplane = iplane1+3;
      iwire = 0;
      gwire_x[9] = sdageom_.dc_wpmid[0][iplane][iwire][0];
      gwire_y[9] = sdageom_.dc_wpmid[0][iplane][iwire][2];

      iplane = iplane1+2;
      iwire = 0;
      gwire_x[10] = sdageom_.dc_wpmid[0][iplane][iwire][0];
      gwire_y[10] = sdageom_.dc_wpmid[0][iplane][iwire][2];

      iplane = iplane1+1;
      iwire = 0;
      gwire_x[11] = sdageom_.dc_wpmid[0][iplane][iwire][0];
      gwire_y[11] = sdageom_.dc_wpmid[0][iplane][iwire][2];

      iplane = iplane1;
      iwire = 0;
      gwire_x[12] = sdageom_.dc_wpmid[0][iplane][iwire][0];
      gwire_y[12] = sdageom_.dc_wpmid[0][iplane][iwire][2];
	  
      printf("box(sl %d): %f,%f %f,%f %f,%f %f,%f %f,%f %f,%f %f,%f\n",iplane1/6,
	    gwire_x[0],gwire_y[0],gwire_x[1],gwire_y[1],gwire_x[2],gwire_y[2],
			 gwire_x[3],gwire_y[3],gwire_x[4],gwire_y[4],gwire_x[5],gwire_y[5],gwire_x[6],gwire_y[6]);
      printf("   dy[%d] = %f\n",iplane1/6,dy[iplane1/6]);
	  
      IPL(13,gwire_x,gwire_y);
	}


  }







  /*************************************************************************************************/
  /*************************************************************************************************/
  /*************************************************************************************************/

  if(handler > 0)
  {
    ret = dcgetevent(handler, dcndig, dcdigi, sl_trigger);
    if(ret==EOF) return(EOF);
  }

  /* Plot hits in DC (digitalization) */
  shift = 0.0; /* -0.36 */
  while(shift <= 0.0) /* <= 0.36 (one cell 0.06, += 6 cells) */
  {
    int ii, jj, il, ir, lay;
    float factor;
    float x_start, xx, xl, xr;
    float xmin, ymin, xmax, ymax;

    Word128 grid[12], segments0, segments1;
    for(lay=0; lay<12; lay++) ClearWord128(&grid[lay]);


    if(viewer>0)
    {
      viewer->GetRange(xmin,ymin,xmax,ymax);
      printf("RANGE: xmin=%f, xmax=%f, ymin=%f, ymax=%f\n",xmin,xmax,ymin,ymax);
    }
    else
    {
      xmin=ymin=0.;
      xmax=ymax=20.;
    }


    /* fills following:
        dcndig[layer] - the number of hits in 'layer'
        dcdigi[layer][hit#][0] = wire#;
        dcdigi[layer][hit#][1] = tdc;
    */

    indb = 0;
    for(i=0; i<36; i++) nnn[i] = 0;
    for(i=1; i<=36; i++)
    {
      nhit = dcndig[i-1];
      for(j=1; j<=nhit; j++)
      {
        iw = dcdigi[i-1][j-1][0];

        itdc = dcdigi[i-1][j-1][1];

        if(indb < max_nhit)
	    {
          indb ++;
          x_bgrd[indb-1] = sdageom_.dc_wpmid[isec-1][i-1][iw][0];
          y_bgrd[indb-1] = sdageom_.dc_wpmid[isec-1][i-1][iw][2];
          t_bgrd[indb-1] = itdc / T0;
	      /*
	      printf("l=%d, w=%d, tdc=%d -> x=%f y=%f tdc(est.)=%f\n",
				 i,iw,itdc,x_bgrd[indb-1],y_bgrd[indb-1],t_bgrd[indb-1]);fflush(stdout);
		  */
          /* use hits withing selected window ! */ 
		  if( x_bgrd[indb-1]>xmin && x_bgrd[indb-1]<xmax && y_bgrd[indb-1]>ymin && y_bgrd[indb-1]<ymax)
		  {
            lay = i-1;
            iww[lay][nnn[lay]] = iw;
            xxx[lay][nnn[lay]] = x_bgrd[indb-1];
            yyy[lay][nnn[lay]] = y_bgrd[indb-1];
	        ttt[lay][nnn[lay]] = t_bgrd[indb-1];
			/*
		    printf("1: lay=%2d, nnn=%2d, itdc=%4d (xxx=%f yyy=%f ttt=%f)\n",
              lay,nnn[lay],itdc,xxx[lay][nnn[lay]],yyy[lay][nnn[lay]],ttt[lay][nnn[lay]]);fflush(stdout);
			*/
            x_start = xmin;
            factor = 5.;

            dh = ((float)lay)*dy[(i-1)/6]; /*vertical distance from layer0 to current layer*/
            dt = ttt[lay][nnn[lay]];

            xl = xxx[lay][nnn[lay]] + shift*dh - dt; /* *dh/sqrt(dh*dh+shift*shift);*/ /*?????*/
			xr = xxx[lay][nnn[lay]] + shift*dh + dt; /* *dh/sqrt(dh*dh+shift*shift);*/

            x_bgrd[indb-1] = (xl+xr)/2.;

            il = (xl-x_start) * factor;
			ir = (xr-x_start) * factor;
			/*
            printf("==> SHIFT=%f: lay=%d(y=%f), L=%f, R=%f\n",shift,lay,yyy[lay][nnn[lay]],xl,xr);fflush(stdout);
            printf("-->                   L=%f(%d), R=%f(%d)\n",xl-x_start,il,xr-x_start,ir);fflush(stdout);
			*/
            SetBitWord128(&grid[lay],il);
            SetBitWord128(&grid[lay],ir);
			
            printf("---------> xl-x_start=%f il=%d -> MOD=%f\n",xl-x_start,il,(xl-x_start)*factor-il); fflush(stdout);
            printf("---------> xr-x_start=%f ir=%d -> MOD=%f\n",xr-x_start,ir,(xr-x_start)*factor-ir); fflush(stdout);
			
            if(((xl-x_start)*factor-il)>0.5) SetBitWord128(&grid[lay],il+1);
            /*else                          SetBitWord128(&grid[lay],il-1);*/

            if(((xr-x_start)*factor-ir)>0.5) SetBitWord128(&grid[lay],ir+1);
            /*else                          SetBitWord128(&grid[lay],ir-1);*/

            printf("\n");fflush(stdout);
            nnn[lay]++;
		  }
	    }
      }
    }



	/*
    printf("SL1:\n");
    for(lay=11; lay>=6; lay--)
	{
      PrintWord128(&grid[lay]);
	}
	*/

	/*
    printf("SL0:\n");
    for(lay=5; lay>=0; lay--)
	{
      PrintWord128(&grid[lay]);
	}
	*/
    SuperLayerSum(grid, &segments0, sum);
	/*
    printf("segments:\n");
    PrintWord128(&segments0);
	*/
    PrintSum128(sum);


    if(indb > 0)
    {
      
      if(viewer>0)
      {
        ISPMCI(ired);
        ISMK(5);

        /*  
        IPM(indb,x_bgrd,y_bgrd);
        */

        IPM_DC(indb,x_bgrd,y_bgrd,t_bgrd);
      }
      

    } /*if(indb > 0)*/

    if(viewer>0)
    {
      ISPMCI(ibgrd);
      ISPLCI(ibgrd);
      viewer->Redraw();
    }

	/*
    printf("Press any key to continue.\n");
    mygetch();
	*/

    shift += 0.01; /* one cell is about 0.06 */

  } /* while(shift < 0.36) */





  return(0);
}
