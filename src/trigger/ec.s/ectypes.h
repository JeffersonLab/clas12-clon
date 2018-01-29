
/* ectypes.h - data types (structs etc) */

#include "hls_fadc_sum.h"
#include "trigger.h"

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


/* fixed point type for peak coordinate calculation - see ecpeakcoord.cc */
#define NUMBER_OF_BITS				24/*32*/
#define NUMBER_OF_DECIMAL_DIGITS	3/*6*/
typedef ap_fixed<NUMBER_OF_BITS,(NUMBER_OF_BITS-NUMBER_OF_DECIMAL_DIGITS)> myfp_t;


#ifdef USE_PCAL

/* for PCAL 'normalization' */

#define NBIT_COORD 8/*9*//*10*/ /* can be 8,9,10 - the number of bits in peak coordinate */

#define USTRIPS 42
#define VSTRIPS 39
#define WSTRIPS 39



#if NBIT_COORD==8
#define UVWDIV 7. /* 42*42/256 and round to bigger integer */
#endif

#if NBIT_COORD==9
#define UVWDIV 4. /* 42*42/512 */
#endif

#if NBIT_COORD==10
#define UVWDIV 2. /* 42*42/1024 */
#endif



#define UFACTOR (38.5/UVWDIV) /*11 or 5.5 or 2.75 */
#define VFACTOR (42./UVWDIV) /*12 or 6   or 3 */
#define WFACTOR (42./UVWDIV) /*12 or 6   or 3 */

#define UHALF   (UFACTOR/2.)
#define VHALF   (VFACTOR/2.)
#define WHALF   (WFACTOR/2.)


static myfp_t fview[3] = {UFACTOR,VFACTOR,WFACTOR};
static myfp_t fhalf[3] = {UHALF,VHALF,WHALF};

#define NBIT_DALITZ 12

#else

/* for ECAL 'normalization' */

#define NBIT_COORD 9 /* the number of bits in peak coordinate */

#define USTRIPS 36
#define VSTRIPS 36
#define WSTRIPS 36

#define UFACTOR 8
#define VFACTOR 8
#define WFACTOR 8

#define UHALF   4 /*UFACTOR/2*/
#define VHALF   4 /*VFACTOR/2*/
#define WHALF   4 /*WFACTOR/2*/

static myfp_t fview[3] = {UFACTOR,VFACTOR,WFACTOR};
static myfp_t fhalf[3] = {UHALF,VHALF,WHALF};

#define NBIT_DALITZ 10

#endif





#define NH_READS   4   /* 8/4 hits area: the number of reads-write for streams */

#define NH_FIFOS  16  /* 8/16 hits area: the number of streams */

/* define strip nfifo - NSTRIP/NH_READS/2 rounded up */
#ifdef USE_PCAL
#define NF1  6  /* 42/NH_READS /2 */
#else
#define NF1  5 /* 36/NH_READS /2 */
#endif

/* define peak0strip nfifo - NSTRIP/NH_READS rounded up */
#ifdef USE_PCAL
#define NF2 11 /* 42/NH_READS */
#else
#define NF2  9 /* 36/NH_READS */
#endif

/* define peak0max nfifo - NPEAKMAX/NH_READS rounded up */
#ifdef USE_PCAL
#define NF3  6 /* 21/NH_READS */
#else
#define NF3  5 /* 18/NH_READS */
#endif

/* define hit sorting fifo */
#define NR4 4
#define NF4 (NHITMAX/NR4)

/* strip persistency */
#define NPER 4
typedef ap_uint<2> nframe_t;


typedef struct trigecpeak *TrigECPeakPtr;
typedef struct trigecpeak
{
  ap_uint<NBIT_COORD> coord;
  ap_uint<16> energy;
  ap_uint<16> time;
#ifndef __SYNTHESIS__
  ap_uint<7>  strip1;
  ap_uint<7>  stripn;
#endif

} TrigECPeak;


