#ifndef _PCLIB_

#include <ap_int.h>
#include <hls_stream.h>

#define USE_PCAL

#ifdef	__cplusplus
extern "C" {
#endif

/* pclib.h */


/************************/
/* some control defines */

/*#define NO_ATTEN*/ /* defining that will suppress attenuation correction */

#define DO_DIVIDE /* will do actial division instead of lookup table */

/* some control defines */
/************************/

#define PSTRIP    68
#define PSTRIPMAX 84
#define PPEAKMAX  (PSTRIPMAX/2)
#define PLAYER    3
#define PSECTOR   6
#define PPEAK     4
#define PHIT      4

#define NSTRIP0   PSTRIP
#define NSTRIP    PSTRIPMAX
#define NPEAKMAX  PPEAKMAX
#define NLAYER    PLAYER
#define NSECTOR   PSECTOR
#define NPEAK     PPEAK
#define NHITMAX   (NPEAK*NPEAK*NPEAK) /* array size if index is calculated as 'u+(v<<2)+(w<<4)' */
#define NHIT      PHIT





/* converting from u/v/w to ind and back assuming NPEAK=4 */
#define IND4(u,v,w)  u|(v<<2)|(w<<4)
#define U4(ind)      ind&0x3
#define V4(ind)      (ind>>2)&0x3
#define W4(ind)      (ind>>4)&0x3


#define ecal pcal
#define ecal_peak pcal_peak
#define ecal_hit pcal_hit
#define ectrig pctrig

#define ecstrip_in  pcstrip_in
#define ecstrip_out pcstrip_out
#define ecpeak_in pcpeak_in
#define echit_in pchit_in

#define ecstrips pcstrips
#define ecstripspersistence pcstripspersistence
#define ecstripspersistence0 pcstripspersistence0
#define ecstripspersistence1 pcstripspersistence1
#define ecstripspersistence2 pcstripspersistence2
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
#define ecpeakeventfiller pcpeakeventfiller
#define ecpeakeventfiller0 pcpeakeventfiller0
#define ecpeakeventfiller1 pcpeakeventfiller1
#define ecpeakeventfiller2 pcpeakeventfiller2
#define ecpeakeventwriter pcpeakeventwriter
#define ecpeakeventreader pcpeakeventreader
#define echiteventfiller pchiteventfiller
#define echiteventwriter pchiteventwriter
#define echiteventreader pchiteventreader


#include "../ec.s/ectypes.h"



#ifdef DEBUG
#define MAXDALZ 100
#endif





#ifdef	__cplusplus
}
#endif


#define _PCLIB_
#endif
