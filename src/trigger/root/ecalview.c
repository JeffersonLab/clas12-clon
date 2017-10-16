/*
   ecalview.c - draw ECAL geometry, hits and clusters

*/



#define NOFFSETS 16
#define NWIRES 112

#define DEBUG
#define DEBUG1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <termios.h>
#include <unistd.h>

#include "evio.h"

#ifdef USE_PCAL
#include "pclib.h"
static uint8_t iview[3] = {USTRIPS,VSTRIPS,WSTRIPS}; /* the number of strips in different views */
static int l2v[3] = {0,2,1}; /* layer to view */
#else
#include "eclib.h"
static uint8_t iview[3] = {NSTRIP,NSTRIP,NSTRIP}; /* the number of strips in different views */
static int l2v[3] = {0,1,2}; /* layer to view */
#endif





#define ABS(x) ((x) < 0 ? -(x) : (x))


TText t(0,0,"a");
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
  /*TPolyLine * */pl = new TPolyLine(n,x,y);	\
  pl->SetLineColor(polyline_color_index); \
  /*pl->SetFillColor(3);*/				  \
  /*pl->Draw("f");*/					  \
  pl->Draw(); \
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

#define REDRAW \
  if(fCanvas>0) \
  { \
    fCanvas->Modified(); \
    fCanvas->Update(); \
  }






int
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




static int iev = 0;
static int goodevent;

#define MAXBUF 1000000
unsigned int buf[MAXBUF];

static double xmin, ymin, xmax, ymax;
static double xdiff, ydiff, xdelta[3], xx1, xx2, yy1, yy2;
static double viewx0[3], viewy0[3], viewx1[3], viewy1[3], viewangle[3], viewcosa[3], viewsina[3];
static double dxy = 5.0;

/*adc data*/
static ECStrip strip[2][3][NSTRIP];
static ECPeak peak[3][NPEAK];
static uint8_t nhits;
static ECHit hitdraw[NHIT];
/*adc data*/

/*trig data*/
static int npeaks_trig[NLAYER]; 
static TrigECPeak peaks_trig[NLAYER][NPEAKMAX];
static int nhits_trig[2];
static TrigECHit hits_trig[2][NHIT];
/*trig data*/

typedef struct drawobject *DrawObjectPtr;
typedef struct drawobject
{
  TObject *obj;
  char name[128];
  double energy;

} DrawObject;


#define NDOBJ 500
static int ndobj = 0;
DrawObject dobj[NDOBJ];


/* PCAL geometry */
#ifdef USE_PCAL
#define ANGLE1 62.889
#define ANGLE2 54.222
#endif


void
place_pmt(int view, double x0pmt, double y0pmt, double h1, double h2, double angle, double xdelta,
          double x[5], double y[5], int level)
{
  int ii;
  double xx[5], yy[5], cosa, sina, delta;

#ifdef USE_PCAL
  /* 0 degree - vertical up, negative - rotate right, positive - rotate left */
  if(view==0)      angle = -(90.0+ANGLE1)*M_PI/180.0;
  else if(view==1) angle = -(90.0-ANGLE2)*M_PI/180.0;
  else if(view==2) angle = 90*M_PI/180.0;
#else
  angle = angle - 150.0*M_PI/180.0;
#endif
  cosa = cos(angle);
  sina = sin(angle);

  /*printf("ANGLE=%f COSA=%f\n",angle,cosa);*/

  delta = xdelta*(1.0-(double)level*(1./(NHIT+1)));

  xx[0] = x0pmt - delta*0.866/2.;
  xx[1] = xx[0] + delta*0.866;
  xx[2] = xx[1];
  xx[3] = xx[0];
  xx[4] = xx[0];

  yy[0] = y0pmt + h1;
  yy[1] = y0pmt + h1;
  yy[2] = y0pmt + h2;
  yy[3] = y0pmt + h2;
  yy[4] = yy[00];
  
  for(ii=0; ii<5; ii++)
  {
    x[ii] = ((xx[ii]-x0pmt) * cosa - (yy[ii]-y0pmt) * sina) + x0pmt;
    y[ii] = ((yy[ii]-y0pmt) * cosa + (xx[ii]-x0pmt) * sina) + y0pmt;
  }
  
}

void
place_line(int view, double x0pmt, double y0pmt, double angle, double xdelta, double x[2], double y[2])
{
  int ii;
  double xx[2], yy[2], cosa, sina;


#ifdef USE_PCAL
  if(view==0)      angle = (180.0-(90.0+ANGLE1))*M_PI/180.0;
  else if(view==1) angle = (180.0-(90.0-ANGLE2))*M_PI/180.0;
  else if(view==2) angle = (180.+90)*M_PI/180.0;
#else
  angle = angle + 30.0*M_PI/180.0;
#endif

  cosa = cos(angle);
  sina = sin(angle);

  xx[0] = x0pmt;
  xx[1] = xx[0];

  yy[0] = y0pmt;
  yy[1] = y0pmt + xdelta*((double)NSTRIP);
  
  for(ii=0; ii<2; ii++)
  {
    x[ii] = ((xx[ii]-x0pmt) * cosa - (yy[ii]-y0pmt) * sina) + x0pmt;
    y[ii] = ((yy[ii]-y0pmt) * cosa + (xx[ii]-x0pmt) * sina) + y0pmt;
  }

}



