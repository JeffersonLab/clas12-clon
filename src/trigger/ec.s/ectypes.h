
/* ectypes.h - data types (structs etc) */


/* converting from u/v/w to ind and back assuming NPEAK=4 */
#define IND4(u,v,w)  u|(v<<2)|(w<<4)
#define U4(ind)      ind&0x3
#define V4(ind)      (ind>>2)&0x3
#define W4(ind)      (ind>>4)&0x3



#define FACTOR1 256
#define SHIFT1  8

#define FACTOR2 128
#define SHIFT2  7

#define SHIFT1_SHIFT2  (SHIFT1+SHIFT2)




typedef struct ecstrip *ECStripPtr;
typedef struct ecstrip
{
  ap_uint<13> energy;       /* strip energy */
} ECStrip;

typedef struct trigecpeak *TrigECPeakPtr;
typedef struct trigecpeak
{
  ap_uint<16> coord;
  ap_uint<16> energy;
  ap_uint<16> time;

} TrigECPeak;


typedef struct trigechit *TrigECHitPtr;
typedef struct trigechit
{
  ap_uint<16> coord[3];
  ap_uint<16> energy;
  ap_uint<16> time;

} TrigECHit;



typedef struct ecpeak0 *ECPeak0Ptr;
typedef struct ecpeak0
{
  ap_uint<16> energy;          /* peak energy */
  ap_uint<6>  strip1;          /* first strip in peak */
  ap_uint<6>  stripn;          /* the number of strips in peak */
  ap_uint<24> energysum4coord; /* pass it to next function to do coordinate calculation */

} ECPeak0;

typedef struct ecpeak *ECPeakPtr;
typedef struct ecpeak
{
  ap_uint<16> energy;       /* peak energy */
  ap_uint<16> coord;        /* peak coordinate in (strip#<<3) units */
  ap_uint<6>  strip1;       /* first strip in peak */
  ap_uint<6>  stripn;       /* the number of strips in peak */

} ECPeak;

typedef struct ecpeak1 *ECPeak1Ptr;
typedef struct ecpeak1
{
  ap_uint<16> energy;       /* peak energy */
  ap_uint<4>  nhits;        /* the number of hits where this peak participates */
  ap_uint<4>  hitid[NHIT];

} ECPeak1;





typedef struct echit *ECHitPtr;
typedef struct echit
{
  ap_uint<16> energy;       /* hit energy [16] */
  ap_uint<16> coord[3];     /* u/v/w coordinate [48] */

} ECHit;





#define NUMBER_OF_BITS_DECIMAL	 18
#define NUMBER_OF_BITS_FRACTIONAL 3
typedef ap_ufixed<(NUMBER_OF_BITS_DECIMAL+NUMBER_OF_BITS_FRACTIONAL),NUMBER_OF_BITS_DECIMAL> fp1803_t;
//typedef ap_uint<24> fp1803_t;




//typedef ap_ufixed<2, 1, AP_RND, AP_SAT> fp0201S_t;
typedef ap_uint<6> fp0201S_t;




typedef struct ecpeakout *ECPeakoutPtr;
typedef struct ecpeakout
{
  ap_uint<16> energy;       /* peak energy */
  fp1803_t    sumE0;         
} ECPeakout;
typedef ECPeakout peakout_t;


typedef struct echitout *ECHitoutPtr;
typedef struct echitout
{
  ap_uint<16> energy[3];    /* peaks energies */
  ap_uint<16> coord[3];     /* u/v/w coordinate */

} ECHitout;
typedef ECHitout hitout_t;


typedef struct echitsume *ECHitSumEPtr;
typedef struct echitsume
{
  ap_uint<24>    sumE;         /* sum of energies for UNIQUE peaks only normalized for the number of peaks (E = E / Nvalid) */
  ap_uint<24>    peak_sumE[3]; 

} ECHitSumE;
typedef ECHitSumE hitsume_t;





/* functions */

int  ecinit(int runnum, int def_adc, int def_tdc, int def_atten);
int  ecl3(unsigned int *bufptr, unsigned short threshold[3], int io, ECHit hitshits[NHIT]);
uint8_t ecal(ECStrip strip[3][NSTRIP], ECHit hit[NHIT]);
int  ectrig(unsigned int *bufptr, int sec, int npeak[NLAYER], TrigECPeak peak[NLAYER][NPEAKMAX], int nhits_trig[2], TrigECHit hits_trig[2][NHITMAX]);
int  ecstrips(unsigned int *bufptr, unsigned short threshold, int sec, int io, ECStrip strip[3][NSTRIP]);

int  ecstrip(uint8_t view, ECStrip stripin[NSTRIP0], ECStrip stripout[NSTRIP]);
int  ecpeak(unsigned short strip_threshold, ECStrip strip[NSTRIP], ECPeak0 peak[NSTRIP]);
int  ecpeaksort(ECPeak0 peakin[NSTRIP], ECPeak0 peakout[NPEAK]);
uint8_t ec_get_least_energy(uint16_t en[NPEAK]);
int  ecpeakcoord(uint8_t view, ECPeak0 peakin[NPEAK], ECPeak peakout[NPEAK]);

int  echit(ECPeak peak[3][NPEAK], fp0201S_t peakcount[3][NPEAK], ap_uint<16> energy[NHITMAX][3], ap_uint<16> coord[NHITMAX][3]);
void ecfrac(fp0201S_t peakcount[3][NPEAK], ap_uint<16> energy[NHITMAX][3], uint16_t fracout[NHITMAX][3]);
uint8_t eccorr(uint16_t threshold, ap_uint<16> energy[NHITMAX][3], ap_uint<16> coord[NHITMAX][3], uint16_t fracin[NHITMAX][3], ap_uint<16> enout[NHITMAX]);
uint8_t echitsort(ap_uint<16> enin[NHITMAX], ap_uint<16> coord[NHITMAX][3], ECHit hitout[NHIT]);


