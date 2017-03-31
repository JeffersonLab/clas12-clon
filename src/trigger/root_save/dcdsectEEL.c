/*
   dcdsectEEL.c - draw sector geometry, tracks and hits

   Inputs  : isec  - sector number
             sxy   - scale factor
             x0,y0 - position in HIGZ window of size 20x20 cm2
*/

#define NOFFSETS 16
#define NWIRES 112



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "prlib.h"

#include "evio.h"
#include "eclib.h"
#include "pclib.h"

#define ABS(x) ((x) < 0 ? -(x) : (x))

#define nsup          6
#define nlay          6
#define max_nwire   194
#define max_nhit 36*192
#define USE_ROOT


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

#define REDRAW \
  if(fCanvas>0) \
  { \
    fCanvas->Modified(); \
    fCanvas->Update(); \
  }


extern "C" void lfitw_(float *X, float *Y, float *W, int *L, int *KEY, float *A, float *B, float *E);

#include <stdio.h>







/*
 #include "linreg.h"
*/
    #include <stdlib.h>
    #include <math.h>                           /* math functions                */

    //#define REAL float
    #define REAL double


    inline static REAL sqr(REAL x) {
        return x*x;
    }



int linreg(int n, const REAL x[], const REAL y[], REAL* m, REAL* b, REAL* r)
    {
        REAL   sumx = 0.0;                        /* sum of x                      */
        REAL   sumx2 = 0.0;                       /* sum of x**2                   */
        REAL   sumxy = 0.0;                       /* sum of x * y                  */
        REAL   sumy = 0.0;                        /* sum of y                      */
        REAL   sumy2 = 0.0;                       /* sum of y**2                   */

       for (int i=0;i<n;i++)   
          { 
          sumx  += x[i];       
          sumx2 += sqr(x[i]);  
          sumxy += x[i] * y[i];
          sumy  += y[i];      
          sumy2 += sqr(y[i]); 
          } 

       REAL denom = (n * sumx2 - sqr(sumx));
       if (denom == 0) {
           // singular matrix. can't solve the problem.
           *m = 0;
           *b = 0;
           *r = 0;
           return 1;
       }

       *m = (n * sumxy  -  sumx * sumy) / denom;
       *b = (sumy * sumx2  -  sumx * sumxy) / denom;
       if (r!=NULL) {
          *r = (sumxy - sumx * sumy / n) /          /* compute correlation coeff     */
                sqrt((sumx2 - sqr(sumx)/n) *
                (sumy2 - sqr(sumy)/n));
       }

       return 0; 
    }





void
mylfitw_(float *X, float *Y, float *W, int *L, int *KEY, float *A, float *B, float *E)
{
#if 0
  int i, n;
  REAL x[6], y[6], m, b, r;
  n = *L;
  for(i=0; i<n; i++)
  {
    x[i] = X[i];
    y[i] = Y[i];
  }
  linreg(n, x, y, &m, &b, &r);
  *A = m;
  *B = b;

  *E = r;
#endif

#if 0
  lfitw_(X, Y, W, L, KEY, A, B, E);
  return;
#endif

#if 1
  float W2, W2X, W2Y, W2XY, W2X2, W2Y2, WW, WWF, WWFI, tmp;
  int ICNT, J;

  /*check input parameters*/
  if(*L<=1)
  {
    *A=0.;
    *B=0.;
    *E=0.;
    return;
  }

  /*CALCULTE SUMS*/
  W2=0.;
  W2X=0.;
  W2Y=0.;
  W2XY=0.;
  W2X2=0.;
  W2Y2=0.;
  ICNT=0;

  for(J=0; J<(*L); J++)
  {
    if((Y[J] == 0.) && (*KEY == 0)) continue; /*ignore points with y=0.*/
    WW=W[J]*W[J];
    W2=WW+W2;
    WWF=WW*X[J];
    W2X=WWF+W2X;
    W2X2=WWF*X[J]+W2X2;
    W2XY=WWF*Y[J]+W2XY;
    WWFI=WW*Y[J];
    W2Y=WWFI+W2Y;
    W2Y2=WWFI*Y[J]+W2Y2;
    ICNT ++;
  }

  /*FIT PARAMETERS*/
  *A = (W2XY - W2X*W2Y/W2)/(W2X2 - W2X*W2X/W2);
  *B = (W2Y - (*A)*W2X)/W2;
  if(ICNT<=2)
  {
    *E = 0.;
    return;
  }
  tmp = W2XY-W2X*W2Y/W2;
  *E = (W2Y2-W2Y*W2Y/W2-tmp*tmp/(W2X2-W2X*W2X/W2))/(ICNT-2);
  /*
  printf("ICNT=%d\n",ICNT);
  printf("tmp=%f *E=%f\n",tmp,*E);
  */
  return;
#endif
}




static int ihits[6][16];
void
dcoutput()
{
  int ii, jj;
  printf("\n");
  for(ii=5; ii>=0; ii--)
  {
    printf("%1d==>",ii);
	for(jj=0; jj<16; jj++)
	{
      printf("%5d",ihits[ii][jj]);
	}
    printf("\n");
  }
}




#define RADDEG 57.2957795130823209
#define DEGRAD  0.0174532925199432958
#define NANGLES 50

/*
#define NHIST 100
static UThistf histf[NHIST];
*/