typedef struct trigechit *TrigECHitPtr;
typedef struct trigechit
{
  ap_uint<NBIT_COORD> coord[3];
  ap_uint<16> energy;
  ap_uint<16> time;
#ifndef __SYNTHESIS__
  ap_uint<6>  ind;               /* original hit index before sorting - can be used to link with peaks */
  ap_uint<16> enpeak[3];         /* u/v/w energy [48] - for drawing purposes */
#endif

} TrigECHit;






typedef struct ecstrip *ECStripPtr;
typedef struct ecstrip
{
  ap_uint<13> energy;       /* strip energy */
} ECStrip;

typedef struct ecstrip_s
{
  ap_uint<13> energy0;
  ap_uint<13> energy1;

} ECStrip_s;







#define ECPEAK0_ENERGY_MASK 0xFFFF
#define ECPEAK0_STRIP1_MASK 0x7F
#define ECPEAK0_STRIPN_MASK 0x7F
#define ECPEAK0_ENESUM_MASK 0xFFFFFF

#define ECPEAK0_ENERGY_BITS 16
#define ECPEAK0_STRIP1_BITS 7
#define ECPEAK0_STRIPN_BITS 7
#define ECPEAK0_ENESUM_BITS 24

#define ECPEAK0_BITS (ECPEAK0_ENERGY_BITS+ECPEAK0_STRIP1_BITS+ECPEAK0_STRIPN_BITS+ECPEAK0_ENESUM_BITS)
/* for packing in following order: [high bit -- ENESUM,STRIPN,STRIP1,ENERGY -- low bit] */
#define ECPEAK0_ENERGY  ECPEAK0_ENERGY_BITS
#define ECPEAK0_STRIP1 (ECPEAK0_ENERGY_BITS+ECPEAK0_STRIP1_BITS)
#define ECPEAK0_STRIPN (ECPEAK0_ENERGY_BITS+ECPEAK0_STRIP1_BITS+ECPEAK0_STRIPN_BITS)
#define ECPEAK0_ENESUM (ECPEAK0_ENERGY_BITS+ECPEAK0_STRIP1_BITS+ECPEAK0_STRIPN_BITS+ECPEAK0_ENESUM_BITS)

#define ECPEAK_ENERGY_BITS 16

typedef struct ecpeak0 *ECPeak0Ptr;
typedef struct ecpeak0
{
  ap_uint<ECPEAK0_ENERGY_BITS> energy;          /* peak energy */
  ap_uint<7>                   strip1;          /* first strip in peak */
  ap_uint<7>                   stripn;          /* the number of strips in peak */
  ap_uint<24>                  energysum4coord; /* pass it to next function to do coordinate calculation */

} ECPeak0;


typedef struct ecpeak0_s
{
  ap_uint<ECPEAK0_ENERGY_BITS> energy;
  ap_uint<7>                   strip1;
  ap_uint<7>                   stripn;
  ap_uint<24>                  energysum4coord;

} ECPeak0_s;




typedef struct ecpeak
{
  ap_uint<ECPEAK_ENERGY_BITS> energy;         /* peak energy */
  ap_uint<NBIT_COORD>         coord; /* peak coordinate in (strip#<<3) units */
#ifndef __SYNTHESIS__
  ap_uint<7>  strip1;
  ap_uint<7>  stripn;
#endif
} ECPeak;



typedef struct ecpeak_s
{
  ap_uint<ECPEAK_ENERGY_BITS> energy;
  ap_uint<NBIT_COORD>         coord;
#ifndef __SYNTHESIS__
  ap_uint<7>  strip1;
  ap_uint<7>  stripn;
#endif
} ECPeak_s;






#define ECHIT_ENERGY_BITS 16
#define ECHIT_COORDU_BITS NBIT_COORD
#define ECHIT_COORDV_BITS NBIT_COORD
#define ECHIT_COORDW_BITS NBIT_COORD
#ifndef __SYNTHESIS__
#define ECHIT_IND_BITS 6
#define ECHIT_EU_BITS 16
#define ECHIT_EV_BITS 16
#define ECHIT_EW_BITS 16
#endif

