
/* sim_main.c - main event simulation routine */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "dclib.h"

/*#include "sdabos.h"*/
#include "sdakeys.h"
#include "sdaevgen.h"
#include "sdageom.h"
#include "sdadraw.h"
#include "sdadigi.h"

extern int iucomp();

void
sim_main(int *ifail)
{
  int isec, ip,il,it,k,kel,ist,if_draw,Level;
  int ev_acc, ipl_min;
  float vins[9], smin_old, smax_old, dang_old;
  float sda_trp[npln][7], sda_tmat[npln][5][5]; /* from sdageom.inc */
  int ifirst = 1;
  int nsect = 1/*6*/;
  int inter = 1;

  if(ifirst)
  {
    sdaevgen_.Nevin_prod = 0;
    sdaevgen_.Nevin_acc  = 0;
    inter      = 0;
    /*IF(iucomp(4HINT ,lmode,1 ).NE.0) inter = 1;*/
    ifirst     = 0;
  }

  sdaevgen_.Nevin_prod = sdaevgen_.Nevin_prod + 1;

  /* Clear arrays needed for digitalization */
  /*
      bm_ndig = 7
      DO k = 1,bm_ndig
        bm_digi(1,k) = 0
        bm_digi(2,k) = 0
        bm_digi(3,k) = 0
      ENDDO
      st_ndig  = 0
      tgt_ndig = 0
      tge_ndig = 0
      DO k = 1,nsect
        st_digi(1,k) = 0
        st_digi(2,k) = 0
        st_digi(3,k) = 0
      ENDDO
  */
  for(isec=0; isec<nsect; isec++)
  {
    for(il=0; il<npl_dc; il++) sdadigi_.dc_ndig[isec][il] = 0;
		/*
        il = 1
        cc_ndig(il,isec) = 0
        sc_ndig(il,isec) = 0
        DO il = 1,6
          ec_ndig(il,isec) = 0
        ENDDO
        DO il = 1,4
          ec1_ndig(il,isec) = 0
        ENDDO
		*/
  }

  Level = 0;
  sdadigi_.Ttrig = 444.;
  sdadraw_.ntrdraw = 0;
  if_draw = 0;
  if(sdakeys_.lanal[0]<=1) if_draw = inter;

  if(sdakeys_.lanal[1]==1 || sdakeys_.lanal[1]==2)
  {
	ipl_min = 4;
  }
  else
  {
	ipl_min = 2;
  }

  /* Loop over all tracks in an event (swimming) */
  it = 1;
  sdakeys_.isim = 1;
  for(k=0; k<5; k++) sdaevgen_.itrin_acc[0][k] = -1;
  isec = 0/*1*/; /*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
  dcrotds_(&sdaevgen_.ev_in[0][0],&sdaevgen_.evin[0][0],&isec);
  sdaevgen_.itrin_sec[0] = isec;
  sdaevgen_.evin[0][1] = sdaevgen_.ev_in[0][0];
  sdaevgen_.evin[0][0] = sdaevgen_.ev_in[0][2];
  sdaevgen_.evin[0][2] = sdaevgen_.ev_in[0][1];
  printf("!!!!!!!! evin=%f %f %f\n",sdaevgen_.evin[0][0],sdaevgen_.evin[0][1],sdaevgen_.evin[0][2]);

  for(k=0; k<9; k++) vins[k] = sdaevgen_.evin[0][k];

  /* In case of PT Field set new STEP parameters (and save old)
      IF(zmagn(4).NE.0.) THEN
        smin_old = zstep(1)
        zstep(1) = 0.10
        smax_old = zstep(2)
        zstep(2) = 5.00
        dang_old = zstep(3)
        zstep(3) = 0.001
      ENDIF
  */

  dcswim(&sdageom_.pln[isec][0][0],ipl_min,npln,0,if_draw,vins,sda_trp,sda_tmat, sdageom_.nstep_xyz, sdageom_.xyz);

  /* In case of PT Field store possible new Sector#
      sdaevgen_.itrin_sec(1) = isec
  */

  /* Store trajectories for trigger simulation and digitalization */

  /* Start Counter
      il = 0
      DO ip = nst_min,nst_max
        il = il + 1
        DO k = 1,7
          st_trps(k,il,1) = sda_trp(k,ip)
        ENDDO
      ENDDO
  */

  /* Drift Chamber */
  il = 0;
  for(ip=ndc_min-1; ip<ndc_max; ip++)
  {
    il ++;
    for(k=0; k<7; k++) sdaevgen_.dc_trps[0][il][k] = sda_trp[ip][k];
  }

  /* Cerenkov Counter
      il = 0
      DO ip = ncc_min,ncc_max
        il = il + 1
        DO k = 1,7
          cc_trps(k,il,1) = sda_trp(k,ip)
        ENDDO
      ENDDO
  */

  /* Scintillator Counter
      il = 0
      DO ip = nsc_min,nsc_max
        il = il + 1
        DO k = 1,7
          sc_trps(k,il,1) = sda_trp(k,ip)
      print*,'sim_main:sc_trps(',k,',',il,')=',sc_trps(k,il,1)
        ENDDO
      ENDDO
  */

  /* Shower Counter
      il = 0
      DO ip = nec_min,nec_max
        il = il + 1
        DO k = 1,7
          ec_trps(k,il,1) = sda_trp(k,ip)
        ENDDO
      ENDDO
  */

  /* Store trajectories for drawing when there is no PR & FF fitting */
  if(if_draw==1 && sdadraw_.ntrdraw<=max_draw)
  {
    sdadraw_.ntrdraw ++;
    sdadraw_.draw_isec[sdadraw_.ntrdraw-1] = sdaevgen_.itrin_sec[0];
    sdadraw_.draw_nstep[sdadraw_.ntrdraw-1] = sdageom_.nstep_xyz;
    for(ist=0; ist<sdageom_.nstep_xyz; ist++)
	{
      sdadraw_.draw_xyz[sdadraw_.ntrdraw-1][ist][0] = sdageom_.xyz[ist][0];
      sdadraw_.draw_xyz[sdadraw_.ntrdraw-1][ist][1] = sdageom_.xyz[ist][1];
      sdadraw_.draw_xyz[sdadraw_.ntrdraw-1][ist][2] = sdageom_.xyz[ist][2];
	}

	/* Store ST hits
        k = 0
        DO il = nst_min,nst_max
          k = k + 1
          draw_hit(1,k,sdadraw_.ntrdraw) = sda_trp(1,il)
          draw_hit(2,k,sdadraw_.ntrdraw) = sda_trp(2,il)
          draw_hit(3,k,sdadraw_.ntrdraw) = sda_trp(3,il)
        ENDDO
	*/

	/* Store CC,SC and EC hits
        k = 2
        DO il = ncc_min,nec_max
          k = k + 1
          draw_hit(1,k,sdadraw_.ntrdraw) = sda_trp(1,il)
          draw_hit(2,k,sdadraw_.ntrdraw) = sda_trp(2,il)
          draw_hit(3,k,sdadraw_.ntrdraw) = sda_trp(3,il)
        ENDDO
	*/
  }

  /* No Trigger (no Data) just only swimming
      IF(ltrig(5).GT.4) THEN
        sdakeys_.lanal(1) = 0
        GOTO 999
      ENDIF
  */

  /* Trigger simulation
      CALL sim_trig(ifail)
      IF(ifail.GT.0) then
*        print *,'sim_main: ifail(sim_trig1)=',ifail
        GOTO 999
      endif
*        print *,'sim_main: ifail(sim_trig2)=',ifail
  */

  /* Digitalization of all detector subsystems */
  /*
      IF(zbeam(1).EQ.0.) THEN
        st_ndig = nsect
        CALL sim_stdigi
        CALL sim_tgdigi
      ENDIF
      call start_microtimer()
  */

  sim_dcdigi();

  /*
      call stop_microtimer(10000)
      CALL sim_ccdigi
      CALL sim_scdigi
      CALL sim_ecdigi
      CALL sim_ec1digi
  */

  /* Define here the event acceptance for DC, SC and EC
      k  = 0
      kel= 0
      DO 30 it = 1,ntrack
        IF(sdaevgen_.evin(9,it).EQ.0.) GOTO 30                     ! Only charged tracks
        IF(zbeam(1).LT.0.) THEN
          IF(sdaevgen_.evin(8,it).LT.0.001 .AND. itrin_acc(5,it).GT.0) kel = 1
        ELSE
          IF(itrin_acc(1,it).GT.0) kel = 1        
        ENDIF
        IF(itrin_acc(2,it).EQ.6 .AND. itrin_acc(4,it).GT.0) k = k + 1                  
 30   CONTINUE
      k = k*kel
      ev_acc = 0
      IF(k.GE.zcut(8)) THEN
        ev_acc = 1
        sdaevgen_.Nevin_acc = sdaevgen_.Nevin_acc + 1
      ENDIF
  */

  /* Add background to the hits in Drift Chambers
  *      IF(isw(2).EQ.0) THEN
  *        DO isec = 1,nsect
  *          CALL sim_bgrd(isec)
  *        ENDDO
  *      ELSE
  *        CALL sim_bgrd(isw(2))
  *      ENDIF
  */

  /* Store the simulation results (REVB) in BOS Banks
      ievt = ievt + 1
*      CALL sim_tobos
C
C Flag  isim  = 1  during the similation, otherwise is zero
C Flag  ifsim = 1  during the analysis of simulated data, otherwise is zero
      isim = 0
      ifsim= 1
C
C In case of PT Field restore old STEP parameters
      IF(zmagn(4).NE.0.) THEN
        zstep(1) = smin_old
        zstep(2) = smax_old
        zstep(3) = dang_old
      ENDIF
C
C Print EVB
      IF(lprnt(2).NE.0) CALL sim_pevb(iw)
C
C Print Acceptance
      IF(lprnt(2).EQ.2) THEN
        k = zbeam(1)
        WRITE(6,41) ev_acc,k
        DO it = 1,ntrack
          WRITE(6,42) it,sdaevgen_.evin(8,it),(itrin_acc(k,it),k=1,5)
        ENDDO
      ENDIF
 41   FORMAT(/' EvtAccept =',I2,'  Qbeam =',I2,
     1        ' (Value =-1 NA,  = 0 NotAcc,  > 0 Acc)'/
     2        ' itr    Mass   ST   DC   CC   SC   EC')               
 42   FORMAT(I4,F8.5,5I5)
C
  999 CONTINUE
      RETURN
      END
  */

  *ifail = 0;

  return;
}