/* try to find time-based segments by rotated coordinate systems */
void
dcpr1(TCanvas *fCanvas, int nnn[12], int iww[12][192],
         float xxx[12][192], float yyy[12][192], float ttt[12][192])
{
  float A, B, E, Eold;
  int L = 6, KEY = 1;
  static int first = 1;
  int id, nbinx;
  char title[16];
  float xminf, xmaxf;
  float x, xrot, y=0., weight=1.;
  float W[12] = {1.0,1.0,1.0,1.0,1.0,1.0,
                 0.01,0.01,0.01,0.01,0.01,0.01};
  static float xxsin[NANGLES],xxcos[NANGLES];

  Int_t iw[6]={0,0,0,0,0,0};
  Float_t xx[6]={0.,0.,0.,0.,0.,0.}, yy[6]={0.,0.,0.,0.,0.,0.}, tt[6]={0.,0.,0.,0.,0.,0.};
  Float_t xt[6]={0.,0.,0.,0.,0.,0.}, yt[6]={0.,0.,0.,0.,0.,0.};
  Int_t iwww[6];
  Float_t xxxx[2], yyyy[2], xsin, xcos;
  Int_t ii, jj, kk, l0, l1, l2, l3, l4, l5, ll0, ll1, ll2, ll3, ll4, ll5;

  if(first)
  {
    first = 0;


	/**************
    uthsetlocal(histf);
    id = 1;
    sprintf(title,"angle%d",0);
    nbinx = 200;
    xminf=2.5;
    xmaxf=5.0;
    uthbook1(histf, id, title, nbinx, xminf, xmaxf);
	*************/

    for(ii=0; ii<NANGLES; ii++)
    {
      xxsin[ii] = sinf( ((float)(ii)) * DEGRAD );
      xxcos[ii] = cosf( ((float)(ii)) * DEGRAD );
      printf("angle=%d sin=%f cos=%f\n",ii,xxsin[ii],xxcos[ii]);
    }
  }

  printf("dcpr1 reached\n");fflush(stdout);

  double xmin, ymin, xmax, ymax;
  if(fCanvas>0)
  {
    fCanvas->GetRange(xmin,ymin,xmax,ymax);
  }
  else
  {
    xmin=ymin=0.;
    xmax=ymax=20.;
  }


  
  printf("range: xmin,ymin=%f %f   xmax,ymax=%f %f\n",xmin,ymin,xmax,ymax);
  for(ii=0; ii<12; ii++)
  {
    printf("layer ii=%d, nhits nnn=%d\n",ii,nnn[ii]);
    for(jj=0; jj<nnn[ii]; jj++)
    {
      printf("hit=%d xxx=%f yyy=%f ttt=%f\n",
	     jj,xxx[ii][jj],yyy[ii][jj],ttt[ii][jj]);
    }
  }
  

  /* select hits within window, remove the rest */
  for(ii=0; ii<6; ii++)
  {
    kk = nnn[ii];
    for(jj=0; jj<nnn[ii]; jj++)
    {
      if(xxx[ii][jj]<xmin || xxx[ii][jj]>xmax || yyy[ii][jj]<ymin || yyy[ii][jj]>ymax)
      {
        iww[ii][jj] = iww[ii][kk-1];
        xxx[ii][jj] = xxx[ii][kk-1];
        yyy[ii][jj] = yyy[ii][kk-1];
        ttt[ii][jj] = ttt[ii][kk-1];
        kk--;
      }
    }
    if(kk<0) kk = 0;
    nnn[ii] = kk;
  }




  /*
  Xn = X0*cos() + Y0*sin();
  Yn = - X0*sin() + Y0*cos()

  xsin = sin(atan(A));
  xcos = cos(atan(A));
  xt[0] = xx[0] + tt[0]*xsin;
  yt[0] = yy[0] - tt[0]*xcos;
  */



  /***************
  for(kk=0; kk<NANGLES; kk++)
  {
    for(ii=0; ii<12; ii++)
    {
      for(jj=0; jj<nnn[ii]; jj++)
      {
        xrot = xxx[ii][jj]*xxcos[kk] + yyy[ii][jj]*xxsin[kk];
        x = xrot + ttt[ii][jj];
        uthfill(histf, id, x, 0., 1.);
        x = xrot - ttt[ii][jj];
        uthfill(histf, id, x, 0., 1.);
      }
    }
    uthreset(histf, id, "");
  }
  ******************/



  /* fit over all hit combinations within window */
  for(ii=0; ii<6; ii++)
  {
    iw[ii] = xx[ii] = yy[ii] = 0.;
  }
  Eold = 1000.;


  for(l0=0; l0<nnn[0]; l0++)
  {
    iw[0] = iww[0][l0];
    xx[0] = xxx[0][l0];
    yy[0] = yyy[0][l0];
    tt[0] = ttt[0][l0];
    for(l1=0; l1<nnn[1]; l1++)
    {
      iw[1] = iww[1][l1];
      xx[1] = xxx[1][l1];
      yy[1] = yyy[1][l1];
      tt[1] = ttt[1][l1];
      for(l2=0; l2<nnn[2]; l2++)
      {
        iw[2] = iww[2][l2];
        xx[2] = xxx[2][l2];
        yy[2] = yyy[2][l2];
        tt[2] = ttt[2][l2];
        for(l3=0; l3<nnn[3]; l3++)
	    {
          iw[3] = iww[3][l3];
          xx[3] = xxx[3][l3];
          yy[3] = yyy[3][l3];
          tt[3] = ttt[3][l3];
          for(l4=0; l4<nnn[4]; l4++)
	      {
            iw[4] = iww[4][l4];
            xx[4] = xxx[4][l4];
            yy[4] = yyy[4][l4];
            tt[4] = ttt[4][l4];
            for(l5=0; l5<nnn[5]; l5++)
            {
              iw[5] = iww[5][l5];
              xx[5] = xxx[5][l5];
              yy[5] = yyy[5][l5];
              tt[5] = ttt[5][l5];

              /*hit-based fit*/
              mylfitw_(xx, yy, W, &L, &KEY, &A, &B, &E);
	      /*
              printf("hit lfitw: x=%f %f %f %f %f %f, y=%f %f %f %f %f %f, a=%f b=%f e=%f\n",
                xx[0],xx[1],xx[2],xx[3],xx[4],xx[5],
                yy[0],yy[1],yy[2],yy[3],yy[4],yy[5],
                A,B,E);
	      */

              /*draw fit results*/
              /*
              xxxx[0] = xx[0]-0.03;
              xxxx[1] = xx[5]+0.03;
              yyyy[0] = A*xxxx[0]+B;
              yyyy[1] = A*xxxx[1]+B;
              printf("draw hit-based fit: x0,y0=%f %f  x1,y1=%f %f\n",
                xxxx[0],yyyy[0],xxxx[1],yyyy[1]);
              TPolyLine* pl = new TPolyLine(2, xxxx, yyyy);
              pl->SetLineColor(kGreen);
              pl->Draw();
              */

              /*doca*/
              xsin = sin(atan(A));
              xcos = cos(atan(A));
	      /*
              printf("tangens=%f angle=%f cos=%f sin=%f\n",
                A,atan(A),cos(atan(A)),sin(atan(A)));
	      */

              /*time-based fit - trying both sides of drift cirlces*/
              for(ll0=0; ll0<2; ll0++)
              for(ll1=0; ll1<2; ll1++)
              for(ll2=0; ll2<2; ll2++)
              for(ll3=0; ll3<2; ll3++)
              for(ll4=0; ll4<2; ll4++)
              for(ll5=0; ll5<2; ll5++)
              {
                if(ll0==0)
                {
                  xt[0] = xx[0] + tt[0]*xsin;
                  yt[0] = yy[0] - tt[0]*xcos;
                }
                else
                {
                  xt[0] = xx[0] - tt[0]*xsin;
                  yt[0] = yy[0] + tt[0]*xcos;
                }

                if(ll1==0)
                {
                  xt[1] = xx[1] + tt[1]*xsin;
                  yt[1] = yy[1] - tt[1]*xcos;
                }
                else
                {
                  xt[1] = xx[1] - tt[1]*xsin;
                  yt[1] = yy[1] + tt[1]*xcos;
                }

                if(ll2==0)
                {
                  xt[2] = xx[2] + tt[2]*xsin;
                  yt[2] = yy[2] - tt[2]*xcos;
                }
                else
                {
                  xt[2] = xx[2] - tt[2]*xsin;
                  yt[2] = yy[2] + tt[2]*xcos;
                }

                if(ll3==0)
                {
                  xt[3] = xx[3] + tt[3]*xsin;
                  yt[3] = yy[3] - tt[3]*xcos;
                }
                else
                {
                  xt[3] = xx[3] - tt[3]*xsin;
                  yt[3] = yy[3] + tt[3]*xcos;
                }

                if(ll4==0)
                {
                  xt[4] = xx[4] + tt[4]*xsin;
                  yt[4] = yy[4] - tt[4]*xcos;
                }
                else
                {
                  xt[4] = xx[4] - tt[4]*xsin;
                  yt[4] = yy[4] + tt[4]*xcos;
                }

                if(ll5==0)
                {
                  xt[5] = xx[5] + tt[5]*xsin;
                  yt[5] = yy[5] - tt[5]*xcos;
                }
                else
                {
                  xt[5] = xx[5] - tt[5]*xsin;
                  yt[5] = yy[5] + tt[5]*xcos;
                }

                mylfitw_(xt, yt, W, &L, &KEY, &A, &B, &E);
				/*
                printf("tim lfitw: x=%f %f %f %f %f %f, y=%f %f %f %f %f %f, a=%f b=%f e=%f\n",
                  xt[0],xt[1],xt[2],xt[3],xt[4],xt[5],
                  yt[0],yt[1],yt[2],yt[3],yt[4],yt[5],
                  A,B,E);
				*/
                if(fCanvas>0)
                {
                  IPM(L, xt, yt);
                }
                /*remember best(so far) fit results*/
                if(E < Eold)
                {
                  Eold = E;
				  for(kk=0;kk<6; kk++)
				  {
                    iwww[kk] = iw[kk]; 
				  }
                  xxxx[0] = xt[0]-0.03;
                  xxxx[1] = xt[5]+0.03;
                  yyyy[0] = A*xxxx[0]+B;
                  yyyy[1] = A*xxxx[1]+B;
                }
              }
            }
          }
        }
      }
    }
  }

  /*draw fit results*/



  if(fCanvas)
  {
    printf("draw time-based fit: x0,y0=%f %f  x1,y1=%f %f  wires %d %d %d %d %d %d Eold=%f\n",
	   xxxx[0],yyyy[0],xxxx[1],yyyy[1],iwww[0],iwww[1],iwww[2],iwww[3],iwww[4],iwww[5],Eold);
  }


  if(Eold < 0.01) /* found something */
  {
    for(kk=0; kk<6; kk++) ihits[kk][iwww[kk]]++;
    /*dcoutput();*/
  }


  if(fCanvas>0)
  {
    TPolyLine* pl = new TPolyLine(2, xxxx, yyyy);
    pl->SetLineColor(kRed);
    pl->Draw();
	/*IPM()*/

  }


  return;
}