#ifndef __SYNTHESIS__
#define ECHIT_BITS (ECHIT_ENERGY_BITS+ECHIT_COORDU_BITS+ECHIT_COORDV_BITS+ECHIT_COORDW_BITS+ECHIT_IND_BITS+ECHIT_EU_BITS+ECHIT_EV_BITS+ECHIT_EW_BITS)
#else
#define ECHIT_BITS (ECHIT_ENERGY_BITS+ECHIT_COORDU_BITS+ECHIT_COORDV_BITS+ECHIT_COORDW_BITS)
#endif

/* for packing in following order: [high bit -- [EW,EV,EU,IND,] COORDW,COORDV,COORDU,ENERGY -- low bit] */
#define ECHIT_ENERGY  ECHIT_ENERGY_BITS
#define ECHIT_COORDU (ECHIT_ENERGY_BITS+ECHIT_COORDU_BITS)
#define ECHIT_COORDV (ECHIT_ENERGY_BITS+ECHIT_COORDU_BITS+ECHIT_COORDV_BITS)
#define ECHIT_COORDW (ECHIT_ENERGY_BITS+ECHIT_COORDU_BITS+ECHIT_COORDV_BITS+ECHIT_COORDW_BITS)
#ifndef __SYNTHESIS__
#define ECHIT_IND    (ECHIT_ENERGY_BITS+ECHIT_COORDU_BITS+ECHIT_COORDV_BITS+ECHIT_COORDW_BITS+ECHIT_IND_BITS)
#define ECHIT_EU     (ECHIT_ENERGY_BITS+ECHIT_COORDU_BITS+ECHIT_COORDV_BITS+ECHIT_COORDW_BITS+ECHIT_IND_BITS+ECHIT_EU_BITS)
#define ECHIT_EV     (ECHIT_ENERGY_BITS+ECHIT_COORDU_BITS+ECHIT_COORDV_BITS+ECHIT_COORDW_BITS+ECHIT_IND_BITS+ECHIT_EU_BITS+ECHIT_EV_BITS)
#define ECHIT_EW     (ECHIT_ENERGY_BITS+ECHIT_COORDU_BITS+ECHIT_COORDV_BITS+ECHIT_COORDW_BITS+ECHIT_IND_BITS+ECHIT_EU_BITS+ECHIT_EV_BITS+ECHIT_EW_BITS)
#endif

typedef struct echit *ECHitPtr;
typedef struct echit
{
  ap_uint<ECHIT_ENERGY_BITS> energy;   /* hit energy [16] */
  ap_uint<NBIT_COORD>        coord[3]; /* u/v/w coordinate */
#ifndef __SYNTHESIS__
  ap_uint<6>  ind;               /* original hit index before sorting - can be used to link with peaks */
  ap_uint<16> enpeak[3];         /* u/v/w energy [48] - for drawing purposes */
#endif

} ECHit;

typedef struct echit_s
{
  ap_uint<ECHIT_ENERGY_BITS> energy;   /* hit energy [16] */
  ap_uint<NBIT_COORD>        coord[3]; /* u/v/w coordinate */
#ifndef __SYNTHESIS__
  ap_uint<6>  ind;               /* original hit index before sorting - can be used to link with peaks */
  ap_uint<16> enpeak[3];         /* u/v/w energy [48] - for drawing purposes */
#endif

} ECHit_s;



typedef struct ecstream16_s
{
  ap_uint<16> word16[3];

} ECStream16_s;

typedef struct ecstream10_s
{
  ap_uint<10> word10[3];

} ECStream10_s;

typedef struct ecstream6_s
{
  ap_uint<6> word6[3];

} ECStream6_s;


/* hits sorting */
typedef ap_uint<ECHIT_BITS> sortz_t;




/* INTERMEDIATE DATA STRUCTURES */

/* used by echit.cc and ecfrac*.cc */
typedef ap_uint<6> ECPeakCount;

typedef struct ecpeakcount_s
{
  ECPeakCount peakcount[3][NPEAK];
} ECPeakCount_s;



