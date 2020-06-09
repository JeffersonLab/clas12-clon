
/* makedict_zero_field.c - generates zero-field pattern recognition dictionary starting from layer 1 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "prlib.h"
#include "dclib.h"
#include "sdageom.h"
#include "sdaevgen.h"
#include "sdakeys.h"


#define ABS(x) ((x) < 0 ? -(x) : (x))
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define MAX(x,y) ((x) > (y) ? (x) : (y))

#define IWMIN 1
#define IWMAX 130
#define THETAMIN 1.
#define THETAMAX 140.


#define DEGRAD  0.0174532925199432958
#define RADDEG 57.2957795130823209












static int jw[10000]; /* TEMP !!!!!!!!!!!!!!!!!!!!! */




/*
 return: -1 if all 6 numbers the same
          1 if at least one number differ more then 1
          0 otherwise
*/
int
icomp6(int iwc[6], int iwcold[6])
{
  if(iwc[0]==iwcold[0]&&iwc[1]==iwcold[1]&&iwc[2]==iwcold[2]&&iwc[3]==iwcold[3]&&iwc[4]==iwcold[4]&&iwc[5]==iwcold[5])
  {
    return(-1);
  }
  else if(ABS(iwc[0]-iwcold[0])>1||ABS(iwc[1]-iwcold[1])>1||
          ABS(iwc[2]-iwcold[2])>1||ABS(iwc[3]-iwcold[3])>1||
          ABS(iwc[4]-iwcold[4])>1||ABS(iwc[5]-iwcold[5])>1)
  {
    return(1);
  }
  else
  {
    return(0);
  }
}


#define MACRO1 \
  sdaevgen_.ev_in[0][0] = x_;	  \
  sdaevgen_.ev_in[0][1] = y_;	  \
  sdaevgen_.ev_in[0][2] = z_;		 \
  ph = phi_ * DEGRAD;	   \
  th = theta_ * DEGRAD;			   \
  sdaevgen_.ev_in[0][3] = sin(th)*cos(ph);	   \
  sdaevgen_.ev_in[0][4] = sin(th)*sin(ph);	   \
  sdaevgen_.ev_in[0][5] = cos(th);			   \
  sdaevgen_.ev_in[0][6] = 0/*p_*/;			   \
  sdaevgen_.ev_in[0][7] = 0.0005;			   \
  sdaevgen_.ev_in[0][8] = 0/*charge*/;		   \
  sdaevgen_.ntrack = 1;					   \
  if(inter==1) \
  { \
	printf("track: iw=%d, coord=%f %f %f, theta=%f, phi=%f\n",iw_,sdaevgen_.ev_in[0][0],sdaevgen_.ev_in[0][1],sdaevgen_.ev_in[0][2],th*RADDEG,ph*RADDEG); \
  }


