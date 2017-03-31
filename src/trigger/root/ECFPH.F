      SUBROUTINE ECFPH(L,S)
c     
c     _begin_doc
c     RCS ID string
c     $Id: ecfph.F,v 1.14 2006/06/23 20:01:22 stepanya Exp $
c     
c     Documentation for subroutine ECFPH
c     
c     
c     Purpose: The name of the subroutine means 'EC - fit peaks to hits'.
c     This routine is called only by ecfit_analyze, after ecfsp has been called
c     for each view (u,v,w)  of the current layer. Therefore the initial guess
c     for the peaks in each view has been defined, and the present routine tries
c     to match them together into 'hits'.
c     
c     First there is a threefold loop which looks at all possible combinations 
c     of peaks in the u, v, and w views. For each iteration of the loop, the 
c     'dalitz' condition is checked using the centroids in each of the three
c     views, using the RMS as an estimate of the uncertainty in each view. A 
c     limit is imposed on the number of possible hits by the parameter MAXpsble,
c     currently defined as being equal to MAXhits, which is currently defined in 
c     Ec_general.PAR as being = 10. If this limit is exceeded, the event is not
c     analyzed.
c     
c     If the 'dalitz' condition is satisfied (geometrically plausible event), 
c     the path from the hit position along the strip is calculated, and the 
c     path length, identity of the peaks, and identity of the hit is stored and
c     the threefold loop ends.
c     
c     Now that the path lengths are known for all three views, the routine
c     ecfsp is called a second time, with iterr=2. This re-calculates the peak
c     characteristics after doing an attenuation length correction, so that
c     in principle a more accurate centroid, rms and higher moments are 
c     obtained. 
c     
c     Next, peaks which are part of more than one hit are treated. If for all
c     hits there is only one peak in each view, then the routine proceeds to 
c     the next step. If a given peak contributes to multiple hits, then the 
c     energy in each hit due to that peak is calculated as being proportional
c     to the relative sizes of the multiple hits as measured in other views.
c     That is, if there are two hits, both of which have the same 'u' peak, 
c     the energy in v and w is added for each of the hits, and the ratio of
c     these summed energies determines how much of the u peak's energy is 
c     assigned to each of the two hits. The code does not attempt to handle
c     more complicated events, which, of course, are topologically possible.
c     
c     Then the hit is again checked for the 'dalitz' condition, using the 
c     refined (attenuation-length-corrected) centroid and RMS values; at the
c     present time no action is taken based on the results of this check.
c     Next the hits are sorted by energy, and a threshold cut is applied. This
c     threshold value is set by the tcl parameter 'EcFitEdge_hit_threshold',
c     and it is a true energy threshold since attenuation corrections have
c     already been applied. After this, a number of characteristics of the
c     hit are stored, and the subroutine returns.
c     
c     The above documentation last updated by W. Brooks, Jan 1998.
c     
c     Input Parameters:  L - integer - Layer number (9-Whole, 10-Inner, 11-Outer)
c     ----------------   S - integer - CLAS Sector number (1 to 6)
c     
c     Output Parameters:  NON
c     -----------------
c     
c     Other routines:
c     ---------------
c     
c     Notes:
c     ------
c     
c     Author:    S. Stepanyan     Created:  Fri Aug  9 10:50:48 EDT 1996
c     -------
c     
c     Major revisions:
c     ----------------
c     
c     W. Brooks, Jan 1998 - added documentation, indenting.
c     
c     Ky Kim, May-11-1999, removed "Timing part" which are now done in "ec_time"
c     function subroutine.
c     
c     _end_doc
c     
      IMPLICIT NONE
      SAVE
c     
c     _begin_inc
c     include files :
c     ---------------------
c     BOS common block  uncomment the next line for BOS include file
c     #include "bcs.inc"
#include "Ec_general.PAR"
#include "EcStrip.CMN"
#include "ecgeom.inc"
#include "EcFit_general.CMN"
#include "EcFit_edge.CMN"
#include "ec_control.inc"
#include "ec_strip.inc"
#include "EcCal.CMN"
#include "ec_peak.inc"
*     !! EcCal.CMN and EcCal.DTE is the same except
c     _end_inc
c     
c     _begin_var
c     input/output variables:
c     -----------------------
c     
      integer L,S
c     Local pre-defined variables:
c     ---------------------------
c     RCS information: 
      CHARACTER*132  CFILE, CREVIS, CSTATE, CDATE, CAUTHO, CRCSID
      PARAMETER (CFILE = '$RCSfile: ecfph.F,v $')
      PARAMETER (CREVIS = '$Revision: 1.14 $')
      PARAMETER (CSTATE = '$State: Exp $')
      PARAMETER (CDATE = '$Date: 2006/06/23 20:01:22 $')
      PARAMETER (CAUTHO = '$Author: stepanya $')
      DATA CRCSID/   
     1     '$Id: ecfph.F,v 1.14 2006/06/23 20:01:22 stepanya Exp $'   
     2     /   
