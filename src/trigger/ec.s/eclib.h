#ifndef _ECLIB_

#include <ap_int.h>
/*#include <ap_fixed.h>*/
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
#define NVIEW     3
#define NSECTOR   6
#define NPEAK     4
#define NHITMAX   (NPEAK*NPEAK*NPEAK) /* array size if index is calculated as 'u+(v<<2)+(w<<4)' */
#define NHIT      4


/* define maximum peaks and hits in entire time interval; MAXTIMES defined in trigger.h */
#define NPEAKS (NPEAK*MAXTIMES)
#define NHITS (NHIT*MAXTIMES)


#include "ectypes.h"


#ifdef DEBUG
#define MAXDALZ 100
#endif


#ifdef	__cplusplus
}
#endif


#define _ECLIB_
#endif
