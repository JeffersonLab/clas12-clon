
/* sim_anal.c - main routine for an event reconstruction */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "dclib.h"

/*#include "sdaparam.h"*/
#include "sdakeys.h"
#include "sdadraw.h"
#include "sdadigi.h"
#include "sdaevgen.h"

void
sim_anal(int *iw, float *rw, int ifevb, int inter, int ievent, int iwl3, int istat[6])
{
  int segm[6][6][nsgmx][12];
  int clust[6][6][nclmx][nsgcmx];
  int nclust[6][6];
  int nsegmc[6][6][nclmx];
  int i, k, isec,isecm,ifail;
  float vect1[6],pin;
  int ecdigi2[3];
  int nsect = 1/*6*/;

  /*
C Set to zero the number of the track candidates, reconstructed tracks,
C and tracks to be drawn
c      ntr_link  = 0
c      ntr_link1 = 0
c      ntr_out   = 0
c      ntrdraw   = 0
c      ntrdraw0  = 0
c      itrk0     = 0
c      DO it = 1,ntrmx
c        itr_level(it) = 0
c      ENDDO 
C
  */
  ievent = sdakeys_.ievt;
  for(i=0; i<6; i++) istat[i] = 0;

  if(inter == 1)
  {
    sdakeys_.Ndbg  = 999999;
  }

  /* following call replaced by dchfill(); adjust if want histograms here 
c      CALL usda_anal(iw,rw,0,trkp,ntr_out,itag0,ev_out,tag_res)
*/

  dc_statb(1);
  if(sdakeys_.lanal[0] == 0) goto a999;

  if(sdakeys_.ltrig[3] == 0)
  {
    isecm = 1;
    nsect = 6;
  }
  else
  {
    isecm = sdakeys_.ltrig[3];
    nsect = sdakeys_.ltrig[3];
  }

  /*
C ?????????????????????????????????
c      do k=1,6
c        do is=1,6
c          do j=1,nsgmx
c            do n=1,12
c              segm(n,j,is,k) = 0 !!!!!!!!!!!!!!!!!!
c            enddo
c          enddo
c          nclust(is,k) = 0
c        enddo
c      enddo
C ?????????????????????????????????
  */

  for(isec=isecm-1; isec<nsect; isec++)
  {
    /*sim_segm_(isec,ifail,segm,clust,nclust,nsegmc);*/
    if(ifail != 0)
	{
      dc_statb(2);
      for(k=0; k<6; k++) vect1[k] = sdaevgen_.dc_trps[0][0][k];
	  /*
          if(ec_ndig(1,isec).GT.0) then
            ecdigi2(1) = ec_digi(1,1,1,isec)*2
            ecdigi2(2) = ec_digi(1,1,2,isec)*2
            ecdigi2(3) = ec_digi(1,1,3,isec)*2
          else
            ecdigi2(1) = 0
            ecdigi2(2) = 0
            ecdigi2(3) = 0
          endif
	  */
      pin = sdaevgen_.evin[0][6] * sdaevgen_.evin[0][8];
	  /*
	  sim_link_(isec,vect1,pin,sc_digi(1,1,1,isec),ecdigi2,ifail,iwl3,istat,segm,clust,nclust,nsegmc);
	  */
    }
  }

a999:

  /*
cC
cC Print statistics here if Interective
c      IF(inter.EQ.1) then
cc        CALL dc_statp
c        print *,'road: ',istat
c        CALL ICLRWK(0,1)
c        CALL sda_dsect(1,0.025,6.0,5.0,ntr_link,itr_sect,segm,lnk_segm)
c        CALL IUWK(0,1)
c        read *,iter
c      endif
C
C
  */
  return;
}
