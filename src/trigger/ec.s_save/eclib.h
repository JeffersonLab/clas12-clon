#ifndef _ECLIB_


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



#define NSTRIP    36
#define NPEAKMAX  (NSTRIP/2)
#define NLAYER    6
#define NSECTOR   6
#define NPEAK     4
#define NHIT      10
#define NSHOWER   30

typedef struct ecstrip *ECStripPtr;
typedef struct ecstrip
{
  uint16_t energy;       /* strip energy */
  /*uint16_t time;*/         /* strip time */
} ECStrip;


typedef struct ecpeak0 *ECPeak0Ptr;
typedef struct ecpeak0
{
  uint16_t energy;          /* peak energy */
  uint8_t strip1;           /* first strip in peak */
  uint8_t stripn;           /* the number of strips in peak */
  uint32_t energysum4coord; /* pass it to next function to do coordinate calculation */

} ECPeak0;

typedef struct ecpeak *ECPeakPtr;
typedef struct ecpeak
{
  uint16_t energy;       /* peak energy */
  /*uint16_t time;*/         /* peak time */

  uint16_t coord;        /* peak coordinate in (strip#<<3) units */
  /*uint16_t width;*/        /* peak width */

  uint8_t strip1;       /* first strip in peak */
  uint8_t stripn;       /* the number of strips in peak */

} ECPeak;

typedef struct ecpeak1 *ECPeak1Ptr;
typedef struct ecpeak1
{
  uint16_t energy;       /* peak energy */

  uint8_t  nhits;        /* the number of hits where this peak participates */
  uint8_t  hitid[NHIT];

} ECPeak1;


typedef struct echit *ECHitPtr;
typedef struct echit
{
  uint16_t energy;         /* hit energy */
  /*uint16_t time;*/           /* hit time */

  uint16_t coord[3];       /* u/v/w coordinate */
  /*uint16_t width;*/        /* hit width */

  uint16_t corren[3];     /* u/v/w corrected energy */

  uint8_t   peak1[3];     /* first peak in hit in each layer */
  uint8_t   peakn[3];     /* the number of peaks in hit in eack layer */

} ECHit;






/* functions */

int  ecinit(int runnum, int def_adc, int def_tdc, int def_atten);
int  ecstrips(unsigned int *bufptr, unsigned short threshold, int sec, ECStrip strip[NLAYER][NSTRIP]);
int  ecl3(unsigned int *bufptr, const unsigned short threshold[3], int io, ECHit hit[NHIT]);
int  ecpeak(unsigned short strip_threshold, ECStrip strip[36], ECPeak0 peak[NSTRIP]);
int  ecpeaksort(ECPeak0 peakin[NSTRIP], ECPeak0 peakout[NPEAK]);
uint8_t ec_get_least_energy(uint16_t en[NPEAK]);
uint8_t ec_get_least_energy1(uint16_t en[NPEAK]);
int  ecpeakcoord(ECPeak0 peakin[NPEAK], ECPeak peakout[NPEAK]);
int  echit(uint8_t npeak[3],  ECPeak peak[3][NPEAK], ECPeak1 peak1[3][NPEAK], ECHit hit[NHIT]);
void ecfracratio(uint8_t edge, uint8_t peakID, uint8_t NhitsINpeakPtr, uint8_t peak_id[3][NHIT], uint16_t energy[3][NPEAK], uint8_t nhits[3][NPEAK], uint8_t hitid[NHIT], uint32_t frac[NHIT]);
void ecfrac(uint8_t npeak[3], ECPeak1 peak1[3][NPEAK], uint8_t npsble, ECHit hit[NHIT], uint32_t hitfrac[3][NHIT]);
void eccorr(uint16_t threshold, uint8_t npeak[3], ECPeak peak[3][NPEAK], ECPeak1 peak1[3][NPEAK], uint8_t npsble, uint8_t *nhits, ECHit hit[NHIT], uint32_t hitfrac[3][NHIT], ECHit hitout[NHIT]);
uint8_t ecal(ECStrip strip[3][36], uint8_t *nhits, ECHit hit[NHIT]);


#ifdef	__cplusplus
}
#endif


#define _ECLIB_
#endif