/* used by ecfrac*.cc functions */
typedef struct echitsume *ECHitSumEPtr;
typedef struct echitsume
{
  ap_uint<24>    sumE;         /* sum of energies for UNIQUE peaks only normalized for the number of peaks (E = E / Nvalid) */
  ap_uint<24>    peak_sumE[3]; 

} ECHitSumE;
typedef ECHitSumE hitsume_t;



typedef ap_uint<NSTRIP> ECfml_t;




/* event builder(s) */

#define MAX_BIN_SCAN_DEPTH  32

#define NBIT_COORD_EB 10 /*in event builder coordinates always 10 bits*/

typedef struct
{
  ap_uint<ECPEAK_ENERGY_BITS> energy;
  ap_uint<NBIT_COORD_EB>      coord;
  ap_uint<2>                  view;
#ifndef __SYNTHESIS__
  ap_uint<7>  strip1;
  ap_uint<7>  stripn;
#endif
} peak_ram_t;

typedef struct
{
  ap_uint<ECHIT_ENERGY_BITS> energy;
  ap_uint<NBIT_COORD_EB>     coordU;
  ap_uint<NBIT_COORD_EB>     coordV;
  ap_uint<NBIT_COORD_EB>     coordW;
#ifndef __SYNTHESIS__
  ap_uint<6>  ind;
  ap_uint<16> enpeakU;
  ap_uint<16> enpeakV;
  ap_uint<16> enpeakW;
#endif
} hit_ram_t;






/* FUNCTIONS */

uint16_t pcal_coord_to_strip(ap_uint<2> view, uint16_t jj);

int  ectrig(unsigned int *bufptr, int sec, int npeak[NVIEW], TrigECPeak peak[NVIEW][NPEAKMAX], int *nhits_trig, TrigECHit hits_trig[NHIT], int sim);
void ec_get_str(ECStrip strip[3][NSTRIP]);
void ec_get_str_all(int sec, ECStrip strip[MAXTIMES][3][NSTRIP]);

void ecstrip_in(hls::stream<ECStrip_s> s_strip[NF1], ECStrip strip[NSTRIP]);
void ecstrip_out(ECStrip strip[NSTRIP], hls::stream<ECStrip_s> s_strip[NF1]);
void ecpeak0strip_in(hls::stream<ECPeak0_s> s_peak0[NF2], ECPeak0 peak0[NSTRIP]);
void ecpeak0strip_out(ECPeak0 peak0[NSTRIP], hls::stream<ECPeak0_s> s_peak0[NF2]);
void ecpeak0max_in(hls::stream<ECPeak0_s> s_peak0[NF3], ECPeak0 peak0[NPEAKMAX]);
void ecpeak0max_out(ECPeak0 peak0[NPEAKMAX], hls::stream<ECPeak0_s> s_peak0[NF3]);
void ecpeak0_in(hls::stream<ECPeak0_s> &s_peak0, ECPeak0 peak0[NPEAK]);
void ecpeak0_out(ECPeak0 peak0[NPEAK], hls::stream<ECPeak0_s> &s_peak0);
void ecpeak_in(hls::stream<ECPeak_s> &s_peak, ECPeak peak[NPEAK]);
void ecpeak_out(ECPeak peak[NPEAK], hls::stream<ECPeak_s> &s_peak);
void echit_in(hls::stream<ECHit> &s_hits, ECHit hit[NHIT]);
void echit_out(ECHit hit[NHIT], hls::stream<ECHit> &s_hits);

void ecal(ap_uint<16> threshold[3], nframe_t nframes, ap_uint<4> dipfactor, ap_uint<12> dalitzmin, ap_uint<12> dalitzmax, ap_uint<4> nstripmax,
          hls::stream<fadc_4ch_t> (s_fadc_words)[NFADCS], hls::stream<ECHit> &s_hits, peak_ram_t buf_ram_u[NPEAK][256], peak_ram_t buf_ram_v[NPEAK][256],
          peak_ram_t buf_ram_w[NPEAK][256], hit_ram_t buf_ram[NHIT][256]);