void
sim_loop_zero_field(int ifevb, int inter, int Nevents, int iwmin, int iwmax, float thetamin, float thetamax)
{
  int iw_, itmp, ipd;
  float p_, x_, y_, z_, theta_, phi_;
  float x1,x2,z1,z2,a,b,getmom;
  float ymin,ymax,zmin,zmax;
  float phimin,phimax;
  float dz,dtheta,theta1;

  int i,iwl3,iwthe[6],iwtheold[6],ifail,ievent;
  int iwphi[6],iwphiold[6],iwy1[6],iwy2[6],iwp1[6],iwp2[6];
  int iwz1[6],iwz2[6];
  int p,theta,y,th, ph,dy,dphi;

  dy = 3.0; /* 3.0 ! along wire resolution of the pattern recognition is about 3.7 cm */
  dphi = 5.0; /* 5.0 ! phi resolution of the pattern recognition is about 7.6 degree */

  /* tracks starts from layer 1 */

  /* DC Wire position, Wire Direction & WireLen for layers: 0 - 35
           dc_wpmid[isec][il][iw][0]) | along beam     |
           dc_wpmid[isec][il][iw][1]) | vertical       |-> Wire position in the middle plane
           dc_wpmid[isec][il][iw][2]) | along wire (0) |
           dc_wlen[isec][il][iw][0])  --> Half wire length (+z or +y AMPL,Reg.1 opposit)
  */

  /* take IWmin from the parameter list */
  iw_ = iwmin - 1;


  /* loop over wire number */

  while(iw_ < iwmax)
  {
    iw_ ++;
	printf("wire=%d\n",iw_);

	/* coordinate along the beam */
    z2 = sdageom_.dc_wpmid[0][0][iw_-1][2/*0*/]-0.01;
    z1 = sdageom_.dc_wpmid[0][0][iw_][2/*0*/]-0.01;

	/* vertical coordinate */
    x2 = sdageom_.dc_wpmid[0][0][iw_-1][0/*1*/]-0.01;
    x1 = sdageom_.dc_wpmid[0][0][iw_][0/*1*/]-0.01;

	/* step for Z coordinate */
    a = (x2-x1)/(z2-z1);
    b = x1-a*z1;
    dz = (z2-z1)*0.34; /* less then cell */

	printf("z1,z2,x1,x2,a,b,dz=%f %f %f %f %f %f %f\n",z1,z2,x1,x2,a,b,dz);


	/* loop over x-z plane coordinates */

	z_ = z1 - dz;

    while(z_ < (z2-dz))
	{
      z_ = z_ + dz;
      x_ = a*z_ + b;
      printf("loop over x-z: x=%f z=%f z2=%f\n",x_,z_,z2);

	  /* get 'ymin' and 'ymax' */
      if(sdageom_.dc_wlen[0][0][iw_-1][0] < 1.) /* less then 1cm */
	  {
        dy = sdageom_.dc_wlen[0][0][iw_-1][0] - 0.1;
        ymax = sdageom_.dc_wlen[0][0][iw_-1][0];
	  }
      else
	  {
		ymax = sdageom_.dc_wlen[0][0][iw_-1][0] - 1.; /* step 1cm from the end of wire */
	  }
      ymin = -ymax;
      printf("ymin=%f ymax=%f\n",ymin,ymax);

	  /* set initial dtheta step; will be adjusted on flight */
      dtheta = 0.4; /* 0.4 ! theta resolution of the pattern recognition is about 2 degree */
	  theta = theta1; /* we already found theta min, it was saved as 'theta1' */
      theta = theta - dtheta;
      if(thetamax > 140.) thetamax = 140.;
      printf("thetamax=%f thetamin=%f theta=%f dtheta=%f\n",thetamax,thetamin,theta,dtheta);
      iwtheold[0] = -1;

	  /* loop over theta */
      while(theta < thetamax)
	  {
        theta = theta + dtheta;
        theta_ = theta;

		/* get iwthe() */
        y_=0.;
        phi_ = 0.;
        MACRO1;
        sim_main(&ifail);
        if(ifail > 0)
		{
          iwtheold[0]=-1;
		  goto a3000;
		}
        sim_anal(jw,jw,ifevb,inter,ievent,iwl3,iwthe);
        itmp = iwthe[0]*iwthe[1]*iwthe[2]*iwthe[3]*iwthe[4]*iwthe[5];
        if(itmp == 0)
		{
          iwtheold[0]=-1;
          goto a3000;
		}

		/* STEP CORRECTION (theta) */
        if(iwtheold[0] != -1)
		{
          if(icomp6(iwthe,iwtheold) == 1) /* decrease step */
		  {
            printf("decrease theta: road: %d  phi=%f theta=%f\n",iwthe,phi_,theta);
            theta = theta - dtheta; /* step back */
            dtheta = dtheta * 0.9; /* decrease step size */
            printf("theta step decreased=%f\n",dtheta);
            if(dtheta < 0.01)
			{
              printf("sda_run: ERROR: dtheta too small - exit\n");
              exit(0);
			}
            goto a2000; /* skip PHI scan, try new THETA */
		  }
          else if(icomp6(iwthe,iwtheold) == -1) /* increase step */
		  {
            dtheta = dtheta * 1.1;
            printf("theta step increased=%f\n",dtheta);
            for(i=0; i<6; i++) iwtheold[i] = iwthe[i];
		  }
		  else if(icomp6(iwthe,iwtheold) == 0) /* go head */
		  {
            printf("road: %d  phi0=%f  theta=%f\n",iwthe,phi_,theta);
            for(i=0; i<6; i++) iwtheold[i] = iwthe[i];
		  }

		}
        else
		{
          printf("road: iwthe=%d\n",iwthe);
          for(i=0; i<6; i++) iwtheold[i] = iwthe[i];
	    }
		/* END OF STEP CORRECTION (theta) */

		/* choose step for 'y' and calculate phimin and phimax */
        phi_ = -0.;
		y_=ymin;
		printf("ymin=%f\n",ymin);
        MACRO1;
        sim_main(&ifail);
        if(ifail > 0)
		{
          printf("1: theta_=%f\n",theta_);
          goto a200;
		}
        sim_anal(jw,jw,ifevb,inter,ievent,iwl3,iwy1);
        itmp = iwy1[0]*iwy1[1]*iwy1[2]*iwy1[3]*iwy1[4]*iwy1[5];
        if(itmp == 0)
		{
          printf("2->%d\n",iwy1);
          printf("2: theta_=%f\n",theta_);
          goto a200;
		}
        printf("ymin->%d\n",iwy1);

        phi_ = 0.;
        y_=ymax;
        printf("ymax=%f\n",ymax);
        MACRO1;
        sim_main(&ifail);
        if(ifail > 0)
        {
          printf("3: theta_=%f\n",theta_);
          goto a200;
        }
        sim_anal(jw,jw,ifevb,inter,ievent,iwl3,iwy2);
        itmp = iwy2[0]*iwy2[1]*iwy2[2]*iwy2[3]*iwy2[4]*iwy2[5];
        if(itmp == 0)
        {
          printf("4->%d\n",iwy2);
          printf("4: theta_=%f\n",theta_);
          goto a200;
        }
        printf("ymax->%d\n",iwy2);
        itmp = ABS(iwy1[0]-iwy2[0]);
        for(i=1; i<6; i++) if(itmp < ABS(iwy1[i]-iwy2[i])) itmp=ABS(iwy1[i]-iwy2[i]);
        dy=ymax*2./((float)(itmp+3));
        y = ymin - dy;
        printf("itmp=%d ymin=%f ymax=%f dy=%f\n",itmp,ymin,ymax,dy);
        goto a210;
a200:
        printf("sda_run: ERROR 200 - stop\n");
        goto a500; /* stop */

a210:



        /* loop over coordinate along wire */

        while(y < (ymax-dy))
        {
          y = y + dy;
          y_ = y;
          printf("y=%f (half len=%f)\n",y,sdageom_.dc_wlen[0][0][iw_-1][0]);

          /* set phi parameters */
          phi_ = -dphi;
          phimax = 30.;
          iwphiold[0] = -1;

          /* loop over phi-1 */

          while(phi_ < phimax)
          {
            phi_ = phi_ + dphi;
            MACRO1;
            sim_main(&ifail);
            if(ifail > 0)
            {
              printf("11\n");
              goto a2000;
            }
            sim_anal(jw,jw,ifevb,inter,ievent,iwl3,iwphi);
            itmp = iwphi[0]*iwphi[1]*iwphi[2]*iwphi[3]*iwphi[4]*iwphi[5];
            if(itmp == 0)
            {
              printf("22\n");
              goto a1500;
            }
/* STEP CORRECTION ------------------------------------------ */

            if(iwphiold[0] != -1)
            {
              if(icomp6(iwphi,iwphiold) == 1) /* decrease step */
              {
                phi_ = phi_ - dphi; /* step back */
                dphi = dphi * 0.5; /* decrease step size */
                printf("1: step decreased(PHI)=%f because of %d != %d\n",dphi,iwphiold,iwphi);
                if(dphi < 0.01)
                {
                  printf("1: sda_run: ERROR: dphi too small(1) - exit\n");
                  exit(0);
                  goto a1500;
                }
                goto a1400; /* try new PHI */
              }
              else if(icomp6(iwphi,iwphiold) == -1) /* increase step */
              {
                dphi = dphi*1.5;
                if(dphi > 9.) dphi=9.;
                printf("1: step increased(PHI)=%f because of %d == %d\n",dphi,iwphi,iwphiold);
              }
              else if(icomp6(iwphi,iwphiold) == 0) /* go head */
              {
                printf("1: road: %d phi1=%f theta=%f\n",iwphi,phi_,theta_);
                for(i=0; i<6; i++) iwphiold[i]=iwphi[i];
              }
            }
            else
            {
              printf("road: iwphi=%d\n",iwphi);
              for(i=0; i<6; i++) iwphiold[i]=iwphi[i];
            }

/* STEP CORRECTION ------------------------------------------ */
            if(ievent >= Nevents) goto a100;
a1400:
			;
          } /* while(phi_<phimax) */
a1500:

/* loop over phi-2 */

          phi_ = dphi;
          phimin = -30.;
          iwphiold[0] = -1;
          printf("2: road0: %d phi2=%f theta=%f\n",iwphiold,phi_,theta_);

          while(phi_>phimin)
          {
            phi_ = phi_ - dphi;
            MACRO1;
            sim_main(&ifail);
            if(ifail>0) goto a2000;
            sim_anal(jw,jw,ifevb,inter,ievent,iwl3,iwphi);
            itmp = iwphi[0]*iwphi[1]*iwphi[2]*iwphi[3]*iwphi[4]*iwphi[5];
            printf("2: road1: %d phi2=%f theta=%f\n",iwphi,phi_,theta_);
            if(itmp==0) goto a2000;
/* STEP CORRECTION ------------------------------------------ */
            if(iwphiold[0] != -1)
            {
              if(icomp6(iwphi,iwphiold) == 1) /* decrease step */
              {
                printf("2: road2: %d phi2=%f theta=%f\n",iwphi,phi_,theta_);
                phi_ = phi_ + dphi; /* step back */
                dphi = dphi * 0.5; /* decrease step size */
                printf("2: step decreased(PHI)=%f because of %d != %d\n",dphi,iwphi,iwphiold);
                if(dphi<0.0001)
                {
                  printf("sda_run: ERROR: dphi too small(2) - stop\n");
                  exit(0);
                  goto a2000;
                }
                goto a1900; /* try new PHI */
              }
              else if(icomp6(iwphi,iwphiold) == -1) /* increase step */
              {
                dphi = dphi*1.5;
                if(dphi>9.) dphi=9.;
                printf("2: step increased(PHI)=%f because of %d == %d\n",dphi,iwphi,iwphiold);
              }
              else if(icomp6(iwphi,iwphiold) == 0) /* go head */
              {
                printf("2: road: %d phi2=%f theta=%f\n",iwphi,phi_,theta_);
                for(i=0; i<6; i++) iwphiold[i] = iwphi[i];
              }
            }
            else
            {
              printf("2: road: iwphi=%d\n",iwphi);
              for(i=0; i<6; i++) iwphiold[i] = iwphi[i];
            }
/* STEP CORRECTION ------------------------------------------ */
            if(ievent>=Nevents) goto a100;
a1900:
			;
          } /*while(phi_>phimin)*/


/* end of all loops */

        }   /* y */
a2000:
a3000:
		;
      } /* theta */
a500:
	  ;
    } /* z */
  } /* iw */


/* *** End of loop over the events *** */

a100:

  return;
}


















