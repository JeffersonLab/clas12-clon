#ifndef _PCLIB_


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



#define PSTRIP    /*84*/68
#define PSTRIPMAX 84
#define PPEAKMAX  10/*(PSTRIPMAX/2)*/
#define PLAYER    3
#define PSECTOR   6
#define PPEAK     3
#define PHIT      20
#define PSHOWER   30

typedef struct pcstrip *PCStripPtr;
typedef struct pcstrip
{
  uint16_t energy;       /* strip energy */
  /*uint16_t time;*/         /* strip time */
} PCStrip;


typedef struct pcpeak0 *PCPeak0Ptr;
typedef struct pcpeak0
{
  uint16_t energy;          /* peak energy */
  uint16_t strip1;          /* first strip in peak */
  uint16_t stripn;          /* the number of strips in peak */
  uint32_t energysum4coord; /* pass it to next function to do coordinate calculation */

} PCPeak0;

typedef struct pcpeak *PCPeakPtr;
typedef struct pcpeak
{
  uint16_t energy;       /* peak energy */
  /*uint16_t time;*/         /* peak time */

  uint16_t coord;        /* peak coordinate in 'some' units */
  /*uint16_t width;*/        /* peak width */

  uint16_t strip1;       /* first strip in peak */
  uint16_t stripn;       /* the number of strips in peak */

} PCPeak;


typedef struct pchit *PCHitPtr;
typedef struct pchit
{
  uint16_t energy;         /* hit energy */
  uint16_t time;           /* hit time */

  uint16_t coord[3];    /* u/v/w-coordinate */
  /*uint16_t width;*/        /* hit width */

  uint16_t   peak1[3];     /* first peak in hit in each layer */
  uint16_t   peakn[3];     /* the number of peaks in hit in eack layer */

} PCHit;



#define MAXDALZ 100


/* functions */

int pcinit(int runnum, int def_adc, int def_tdc, int def_atten);
int pcl3(unsigned int *bufptr, const unsigned short threshold[3], int io, PCHit hit[PHIT]);
int pcstrips(int *bufptr, unsigned short threshold, int sec, PCStrip strip[PLAYER][PSTRIP]);
int pcstrip(uint8_t view, PCStrip stripin[PSTRIP], PCStrip strip[PSTRIPMAX]);
int pcpeak(unsigned short strip_threshold, uint8_t view, PCStrip strip[PSTRIP], PCPeak0 peak[PPEAKMAX]);
int pcpeaksort(PCPeak0 peakin[PPEAKMAX], PCPeak0 peakout[PPEAK]);
int pcpeakcoord(uint8_t view, PCPeak0 peakin[PPEAK], PCPeak peakout[PPEAK]);
int pchit(uint8_t npeak[3],  PCPeak peak[3][PPEAK], uint16_t peakfrac[3][PPEAK], PCHit hit[PHIT]);
void pccorr(uint16_t threshold, uint8_t npeak[3], PCPeak peak[3][PPEAK], uint16_t peakfrac[3][PPEAK], uint8_t npsble, uint8_t *nhits, PCHit hit[PHIT], PCHit hitout[PHIT]);
uint8_t pcal(PCStrip strip[3][PSTRIP], uint8_t *nhits, PCHit hit[PHIT]);







/*6468/84=77*/
/*6468/77=84*/

#define UVWADD  40/*average between 77/2 and 84/2*/
#define UFACTOR 77
#define VFACTOR 84
#define WFACTOR VFACTOR

static uint8_t fview[3] = {UFACTOR,VFACTOR,WFACTOR};

static uint8_t iview[3] = {52,15,15};








#ifdef	__cplusplus
}
#endif


#define _PCLIB_
#endif