int  ecpeakeventreader(hls::stream<eventdata_t> &event_stream, TrigECPeak peak[NPEAK], uint32_t *bufout);
int  echiteventreader(hls::stream<eventdata_t> &event_stream, TrigECHit hit[NHIT], uint32_t *bufout);

/*FPGA*/
void ecstrips(ap_uint<16> strip_threshold, hls::stream<fadc_4ch_t> s_fadc_words[NFADCS], hls::stream<ECStrip_s> s_strip_u[NF1], hls::stream<ECStrip_s> s_strip_v[NF1], hls::stream<ECStrip_s> s_strip_w[NF1]);
void ecstripsfanout(hls::stream<ECStrip_s> s_strip[NF1], hls::stream<ECStrip_s> s_strip1[NF1], hls::stream<ECStrip_s> s_strip2[NF1]);
void ecstripspersistence(nframe_t nframes, hls::stream<ECStrip_s> s_stripin[NF1], hls::stream<ECStrip_s> s_stripout[NF1]);
void ecstripspersistence0(nframe_t nframes, hls::stream<ECStrip_s> s_stripin[NF1], hls::stream<ECStrip_s> s_stripout[NF1]);
void ecstripspersistence1(nframe_t nframes, hls::stream<ECStrip_s> s_stripin[NF1], hls::stream<ECStrip_s> s_stripout[NF1]);
void ecstripspersistence2(nframe_t nframes, hls::stream<ECStrip_s> s_stripin[NF1], hls::stream<ECStrip_s> s_stripout[NF1]);

void ecpeak(ap_uint<16> strip_threshold, ap_uint<16> peak_threshold, hls::stream<ECStrip_s> s_strip[NF1], hls::stream<ECPeak0_s> s_peak0strip[NF2]);
void ecpeak1(ap_uint<16> strip_threshold, ap_uint<4> strip_dip_factor, ap_uint<4> nstrip_max, hls::stream<ECStrip_s> s_strip1[NF1], hls::stream<ECfml_t> &s_first, hls::stream<ECfml_t> &s_middle, hls::stream<ECfml_t> &s_last);
void ecpeak2(ap_uint<16> peak_threshold, hls::stream<ECStrip_s> s_strip2[NF1], hls::stream<ECfml_t> &s_first, hls::stream<ECfml_t> &s_middle, hls::stream<ECfml_t> &s_last, hls::stream<ECPeak0_s> s_peak0strip[NF2]);

void ecpeakzerosuppress(hls::stream<ECPeak0_s> s_peak0strip[NF2], hls::stream<ECPeak0_s> s_peak0max[NF3]);
void ecpeaksort(hls::stream<ECPeak0_s> s_peak0max[NF3], hls::stream<ECPeak0_s> &s_peak0);
void ecpeakcoord(ap_uint<2> view, hls::stream<ECPeak0_s> &s_peak0, hls::stream<ECPeak_s> &s_peak);
void ecpeakfanout(hls::stream<ECPeak_s> &s_peak, hls::stream<ECPeak_s> &s_peak1, hls::stream<ECPeak_s> &s_peak2, volatile ap_uint<1> &peak_scaler_inc);
void echit(ap_uint<NBIT_DALITZ> dalitzmin, ap_uint<NBIT_DALITZ> dalitzmax, hls::stream<ECPeak_s> &s_peak_u, hls::stream<ECPeak_s> &s_peak_v, hls::stream<ECPeak_s> &s_peak_w,
		   hls::stream<ECStream6_s> s_pcount[NH_FIFOS], hls::stream<ECStream16_s> s_energy[NH_FIFOS], hls::stream<ECStream10_s> s_coord[NH_FIFOS]);
