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
#define PPEAKMAX  10/*(PSTRIPMAX/2)*/
#define PLAYER    3
#define PSECTOR   6
#define PPEAK     4
#define PHIT      10

/* converting from u/v/w to ind and back assuming NPEAK=4 */
#define IND4(u,v,w)  u|(v<<2)|(w<<4)
#define U4(ind)      ind&0x3
#define V4(ind)      (ind>>2)&0x3
#define W4(ind)      (ind>>4)&0x3

#define NSTRIP0   PSTRIP
#define NSTRIP    PSTRIPMAX
#define NPEAKMAX  PPEAKMAX
#define NLAYER    PLAYER
#define NSECTOR   PSECTOR
#define NPEAK     PPEAK
#define NHIT      PHIT
#define NHITMAX   (NPEAK*NPEAK*NPEAK) /* array size if index is calculated as 'u+(v<<2)+(w<<4)' */


#define ECPEAK0_ENERGY_MASK 0xFFFF
#define ECPEAK0_STRIP1_MASK 0x7F
#define ECPEAK0_STRIPN_MASK 0x7F
#define ECPEAK0_ENESUM_MASK 0xFFFFFF

#define ECPEAK0_ENERGY_BITS 16
#define ECPEAK0_STRIP1_BITS 7
#define ECPEAK0_STRIPN_BITS 7
#define ECPEAK0_ENESUM_BITS 24

#define ECPEAK0_BITS (ECPEAK0_ENERGY_BITS+ECPEAK0_STRIP1_BITS+ECPEAK0_STRIPN_BITS+ECPEAK0_ENESUM_BITS)


#define ecinit pcinit
#define ecl3 pcl3
#define ecal pcal
#define ectrig pctrig
#define ecstrips pcstrips
#define ecstrip pcstrip
#define ecpeak pcpeak
#define ecpeaksort pcpeaksort
#define ecpeakcoord pcpeakcoord
#define echit pchit
#define ecfrac pcfrac
#define eccorr pccorr
#define echitsort pchitsort


#include "ectypes.h"


  /* for PCAL 'normalization' */
#define UVWADD  40/*average between 77/2 and 84/2*/
#define UFACTOR 77
#define VFACTOR 84
#define WFACTOR 84
static uint8_t fview[3] = {UFACTOR,VFACTOR,WFACTOR};




#ifdef DEBUG
#define MAXDALZ 100
#endif





#ifdef	__cplusplus
}
#endif


#define _PCLIB_
#endif