int
draw_ecstrips(TCanvas *fCanvas, double energy[NHIT+1][3][NSTRIP], int maxhits)
{
#if 1
  int ii, jj, kk, view, color;
  double emax;
  double x0pmt[3][NSTRIP], y0pmt[3][NSTRIP], x1pmt[3][NSTRIP], y1pmt[3][NSTRIP], x0, y0;
  double xx[NSTRIP][5], yy[NSTRIP][5];
  double x[NSTRIP][5], y[NSTRIP][5];
  double xl[2], yl[2];
  double endraw;
  TPolyLine *pl;
  char txt[128];
#endif


#ifdef DEBUG
  printf("draw_ecstrips reached\n\n");fflush(stdout);
  for(ii=0; ii<maxhits; ii++) for(jj=0; jj<3; jj++) for(kk=0; kk<NSTRIP; kk++) printf("draw_ecstrips: energy[%2d][%2d][%2d]=%f\n",ii,jj,kk,energy[ii][jj][kk]);
  printf("\n");
#endif

#if 1
  /* find maximum energy */
  emax = 0.;
  for(kk=0; kk<maxhits; kk++) for(jj=0; jj<3; jj++) for(ii=0; ii<NSTRIP; ii++) if(emax < energy[kk][jj][ii]) emax = energy[kk][jj][ii];
#ifdef DEBUG
  printf("draw_ecstrips: emax=%f\n",emax);
#endif

  /* if all strips are zero, set emax for drawing purpose */
  if(emax < 0.00000001) emax = 1.0;

#endif

#if 1
  /* normalize energy (divide 'ydiff' as needed!) */
  for(kk=0; kk<maxhits; kk++) for(jj=0; jj<3; jj++) for(ii=0; ii<NSTRIP; ii++) energy[kk][jj][ii] = ( energy[kk][jj][ii] * (ydiff/5.) )/ emax;
#endif

  if(fCanvas>0)
  {


#if 1

    for(view=0; view<3; view++) /* loop over views */
	{
      color = 1;


      for(ii=0; ii<iview[view]; ii++)
	  {

        x0pmt[view][ii] = xx1 + xdelta[view]/2. + xdelta[view]*ii;
        y0pmt[view][ii] = yy1;
		//printf("XY1: %f %f\n",x0pmt[view][ii],y0pmt[view][ii]);fflush(stdout);



        /* U-V-W adjustment */
        x0 = ((x0pmt[view][ii]-xx1) * viewcosa[view] - (y0pmt[view][ii]-yy1) * viewsina[view]) + viewx0[view];
        y0 = ((y0pmt[view][ii]-yy1) * viewcosa[view] + (x0pmt[view][ii]-xx1) * viewsina[view]) + viewy0[view];
		//printf("XY2: %f %f\n",x0,y0);fflush(stdout);



        /* draw original energy */
        place_pmt(view,x0,y0,0.0,energy[0][view][ii],viewangle[view],xdelta[view],x[ii],y[ii],0);
        ISPLCI(color);
        IPL(5,x[ii],y[ii]);
            dobj[ndobj].obj = pl;
            strncpy(txt,"initial energy",127);
            strncpy(dobj[ndobj].name,txt,strlen(txt));
            dobj[ndobj].energy = energy[0][view][ii];
            ndobj ++;
        /*endraw = energy[0][view][ii];*/


		/* draw corrected energies */
		for(kk=0; kk<(maxhits-1); kk++)
		{
          place_pmt(view,x0,y0,0.0/*endraw*/,energy[kk+1][view][ii],viewangle[view],xdelta[view],x[ii],y[ii],kk+1);
          ISPLCI(color+1+kk);
          if(energy[kk+1][view][ii]>0.)
		  {
            IPL(5,x[ii],y[ii]);

            dobj[ndobj].obj = pl;
            strncpy(txt,"corrected energy",127);
            strncpy(dobj[ndobj].name,txt,strlen(txt));
            dobj[ndobj].energy = energy[kk+1][view][ii];
            ndobj ++;
		  }
          /*endraw = energy[kk+1][view][ii];*/
		}

      }


	} /* loop over views */

#endif

  }

  return(0);
}










void
draw_init()
{
  int view;
  double angle, cosa, sina, xdelta0, x0, y0, x1, y1, h, len1;

  printf("draw_init reached, xmin=%f, xmax=%f, ymin=%f, ymax=%f\n",xmin,xmax,ymin,ymax);fflush(stdout);

#ifdef USE_PCAL

  /*

                          x
                         x62.9deg=ANGLE1
                        x
                       x
                      x   |
                     x    |
                    x     |
           432.7cm x      |          394.2cm
                  x       |
                 x        |
                x      h=350.9473
               x          |
              x           |
             x            | 
            x             |
           x54.2deg=ANGLE2|              62.9deg=ANGLE1
    beam  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
    here      len1=cm      432.7cm

  */

  /*1/4 of drawing area*/
  dxy = FULLSIZE/4. /*432.7/2*/;

  /* staring point for drawing (left bottom corner */
  xx1 = xmin + dxy;
  yy1 = ymin + dxy;

  xx2 = xmax - dxy; /*not used*/
  yy2 = ymax - dxy; /*not used*/

  h = 432.7*sin(ANGLE2*M_PI/180.0);
  len1 = sqrt( pow(432.7,2.) - pow(h,2.) );

  xdiff = 432.7;
  ydiff = 410.0 /* approximately */;

#else
  dxy = (xmax-xmin)/4.0;
  xx1 = xmin + dxy;
  xx2 = xmax - dxy;
  yy1 = ymin + dxy;
  yy2 = ymax - dxy;
  xdiff = xx2 - xx1;
  ydiff = yy2 - yy1;
#endif

  for(view=0; view<3; view++)
  {

#ifdef USE_PCAL
    if(view==0)
	{
      angle = 0.0 * M_PI / 180.0;
      cosa = cos(angle);
      sina = sin(angle);
      xdelta0 = 432.7 / (double)iview[view];
      x0 = dxy;
      y0 = dxy;
      x1 = x0+432.7;
	  y1 = y0;
	}
    else if(view==1)
	{
	  /* rotation */
      angle = (180. - ANGLE1) * M_PI / 180.0;
      cosa = cos(angle);
      sina = sin(angle);
      xdelta0 = 394.2 / (double)iview[view];
      x0 = x1; /* end of previous view */
      y0 = y1; /* end of previous view */
	  printf("++++++++++++++ h=%f len1=%f -> %f %f\n",h,len1,x1,y1);
      x1 = dxy+len1;
      y1 = y0+h;
	}
    else if(view==2)
	{
	  /* rotation */
      angle = -(180. - ANGLE2) * M_PI / 180.0;
      cosa = cos(angle);
      sina = sin(angle);
      xdelta0 = 432.7 / (double)iview[view];
      x0 = x1;
      y0 = y1;
      x1 = viewx0[0]; /* returns to starting point */
      y1 = viewy0[0]; /* returns to starting point */
    }
#else
    if(view==0)
	{
      angle = 0.0 * M_PI / 180.0;
      cosa = cos(angle);
      sina = sin(angle);
      xdelta0 = xdiff / (double)NSTRIP;
      x0 = dxy;
      y0 = dxy;
	}
    else if(view==1)
	{
	  /* rotation */
      angle = 120.0 * M_PI / 180.0; /* convert 120 degree to radians */
      cosa = cos(angle);
      sina = sin(angle);
      xdelta0 = xdiff / (double)NSTRIP;
      x0 = xmax-dxy;
      y0 = dxy;
	}
    else if(view==2)
	{
	  /* rotation */
      angle = -120.0 * M_PI / 180.0; /* convert 120 degree to radians */
      cosa = cos(angle);
      sina = sin(angle);
      xdelta0 = xdiff / (double)NSTRIP;
      x0 = (xmax-xmin)/2.0;
      y0 = dxy + ydiff*ABS(sina);
    }
#endif

    xdelta[view] = xdelta0;
    viewangle[view] = angle;
    viewcosa[view] = cosa;
    viewsina[view] = sina;
    viewx0[view] = x0;
    viewy0[view] = y0;

#ifdef DEBUG
    printf("draw_init: view=%d cosa=%f sina=%f xdelta=%f xdiff=%f, y0=%f\n",view,cosa,sina,xdelta0,xdiff,y0);
#endif
  }

  printf("draw_init done\n");fflush(stdout);

  return;
}