#include <stdio.h>
#include <termios.h>
#include <unistd.h>


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



void
dclfitsl(TCanvas *fCanvas, int sl, int nnn[12], int iww[12][192],
         float xxx[12][192], float yyy[12][192], float ttt[12][192])
{
  float A, B, E, Eold;
  int L = 6, KEY = 1;
  float W[12];

  /*printf("dclfitsl reached\n");fflush(stdout);*/

  double xmin, ymin, xmax, ymax;
  int offset = 6*sl;

  if(fCanvas>0)
  {
    fCanvas->GetRange(xmin,ymin,xmax,ymax);
  }
  else
  {
	xmin=ymin=0.;
    xmax=ymax=20.;
  }

  int iww_in_range[6][192];
  float xxx_in_range[6][192], yyy_in_range[6][192], ttt_in_range[6][192];

  Int_t iw[6]={0,0,0,0,0,0};
  Float_t xx[6]={0.,0.,0.,0.,0.,0.}, yy[6]={0.,0.,0.,0.,0.,0.}, tt[6]={0.,0.,0.,0.,0.,0.};
  Float_t xt[6]={0.,0.,0.,0.,0.,0.}, yt[6]={0.,0.,0.,0.,0.,0.};
  Int_t iwww[6];
  Float_t xxxx[2], yyyy[2], xsin, xcos;
  Int_t ii, jj, kk, l0, l1, l2, l3, l4, l5, ll0, ll1, ll2, ll3, ll4, ll5;


  /* fills weights */
  for(ii=0; ii<12; ii++)
  {
    W[ii] = 1.0;
  }


  printf("dclfitsl: all hits in sl=%d:\n",sl);
  for(ii=0; ii<6; ii++)
  {
    printf("  dclfitsl: nnn=%d\n",nnn[ii+offset]);
    for(jj=0; jj<nnn[ii+offset]; jj++)
    {
      printf("  dclfitsl: lay=%d hit=%d xxx=%f yyy=%f ttt=%f\n",
	     ii,nnn[ii+offset],xxx[ii+offset][jj],yyy[ii+offset][jj],ttt[ii+offset][jj]);
    }
  }



  /* select hits within window, remove the rest */
  for(ii=0; ii<6; ii++)
  {
    kk = 0;
    for(jj=0; jj<nnn[ii+offset]; jj++)
    {
      if(xxx[ii+offset][jj]>xmin && xxx[ii+offset][jj]<xmax && yyy[ii+offset][jj]>ymin && yyy[ii+offset][jj]<ymax)
      {
        iww_in_range[ii][kk] = iww[ii+offset][jj];
        xxx_in_range[ii][kk] = xxx[ii+offset][jj];
        yyy_in_range[ii][kk] = yyy[ii+offset][jj];
        ttt_in_range[ii][kk] = ttt[ii+offset][jj];
        kk++;
      }
    }
    nnn[ii+offset] = kk;
    for(jj=0; jj<kk; jj++)
	{
      iww[ii+offset][jj] = iww_in_range[ii][jj];
      xxx[ii+offset][jj] = xxx_in_range[ii][jj];
      yyy[ii+offset][jj] = yyy_in_range[ii][jj];
      ttt[ii+offset][jj] = ttt_in_range[ii][jj];
	}
  }

  printf("dclfitsl: range: xmin,ymin=%f %f   xmax,ymax=%f %f\n",xmin,ymin,xmax,ymax);
  printf("dclfitsl: hits within the range:\n");
  for(ii=0; ii<6; ii++)
  {
    printf("  dclfitsl: nnn=%d\n",nnn[ii+offset]);
    for(jj=0; jj<nnn[ii+offset]; jj++)
    {
      printf("  dclfitsl: lay=%d hit=%d xxx=%f yyy=%f ttt=%f\n",
	     ii,nnn[ii+offset],xxx[ii+offset][jj],yyy[ii+offset][jj],ttt[ii+offset][jj]);
    }
  }





  /* fit over all hit combinations within window */
  for(ii=0; ii<6; ii++)
  {
    iw[ii] = xx[ii] = yy[ii] = 0.;
  }
  Eold = 1000.;


  for(l0=0; l0<nnn[0+offset]; l0++)
  {
    iw[0] = iww[0+offset][l0];
    xx[0] = xxx[0+offset][l0];
    yy[0] = yyy[0+offset][l0];
    tt[0] = ttt[0+offset][l0];
    for(l1=0; l1<nnn[1+offset]; l1++)
    {
      iw[1] = iww[1+offset][l1];
      xx[1] = xxx[1+offset][l1];
      yy[1] = yyy[1+offset][l1];
      tt[1] = ttt[1+offset][l1];
      for(l2=0; l2<nnn[2+offset]; l2++)
      {
        iw[2] = iww[2+offset][l2];
        xx[2] = xxx[2+offset][l2];
        yy[2] = yyy[2+offset][l2];
        tt[2] = ttt[2+offset][l2];
        for(l3=0; l3<nnn[3+offset]; l3++)
	    {
          iw[3] = iww[3+offset][l3];
          xx[3] = xxx[3+offset][l3];
          yy[3] = yyy[3+offset][l3];
          tt[3] = ttt[3+offset][l3];
          for(l4=0; l4<nnn[4+offset]; l4++)
	      {
            iw[4] = iww[4+offset][l4];
            xx[4] = xxx[4+offset][l4];
            yy[4] = yyy[4+offset][l4];
            tt[4] = ttt[4+offset][l4];
            for(l5=0; l5<nnn[5+offset]; l5++)
            {
              iw[5] = iww[5+offset][l5];
              xx[5] = xxx[5+offset][l5];
              yy[5] = yyy[5+offset][l5];
              tt[5] = ttt[5+offset][l5];


              /***************/
              /*HIT-BASED FIT*/
              /***************/

              mylfitw_(xx, yy, W, &L, &KEY, &A, &B, &E);
	      /*
              printf("hit lfitw: x=%f %f %f %f %f %f, y=%f %f %f %f %f %f, a=%f b=%f e=%f\n",
                xx[0],xx[1],xx[2],xx[3],xx[4],xx[5],
                yy[0],yy[1],yy[2],yy[3],yy[4],yy[5],
                A,B,E);
	      */

              /*draw fit results*/
              
                  yyyy[0] = yt[0]-0.03;
                  yyyy[1] = yt[5]+0.03;
                  xxxx[0] = (yyyy[0]-B)/A;
                  xxxx[1] = (yyyy[1]-B)/A;
				  /*
              xxxx[0] = xx[0]-0.03;
              xxxx[1] = xx[5]+0.03;
              yyyy[0] = A*xxxx[0]+B;
              yyyy[1] = A*xxxx[1]+B;
				  */
              printf("draw hit-based fit: x0,y0=%f %f  x1,y1=%f %f\n",
                xxxx[0],yyyy[0],xxxx[1],yyyy[1]);
              TPolyLine* pl = new TPolyLine(2, xxxx, yyyy);
              pl->SetLineColor(kGreen);
              pl->Draw();
              

    printf("Press any key to continue.\n");
    mygetch();
    printf("Continue.\n");

              /*doca*/
              xsin = sin(atan(A));
              xcos = cos(atan(A));
			  /*
              printf("tangens=%f angle=%f cos=%f sin=%f\n",
                A,atan(A),cos(atan(A)),sin(atan(A)));
	      */

              /*time-based fit - trying both sides of drift cirlces*/
              for(ll0=0; ll0<2; ll0++)
              for(ll1=0; ll1<2; ll1++)
              for(ll2=0; ll2<2; ll2++)
              for(ll3=0; ll3<2; ll3++)
              for(ll4=0; ll4<2; ll4++)
              for(ll5=0; ll5<2; ll5++)
              {
                if(ll0==0)
                {
                  xt[0] = xx[0] + tt[0]*xsin;
                  yt[0] = yy[0] - tt[0]*xcos;
                }
                else
                {
                  xt[0] = xx[0] - tt[0]*xsin;
                  yt[0] = yy[0] + tt[0]*xcos;
                }

                if(ll1==0)
                {
                  xt[1] = xx[1] + tt[1]*xsin;
                  yt[1] = yy[1] - tt[1]*xcos;
                }
                else
                {
                  xt[1] = xx[1] - tt[1]*xsin;
                  yt[1] = yy[1] + tt[1]*xcos;
                }

                if(ll2==0)
                {
                  xt[2] = xx[2] + tt[2]*xsin;
                  yt[2] = yy[2] - tt[2]*xcos;
                }
                else
                {
                  xt[2] = xx[2] - tt[2]*xsin;
                  yt[2] = yy[2] + tt[2]*xcos;
                }

                if(ll3==0)
                {
                  xt[3] = xx[3] + tt[3]*xsin;
                  yt[3] = yy[3] - tt[3]*xcos;
                }
                else
                {
                  xt[3] = xx[3] - tt[3]*xsin;
                  yt[3] = yy[3] + tt[3]*xcos;
                }

                if(ll4==0)
                {
                  xt[4] = xx[4] + tt[4]*xsin;
                  yt[4] = yy[4] - tt[4]*xcos;
                }
                else
                {
                  xt[4] = xx[4] - tt[4]*xsin;
                  yt[4] = yy[4] + tt[4]*xcos;
                }

                if(ll5==0)
                {
                  xt[5] = xx[5] + tt[5]*xsin;
                  yt[5] = yy[5] - tt[5]*xcos;
                }
                else
                {
                  xt[5] = xx[5] - tt[5]*xsin;
                  yt[5] = yy[5] + tt[5]*xcos;
                }


                /****************/
                /*TIME-BASED FIT*/
                /****************/

                mylfitw_(xt, yt, W, &L, &KEY, &A, &B, &E);
				
                printf("tim lfitw: x=%f %f %f %f %f %f, y=%f %f %f %f %f %f, a=%f b=%f e=%f\n",
                  xt[0],xt[1],xt[2],xt[3],xt[4],xt[5],
                  yt[0],yt[1],yt[2],yt[3],yt[4],yt[5],
                  A,B,E);
				
                if(fCanvas>0)
                {
                  IPM(L, xt, yt);
                }
                /*remember best(so far) fit results*/
                if(E < Eold)
                {
                  Eold = E;
				  for(kk=0;kk<6; kk++)
				  {
                    iwww[kk] = iw[kk]; 
				  }
				  /*
                  xxxx[0] = xt[0]-0.03;
                  xxxx[1] = xt[5]+0.03;
                  yyyy[0] = A*xxxx[0]+B;
                  yyyy[1] = A*xxxx[1]+B;
				  */
                  yyyy[0] = yt[0]-0.03;
                  yyyy[1] = yt[5]+0.03;
                  xxxx[0] = (yyyy[0]-B)/A;
                  xxxx[1] = (yyyy[1]-B)/A;
                }
              }
            }
          }
        }
      }
    }
  }

  /*draw fit results*/



  if(fCanvas)
  {
    printf("draw time-based fit: (x0,y0)=%f %f  (x1,y1)=%f %f  wires L1=%d L2=%d L3=%d L4=%d L5=%d L6=%d Eold=%f\n",
		   xxxx[0],yyyy[0],xxxx[1],yyyy[1],iwww[0],iwww[1],iwww[2],iwww[3],iwww[4],iwww[5],Eold);
  }


  if(Eold < 0.01) /* found something */
  {
    for(kk=0; kk<6; kk++) ihits[kk][iwww[kk]]++;
    /*dcoutput();*/
  }


  if(fCanvas>0)
  {
    TPolyLine* pl = new TPolyLine(2, xxxx, yyyy);
    pl->SetLineColor(kRed);
    pl->Draw();
  }


  return;
}








