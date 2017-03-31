#ifndef _ECLIB_

#include <ap_int.h>
#include <hls_stream.h>

#ifdef	__cplusplus
extern "C" {
#endif

/* eclib.h */


/************************/
/* some control defines */

/*#define NO_ATTEN*/ /* defining that will suppress attenuation correction */

/*#define DO_DIVIDE*/ /* will do actial division instead of lookup table */

/* some control defines */
/************************/



#define NSTRIP0   36
#define NSTRIP    36
#define NPEAKMAX  (NSTRIP/2)
#define NLAYER    6
#define NSECTOR   6
#define NPEAK     4
#define NHITMAX   (NPEAK*NPEAK*NPEAK) /* array size if index is calculated as 'u+(v<<2)+(w<<4)' */
#define NHIT      NHITMAX


#define ECPEAK0_ENERGY_MASK 0xFFFF
#define ECPEAK0_STRIP1_MASK 0x3F
#define ECPEAK0_STRIPN_MASK 0x3F
#define ECPEAK0_ENESUM_MASK 0xFFFFFF

#define ECPEAK0_ENERGY_BITS 16
#define ECPEAK0_STRIP1_BITS 6
#define ECPEAK0_STRIPN_BITS 6
#define ECPEAK0_ENESUM_BITS 24

#define ECPEAK0_BITS (ECPEAK0_ENERGY_BITS+ECPEAK0_STRIP1_BITS+ECPEAK0_STRIPN_BITS+ECPEAK0_ENESUM_BITS)


#include "ectypes.h"


#ifdef DEBUG
#define MAXDALZ 100
#endif


#ifdef	__cplusplus
}
#endif


#define _ECLIB_
#endif