/* draw hits, clusters etc for one event */

void
ecaldrawevent(TCanvas *fCanvas)
{
  int io=0;
  int ii, jj, kk, nn;
  double energy[NHIT+1][3][NSTRIP];
  double corenergy[NSTRIP];
  double coord[3];
  TPolyLine *pl;
  TArc *circle;
  char txt[128];

  ndobj = 0;
  double A[3], B[3], C[3], DET, X[3], Y[3], XX, YY, ENERGY;

  fCanvas->GetRange(xmin,ymin,xmax,ymax);
#ifdef DEBUG
  printf("ecaldrawevent: xmin=%f xmax=%f ymin=%f ymax=%f\n",xmin,xmax,ymin,ymax);
#endif

  fCanvas->Clear();

#ifdef DEBUG
  printf("- ecaldrawevent: xx1=%f xx2=%f yy1=%f yy2=%f dxy=%f -> xdiff=%f ydiff=%f\n",xx1,xx2,yy1,yy2,dxy,xdiff,ydiff);fflush(stdout);
#endif

  draw_init();






  /* copy initial energies for drawing */
  for(ii=0; ii<3; ii++) for(jj=0; jj<NSTRIP; jj++)
  {
    energy[0][ii][jj] = (double)strip[io][ii][jj].energy;
#ifdef DEBUG
    printf("===== [%2d][%2d][%2d] %f\n",0,ii,jj,energy[0][ii][jj]);
#endif
  }




#ifdef DEBUG
  cout<<"11: nhits="<<+nhits<<endl;fflush(stdout);
#endif

  for(kk=0; kk<nhits; kk++)
  {
    int color = kk+2;
    double x0pmt, y0pmt, x0, y0, xl[2], yl[2];
	int ipeak[3];

#ifdef USE_PCAL
#ifdef DEBUG
	cout<<"||||||||||||||||||||||||||||||||||||| fview="<<(double)fview[0]<<" "<<(double)fview[1]<<" "<<(double)fview[2]<<endl;
#endif
    for(ii=0; ii<3; ii++) coord[ii] = (double)((uint32_t)hitdraw[kk].coord[ii])/(double)fview[ii];
#else
    for(ii=0; ii<3; ii++) coord[ii] = (double)hitdraw[kk].coord[ii]/8.0;
#endif

#ifdef DEBUG
    cout<<"ecaldrawevent: hitdraw["<<+kk<<"]: energy="<<+hitdraw[kk].energy<<" coord="<<+coord[0]<<" "<<+coord[1]<<" "<<+coord[2]<<endl;
#endif

    /* loop over 3 views calculating correction for every strip*/
    ENERGY = 0.0;
    ipeak[0] = U4(hitdraw[kk].ind);
    ipeak[1] = V4(hitdraw[kk].ind);
    ipeak[2] = W4(hitdraw[kk].ind);

    for(jj=0; jj<3; jj++)
	{
      int energy0, energy1, strip1, stripn; /* peak values */







	  /* correction */
      energy0 = peak[jj][ipeak[jj]].energy;
      strip1  = peak[jj][ipeak[jj]].strip1;
      stripn  = peak[jj][ipeak[jj]].stripn;
      energy1 = hitdraw[kk].enpeak[jj];
#ifdef DEBUG
      cout<<"  ecaldrawevent: hitdraw["<<+kk<<"] strips["<<+jj<<"]: strip1="<<+strip1<<", stripn="<<+stripn<<", energy0="<<+energy0<<", energy1="<<+energy1<<endl;
#endif

	  for(nn=0; nn<NSTRIP; nn++) corenergy[nn] = 0.0;
      for(nn=strip1; nn<strip1+stripn; nn++)
	  {
        corenergy[nn] = (double)strip[io][jj][nn].energy * ((double)energy1) / ((double)energy0); /* for drawing */
#ifdef DEBUG
        cout<<"    ecaldrawevent: strip["<<+jj<<"]["<<+nn<<"] energy = "<<+strip[io][jj][nn].energy<<" -> corrected strip energy = "<<+corenergy[nn]<<endl;
#endif
	  }



	  /* no correction 
      strip1 = 0;
      stripn = 36;
      for(nn=strip1; nn<strip1+stripn; nn++)
	  {
        corenergy[nn] = energy[kk][jj][nn];
	  }
	  */
















#if 1

      /**************/
      /* draw lines */

      x0pmt = xx1 + xdelta[jj]*coord[jj];
#ifdef DEBUG
      cout << "++++++++++++++++++++ " << +hitdraw[kk].coord[jj] << endl;
#endif
      y0pmt = yy1;
	  //printf("XY3: %f %f\n",x0pmt,y0pmt);

      x0pmt += kk*(xdelta[jj]/5.); /*???????????????*/

      /* U-V-W adjustment */
      x0 = ((x0pmt-xx1) * viewcosa[jj] - (y0pmt-yy1) * viewsina[jj]) + viewx0[jj];
      y0 = ((y0pmt-yy1) * viewcosa[jj] + (x0pmt-xx1) * viewsina[jj]) + viewy0[jj];
	  //printf("XY4: %f %f\n",x0,y0);

      place_line(jj, x0, y0, viewangle[jj], xdelta[jj], xl, yl);
      ISPLCI(color);
      IPL(2,xl,yl);
            dobj[ndobj].obj = pl;
            sprintf(txt,"hit %d, view %d\0",kk,jj);
            strcpy(dobj[ndobj].name,txt);
            dobj[ndobj].energy = energy0;
            ndobj ++;

      /* copy corrected energies for drawing */
      for(nn=0; nn<NSTRIP; nn++)
	  {
        energy[kk+1][jj][nn] = corenergy[nn]; /* kk+1 - ERRORRRRRRRRRRRRRR ???*/
        //printf("HIT %d: coping energy energy[%d][%d][%d]=%f\n",kk,kk+1,jj,nn,energy[kk+1][jj][nn]);
	  }

      /* calculate some values for the following line-line crossing */
      A[jj] = yl[1] - yl[0];
      B[jj] = xl[0] - xl[1];
      C[jj] = A[jj] * xl[0] + B[jj] * yl[0];
      ENERGY += energy0;

#endif

	}

	/* line-line cross
    A = y2-y1
    B = x1-x2
    C = A*x1+B*y1

    A1*x + B1*y = C1
    A2*x + B2*y = C2

    double det = A1*B2 - A2*B1
    if(det == 0){
        //Lines are parallel
    }else{
        double x = (B2*C1 - B1*C2)/det
        double y = (A1*C2 - A2*C1)/det
	*/

#if 0

    /* draw circle here */
    DET = A[0] * B[1] - A[1] * B[0];
    X[0] = (B[1]*C[0] - B[0]*C[1]) / DET;
    Y[0] = (A[0]*C[1] - A[1]*C[0]) / DET;

    DET = A[1] * B[2] - A[2] * B[1];
    X[1] = (B[2]*C[1] - B[1]*C[2]) / DET;
    Y[1] = (A[1]*C[2] - A[2]*C[1]) / DET;

    DET = A[0] * B[2] - A[2] * B[0];
    X[2] = (B[2]*C[0] - B[0]*C[2]) / DET;
    Y[2] = (A[0]*C[2] - A[2]*C[0]) / DET;

    XX = (X[0]+X[1]+X[2])/3.;
	YY = (Y[0]+Y[1]+Y[2])/3.;

    circle = new TArc(XX,YY,0.1);
	//printf("OBJ circle=0x%08x\n",circle);
    circle->SetLineColor(color);
    circle->Draw();
            dobj[ndobj].obj = circle;
            sprintf(txt,"hit %d\0",kk);
            strcpy(dobj[ndobj].name,txt);
            dobj[ndobj].energy = ENERGY;
            ndobj ++;

#endif

  }


  /* draw */
  draw_ecstrips(fCanvas, energy, nhits+1 /* +1 ?????????*/);


  return;
}