/* ST <-> AX*/

static int board_layer[96] = {
  2, 4, 6, 1, 3, 5, 2, 4, 6, 1, 3, 5, 2, 4, 6, 1,
  3, 5, 2, 4, 6, 1, 3, 5, 2, 4, 6, 1, 3, 5, 2, 4,
  6, 1, 3, 5, 2, 4, 6, 1, 3, 5, 2, 4, 6, 1, 3, 5,
  2, 4, 6, 1, 3, 5, 2, 4, 6, 1, 3, 5, 2, 4, 6, 1,
  3, 5, 2, 4, 6, 1, 3, 5, 2, 4, 6, 1, 3, 5, 2, 4,
  6, 1, 3, 5, 2, 4, 6, 1, 3, 5, 2, 4, 6, 1, 3, 5
};

static int board_wire[96] = {
  1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3,
  3, 3, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 6, 6,
  6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8,
  9, 9, 9, 9, 9, 9,10,10,10,10,10,10,11,11,11,11,
 11,11,12,12,12,12,12,12,13,13,13,13,13,13,14,14,
 14,14,14,14,15,15,15,15,15,15,16,16,16,16,16,16
};

static int superlayer[21][96];
static int layer[21][96];
static int wire[21][96];


#define ABS(x) ((x) < 0 ? -(x) : (x))


#define DC_DATA_BLOCK_HEADER      0x00
#define DC_DATA_BLOCK_TRAILER     0x01
#define DC_DATA_EVENT_HEADER      0x02
#define DC_DATA_TRIGGER_TIME      0x03
#define DC_DATA_DCRBEVT           0x08
#define DC_DATA_INVALID           0x0E
#define DC_DATA_FILLER            0x0F


#define NCHAN 256 /*how many channels to draw*/



static int goodevent;

#define MAXBUF 1000000
unsigned int buf[MAXBUF];


