#ifndef _SGUTILOLD_

/* sgutilold.h */

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct word128 *Word128Ptr;
typedef struct word128
{
  unsigned int words[4]; /* words[0] is least significant */
  unsigned int lword;
  unsigned int hword;
  
} Word128;

typedef struct dc0data  *DC0DataPtr;
typedef struct dc0data
{
  unsigned short id; /* wire - layer are bytes stuffed into id */
  unsigned short tdc;
} DC0Data;

typedef enum {False, True} Boolean;
typedef unsigned int (*UIFUNCPTR) ();


/* structures for the segment finding algorithms */

#define NGAP 14/*7*/ /* +/- in stereo shift */
#define NHL  20 /* # hits per layer */
#define NCR  20 /* # clusters per region */
#define NLAY  6 /* max number of layers in superlayer */

/* the number of segments */
#define NSHIFT_HALF 28
#define NSHIFT (NSHIFT_HALF*2+1)

typedef struct prla
{
  int   nhit;
  short iw[NHL];
  short tdc[NHL];
} PRLA;

typedef struct prsl
{
  int  iwmin;
  int  iwmax;
  int  ird;      /* wire # in layer 3 to be used in road finding
                    (can be missing in la[] !!!) */
  PRLA la[NLAY];
} PRSL;

typedef struct prrg
{
  UIFUNCPTR axmin;
  UIFUNCPTR stmin;
  UIFUNCPTR rgmin;
  /*for example:*/
  /*UIFUNCPTR fun = clust->axmin;*/
  /*printf("%d - %d\n",e2,fun(e0,e1,e2));*/
  int  nax;
  int  nst; /* if nst<0 then nax=# of pairs */
  PRSL ax[NCR];
  PRSL st[NCR];
} PRRG; /* segments for one region */

typedef struct prsec
{
  PRRG rg[3];
} PRSEC; /* segments for one sector */


#define GOODHIT \
  wire > 0 && wire < 193 && \
  layer > 0 && layer < 37 && layer != 5 && layer != 6

#define BREAKSHORT(sw, low, high) \
  high = (unsigned char)(((sw) >> 8) & 0x00FF); \
  low = (unsigned char)((sw) & 0x00FF)



/* functions */

void AxialPlusStereoInitOld();
void SegmentSearch128(Word128 axial[4], Word128 stereo[4], int *ncl, PRRG *clust);
void SuperLayerSum(Word128 data[6], Word128Ptr output, unsigned char sum[128]);
void SetBitWord128(Word128Ptr hw, int n);
void ClearWord128(Word128Ptr hw);
void PrintSum128(unsigned char sum[128]);
void PrintWord128(Word128Ptr hw);




#ifdef	__cplusplus
}
#endif

#define _SGUTILOLD_
#endif





