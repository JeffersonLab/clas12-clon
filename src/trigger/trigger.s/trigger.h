
/* trigger.h */

#include "hls_fadc_sum.h"

#define MAXTIMES   8 /*8 for data, 1 for GEMC*/   /* maximum number of the time slices */
#define NTICKS     8    /* the number of ticks in one timing slice */
#define TIME2TICKS 4000 /* conversion factor from FADC250 integral time to 4ns ticks */

#if 0 /*was for PCAL*/
#define MAXTIMES   12/*0*/   /* maximum number of the time slices */
#define NTICKS     8    /* the number of ticks in one timing slice */
#define TIME2TICKS 4 /* conversion factor from FADC250 integral time to 4ns ticks */
#endif

/* possible values for parameter 'detector' */
#define ECIN  0
#define ECOUT 1
#define ECAL  2
#define PCAL  3
#define DCRB  4
#define HTCC  5
#define FTOF  6
#define CTOF  7
#define CND   8
#define FT1   9
#define FT2  10
#define FT3  11
#define NDET 12 /* update if adding new detector */



/* ECAL */

#define EC_THRESHOLD1 1
#define EC_THRESHOLD2 1
#define EC_THRESHOLD3 3

#define EC_NFRAMES 0
#define EC_STRIP_DIP_FACTOR 1
#define EC_NSTRIPMAX 0

#define EC_DALITZ_MIN (70<<3) /* about 560 */
#define EC_DALITZ_MAX (73<<3) /* about 584 */




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


#ifdef	__cplusplus
extern "C" {
#endif

int fadcs(unsigned int *bufptr, unsigned short threshold, int sec, int detector, hls::stream<fadc_16ch_t> s_fadc_words[NFADCS],
          int dtimestamp, int dpulsetime, int *iev, unsigned long long *timestamp);
void fadcs_32ns_to_8ns(hls::stream<fadc_16ch_t> &s_fadc_in, hls::stream<fadc_4ch_t> &s_fadc_out);
void fadcs_32ns_to_4ns(hls::stream<fadc_16ch_t> &s_fadc_in, hls::stream<fadc_2ch_t> &s_fadc_out);
void fadcs_to_onestream(int nslot, hls::stream<fadc_16ch_t> s_fadc_in[NFADCS], hls::stream<fadc_256ch_t> &s_fadc_out);

int trigbank_open(uint32_t *bufptr, int fragtag, int banktag, int iev, unsigned long long timestamp);
int trigbank_write(uint32_t *data);
int trigbank_close();

void eclib(unsigned int *bufptr, uint16_t threshold[3], uint16_t nframes, uint16_t dipfactor, uint16_t dalitzmin, uint16_t dalitzmax, uint16_t nstripmax);
void pclib(unsigned int *bufptr, uint16_t threshold[3], uint16_t nframes, uint16_t dipfactor, uint16_t dalitzmin, uint16_t dalitzmax, uint16_t nstripmax);
void htcclib(uint32_t *bufptr, uint16_t threshold[3], uint16_t nframes);
void ftoflib(uint32_t *bufptr, uint16_t threshold[3], uint16_t nframes);

#ifdef	__cplusplus
}
#endif