extern "C" int pcl3(unsigned int *bufptr, const unsigned short threshold[3], int io, PCHit hit[PHIT]);

int
dcgetevent(int handler, int dcndig[36], int dcdigi[36][192][2], unsigned char sl[2][NOFFSETS][112])
{
  int status, ifpga, nchannels, nogoodevents;

  int trig,chan,fpga,apv,hybrid;
  int i1, type, timestamp_flag;
  float f1,f2;
  unsigned int word, *gsegm;

  int nhitp;
  int *bufptr;
  const unsigned short threshold[3] = {1,1,3};
  int io = 0;
  PCHit hitout[PHIT];

  int ii, ret, sec, uvw, npsble;
  PCStrip strip[3][PSTRIP];
  int npeak[3][3];
  uint8_t nhits;
  PCPeak peak[3][3][PPEAK];
  PCHit hit[1000];
  uint16_t peakfrac[3][PPEAK];

  int nr,nl,ncol,nrow,i,j, k, jj,kk,l,l1,l2,ichan,nn,iev,nbytes,ind,ind1;
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



    ret = 0;
    for(sec=0; sec<PSECTOR; sec++)
    {
      if(pcstrips(bufptr, threshold[0], sec, strip) > 0)
      {
        ret = pcal(strip, &nhits, &hit[0]);
        for(ii=0; ii<nhits; ii++) hitout[ii] = hit[ii];

	    printf("PCAL\n");
	    printf("pcl3: ret=%d nhits=%d\n",ret,nhits);
        for(ii=0; ii<nhits; ii++)
	    {
          printf("pcl3: hit[%d]: energy=%d time=%d coordU=%d coordV=%d coordW=%d\n",ii,
			     hit[ii].energy,  /*hit[ii].time*/0,  hit[ii].coord[0],hit[ii].coord[1],hit[ii].coord[2]);
	    }
      }
    }


    nhitp = ret;
    goodevent=1;
    nogoodevents=0;




#if 0 /*ABC*/


goto qwerty;

    ind1 = evNlink(buf, 42, 0xe105, 42, &nbytes);
#ifdef DEBUG
      printf("ind1(1)=%d\n",ind1);
#endif
    if(ind1 <= 0) ind1 = evNlink(buf, 67, 0xe105, 67, &nbytes);
#ifdef DEBUG
    printf("ind1(2)=%d\n",ind1);
#endif
    if(ind1 > 0)
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
      printf("ind1=%d, nbytes=%d\n",ind1,nbytes);fflush(stdout);
#endif
      start = b08 = (unsigned char *) &buf[ind1];
      end = b08 + nbytes;
#ifdef DEBUG
      printf("ind1=%d, nbytes=%d (from 0x%08x to 0x%08x)\n",ind1,nbytes,b08,end);fflush(stdout);
#endif

      goodevent = 0;
      nhits = 0;
      for(i=0; i<21; i++) for(j=0; j<96; j++) dcrb_check[i][j] = 0;


goto a123;

      /*TEMP 1290*/
      GET32(word);
      nw = word;
#ifdef DEBUG
      printf("TDC (0x%08x) nw=%d\n",word,nw);
#endif
      for(jj=1; jj<nw; jj++)
      {
        GET32(word);
        chan = (word>>21)&0x1F;
        tdc = word&0x1FFFFF;
        edge = (word>>26)&0x1;
#ifdef DEBUG
        printf("TDC (0x%08x) chan=%d tdc=%d edge=%d\n",word,chan,tdc,edge);
#endif
        if(edge==0)
	    {
          if(chan==0)
	      {
            idn = 1;
            tmpx0 = ((float)tdc)/40.;
	        ww   = 1.;
	        /*hf1_(&idn,&tmpx0,&ww);*/
		    /*printf("tmpx0=%f\n",tmpx0);*/
	      }
	      else
	      {
            idn = 2;
            tmpx2 = ((float)tdc)/40.;
	        ww   = 1.;
	        /*hf1_(&idn,&tmpx2,&ww);*/
		    /*printf("tmpx2=%f\n",tmpx2);*/

            idn = 3;
            tmpx = tmpx0-tmpx2;
	        ww   = 1.;
	        /*hf1_(&idn,&tmpx,&ww);*/

            dcrbref = tmpx;
#ifdef DEBUG
            printf("dcrbref=%f\n",dcrbref);
#endif

			dcrbref = -850.; /*dcrbtest_011054.evio.0*/

			dcrbref = -910.; /*dcrbgtp_000088.evio.0*/

	      }
	    }
      }
      /*TEMP 1290*/

a123:

	  dcrbref = -910;

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
        GET32(word);
        if(timestamp_flag)
	    {
	      timestamp |= (word&0xffffff);
#ifdef DEBUG
	      printf(" {TRGTIME} timestamp=%lld (%lld)\n",timestamp,timestamp_old);fflush(stdout);
#endif

	      if(slot==4)
	      {
            if(timestamp_old!=0)
	        {
              idn = 10;
              tmpx = ((float)((timestamp-timestamp_old)/*%(long long)1000000*/))/1000000.;
			  /*printf("timestampdiff=%f\n",tmpx);*/
	          ww   = 1.;
	          /*hf1_(&idn,&tmpx,&ww);*/
	        }
            timestamp_old = timestamp;
	      }

          timestamp_flag = 0;
	      continue;
	    }

	    if(word & 0x80000000)
	    {
	      type = (word>>27)&0xF;
	      switch(type)
          {
	    case DC_DATA_BLOCK_HEADER:
              slot = (word>>22)&0x1f;
#ifdef DEBUG
	      printf(" {BLKHDR} SLOTID: %d", (word>>22)&0x1f);fflush(stdout);
	      printf(" NEVENTS: %d", (word>>11)&0x7ff);fflush(stdout);
	      printf(" BLOCK: %d\n", (word>>0)&0x7ff);fflush(stdout);
#endif
	      break;
	    case DC_DATA_BLOCK_TRAILER:
#ifdef DEBUG
	      printf(" {BLKTLR} SLOTID: %d", (word>>22)&0x1f);fflush(stdout);
	      printf(" NWORDS: %d\n", (word>>0)&0x3fffff);fflush(stdout);
#endif
	      break;
	    case DC_DATA_EVENT_HEADER:
#ifdef DEBUG
	      printf(" {EVTHDR} EVENT: %d\n", (word>>0)&0x7ffffff);fflush(stdout);
#endif
	      break;
	    case DC_DATA_TRIGGER_TIME:
	      timestamp = (((unsigned long long)word&0xffffff)<<24);
              timestamp_flag = 1;
	      break;
	    case DC_DATA_DCRBEVT:
          chan = (word>>16)&0x7F;
          tdc = (word>>0)&0xFFFF;
          tdc += (int)dcrbref;
          if(tdcref==0&&chan==0&&slot==4) tdcref = tdc;
#ifdef DEBUG
	      printf(" {DCRBEVT} 0x%08x",word);fflush(stdout);
	      printf(" CH: %3u", (word>>16)&0x7F);fflush(stdout);
	      printf(" TDC: %6u\n", (word>>0)&0xFFFF);fflush(stdout);
#endif
	      printf(" TDC: %6u\n", (word>>0)&0xFFFF);fflush(stdout);

	      /*events selection*/
	      if(tdc>0 && tdc<350 )
	      {
            nhits ++;

            il = layer[slot][chan]+6*superlayer[slot][chan];
            if(dcrb_check[slot][chan]==0)
			{
              dcrb_check[slot][chan]=1;
              dcndig[il] ++;
              ih = dcndig[il] - 1;
              dcdigi[il][ih][0] = wire[slot][chan];
              dcdigi[il][ih][1] = /*350 -*/ tdc;	    /*inverted tdcs*/

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


	      if(0)
	      {
          	idn = slot*100+chan;
            tmpx = (float)tdc;
	        ww   = 1.;
	        /*hf1_(&idn,&tmpx,&ww);*/

          	idn = slot*1000+chan;
            tmpx = (float)tdc-(float)tdcref;
	        ww   = 1.;
	        /*hf1_(&idn,&tmpx,&ww);*/
	      }

	      break;
	    case DC_DATA_INVALID:
	      printf(" {***DNV***}\n");fflush(stdout);
	      break;
	    case DC_DATA_FILLER:
#ifdef DEBUG
	      printf(" {FILLER}\n");fflush(stdout);
#endif
	      break;
	    default:
	      printf(" {***DATATYPE ERROR***}\n");fflush(stdout);
	      break;
          }
	  }
#ifdef DEBUG
	  else
	  {
	    printf("\n");fflush(stdout);
	  }
#endif

#ifdef DEBUG
        /*printf("end loop: b08=0x%08x\n",b08);*/
#endif
	}


qwerty:

	/*dcrbgtp*/
printf("dcrbgtp ???\n");
	if((ind1 = evNlink(buf, 11, 0xe108, 1, &nbytes)) > 0)
	{
      int k1, k2, k3;

      int half,chip,chan,bco,tdc,tdcref,chan1,edge,nw;
      unsigned char *end, *start;
      unsigned int tmp;
      float tmpx0, tmpx2, dcrbref;
      unsigned int temp[6];
      unsigned sample[6];
      int slot;
      int ndata0[22], data0[21][8];
      int baseline, sum, channel, ch1;
printf("dcrbgtp !!!\n");
#ifdef DEBUG1
      printf("ind1=%d, nbytes=%d\n",ind1,nbytes);fflush(stdout);
#endif
      start = b08 = (unsigned char *) &buf[ind1];
      end = b08 + nbytes;
#ifdef DEBUG1
      printf("ind1=%d, nbytes=%d (from 0x%08x to 0x%08x)\n",ind1,nbytes,b08,end);fflush(stdout);
#endif
	  /*
      goodevent = 0;
  	*/

      for(ii=0; ii<16; ii++)
      {
        for(jj=0; jj<112; jj++)
        {
          sl[0][ii][jj] = 0;
          sl[1][ii][jj] = 0;
	    }
        k1 = k2 = k3 = 0;
      }

#ifdef DEBUG1
      printf("\n\nBEGIN DCRBGTP EVENT =================================\n");
#endif

      tdcref = 0;
      timestamp_flag = 0;
      ind = 0;
      while(b08<end)
	  {
#ifdef DEBUG1
        /*printf("begin while: b08=0x%08x\n",b08);*/
#endif
        GET32(word);
        /*word = LSWAP(word);*/
#ifdef DEBUG1
        printf("dcrbgtp data word [%4d] hex=0x%08x uint=%u int=%d\n",ind,word,word,word);
#endif

	    if(ind==0)
	    {
          ;
          /*printf("\n\nGTP event %d\n",word);*/
        }
	    else if(ind==1)
		{
          timestamp = word;
	    }
        else if(ind==2)
	    {
          timestamp = timestamp | (((unsigned long long)word&0xffff)<<32);
#ifdef DEBUG1
	      printf("GTP timestamp=%lld ns (%lld us)\n",timestamp,timestamp/(long long)1000);
#endif
		}

#ifdef DEBUG1
        /*printf("end loop: b08=0x%08x\n",b08);*/
#endif
        ind++;
      }


        
      gsegm = (unsigned int *) &buf[ind1+3];
      /*for(ii=0; ii<112; ii++) gsegm[ii] = LSWAP(gsegm[ii]);*/


      k3 = 0; /*2 superlayers*/
      k2 = 0; /*16 angles*/
      k1 = 0; /*112 wires*/
      for(ii=0; ii<112; ii++)
	  { 
#ifdef DEBUG1
        printf("===> gsegm[%3d] = 0x%08x\n",ii,gsegm[ii]);
#endif
        if(ii==56)
	    {
          k3 = 1;
          k1 = k1 - 112;
		}

        for(k2=0; k2<16; k2++)
	    {
#ifdef DEBUG1__
          printf("ii=%d k1=%d k2=%d k3=%d\n",ii,k1,k2,k3);
#endif
          if(gsegm[ii]&(1<<k2))
		  {
            sl[k3][k2][k1] = 1;
#ifdef DEBUG1
            printf("---> HIT !!!!!!!!!!!!!!!!!!!!!\n");
#endif
		  }
	    }

        for(k2=16; k2<32; k2++)
	    {
#ifdef DEBUG1__
          printf("ii=%d k1=%d k2=%d k3=%d\n",ii,k1+1,k2-16,k3);
#endif
          if(gsegm[ii]&(1<<k2))
		  {
            sl[k3][k2-16][k1+1] = 1;
#ifdef DEBUG1
            printf("---> HIT !!!!!!!!!!!!!!!!!!!!!\n");
#endif
		  }
	    }

        k1+=2;
	  }


	  
#ifdef DEBUG1
	  
      printf("\n\n");
      for(k3=1; k3>=0; k3--) /* 2 sl */
      {
        for(k2=0; k2<16; k2++) /* 16 angles */
        {
          printf("%3d> ",(15-k2)-8);
          for(k1=111; k1>=0; k1--) /* 112 wires */
          {
            if(!((k1+1)%16) && k1<111) printf(" ");
            if(sl[k3][k2][k1]==0) printf("-");
            else printf("X");
          }
          printf("\n");
  	    }
		/*
        printf("   ");
        for(ii=0; ii<112; ii++) printf("+");
        printf("\n");
		*/
      }
      /*printf("\n");*/
	  
#endif
	  


#ifdef DEBUG1
      printf("END DCRBGTP EVENT =================================\n\n\n");
#endif
		/*
        if(goodevent) icedev ++;
		*/
	}


      if(goodevent)
      {
        nogoodevents = 0;
      }
    }




#endif /*ABC*/

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

  return(0);
}