void ecenergyfanout(hls::stream<ECStream16_s> s_energy[NH_FIFOS],hls::stream<ECStream16_s> s_energy1[NH_FIFOS],hls::stream<ECStream16_s> s_energy2[NH_FIFOS]);
void eccoordfanout(hls::stream<ECStream10_s> s_coord[NH_FIFOS],hls::stream<ECStream10_s> s_coord1[NH_FIFOS],hls::stream<ECStream10_s> s_coord2[NH_FIFOS]);
void ecfrac1(hls::stream<ECStream6_s> s_pcount[NH_FIFOS], hls::stream<ECStream16_s> s_energy1[NH_FIFOS], hls::stream<hitsume_t> s_hitout1[NH_FIFOS]);
void ecfrac2(hls::stream<hitsume_t> s_hitout1[NH_FIFOS], hls::stream<hitsume_t> s_hitout2[NH_FIFOS]);
void ecfrac3(hls::stream<hitsume_t> s_hitout2[NH_FIFOS], hls::stream<ECStream16_s> s_frac[NH_FIFOS]);
void eccorr(ap_uint<16> hit_threshold, hls::stream<ECStream16_s> s_energy2[NH_FIFOS], hls::stream<ECStream10_s> s_coord1[NH_FIFOS], hls::stream<ECStream16_s> s_frac[NH_FIFOS], hls::stream<ECStream16_s> s_enpeak[NH_FIFOS]);
void echitsort(hls::stream<ECStream10_s> s_coord2[NH_FIFOS], hls::stream<ECStream16_s> s_enpeak[NH_FIFOS], hls::stream<ECHit> &s_hits);
void echitsortin(hls::stream<ECStream10_s> s_coord2[NH_FIFOS], hls::stream<ECStream16_s> s_enpeak[NH_FIFOS], hls::stream<sortz_t> z1[NF4]);
void echitsortsort(hls::stream<sortz_t> z2[NF4], hls::stream<sortz_t> z1[NF4]);
void echitsort1(hls::stream<sortz_t> z2[NF4], hls::stream<sortz_t> z1[NF4]);
void echitsort11(hls::stream<sortz_t> z2[NF4], hls::stream<sortz_t> z1[NF4]);
void echitsort2(hls::stream<sortz_t> z2[NF4], hls::stream<sortz_t> z1[NF4]);
void echitsort21(hls::stream<sortz_t> z2[NF4], hls::stream<sortz_t> z1[NF4]);
void echitsort3(hls::stream<sortz_t> z2[NF4], hls::stream<sortz_t> z1[NF4]);
void echitsort4(hls::stream<sortz_t> z2[NF4], hls::stream<sortz_t> z1[NF4]);
void echitsort5(hls::stream<sortz_t> z2[NF4], hls::stream<sortz_t> z1[NF4]);
void echitsortout(hls::stream<sortz_t> z2[NF4], hls::stream<ECHit> &s_hits);
void echitfanout(hls::stream<ECHit> &s_hit, hls::stream<ECHit> &s_hit1, hls::stream<ECHit> &s_hit2, volatile ap_uint<1> &hit_scaler_inc);

void ecpeakeventfiller(ap_uint<2> view, hls::stream<ECPeak_s> &s_peakin, peak_ram_t buf_ram[NPEAK][256]);
void ecpeakeventwriter(ap_uint<2> view, ap_uint<1> inst, hls::stream<trig_t> &trig_stream, hls::stream<eventdata_t> &event_stream,
                       peak_ram_t buf_ram_read[NPEAK][256]);
void echiteventfiller(hls::stream<ECHit> &s_hitin, hit_ram_t buf_ram[NHIT][256]);
void echiteventwriter(ap_uint<1> inst, hls::stream<trig_t> &trig_stream, hls::stream<eventdata_t> &event_stream,
                 hit_ram_t buf_ram_read[NHIT][256]);
/*FPGA*/

void ecpeakeventfiller0(hls::stream<ECPeak_s> &s_peakin, peak_ram_t buf_ram[NPEAK][256]);
void ecpeakeventfiller1(hls::stream<ECPeak_s> &s_peakin, peak_ram_t buf_ram[NPEAK][256]);
void ecpeakeventfiller2(hls::stream<ECPeak_s> &s_peakin, peak_ram_t buf_ram[NPEAK][256]);