c     Module information:
      CHARACTER*(*)  CRNAME, CRAUTH
      CHARACTER*100  CRMESS
      PARAMETER (CRNAME='ECFPH')
      PARAMETER (CRAUTH='Stepan Stepanyan')
c     
c     Local User defined variables:
c     -----------------------------
c     INTEGER NWRITE
*     
      integer id,ids( MAXstrips)
      real d(Ec_i:Ec_dk)
      real du,dv,dw,ddu,ddv,ddw
      integer ith,axis,hit
*     
      integer MAXpsble
      parameter (MAXpsble=  MAXhits)
*     
      integer Npsble,edge,Nhit
      integer peakID
      real psble( Ec_i: last_par,MAXpsble)
      real possibleE(MAXpsble)
      integer peak( Ec_u: Ec_w),pid,pUid,pVid,pWid
      integer peak_hitID(Ec_u: Ec_w,MAXpsble)
      real peak_assnd( MAXpeaks, Ec_u: Ec_w)
*     
      EXTERNAL Ec_dalitz
      LOGICAL Ec_dalitz         !FUNCTION
      logical good,OK,go_back
      real RMS_spread,err,path( Ec_u: Ec_w)
      integer prj,iterr,hit_count, jth, peak_stat(MAXpeaks,MAXpeaks,MAXpeaks)
      integer hitID, peak_id
      real sume0,sume(maxhits),timing_we,timing_wr, hit_ch2(maxhits)
      integer nvalid(maxhits)
      integer istr,ist,pth,ierr
*     
c     _end_var
c     
c     executable code for routine ECFPH:
c---- 6----------------------------------------------------------------72
*     
      DO pUid= 1,EcFit_Npeak(Ec_U,L,s) !loop by U peaks 
         Do pVID= 1,EcFit_Npeak(Ec_V,L,s) !loop by V peaks
            do pWID= 1,EcFit_Npeak(Ec_W,L,s) !loop by W peaks
               peak_stat(pUid,pVid,pWid)=0
            enddo
         enddo
      enddo
 711  Npsble= 0                 !number possible hits
      iterr=1    
      do ith=1,maxhits
         hit_ch2(ith)=100.0
      enddo
*     
      DO edge= Ec_U,Ec_W        !clear assignments
         Do pid= 1,EcFit_Npeak(edge,L,s)
            peak_Nhit(pid,edge)= 0 !number of hits assigned 
            peak_assnd(pid,edge)= 0. !to the peak
         EndDo
      ENDDO
*     
      DO pUid= 1,EcFit_Npeak(Ec_U,L,s) !loop by U peaks 
         peak(Ec_U)= pUid
         Do pVID= 1,EcFit_Npeak(Ec_V,L,s) !loop by V peaks
            peak(EC_V)= pVid
            do pWID= 1,EcFit_Npeak(Ec_W,L,s) !loop by W peaks
               peak(Ec_W)= pWid
               if(peak_stat(pUid,pVid,pWid).eq.0)then
*     
                  du=EcFit_peak(DIST,pUid,Ec_U,L,s)
                  dv=EcFit_peak(DIST,pVid,Ec_V,L,s)
                  dw=EcFit_peak(DIST,pWid,Ec_W,L,s)
                  ddu=EcFit_peak(WIDTH,pUid,Ec_U,L,s)
                  ddv=EcFit_peak(WIDTH,pVid,Ec_V,L,s)
                  ddw=EcFit_peak(WIDTH,pWid,Ec_W,L,s)
*     
                  good= Ec_dalitz(du,dv,dw,ddu,ddv,ddw,d(EC_I),d(EC_J),
     &                 d(Ec_K),d(EC_dI),d(EC_dJ),RMS_spread,L,s,err,iterr)       
*     
                  OK= Npsble.LT.MAXpsble
                  If(good .and. .NOT.OK) Then
                     EcFit_Nhit(L,s)= 0 !Event is skipped
                     RETURN
                  ENDIF
*     
                  IF(good) THEN
*     Posible hit is founed
                     Npsble= Npsble+1
                     hit_ch2(npsble)=err
*     
*     
*     -find path from hit position to the edge (along the strip)
                     call Ec_ij_path(d(Ec_I),d(Ec_J),
     &                    path(Ec_U),path(Ec_V),path(Ec_W),L,s)
*     
                     Do edge= Ec_U,Ec_W
*     - store ID of peak(s) making up a possible hit
                        peak_hitID(edge,Npsble)= peak(edge)