int
main(int argc, char **argv)
{
  int iwmin, iwmax, icharge, tmp, inter, ifevb, Nevt;
  float thetamin, thetamax, ltarget, rtarget, ztarget;
  char fname[1000];

  if(argc != 3 && argc != 5)
  {
    printf("Usage: makedict_zero_field IWmin IWmax [THETAmin THETAmax]\n");
    printf("                                        Degree\n");
    printf("Output will go to the prlink.bos file in the current directory.\n");
    exit(0);
  }

  iwmin  = atoi(argv[1]);
  iwmax  = atoi(argv[2]);
  if(iwmin < IWMIN) iwmin = IWMIN;
  if(iwmax > IWMAX) iwmax = IWMAX;
  if(iwmin > iwmax) {printf("error in wire number - exit.\n"); exit(0);}

  if(argc == 5)
  {
    thetamin = atof(argv[3]);
    thetamax = atof(argv[4]);
    if(thetamin < THETAMIN) thetamin = THETAMIN;
    if(thetamax > THETAMAX) thetamax = THETAMAX;
    if(thetamin >= thetamax) {printf("error in theta - exit.\n"); exit(0);}
  }
  else
  {
    thetamin = THETAMIN;
    thetamax = THETAMAX;
  }

  printf("\nParameters: IWmin=%8d     IWmax=%8d     THETAmin=%5.1f     THETAmax=%5.1f\n",
          iwmin, iwmax, thetamin, thetamax);

  cminit();

  inter = 0;         /* no interactive for dictionary generating */
  ifevb = 2;
  Nevt = 1000000000; /* set maximum event to big number */
  ltarget = -2.; /* if negative - start from 1st DC layer */

  /*
  sim_loop_fromlayer1_(bcs_.iw,&ifevb,&inter,&Nevt,&iwmin,&iwmax,&pmin,&pmax,
            &charge,&thetamin,&thetamax);
  */
  sim_loop_zero_field(ifevb, inter, sdakeys_.Nevt, iwmin, iwmax, thetamin, thetamax);

  /*
  dcerun(bcs_.iw);
  dclast(bcs_.iw);
  */

  /*
  strcpy(fname,"prlink");
  tmp = strlen(fname);
  sprintf((char *)&fname[tmp],"_%3.1f_%3.1f_\0",pmin,pmax);
  tmp = strlen(fname);
  if(charge < -0.1) fname[tmp++] = 'e';
  else              fname[tmp++] = 'p';
  fname[tmp] = '\0';
  strcat(fname,".bos");
  prwrite(fname);
  */

  exit(0);
}