static int dcndig[36], dcdigi[36][192][2];
static int ifirst = 1;

static unsigned char sl_trigger[2][NOFFSETS][NWIRES];
static unsigned char sl_offline[2][NOFFSETS][NWIRES];



#define STRLEN 128
#define mxstep 10000
#define npl_dc 36
#define npln    46


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




typedef struct sdageom
{
  float pln[6][npln][20];
  float dc_wpmid[6][npl_dc][192][3];
  float dc_wdir[6][npl_dc][192][3];
  float dc_wlen[6][npl_dc][192][2];
  float sc_grid[6][6][24][4];
  float sc_hleng[6][57];
  int   nstep_xyz;
  float xyz[mxstep][3];
  float sda_trp_obsolete[npln][7];
  float sda_tmat_obsolete[npln][5][5];
} SDAgeom;
SDAgeom sdageom_;






int
dcdsectEEL(int handler, TCanvas *fCanvas, int isec, float sxy, float x0, float y0, float T0)
{
  TArc *circle = new TArc();

  FILE *fd;
  char *ch, str[STRLEN];
  int iplane,iwire,ilayer,iplane1,iplane2,ret;
  float midx,midy,midz,cosx,cosy,cosz;
  PRTRACK *road;
  int i,j,k,nd,il,ip,iw,it,isc,ind,ig, nw,lmax,nwire,nwmin,nwmax,
    Nplane,j_plane, nstepm, npoint, nhit, indb, /*ind_sc[57][2],*/
    ibgrd,ired,igreen,iblue,ipurp,ilblue, /*nslab[6],*/ la, itdc;

  float *xmax, *ymax, deltaR, guardr, phi, degrad,
        phi_plane, x_plane, y_plane, delta_pl, xlast,ylast, xfirst,
        yfirst, phimin,phimax, phi25,d_out,dphi,Ndiv, phiort,
        x,y, x1,y1,x2,y2,x3,y3,x4,y4, dist,din,dout,d20, x00[3],
        f_s, fox, rad, angle,angle1,angle2, sas,sac;

  float lcell;
  float x_hit[mxstep], y_hit[mxstep], x_bgrd[max_nhit],
    y_bgrd[max_nhit], gwire_x[max_nwire], gwire_y[max_nwire],
    t_bgrd[max_nwire];

  /* for superlayer-based fit */
  int nnn[12]; /* the number of hits in layers 1..6 */
  int iww[12][192]; /* wire number */
  float xxx[12][192]; /* x-coords */
  float yyy[12][192]; /* y-coords */
  float ttt[12][192]; /* time */

  char legend[3];
  int slot, chan;

 printf("dcdsectEEL reached\n");fflush(stdout);

  /*should do it once*/
  if(ifirst)
  {
    ifirst = 0;
#if 0
    AxialPlusStereoInit(); /* should be done once */
    for(slot=4; slot<11; slot++)
    {
      for(chan=0; chan<96; chan++)
      {
        superlayer[slot][chan] = 0;
        layer[slot][chan] = board_layer[chan]-1;
        wire[slot][chan] = (board_wire[chan]-1)+(slot-4)*16;
  	  /*printf("translation table: slot %d, chan %d -> sl=%d la=%d w=%d\n",
          slot,chan,superlayer[slot][chan],layer[slot][chan],wire[slot][chan]);*/
  	  }
    }
    for(slot=13; slot<20; slot++)
    {
      for(chan=0; chan<96; chan++)
      {
        superlayer[slot][chan] = 1;
        layer[slot][chan] = board_layer[chan]-1;
        wire[slot][chan] = (board_wire[chan]-1)+(slot-13)*16;
  	  /*printf("translation table: slot %d, chan %d -> sl=%d la=%d w=%d\n",
          slot,chan,superlayer[slot][chan],layer[slot][chan],wire[slot][chan]);*/
  	  }
    }
#endif
  }


  /* if(first) */
  {
/*HIGZ world coordinates*/

#if 1
sdadraw_.sda_wc[0] = 0.;
sdadraw_.sda_wc[1] = 20.;
sdadraw_.sda_wc[2] = 0.;
sdadraw_.sda_wc[3] = 20.;
    xmax = &sdadraw_.sda_wc[1];
    ymax = &sdadraw_.sda_wc[3];
#endif
    /*
    nslab[0] = sdageom_.pln[isec-1][nsc_min-1][17];
    nslab[1] = sdageom_.pln[isec-1][nsc_min+1-1][17];
    nslab[2] = sdageom_.pln[isec-1][nsc_min+2-1][17];
    nslab[3] = sdageom_.pln[isec-1][nsc_min+2-1][18];
    nslab[4] = sdageom_.pln[isec-1][nsc_min+3-1][17];
    nslab[5] = sdageom_.pln[isec-1][nsc_min+3-1][18];
    isc = 0;
    for(ip=1; ip<=6; ip++)
    {
      for(i=1; i<=nslab[ip-1]; i++)
      {
        isc ++;
        ind_sc[isc-1][0] = i;
        ind_sc[isc-1][1] = ip;
      }
    }
    */
    ibgrd  = 1;
    ired   = 2;
    igreen = 3;
    iblue  = 4;
    ipurp  = 6;
    ilblue = 7;
    degrad = acos(-1.)/180.;
  }

  /* Set up a view box for the convenience of hard_copy output */
  if(fCanvas>0)
  {
    ISPMCI(ibgrd);
    ISPLCI(ibgrd);

    if(ABS(sxy) != 1.)
    {
	  
      /*IGBOX(0.,(*xmax),0.05,0.77*(*ymax));
      sprintf(legend,"%2d",isec);
	  */

	  /*sergey: slowing down more and more with every event
      IGTEXT((*xmax)*.05,(*ymax)*.75,
        "CMON: Middle Plane View of the Sector 1",(*ymax)/100.,0.,"L");
      IGAXIS(x0,16.,y0,y0,0.,1.,0,"AU");
      IGTEXT(16.2,y0,"Xs",(*ymax)/150.,0.,"L");
      IGAXIS(x0,x0,y0,12.,0.,1.,0,"AU");
      IGTEXT(x0,12.2,"Ys",(*ymax)/150.,0.,"L");
	  */
    }
  }
  







  /* Draw geometry of DC */

  lcell = 5.; /*cell size in cm*/

  for(ilayer=0; ilayer<12; ilayer++)
  {
    for(iwire=0; iwire<112; iwire++)
    {

#if 1
      /*Wire position in the middle plane*/
      sdageom_.dc_wpmid[0][ilayer][iwire][0] = lcell*iwire;
      if(ilayer%2) sdageom_.dc_wpmid[0][ilayer][iwire][0] -= lcell/2.;
      sdageom_.dc_wpmid[0][ilayer][iwire][1] = (lcell*sqrt(3.)/2.)*ilayer;
      sdageom_.dc_wpmid[0][ilayer][iwire][2] = 0.;

      /*Wire direction (unit vector)*/
      sdageom_.dc_wdir[0][ilayer][iwire][0] = 0.;
      sdageom_.dc_wdir[0][ilayer][iwire][1] = 0.;
      sdageom_.dc_wdir[0][ilayer][iwire][2] = 0.;
      /*
      printf("ilayer=%d iwire=%d x=%f y=%f\n",ilayer,iwire,sdageom_.dc_wpmid[0][ilayer][iwire][0],
	     sdageom_.dc_wpmid[0][ilayer][iwire][1]);
      */
#endif
    }
  }





        /*draw boxes around wires areas*/




  if(fCanvas>0)
  {

	/* superlayer 0 */

#if 1
      iplane = 0;
      iwire = 0;
      gwire_x[0] = sdageom_.dc_wpmid[0][iplane][iwire][0] * ABS(sxy) + x0;
      gwire_y[0] = sdageom_.dc_wpmid[0][iplane][iwire][1] * sxy + y0;


      iplane = 0;
      iwire = 111;
      gwire_x[1] = sdageom_.dc_wpmid[0][iplane][iwire][0] * ABS(sxy) + x0;
      gwire_y[1] = sdageom_.dc_wpmid[0][iplane][iwire][1] * sxy + y0;

      iplane = 1;
      iwire = 111;
      gwire_x[2] = sdageom_.dc_wpmid[0][iplane][iwire][0] * ABS(sxy) + x0;
      gwire_y[2] = sdageom_.dc_wpmid[0][iplane][iwire][1] * sxy + y0;

      iplane = 2;
      iwire = 111;
      gwire_x[3] = sdageom_.dc_wpmid[0][iplane][iwire][0] * ABS(sxy) + x0;
      gwire_y[3] = sdageom_.dc_wpmid[0][iplane][iwire][1] * sxy + y0;

      iplane = 3;
      iwire = 111;
      gwire_x[4] = sdageom_.dc_wpmid[0][iplane][iwire][0] * ABS(sxy) + x0;
      gwire_y[4] = sdageom_.dc_wpmid[0][iplane][iwire][1] * sxy + y0;

      iplane = 4;
      iwire = 111;
      gwire_x[5] = sdageom_.dc_wpmid[0][iplane][iwire][0] * ABS(sxy) + x0;
      gwire_y[5] = sdageom_.dc_wpmid[0][iplane][iwire][1] * sxy + y0;

      iplane = 5;
      iwire = 111;
      gwire_x[6] = sdageom_.dc_wpmid[0][iplane][iwire][0] * ABS(sxy) + x0;
      gwire_y[6] = sdageom_.dc_wpmid[0][iplane][iwire][1] * sxy + y0;



      iplane = 5;
      iwire = 0;
      gwire_x[7] = sdageom_.dc_wpmid[0][iplane][iwire][0] * ABS(sxy) + x0;
      gwire_y[7] = sdageom_.dc_wpmid[0][iplane][iwire][1] * sxy + y0;

      iplane = 4;
      iwire = 0;
      gwire_x[8] = sdageom_.dc_wpmid[0][iplane][iwire][0] * ABS(sxy) + x0;
      gwire_y[8] = sdageom_.dc_wpmid[0][iplane][iwire][1] * sxy + y0;

      iplane = 3;
      iwire = 0;
      gwire_x[9] = sdageom_.dc_wpmid[0][iplane][iwire][0] * ABS(sxy) + x0;
      gwire_y[9] = sdageom_.dc_wpmid[0][iplane][iwire][1] * sxy + y0;

      iplane = 2;
      iwire = 0;
      gwire_x[10] = sdageom_.dc_wpmid[0][iplane][iwire][0] * ABS(sxy) + x0;
      gwire_y[10] = sdageom_.dc_wpmid[0][iplane][iwire][1] * sxy + y0;

      iplane = 1;
      iwire = 0;
      gwire_x[11] = sdageom_.dc_wpmid[0][iplane][iwire][0] * ABS(sxy) + x0;
      gwire_y[11] = sdageom_.dc_wpmid[0][iplane][iwire][1] * sxy + y0;

      iplane = 0;
      iwire = 0;
      gwire_x[12] = sdageom_.dc_wpmid[0][iplane][iwire][0] * ABS(sxy) + x0;
      gwire_y[12] = sdageom_.dc_wpmid[0][iplane][iwire][1] * sxy + y0;
#endif

	  /*
      printf("box(sl0): %f,%f %f,%f %f,%f %f,%f %f,%f\n",
	    gwire_x[0],gwire_y[0],gwire_x[1],gwire_y[1],gwire_x[2],gwire_y[2],
        gwire_x[3],gwire_y[3],gwire_x[4],gwire_y[4]);
	  */
      IPL(13,gwire_x,gwire_y);




  }
















  /* Plot hits in DC (digitalization) */
  {
    int lay;
    if(handler > 0)
	{
      ret = dcgetevent(handler, dcndig, dcdigi, sl_trigger);
      if(ret==EOF) return(EOF);
	}
	printf("dcgetevent done\n"); 
    /* fills following:
        dcndig[layer] - the number of hits in 'layer'
        dcdigi[layer][hit#][0] = wire#;
        dcdigi[layer][hit#][1] = tdc;
    */
    indb = 0;
    for(i=0; i<12; i++) nnn[i] = 0;
    for(i=1; i<=npl_dc; i++)
    {
      nhit = dcndig[i-1];
      for(j=1; j<=nhit; j++)
      {
        iw = dcdigi[i-1][j-1][0];

        itdc = dcdigi[i-1][j-1][1];

        if(indb < max_nhit)
	    {
          indb ++;
#if 1
          x_bgrd[indb-1] = sdageom_.dc_wpmid[isec-1][i-1][iw][0] * ABS(sxy) + x0;
          y_bgrd[indb-1] = sdageom_.dc_wpmid[isec-1][i-1][iw][1] * sxy + y0;
#endif
          t_bgrd[indb-1] = itdc * sxy / T0;
	      /*
	      printf("l=%d, w=%d, tdc=%d -> x=%f y=%f tdc(est.)=%f\n",
		    i,iw,itdc,x_bgrd[indb-1],y_bgrd[indb-1],t_bgrd[indb-1]);
	      */
          lay = i-1;
          iww[lay][nnn[lay]] = iw;
          xxx[lay][nnn[lay]] = x_bgrd[indb-1];
          yyy[lay][nnn[lay]] = y_bgrd[indb-1];
	      ttt[lay][nnn[lay]] = t_bgrd[indb-1];
          nnn[lay]++;
	    }
      }
    }


    if(indb > 0)
    {
      
      if(fCanvas>0)
      {
        ISPMCI(ired);
        ISMK(5);

        /*  
        IPM(indb,x_bgrd,y_bgrd);
        */

        IPM_DC(indb,x_bgrd,y_bgrd,t_bgrd);
      }
      


      /*dcpr1(fCanvas,nnn,iww,xxx,yyy,ttt);*/


	  
      dclfitsl(fCanvas,0,nnn,iww,xxx,yyy,ttt);
      dclfitsl(fCanvas,1,nnn,iww,xxx,yyy,ttt);
	  printf("\n");

#if 0
	  {
		Word192 data0[6], data1[6];
        int nclust;
        PRRG clust;

        for(lay=0; lay<6; lay++)
		{
          ClearWord192(&data0[lay]);
          ClearWord192(&data1[lay]);
		}

        /*fills data*/
        for(lay=0; lay<6; lay++)
		{
          /*printf("lay=%d, nnn[lay]=%d\n",lay,nnn[lay]);*/
          for(i=0; i<nnn[lay]; i++)
		  {
            SetBitWord192(&data0[lay],iww[lay][i]);
            /*printf("[%d] wire=%d\n",i,iww[lay][i]);*/
		  }
          /*PrintWord192(&data0[lay]);*/
		}

        for(lay=6; lay<12; lay++)
		{
          /*printf("lay=%d, nnn[lay]=%d\n",lay,nnn[lay]);*/
          for(i=0; i<nnn[lay]; i++)
		  {
            SetBitWord192(&data1[lay-6],iww[lay][i]);
            /*printf("[%d] wire=%d\n",i,iww[lay][i]);*/
		  }
          /*PrintWord192(&data1[lay-6]);*/
		}

		/*printf("SL1 ===>\n");*/
        SegmentSearch192(data1, &nclust, &clust);
        for(i=0; i<NOFFSETS; i++)
		{
          for(j=0; j<NWIRES; j++)
		  {
            sl_offline[1][i][j] = outputt[i][j];
		  }
		}

		/*printf("SL0 ===>\n");*/
        SegmentSearch192(data0, &nclust, &clust);
        for(i=0; i<NOFFSETS; i++)
          for(j=0; j<NWIRES; j++)
            sl_offline[0][i][j] = outputt[i][j];
	  }
#endif
    
    }


	/* compare sl_trigger and sl_offline */
    for(k=0; k<2; k++)
      for(i=0; i<NOFFSETS; i++)
          for(j=0; j<NWIRES; j++)
            if(sl_trigger[k][i][j] != sl_offline[k][i][j])
			{
			  /*
              printf("DIFF !!! [%d][%d][%d] %d %d\n",k,i,j,
			  sl_trigger[k][i][j],sl_offline[k][i][j])*/;
			}

  }


  if(fCanvas>0)
  {
    ISPMCI(ibgrd);
    ISPLCI(ibgrd);
    REDRAW;
  }

  return(0);
}