*     
*     - store ID&# of hit(s) that peak may belong to
                        peak_Nhit(peak(edge),edge)=
     &                       peak_Nhit(peak(edge),edge) + 1
                        peak_hit(peak_Nhit(peak(edge),edge),
     &                       peak(edge),edge)=Npsble
*     - store the path for hit(s) that peak may belong to
                        peak_path(Npsble,edge)=
     &                       path(edge)
*     
                     EndDo
*     
                  ENDIF         !dalitz rule
*     
               endif
            enddo
         EndDo
      ENDDO                     !  end all peak loop
*******************************************************************
*     
*     At this point all posible hits are identified and paths for each 
*     hit on the peak are defined, time for attenuation correction   
*     
*******************************************************************
*     
      IF ( Npsble .GT. 0 ) THEN
         iterr=2
         ierr=0
         DO axis= Ec_U,Ec_W 
            CALL Ecfsp(axis,l,s,iterr,ierr)
         ENDDO              
*******************************************************************
*     
*     A
*     
*     
*******************************************************************
*     
*  Count how many peaks belong to a single hit   
         hit_count=0
         DO ith=1,Npsble
            Do edge= Ec_U,Ec_W
               peakID=peak_hitID(edge,ith)
               ecfit_hit_fraction(edge,ith,l,s)=0.
               IF ( peak_Nhit(peakID,edge).EQ.1 ) THEN
                  ecfit_hit_fraction(edge,ith,l,s)=1.
                  hit_count=hit_count+1
               ENDIF
            EndDo
         ENDDO
c if each peak belongs to a single hit (cluster) go determonation of the energy and time of clusters
         IF ( hit_count.EQ.3*Npsble) GO TO 701
c for peaks that belong to more than 1 cluster, calculate energy fractions 
         DO edge= Ec_U,Ec_W
            Do peakID=1,EcFit_Npeak(edge,L,s)
               IF ( peak_Nhit(peakID,edge).GT.1 ) THEN
                  sumE0 = 0.0
                  do jth = 1,peak_Nhit(peakID,edge)
                     hitID=peak_hit(jth,peakID,edge) ! cluster ID
                     sumE(jth) = 0.0
                     Nvalid(jth) = 0.0
                     dO axis = Ec_U,Ec_W
                        If(axis.ne.edge)Then
                           peak_ID=peak_hitID(axis,hitID)
c find peaks on other two edges that are involved in this cluser only
                           if ( peak_Nhit(peak_ID,axis).EQ.1 ) then
                              Nvalid(jth) = Nvalid(jth) + 1
                              sumE(jth) = sumE(jth) + 
     &                             Ec_peak(ENERGY,hitID,axis,L,s) 
     &                             * ecfit_hit_fraction(axis,hitID,l,s) 
                           endif
                        EndIf
                     enddO
c at least one of peaks should belong to one cluster
                     if ( Nvalid(jth).gt.0 ) then
                        sumE(jth) = sumE(jth) / Nvalid(jth)
                     endif
                     sumE0 = sumE0 + sumE(jth)
                  enddo 
                  if ( sumE0.le.0 ) go to 601
                  do jth = 1,peak_Nhit(peakID,edge)
                     hitID=peak_hit(jth,peakID,edge)
                     ecfit_hit_fraction(edge,hitID,l,s) = sumE(jth)/sumE0
                  enddo
                  hit_count=hit_count+peak_Nhit(peakID,edge)
                  peak_Nhit(peakID,edge)=1
 601              continue
               ENDIF
            EndDo
         ENDDO
c         
         IF ( hit_count.EQ.0) then
            go_back=.false.
            do ith=1,npsble
               do jth = ith+1,npsble
                  hit_count=0
                  if(peak_hitID(EC_U,ith).eq.peak_hitID(EC_U,jth))hit_count=hit_count+1
                  if(peak_hitID(EC_V,ith).eq.peak_hitID(EC_V,jth))hit_count=hit_count+1
                  if(peak_hitID(EC_W,ith).eq.peak_hitID(EC_W,jth))hit_count=hit_count+1
                  if(hit_count.ge.2)then
                     go_back=.true.
                     if(hit_ch2(ith).lt.hit_ch2(jth))then
                        peak_stat(peak_hitID(EC_U,jth),peak_hitID(EC_V,jth),peak_hitID(EC_W,jth))=-1
                     else
                        peak_stat(peak_hitID(EC_U,ith),peak_hitID(EC_V,ith),peak_hitID(EC_W,ith))=-1
                     endif
                  endif
               enddo
            enddo
            if(go_back)goto 711
         endif
