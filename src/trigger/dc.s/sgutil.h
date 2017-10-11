#ifndef _SGUTIL_

/* sgutil.h */

#include <ap_int.h>
#include <hls_stream.h>

#define MIN(a,b)    (a < b ? a : b)
#define MAX(a,b)    (a > b ? a : b)

#ifdef	__cplusplus
extern "C" {
#endif


/* NOFFSETS layers for -8<offset<8, 112 bits for wire # */
#define NWIRES 112
#define NBLEED 8 /* extra 'fake' wires on both sides of the layer to accomodate shofts etc */
#define NWIRESTOT (NWIRES+NBLEED) 
#define NLAY 6 /* the number of layers in superlayer */
#define MAXSHIFT 7 /* maximum deviation in segment between layer 0 and layer 6, CANNOT BE MORE THEN 8 (BLEED ON LEFT AND RIGHT IS 8) */ 
#define NOFFSETS (MAXSHIFT*2+1)

typedef ap_uint<1>   word1_t;
typedef ap_uint<16>  word16_t;
typedef ap_uint<96>  word96_t;
typedef ap_uint<112> word112_t;
typedef ap_uint<128> word128_t;
typedef ap_uint<224> word224_t;


/* old style long words */
#define NW112 7         /* # of shorts in 112 bits */
typedef struct word112 *Word112Ptr;
typedef struct word112
{
  unsigned short words[NW112]; /* words[0] is least significant */
  unsigned short lword;
  unsigned short hword;

} Word112;


typedef struct dc0data  *DC0DataPtr;
typedef struct dc0data
{
  unsigned short id; /* wire - layer are bytes stuffed into id */
  unsigned short tdc;
} DC0Data;

typedef enum {False, True} Boolean;
typedef unsigned int (*UIFUNCPTR) ();


/* functions */
void Word96Print(int num, word96_t w);
void Word112Print(int num, word112_t w);
void Word224Print(int num, word224_t w);
void word112_to_shorts7(word112_t in, Word112Ptr out);
void shorts7_to_word112(Word112Ptr in, word112_t *outout);

void SegmentSearch1(Word112 axial[6], ap_uint<1> output[NOFFSETS][NWIRES]);
void SegmentSearch2(word112_t axial[6], word112_t out[NOFFSETS]);
void SegmentSearch3(word112_t axial[6], word112_t out[NOFFSETS], ap_uint<3> hit_threshold);
ap_uint<1> RegionSearch1(word112_t in1[6], word112_t in2[6], word112_t out[NOFFSETS], ap_uint<3> hit_threshold);
void RoadSearch2(word112_t sl[6], ap_uint<112> *res, ap_uint<3> sl_threshold);

#ifdef	__cplusplus
}
#endif

#define _SGUTIL_
#endif





