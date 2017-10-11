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

#define NSLOT      7
#define MAXTIMES   8 /*8 for data, 1 for GEMC*/   /* maximum number of the time slices */
#define NTICKS     8    /* the number of ticks in one timing slice */
#define TIME2TICKS 4000 /* conversion factor from FADC250 integral time to 4ns ticks */

#define NSTRIP    36
#define NPEAKMAX  (NSTRIP/2)
#define NLAYER    6
#define NSECTOR   6
#define NPEAK     4
#define NHITMAX   (NPEAK*NPEAK*NPEAK) /* array size if index is calculated as 'u+(v<<2)+(w<<4)' */
#define NHIT      4




#include "ectypes.h"



#ifdef DEBUG
#define MAXDALZ 100
#endif


#ifdef	__cplusplus
}
#endif


#define _ECLIB_
#endif