*   esum(layer)  
 701     CONTINUE

         DO ith=1,Npsble
            possibleE(ith) = 0.0
            psble(TIME,ith)=0.0
            timing_we=0.0
            timing_wr=0.0
            Do edge= Ec_U,Ec_W
               possibleE(ith) = possibleE(ith) + 
     &              Ec_peak(ENERGY,ith,edge,L,s) * 
     &              ecfit_hit_fraction(edge,ith,l,s)
               psble(TIME,ith)=psble(TIME,ith)+
     &              Ec_peak(TIME,ith,edge,L,s)
               timing_we=timing_we+time_we(ith,edge,L,s)
            EndDo
            if(L.ne.whole.and.timing_we.gt.0.)then
               psble(TIME,ith)=psble(TIME,ith)/timing_we
            else
               psble(TIME,ith)=-999.
            endif
*     
            du=Ec_peak(DIST,ith,Ec_U,L,s)
            dv=Ec_peak(DIST,ith,Ec_V,L,s)
            dw=Ec_peak(DIST,ith,Ec_W,L,s)
            ddu=Ec_peak(WIDTH,ith,Ec_U,L,s)
            ddv=Ec_peak(WIDTH,ith,Ec_V,L,s)
            ddw=Ec_peak(WIDTH,ith,Ec_W,L,s)
*     
            good= Ec_dalitz(du,dv,dw,ddu,ddv,ddw,
     &           psble(EC_I,ith),psble(EC_J,ith),psble(Ec_K,ith),
     &           psble(EC_dI,ith),psble(EC_dJ,ith),psble(WIDTH,ith),
     &           L,s,err,iterr)
            hit_ch2(ith)=err
         ENDDO
*     
*     -sort by decreasing energy
         call EcGus_sort(Npsble,possibleE,ids)
*     - pick out those above threshold
         Nhit= 0
         DO ith=1,Npsble
            If(possibleE(ids(ith)).lt.EcFitEdge_hit_threshold) go to 88 !above thre+
            Nhit= Nhit+1        !#
         ENDDO
*     
 88      continue
         Nhit= MIN(Nhit, MAXhits) !restrict space
*     
         EcFit_Nhit(L,s)= Nhit  !total hits in this layer
*     
         DO hit= 1,Nhit
            id= ids(hit)
            EcFit_hit(ENERGY,hit,L,s) = possibleE(id)
            EcFit_hit(TIME,hit,L,s)= psble(TIME,id)
*     
            Do prj= Ec_I, Ec_dJ
               EcFit_hit(prj,hit,L,s)= psble(prj,id)
            EndDo
*     
            ntstripc(hit,l,s)=0
            dO axis=Ec_U,Ec_W
               pth=peak_hitID(axis,id)
               ppath(hit,axis,l,s)=peak_path(id,axis)
               nstripc(hit,axis,l,s)=EcFit_peak_Nstrip(pth,axis,l,s)
               ntstripc(hit,l,s)=ntstripc(hit,l,s)+nstripc(hit,axis,l,s)
               Do ist=1,MAXwstrips
                  stripc(ist,hit,axis,L,s)=0.
                  ec_strip_e(hit,ist,axis,l,s)=0.
                  ec_strip_t(hit,ist,axis,l,s)=0.
                  do istr=1,EcFit_peak_Nstrip(pth,axis,l,s)
                     if(ist.eq.EcFit_peak_strip(istr,pth,axis,l,s))then
                        stripc(ist,hit,axis,L,s)=1.
                        ec_strip_e(hit,ist,axis,l,s)=
     &                       ec_stripe(id,istr,axis,l,s)
                        ec_strip_t(hit,ist,axis,l,s)=
     &                       ec_stript(id,istr,axis,l,s)
                     endif
                  enddo
               EndDo
            enDdO
*     
            call Ec_ijk_xyz(
     &           EcFit_hit(Ec_I,hit,L,s),EcFit_hit(Ec_J,hit,L,s),
     &           EcFit_hit(Ec_K,hit,L,s),EcFit_hit(Ec_x,hit,L,s),
     &           EcFit_hit(Ec_Y,hit,L,s),EcFit_hit(Ec_Z,hit,L,s),s,1)
*     
            call Ec_ijk_xyz(
     &           EcFit_hit(Ec_dI,hit,L,s),EcFit_hit(Ec_dJ,hit,L,s),
     &           EcFit_hit(Ec_dK,hit,L,s),EcFit_hit(Ec_dx,hit,L,s),
     &           EcFit_hit(Ec_dY,hit,L,s),EcFit_hit(Ec_dZ,hit,L,s),s,2)
*     

            EcFit_hit(WIDTH,hit,L,s)= psble(WIDTH,id)
            EcFit_hit(LAST_PAR,hit,L,s)= 10.
*     
*     - add in quality/probablity estimates later
*     
         ENDDO
      ELSE
         EcFit_Nhit(L,s)= 0     !total hits in this layer
      ENDIF
c
      RETURN
      END
c     
c------------------------------------------------------------------------------
