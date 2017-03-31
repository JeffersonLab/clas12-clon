#ifndef _PRLIB_

#ifdef	__cplusplus
extern "C" {
#endif


#undef SL5

/* prlib.h - header file for prlib package */


/* definitions for SDA related functions - have to be consistent with SDA ! */
/* and with dclib.h - IT IS BAD !!! */
#define nsgmx    60/*600*//*those four increased for Konstantin's test*/
#define nsgcmx   30/*300*/
#define nclmx    100/*1000*/
#define ntrmx    50 /*1000*/
#define nlnkvect 16
#define nlnkec   12
#define NVEC 11

/* */

#define NLAY     6        /* the number of layers in the superlayer */
#define NTRACK   ntrmx    /* the maximum number of tracks in ... */
#define NFOUND   200      /* the maximum number of segments in ... */
#define NS2      142
#define MAX_NPAT 200
#define NLISTAX  256*64*256  /* 22 bits */
#define NLISTST  256*128*128  /* 22 bits */
#define NO_HIT   0

/* accuracy */

#define VECT1 10.         /* x -  1 mm */
#define VECT2 10.         /* y -  1 mm */
#define VECT3 1.          /* z -  1 cm */
#define VECT4 100.        /* dx - 1 % */
#define VECT5 100.        /* dy - 1 % */
#define VECT6 100.        /* dz - 1 % */
#define VECT7 1000.       /* p -  1 MeV/c */

#define P0    0.02        /* GeV/c */
#define DP    0.025       /* 2.5 % (acctual PR resolution about 8-11 %) */




/* road structures */


/* for compatibility */
#define MAXNPAT 10
typedef struct linksegm
{
  char  npat[6];
  char  ipat[6][MAXNPAT];
} LINKsegm;




typedef struct
{
  unsigned path : 24;
  unsigned p    : 8; /* for reconstruction only */

  unsigned ntof : 6;
  unsigned nu   : 7;
  unsigned nv   : 7;
  unsigned nw   : 7;
  signed charge : 2;
  unsigned res : 3;

  short x;
  short y;

  char  z;
  char  dx;
  char  dy;
  char  dz;

#ifdef SIM
  unsigned short psim;
  unsigned short np;

  char  npat[6];
  char  ipat[6][MAXNPAT];
#endif

} DCroad;




typedef struct
{
  int           nroads;
  unsigned char min[4];
  unsigned char max[4];
  DCroad        *road;
} DCstereo;
typedef DCstereo DCaxial;






/* track structures */

#define MINHITSEGM 3
#define NHL 20

typedef struct eccluster
{
  float u;
  float v;
  float w;
  float e;
} ECCLUSTER;

typedef struct prlayer
{
  int   nhit;
  short iw[NHL];
  short tdc[NHL];
} PRLAYER;

typedef struct prcluster
{
  int       iwmin;
  int       iwmax;
  PRLAYER   layer[6];

} PRCLUSTER;

typedef struct prtrack
{
  int       sector;
  PRCLUSTER cluster[6];
  int       ntrk;
  float     vect[6];
  float     p;
  int       charge;
  int       ntof;
  int       nu;
  int       nv;
  int       nw;
  ECCLUSTER ec[3];
} PRTRACK;



/* functions */

void prinit(char *filename, int lanal1, int lanal2, int lanal5);
int  prlib(int *iw, PRTRACK *track);
int  eclib(unsigned int *iw, const float threshold[3], int *ntrk, PRTRACK *trk);
int  prbos(int *iw, int *ntrk, PRTRACK *trk);

void l3config(char *);
void l3init(int runno);
int  l3lib(int *jw, int *ifail);

void l3_reset();
void l3_init(int runno);
int  l3_event(int *jw, int *keepit);

#ifdef	__cplusplus
}
#endif

#define _PRLIB_
#endif