#ifdef USE_PCAL

#define ecstrip_in  pcstrip_in
#define ecstrip_out pcstrip_out
#define ecpeak_in pcpeak_in
#define echit_in pchit_in

#define ecstrips pcstrips
#define ecstripspersistence pcstripspersistence
#define ecstripsfanout pcstripsfanout
#define ecpeak1 pcpeak1
#define ecpeak2 pcpeak2
#define ecpeakzerosuppress pcpeakzerosuppress
#define ecpeaksort pcpeaksort
#define ecpeakcoord pcpeakcoord
#define ecpeakfanout pcpeakfanout
#define echit pchit
#define ecenergyfanout pcenergyfanout
#define eccoordfanout pccoordfanout
#define ecfrac1 pcfrac1
#define ecfrac2 pcfrac2
#define ecfrac3 pcfrac3
#define eccorr pccorr
#define echitsortin pchitsortin
#define echitsort1 pchitsort1
#define echitsort2 pchitsort2
#define echitsort3 pchitsort3
#define echitsort4 pchitsort4
#define echitsort5 pchitsort5
#define echitsortout pchitsortout
#define echitfanout pchitfanout

#endif


//#define TEST_BENCH
#ifdef TEST_BENCH
#define DALITZ_MIN 0
#define DALITZ_MAX 0xFFF
#endif

/*
#define DALITZ_MIN 700
#define DALITZ_MAX 1000
*/
#define DALITZ_MIN 0
#define DALITZ_MAX 0xFFF


/*
TEST[0][0]=0
TEST[0][1]=1
TEST[0][2]=1
TEST[0][3]=1
TEST[0][4]=1
TEST[0][5]=1
TEST[0][6]=1
TEST[0][7]=1
TEST[1][0]=1
TEST[1][1]=0
TEST[1][2]=1
TEST[1][3]=1
TEST[1][4]=1
TEST[1][5]=1
TEST[1][6]=1
TEST[1][7]=1
TEST[2][0]=1
TEST[2][1]=1
TEST[2][2]=0
TEST[2][3]=1
TEST[2][4]=1
TEST[2][5]=1
TEST[2][6]=1
TEST[2][7]=1
TEST[3][0]=1
TEST[3][1]=1
TEST[3][2]=1
TEST[3][3]=0
TEST[3][4]=1
TEST[3][5]=1
TEST[3][6]=1
TEST[3][7]=1
TEST[4][0]=1
TEST[4][1]=1
TEST[4][2]=1
TEST[4][3]=1
TEST[4][4]=0
TEST[4][5]=1
TEST[4][6]=1
TEST[4][7]=1
TEST[5][0]=1
TEST[5][1]=1
TEST[5][2]=1
TEST[5][3]=1
TEST[5][4]=1
TEST[5][5]=0
TEST[5][6]=1
TEST[5][7]=1
TEST[6][0]=1
TEST[6][1]=1
TEST[6][2]=1
TEST[6][3]=1
TEST[6][4]=1
TEST[6][5]=1
TEST[6][6]=0
TEST[6][7]=1
TEST[7][0]=1
TEST[7][1]=1
TEST[7][2]=1
TEST[7][3]=1
TEST[7][4]=1
TEST[7][5]=1
TEST[7][6]=1
TEST[7][7]=0
RRR=0
*/

