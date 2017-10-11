
/* trigger.h */


/* ECAL */

#define EC_THRESHOLD1 1
#define EC_THRESHOLD2 1
#define EC_THRESHOLD3 3

#define EC_NFRAMES 0
#define EC_STRIP_DIP_FACTOR 1
#define EC_NSTRIPMAX 0

#define EC_DALITZ_MIN (70<<3) /* about 560 */
#define EC_DALITZ_MAX (73<<3) /* about 584 */

void eclib(unsigned int *bufptr, uint16_t threshold[3], uint16_t nframes, uint16_t dipfactor, uint16_t dalitzmin, uint16_t dalitzmax, uint16_t nstripmax);




/* PCAL */

#define PC_THRESHOLD1 1
#define PC_THRESHOLD2 1
#define PC_THRESHOLD3 3

#define PC_NFRAMES 0
#define PC_STRIP_DIP_FACTOR 1
#define PC_NSTRIPMAX 0

#define ABC (80*81*2/28./*UVWDIV*/) /* about 925 */
#define PC_DALITZ_MIN (ABC-ABC/50) /* -2% */
#define PC_DALITZ_MAX (ABC+ABC/50) /* +2% */

void pclib(unsigned int *bufptr, uint16_t threshold[3], uint16_t nframes, uint16_t dipfactor, uint16_t dalitzmin, uint16_t dalitzmax, uint16_t nstripmax);