/* use it for time scan
#define TIMESTAMP1 0
#define TIMESTAMP2 8
#define PULSETIME1 0
#define PULSETIME2 8
*/
#define TIMESTAMP1 0
#define TIMESTAMP2 1
#define PULSETIME1 0
#define PULSETIME2 1

static int test[TIMESTAMP2][PULSETIME2];
static trig_t trig[4]; /* assumed to be cleaned up because of 'static' */


/*
event 19: missing sim hits
event 54: 5-th hit in sim
event 57: missing hit in sim
event 83: missing sim
*/

int
ecalgetevent(int handler, TCanvas *fCanvas, int sec, int dtimestamp, int dpulsetime, int nextevent)
{
  int status, nogoodevents;
  unsigned int *bufptr;
  ap_uint<16> threshold[3] = {EC_THRESHOLD1, EC_THRESHOLD2, EC_THRESHOLD3};

  uint32_t bufout0[256], bufout1[256], bufout2[256], bufout3[256];
  int nev;
  unsigned long long timestamp;

  //fadc_word_t fadcs[NFADCS][8];
  ECStrip str[3][NSTRIP];
  ECPeak peak_tmp;
  ECHit hit[NHIT];

  hls::stream<fadc_word_t> s_fadc_words[NFADCS];
  hls::stream<ECStrip_s> s_strip0_u[NF1], s_strip0_v[NF1], s_strip0_w[NF1];
  hls::stream<ECStrip_s> s_strip_u[NF1], s_strip_v[NF1], s_strip_w[NF1];
  hls::stream<ECStrip_s> s_strip1_u[NF1], s_strip1_v[NF1], s_strip1_w[NF1];
  hls::stream<ECStrip_s> s_strip2_u[NF1], s_strip2_v[NF1], s_strip2_w[NF1];
  hls::stream<ECPeak_s> s_peak_u, s_peak_v, s_peak_w;
  hls::stream<ECPeak_s> s_peak1_u, s_peak1_v, s_peak1_w;
  hls::stream<ECPeak_s> s_peak2_u, s_peak2_v, s_peak2_w;
  hls::stream<ECStream6_s> s_pcount[NH_FIFOS];
  hls::stream<ECStream16_s> s_energy[NH_FIFOS];
  hls::stream<ECStream16_s> s_energy1[NH_FIFOS];
  hls::stream<ECStream16_s> s_energy2[NH_FIFOS];
  hls::stream<ECStream10_s> s_coord[NH_FIFOS];
  hls::stream<ECStream10_s> s_coord1[NH_FIFOS];
  hls::stream<ECStream10_s> s_coord2[NH_FIFOS];
  hls::stream<ECStream16_s> s_frac[NH_FIFOS];
  hls::stream<ECStream16_s> s_enpeak[NH_FIFOS];
  hls::stream<hitsume_t> s_hitout1[NH_FIFOS];
  hls::stream<hitsume_t> s_hitout2[NH_FIFOS];
  hls::stream<ECHit> s_hits;
  hls::stream<ECHit> s_hits1;
  hls::stream<ECHit> s_hits2;

  hls::stream<ECPeak0_s> s_peak0strip_u[NF2], s_peak0strip_v[NF2], s_peak0strip_w[NF2];
  hls::stream<ECPeak0_s> s_peak0max_u[NF3], s_peak0max_v[NF3], s_peak0max_w[NF3];
  hls::stream<sortz_t> z1[NF4];
  hls::stream<sortz_t> z2[NF4];
  hls::stream<ECPeak0_s> s_peak0_u, s_peak0_v, s_peak0_w;
  hls::stream<ECfml_t> s_first_u, s_first_v, s_first_w;
  hls::stream<ECfml_t> s_middle_u, s_middle_v, s_middle_w;
  hls::stream<ECfml_t> s_last_u, s_last_v, s_last_w;
  volatile ap_uint<1> peak_scaler_inc_u, peak_scaler_inc_v, peak_scaler_inc_w, hit_scaler_inc;

  hls::stream<trig_t> trig_stream[4];
  peak_ram_t buf_ram_u[NPEAK][256];
  peak_ram_t buf_ram_v[NPEAK][256];
  peak_ram_t buf_ram_w[NPEAK][256];
  hit_ram_t buf_ram[NHIT][256];
  hls::stream<eventdata_t> event_stream_u;
  hls::stream<eventdata_t> event_stream_v;
  hls::stream<eventdata_t> event_stream_w;
  hls::stream<eventdata_t> event_stream;

  int i, ii, jj, kk, io, nret, it, ret;

#ifdef DEBUG
  printf("ecalgetevent reached\n");
#endif

  nogoodevents = 1;
  while(nogoodevents)
  {
    printf("ENTER WHILE, nextevent=%d\n",nextevent);
    if(nextevent)
	{
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
      iev ++;
      printf("\nEVENT %5d\n\n",iev);
	}

    bufptr = (unsigned int *) buf;



    /*TEMPORARY !!!!!!!!!!!!!!!!!!!!!!!!!*/
    /*TEMPORARY !!!!!!!!!!!!!!!!!!!!!!!!!*/
    /*TEMPORARY !!!!!!!!!!!!!!!!!!!!!!!!!*/
    if(nextevent)
    {
      printf("CALLING ectrig\n");

      ectrig(bufptr, sec, npeaks_trig, peaks_trig, nhits_trig, hits_trig);

      for(ii=0; ii<3; ii++)
      {
        for(jj=0; jj<npeaks_trig[ii]; jj++)
	    {
          cout<<"TRIG PEAK ["<<+ii<<"]["<<+jj<<"]:  coord="<<peaks_trig[ii][jj].coord<<"   energy="<<peaks_trig[ii][jj].energy<<"   time="<<peaks_trig[ii][jj].time<<endl;
	    }
      }
      ii=0;
      {
        for(jj=0; jj<nhits_trig[ii]; jj++)
	    {
          cout<<"TRIG HIT ["<<+jj<<"]: coord="<<hits_trig[ii][jj].coord[0]<<" "<<hits_trig[ii][jj].coord[1]<<" "<<hits_trig[ii][jj].coord[2]<<"   energy="<<hits_trig[ii][jj].energy<<"   time="<<hits_trig[ii][jj].time<<endl;
	    }
      }
      cout<<endl;
    }

    /*TEMPORARY !!!!!!!!!!!!!!!!!!!!!!!!!*/
    /*TEMPORARY !!!!!!!!!!!!!!!!!!!!!!!!!*/
    /*TEMPORARY !!!!!!!!!!!!!!!!!!!!!!!!!*/






    //printf("DT=%d, DP=%d\n",dtimestamp,dpulsetime);



#ifdef USE_ECAL
    io = 0;    /*ECAL*/
#else
    io = 3;
#endif

    /* cleanup for drawing */
    for(ii=0; ii<3; ii++)
    {
      for(jj=0; jj<NSTRIP; jj++)
	  {
        strip[io][ii][jj].energy = 0;
	  }
    }
	
    ret = fadcs(bufptr, threshold[0], sec, io, s_fadc_words, dtimestamp, dpulsetime, &nev, &timestamp);
#ifdef TEST_BENCH
    if(1)
#else
    if(ret > 0)
#endif
    {

    /* trig will be incremented for every sector, because 'static addr' in ...eventfill is common for all sectors ..*/
    for(int i=0; i<4; i++) trig[i].t_stop = trig[i].t_start + MAXTIMES*8; /* set readout window MAXTIMES*32ns in 4ns ticks */
    for(int i=0; i<4; i++)
    {
      trig_stream[i].write(trig[i]);
    }



	/*SERGEY: stop following loop at it=6 where most clusters are to draw 6th one - have to redo that part !!!!!!!!!!!!!!!!!!!*/
	for(it=0; it</*7*/MAXTIMES; it++)
	{
	  /*FPGA*/
      ecstrips(threshold[0], s_fadc_words, s_strip0_u, s_strip0_v, s_strip0_w);

      ecstripspersistence0(EC_NFRAMES, s_strip0_u, s_strip_u);
      ecstripspersistence1(EC_NFRAMES, s_strip0_v, s_strip_v);
      ecstripspersistence2(EC_NFRAMES, s_strip0_w, s_strip_w);
      ecstripsfanout(s_strip_u, s_strip1_u, s_strip2_u);
      ecstripsfanout(s_strip_v, s_strip1_v, s_strip2_v);
      ecstripsfanout(s_strip_w, s_strip1_w, s_strip2_w);
	  /*FPGA*/

	  /* for drawing */
      ecstrip_in(s_strip1_u, str[0]);
      ecstrip_in(s_strip1_v, str[1]);
      ecstrip_in(s_strip1_w, str[2]);
      ecstrip_out(str[0], s_strip1_u);
      ecstrip_out(str[1], s_strip1_v);
      ecstrip_out(str[2], s_strip1_w);
	  /* for drawing */

	  /*FPGA*/
      ecpeak1(threshold[0], EC_STRIP_DIP_FACTOR, EC_NSTRIPMAX, s_strip1_u, s_first_u, s_middle_u, s_last_u);
      ecpeak2(threshold[1], s_strip2_u, s_first_u, s_middle_u, s_last_u, s_peak0strip_u);
      ecpeakzerosuppress(s_peak0strip_u, s_peak0max_u);
      ecpeaksort(s_peak0max_u, s_peak0_u);
      ecpeakcoord(0, s_peak0_u, s_peak_u);
      ecpeakfanout(s_peak_u, s_peak1_u, s_peak2_u, peak_scaler_inc_u);

      ecpeak1(threshold[0], EC_STRIP_DIP_FACTOR, EC_NSTRIPMAX, s_strip1_v, s_first_v, s_middle_v, s_last_v);
      ecpeak2(threshold[1], s_strip2_v, s_first_v, s_middle_v, s_last_v, s_peak0strip_v);
      ecpeakzerosuppress(s_peak0strip_v, s_peak0max_v);
      ecpeaksort(s_peak0max_v, s_peak0_v);
      ecpeakcoord(1, s_peak0_v, s_peak_v);
      ecpeakfanout(s_peak_v, s_peak1_v, s_peak2_v, peak_scaler_inc_v);

      ecpeak1(threshold[0], EC_STRIP_DIP_FACTOR, EC_NSTRIPMAX, s_strip1_w, s_first_w, s_middle_w, s_last_w);
      ecpeak2(threshold[1], s_strip2_w, s_first_w, s_middle_w, s_last_w, s_peak0strip_w);
      ecpeakzerosuppress(s_peak0strip_w, s_peak0max_w);
      ecpeaksort(s_peak0max_w, s_peak0_w);
      ecpeakcoord(2, s_peak0_w, s_peak_w);
      ecpeakfanout(s_peak_w, s_peak1_w, s_peak2_w, peak_scaler_inc_w);
	  /*FPGA*/

	  /* for drawing */
      ecpeakeventfiller0(s_peak2_u, buf_ram_u);
      ecpeakeventfiller1(s_peak2_v, buf_ram_v);
      ecpeakeventfiller2(s_peak2_w, buf_ram_w);

#ifdef DEBUG1
	  
      if(peak[0][0].energy>0)
	  {
        printf("\nU: IT=%d\n",it);
        for(ii=0; ii<NPEAK; ii++) cout<<"peakU["<<+ii<<"]: coord="<<+peak[0][ii].coord<<" energy="<<+peak[0][ii].energy<<endl;
	  }
      if(peak[1][0].energy>0)
	  {
        printf("\nV: IT=%d\n",it);
        for(ii=0; ii<NPEAK; ii++) cout<<"peakV["<<+ii<<"]: coord="<<+peak[1][ii].coord<<" energy="<<+peak[1][ii].energy<<endl;
	  }
      if(peak[2][0].energy>0)
	  {
        printf("\nW: IT=%d\n",it);
        for(ii=0; ii<NPEAK; ii++) cout<<"peakW["<<+ii<<"]: coord="<<+peak[2][ii].coord<<" energy="<<+peak[2][ii].energy<<endl;
	  }
	  
#endif

	  /* for drawing */


	  /*FPGA*/
      echit(EC_DALITZ_MIN, EC_DALITZ_MAX, s_peak1_u, s_peak1_v, s_peak1_w, s_pcount, s_energy, s_coord);
      ecenergyfanout(s_energy, s_energy1, s_energy2);
      eccoordfanout(s_coord, s_coord1, s_coord2);
      ecfrac1(s_pcount, s_energy1, s_hitout1);
      ecfrac2(s_hitout1, s_hitout2);
      ecfrac3(s_hitout2, s_frac);
      eccorr(threshold[2], s_energy2, s_coord1, s_frac, s_enpeak);

      echitsortin(s_coord2, s_enpeak, z1);
      echitsort1(z1, z2);
      echitsort2(z2, z1);
      echitsort3(z1, z2);
      echitsort4(z2, z1);
      echitsort5(z1, z2);
      echitsortout(z2, s_hits);
      echitfanout(s_hits, s_hits1, s_hits2, hit_scaler_inc);
	  /*FPGA*/

      /* for drawing */
      echiteventfiller(s_hits1, buf_ram); /* do not need to read that stream, but do it to avoid leftover data */
      echiteventfiller(s_hits2, buf_ram);


      ecpeakeventwriter(0, 0, trig_stream[0], event_stream_u, buf_ram_u);
      ecpeakeventwriter(1, 0, trig_stream[1], event_stream_v, buf_ram_v);
      ecpeakeventwriter(2, 0, trig_stream[2], event_stream_w, buf_ram_w);
      echiteventwriter(0, trig_stream[3], event_stream, buf_ram);

      ecpeakeventreader(trig_stream[0], event_stream_u, peak[0], bufout0);
      ecpeakeventreader(trig_stream[1], event_stream_v, peak[1], bufout1);
      ecpeakeventreader(trig_stream[2], event_stream_w, peak[2], bufout2);
      echiteventreader(trig_stream[3], event_stream, hit, bufout3);


      /* for drawing */

      nhits = 0;
      for(ii=0; ii<NHIT; ii++) if(hit[ii].energy>0) nhits++;
      for(ii=0; ii<nhits; ii++) cout<<"SOFT HIT: coord="<<hit[ii].coord[0]<<" "<<hit[ii].coord[1]<<" "<<hit[ii].coord[2]<<"   energy="<<hit[ii].energy<<endl;

#ifdef DEBUG1
      if( (nhits_trig[0]==1) && (hits_trig[0][0].energy>0) && (hits_trig[0][0].time == 7) && (hit[0].energy>0) )
	  {
        //printf("\nIT=%d\n",it);
        int bad = 1;
        for(ii=0; ii<nhits; ii++)
		{
          if( (hit[ii].coord[0] == hits_trig[0][0].coord[0]) 
              && (hit[ii].coord[1] == hits_trig[0][0].coord[1]) 
              && (hit[ii].coord[2] == hits_trig[0][0].coord[2])  
              && (hit[ii].energy   == hits_trig[0][0].energy)  )
		  {
            bad = 0;
            cout<<"GOOD: TRIG HIT: coord="<<hits_trig[0][0].coord[0]<<" "<<hits_trig[0][0].coord[1]<<" "<<hits_trig[0][0].coord[2]<<"   energy="<<hits_trig[0][0].energy<<endl;
            cout<<"GOOD: SOFT HIT: coord="<<hit[ii].coord[0]<<" "<<hit[ii].coord[1]<<" "<<hit[ii].coord[2]<<"   energy="<<hit[ii].energy<<endl;
		  }
          else
		  {
            cout<<"BAD: TRIG HIT: coord="<<hits_trig[0][0].coord[0]<<" "<<hits_trig[0][0].coord[1]<<" "<<hits_trig[0][0].coord[2]<<"   energy="<<hits_trig[0][0].energy<<endl;
            cout<<"BAD: SOFT HIT: coord="<<hit[ii].coord[0]<<" "<<hit[ii].coord[1]<<" "<<hit[ii].coord[2]<<"   energy="<<hit[ii].energy<<endl;
		  }
		}
        if(bad)
		{
          printf("--> MARKING BAD\n");
          test[dtimestamp][dpulsetime] = 1;
		}
	  }
#endif


      for(jj=0; jj<NSTRIP; jj++)
	  {
        strip[io][0][jj] = str[0][jj];
#ifdef USE_PCAL
        strip[io][2][jj] = str[1][jj];
        strip[io][1][jj] = str[2][jj];
#else
        strip[io][1][jj] = str[1][jj];
        strip[io][2][jj] = str[2][jj];
#endif
#ifdef DEBUG
        for(ii=0; ii<3; ii++) cout<<"INDATA ["<<io<<"]["<<ii<<"]["<<jj<<"] = "<<strip[io][ii][jj].energy<<endl;
#endif
	  }

#ifdef USE_PCAL
      for(jj=0; jj<NPEAK; jj++)
	  {
        peak_tmp = peak[1][jj];
        peak[1][jj] = peak[2][jj];
        peak[2][jj] = peak_tmp;
      }
#endif

      /* for drawing */
      /***************/







	  /*
	  if(io==0) printf("INNER\n");
      else if(io==1) printf("OUTER\n");
  	  */


	  /* CALCULATE nhits HERE !!! */
      nhits = 0;
      for(ii=0; ii<NHIT; ii++) if(hit[ii].energy>0) nhits ++;

#ifdef DEBUG
	  cout<<"ecalgetevent: sec=" << +sec << " nhits=" << +nhits << endl;
#endif
#ifdef DEBUG
	  if(io==0) printf("INNER\n");
	  else if(io==1) printf("OUTER\n");
#ifdef DEBUG
	  cout<<"ecalgetevent: nhits="<<+nhits<<endl;
#endif
      nret = 0;
      for(ii=0; ii<nhits; ii++)
	  {
        if(hit[ii].energy>0) nret ++;
#ifdef DEBUG
        if(hit[ii].energy>0) cout<<"ecalgetevent: hit["<<+ii<<"]: energy="<<+hit[ii].energy<<" coordU="<<+hit[ii].coord[0]<<" coordV="<<+hit[ii].coord[1]<<" coordW="<<+hit[ii].coord[2]<<endl;
#endif
	  }
#ifdef DEBUG
	  cout<<"ecalgetevent: nret="<<+nret<<endl;
#endif
#endif

      /* reorder hits keeping non-zero hits only */
      nret=0;
      for(ii=0; ii<nhits; ii++)
	  {
        if(hit[ii].energy>0)
	    {
          hitdraw[nret].energy    = hit[ii].energy;
          hitdraw[nret].coord[0]  = hit[ii].coord[0];
#ifdef USE_PCAL
          hitdraw[nret].coord[2]  = hit[ii].coord[1];
          hitdraw[nret].coord[1]  = hit[ii].coord[2];
#else
          hitdraw[nret].coord[1]  = hit[ii].coord[1];
          hitdraw[nret].coord[2]  = hit[ii].coord[2];
#endif

#ifdef USE_PCAL
          hitdraw[nret].ind       = ((V4(hit[ii].ind))<<4) + ((W4(hit[ii].ind))<<2) + (U4(hit[ii].ind));
#ifdef DEBUG
		  cout<<"++++++++++ind++++++++++++++++++ "<<hit[ii].ind<<"   "<<hitdraw[nret].ind<<endl;
#endif
#else
          hitdraw[nret].ind       = hit[ii].ind;
#endif

          hitdraw[nret].enpeak[0] = hit[ii].enpeak[0];
#ifdef USE_PCAL
          hitdraw[nret].enpeak[2] = hit[ii].enpeak[1];
          hitdraw[nret].enpeak[1] = hit[ii].enpeak[2];
#else
          hitdraw[nret].enpeak[1] = hit[ii].enpeak[1];
          hitdraw[nret].enpeak[2] = hit[ii].enpeak[2];
#endif
          nret++;
		}
	  }
      nhits = nret;
#ifdef DEBUG
	  cout<<"ecalgetevent(final): nhits="<<+nhits<<endl;
      for(ii=0; ii<nhits; ii++)
	  {
        cout<<"ecalgetevent(final): hitdraw["<<+ii<<"]: energy="<<+hitdraw[ii].energy<<" coordU="<<+hitdraw[ii].coord[0]<<" coordV="<<+hitdraw[ii].coord[1]<<" coordW="<<+hitdraw[ii].coord[2];
        cout<<" enU="<<+hitdraw[ii].enpeak[0]<<" enV="<<+hitdraw[ii].enpeak[1]<<" enW="<<+hitdraw[ii].enpeak[2];
        cout<<" peakU="<<+(U4(hitdraw[ii].ind))<<" peakV="<<+(V4(hitdraw[ii].ind))<<" peakW="<<+(W4(hitdraw[ii].ind))<<endl;
	  }
#endif
      goodevent=1;


	} /* it */

    for(int i=0; i<4; i++) trig[i].t_start += MAXTIMES*8; /* in preparation for next event, step up MAXTIMES*32ns in 4ns ticks */


    }
    else
	{
#ifdef DEBUG
      printf("==================================================================== ecalgetevent: there is no data\n");
#endif
      nhits = 0;
      goodevent = 0;
	}


    nogoodevents=0;

    printf("EXIT WHILE\n");

  } /*while(nogoodevents)*/

#ifdef DEBUG
  printf("ecalgetevent done, iev=%d, nhits=%d\n",iev,nhits);
#endif

  return(nhits);
}







/******************************/
/* main function 'ecalview()' */
/******************************/

static int ifirst = 1;

#define STRLEN 128


int
ecalview(int handler, TCanvas *fCanvas, int redraw)
{
#if 0
#ifdef USE_PCAL
  int sec = 1; /* 1 for sector 2  (all simulation is sector 2) */
#else
  int sec = 0; /* 1 for sector 2  (all simulation is sector 2) */
#endif
#endif

  /*GEMC */
  /*int sec=5;*/ /* /work/boiarino/data/vtp1_001294.evio.0 - #define USE_PCAL !!!!!!!! */
  int sec=1; /* /work/boiarino/data/ec_pcal_TCS_1_.evio - #undef USE_PCAL !!!!!!!! */


  TArc *circle = new TArc();

  int ret;

#ifdef DEBUG
  printf("ecalview reached\n");fflush(stdout);
#endif

  /*should do it once*/
  /* if(first) */
  {
    ifirst = 0;
  }



  if(handler > 0)
  {
readnext:

	{
      int nextevent = 1;
      int rrr = 1;
      for(int dtimestamp = TIMESTAMP1; dtimestamp<TIMESTAMP2; dtimestamp++)
	  {
		for(int dpulsetime = PULSETIME1; dpulsetime<PULSETIME2; dpulsetime++)
		{
          ret = ecalgetevent(handler, fCanvas, sec, dtimestamp, dpulsetime, nextevent);
          nextevent = 0;
		}
      }

      for(int dtimestamp = TIMESTAMP1; dtimestamp<TIMESTAMP2; dtimestamp++)
      for(int dpulsetime = PULSETIME1; dpulsetime<PULSETIME2; dpulsetime++)
      {
        printf("TEST[%d][%d]=%d\n",dtimestamp,dpulsetime,test[dtimestamp][dpulsetime]);
        if(test[dtimestamp][dpulsetime]==0) rrr = 0;
      }
      printf("RRR=%d\n\n",rrr);

	}

    /*if(ret==0) goto readnext;*//*return(0);*/ /* no hits */
    if(ret==EOF) return(EOF);
  }

  if(redraw) ecaldrawevent(fCanvas);
  /*sleep(3);*/


#ifdef DEBUG
  printf("ecalview done, iev=%d\n",iev);fflush(stdout);
#endif

  return(0);
}
