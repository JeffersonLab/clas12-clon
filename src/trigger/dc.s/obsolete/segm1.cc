
/* segm1.cc - if-else-based alghorithm */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include <iostream>
using namespace std;


#include "dclib.h"
#include "sgutil.h"

#define SHORT_BIT 16


#define DEBUG



/*
#include "cinclude/dcsegfinder.c"
*/
void
dcsegfinder(int thres, int iw_in, ap_uint<1> in[6][NWIRESTOT], ap_uint<1> output[NOFFSETS][NWIRES])
{
#pragma HLS INLINE

  int iw = iw_in + NBLEED;

  if( (  (in[0][-1+iw] | in[0][0+iw]) +
 (in[1][-2+iw] | in[1][-1+iw]) +
 (in[2][-3+iw] | in[2][-2+iw]) +
 (in[3][-4+iw] | in[3][-3+iw]) +
 (in[4][-5+iw] | in[4][-4+iw]) +
 (in[5][-6+iw] | in[5][-5+iw]) ) >= thres ) output[ 1][iw_in] = 1;
  if( (  (in[0][-1+iw] | in[0][0+iw]) +
 (in[1][-2+iw] | in[1][-1+iw]) +
 (in[2][-3+iw] | in[2][-2+iw]) +
 (in[3][-4+iw] | in[3][-3+iw]) +
 (in[4][-5+iw] | in[4][-4+iw]) +
 (in[5][-6+iw] | in[5][-5+iw]) ) >= thres ) output[ 2][iw_in] = 1;
  if( (  (in[0][-1+iw] | in[0][0+iw]) +
 (in[1][-1+iw] | in[1][0+iw]) +
 (in[2][-3+iw] | in[2][-2+iw]) +
 (in[3][-3+iw] | in[3][-2+iw]) +
 (in[4][-5+iw] | in[4][-4+iw]) +
 (in[5][-5+iw] | in[5][-4+iw]) ) >= thres ) output[ 2][iw_in] = 1;
  if( (  (in[0][0+iw] | in[0][1+iw]) +
 (in[1][-1+iw] | in[1][0+iw]) +
 (in[2][-2+iw] | in[2][-1+iw]) +
 (in[3][-3+iw] | in[3][-2+iw]) +
 (in[4][-4+iw] | in[4][-3+iw]) +
 (in[5][-5+iw] | in[5][-4+iw]) ) >= thres ) output[ 2][iw_in] = 1;
  if( (  (in[0][-1+iw] | in[0][0+iw]) +
 (in[1][-1+iw] | in[1][0+iw]) +
 (in[2][-3+iw] | in[2][-2+iw]) +
 (in[3][-3+iw] | in[3][-2+iw]) +
 (in[4][-5+iw] | in[4][-4+iw]) +
 (in[5][-5+iw] | in[5][-4+iw]) ) >= thres ) output[ 3][iw_in] = 1;
  if( (  (in[0][0+iw] | in[0][1+iw]) +
 (in[1][-1+iw] | in[1][0+iw]) +
 (in[2][-2+iw] | in[2][-1+iw]) +
 (in[3][-3+iw] | in[3][-2+iw]) +
 (in[4][-4+iw] | in[4][-3+iw]) +
 (in[5][-5+iw] | in[5][-4+iw]) ) >= thres ) output[ 3][iw_in] = 1;
  if( (  (in[0][0+iw] | in[0][1+iw]) +
 (in[1][0+iw] | in[1][1+iw]) +
 (in[2][-2+iw] | in[2][-1+iw]) +
 (in[3][-2+iw] | in[3][-1+iw]) +
 (in[4][-4+iw] | in[4][-3+iw]) +
 (in[5][-4+iw] | in[5][-3+iw]) ) >= thres ) output[ 3][iw_in] = 1;
  if( (  (in[0][-1+iw] | in[0][0+iw]) +
 (in[1][-1+iw] | in[1][0+iw]) +
 (in[2][-2+iw]) +
 (in[3][-3+iw] | in[3][-2+iw]) +
 (in[4][-4+iw] | in[4][-3+iw]) +
 (in[5][-4+iw]) ) >= thres ) output[ 3][iw_in] = 1;
  if( (  (in[0][0+iw]) +
 (in[1][-1+iw] | in[1][0+iw]) +
 (in[2][-2+iw] | in[2][-1+iw]) +
 (in[3][-2+iw]) +
 (in[4][-4+iw] | in[4][-3+iw]) +
 (in[5][-4+iw] | in[5][-3+iw]) ) >= thres ) output[ 3][iw_in] = 1;
  if( (  (in[0][-1+iw] | in[0][0+iw]) +
 (in[1][-1+iw] | in[1][0+iw]) +
 (in[2][-2+iw]) +
 (in[3][-2+iw]) +
 (in[4][-4+iw] | in[4][-3+iw]) +
 (in[5][-4+iw] | in[5][-3+iw]) ) >= thres ) output[ 3][iw_in] = 1;
  if( (  (in[0][-1+iw] | in[0][0+iw]) +
 (in[1][-1+iw]) +
 (in[2][-2+iw]) +
 (in[3][-3+iw] | in[3][-2+iw]) +
 (in[4][-4+iw] | in[4][-3+iw]) +
 (in[5][-4+iw] | in[5][-3+iw]) ) >= thres ) output[ 3][iw_in] = 1;
  if( (  (in[0][-1+iw] | in[0][0+iw]) +
 (in[1][-1+iw] | in[1][0+iw]) +
 (in[2][-2+iw] | in[2][-1+iw]) +
 (in[3][-2+iw]) +
 (in[4][-3+iw]) +
 (in[5][-4+iw] | in[5][-3+iw]) ) >= thres ) output[ 3][iw_in] = 1;
  if( (  (in[0][0+iw] | in[0][1+iw]) +
 (in[1][0+iw] | in[1][1+iw]) +
 (in[2][-2+iw] | in[2][-1+iw]) +
 (in[3][-2+iw] | in[3][-1+iw]) +
 (in[4][-4+iw] | in[4][-3+iw]) +
 (in[5][-4+iw] | in[5][-3+iw]) ) >= thres ) output[ 4][iw_in] = 1;
  if( (  (in[0][0+iw]) +
 (in[1][-1+iw] | in[1][0+iw]) +
 (in[2][-2+iw] | in[2][-1+iw]) +
 (in[3][-2+iw]) +
 (in[4][-4+iw] | in[4][-3+iw]) +
 (in[5][-4+iw] | in[5][-3+iw]) ) >= thres ) output[ 4][iw_in] = 1;
  if( (  (in[0][0+iw] | in[0][1+iw]) +
 (in[1][0+iw] | in[1][1+iw]) +
 (in[2][-1+iw]) +
 (in[3][-2+iw] | in[3][-1+iw]) +
 (in[4][-3+iw] | in[4][-2+iw]) +
 (in[5][-3+iw]) ) >= thres ) output[ 4][iw_in] = 1;
  if( (  (in[0][-1+iw] | in[0][0+iw]) +
 (in[1][-1+iw] | in[1][0+iw]) +
 (in[2][-2+iw]) +
 (in[3][-2+iw]) +
 (in[4][-4+iw] | in[4][-3+iw]) +
 (in[5][-4+iw] | in[5][-3+iw]) ) >= thres ) output[ 4][iw_in] = 1;
  if( (  (in[0][0+iw] | in[0][1+iw]) +
 (in[1][0+iw] | in[1][1+iw]) +
 (in[2][-1+iw]) +
 (in[3][-1+iw]) +
 (in[4][-3+iw] | in[4][-2+iw]) +
 (in[5][-3+iw] | in[5][-2+iw]) ) >= thres ) output[ 4][iw_in] = 1;
  if( (  (in[0][-1+iw] | in[0][0+iw]) +
 (in[1][-1+iw]) +
 (in[2][-2+iw]) +
 (in[3][-3+iw] | in[3][-2+iw]) +
 (in[4][-4+iw] | in[4][-3+iw]) +
 (in[5][-4+iw] | in[5][-3+iw]) ) >= thres ) output[ 4][iw_in] = 1;
  if( (  (in[0][-1+iw] | in[0][0+iw]) +
 (in[1][-1+iw] | in[1][0+iw]) +
 (in[2][-2+iw] | in[2][-1+iw]) +
 (in[3][-2+iw]) +
 (in[4][-3+iw]) +
 (in[5][-4+iw] | in[5][-3+iw]) ) >= thres ) output[ 4][iw_in] = 1;
  if( (  (in[0][0+iw] | in[0][1+iw]) +
 (in[1][0+iw]) +
 (in[2][-1+iw]) +
 (in[3][-2+iw] | in[3][-1+iw]) +
 (in[4][-3+iw] | in[4][-2+iw]) +
 (in[5][-3+iw] | in[5][-2+iw]) ) >= thres ) output[ 4][iw_in] = 1;
  if( (  (in[0][0+iw] | in[0][1+iw]) +
 (in[1][0+iw] | in[1][1+iw]) +
 (in[2][-1+iw] | in[2][0+iw]) +
 (in[3][-1+iw]) +
 (in[4][-2+iw]) +
 (in[5][-3+iw] | in[5][-2+iw]) ) >= thres ) output[ 4][iw_in] = 1;
  if( (  (in[0][-1+iw] | in[0][0+iw]) +
 (in[1][-1+iw] | in[1][0+iw]) +
 (in[2][-2+iw] | in[2][-1+iw]) +
 (in[3][-2+iw] | in[3][-1+iw]) +
 (in[4][-3+iw] | in[4][-2+iw]) +
 (in[5][-3+iw] | in[5][-2+iw]) ) >= thres ) output[ 4][iw_in] = 1;
  if( (  (in[0][0+iw] | in[0][1+iw]) +
 (in[1][0+iw] | in[1][1+iw]) +
 (in[2][-1+iw]) +
 (in[3][-1+iw]) +
 (in[4][-3+iw] | in[4][-2+iw]) +
 (in[5][-3+iw] | in[5][-2+iw]) ) >= thres ) output[ 5][iw_in] = 1;
  if( (  (in[0][0+iw] | in[0][1+iw]) +
 (in[1][0+iw]) +
 (in[2][-1+iw]) +
 (in[3][-2+iw] | in[3][-1+iw]) +
 (in[4][-3+iw] | in[4][-2+iw]) +
 (in[5][-3+iw] | in[5][-2+iw]) ) >= thres ) output[ 5][iw_in] = 1;
  if( (  (in[0][0+iw] | in[0][1+iw]) +
 (in[1][0+iw] | in[1][1+iw]) +
 (in[2][-1+iw] | in[2][0+iw]) +
 (in[3][-1+iw]) +
 (in[4][-2+iw]) +
 (in[5][-3+iw] | in[5][-2+iw]) ) >= thres ) output[ 5][iw_in] = 1;
  if( (  (in[0][-1+iw] | in[0][0+iw]) +
 (in[1][-1+iw] | in[1][0+iw]) +
 (in[2][-2+iw] | in[2][-1+iw]) +
 (in[3][-2+iw] | in[3][-1+iw]) +
 (in[4][-3+iw] | in[4][-2+iw]) +
 (in[5][-3+iw] | in[5][-2+iw]) ) >= thres ) output[ 5][iw_in] = 1;
  if( (  (in[0][0+iw] | in[0][1+iw]) +
 (in[1][0+iw] | in[1][1+iw]) +
 (in[2][-1+iw] | in[2][0+iw]) +
 (in[3][-1+iw] | in[3][0+iw]) +
 (in[4][-2+iw] | in[4][-1+iw]) +
 (in[5][-2+iw] | in[5][-1+iw]) ) >= thres ) output[ 5][iw_in] = 1;
  if( (  (in[0][-1+iw] | in[0][0+iw]) +
 (in[1][0+iw]) +
 (in[2][-1+iw]) +
 (in[3][-1+iw]) +
 (in[4][-2+iw]) +
 (in[5][-2+iw] | in[5][-1+iw]) ) >= thres ) output[ 5][iw_in] = 1;
  if( (  (in[0][0+iw] | in[0][1+iw]) +
 (in[1][0+iw] | in[1][1+iw]) +
 (in[2][-1+iw] | in[2][0+iw]) +
 (in[3][-1+iw] | in[3][0+iw]) +
 (in[4][-2+iw] | in[4][-1+iw]) +
 (in[5][-2+iw] | in[5][-1+iw]) ) >= thres ) output[ 6][iw_in] = 1;
  if( (  (in[0][-1+iw] | in[0][0+iw]) +
 (in[1][0+iw]) +
 (in[2][-1+iw]) +
 (in[3][-1+iw]) +
 (in[4][-2+iw]) +
 (in[5][-2+iw] | in[5][-1+iw]) ) >= thres ) output[ 6][iw_in] = 1;
  if( (  (in[0][0+iw] | in[0][1+iw]) +
 (in[1][1+iw]) +
 (in[2][0+iw]) +
 (in[3][0+iw]) +
 (in[4][-1+iw]) +
 (in[5][-1+iw] | in[5][0+iw]) ) >= thres ) output[ 6][iw_in] = 1;
  if( (  (in[0][-1+iw] | in[0][0+iw]) +
 (in[1][0+iw]) +
 (in[2][-1+iw]) +
 (in[3][-1+iw]) +
 (in[4][-2+iw] | in[4][-1+iw]) +
 (in[5][-1+iw]) ) >= thres ) output[ 6][iw_in] = 1;
  if( (  (in[0][0+iw]) +
 (in[1][0+iw] | in[1][1+iw]) +
 (in[2][0+iw]) +
 (in[3][0+iw]) +
 (in[4][-1+iw]) +
 (in[5][-1+iw] | in[5][0+iw]) ) >= thres ) output[ 6][iw_in] = 1;
  if( (  (in[0][-1+iw] | in[0][0+iw]) +
 (in[1][0+iw]) +
 (in[2][-1+iw]) +
 (in[3][-1+iw] | in[3][0+iw]) +
 (in[4][-1+iw]) +
 (in[5][-1+iw]) ) >= thres ) output[ 6][iw_in] = 1;
  if( (  (in[0][0+iw]) +
 (in[1][0+iw]) +
 (in[2][-1+iw] | in[2][0+iw]) +
 (in[3][0+iw]) +
 (in[4][-1+iw]) +
 (in[5][-1+iw] | in[5][0+iw]) ) >= thres ) output[ 6][iw_in] = 1;
  if( (  (in[0][-1+iw] | in[0][0+iw]) +
 (in[1][0+iw]) +
 (in[2][-1+iw]) +
 (in[3][0+iw]) +
 (in[4][-1+iw]) +
 (in[5][-1+iw] | in[5][0+iw]) ) >= thres ) output[ 6][iw_in] = 1;
  if( (  (in[0][0+iw] | in[0][1+iw]) +
 (in[1][1+iw]) +
 (in[2][0+iw]) +
 (in[3][0+iw]) +
 (in[4][-1+iw]) +
 (in[5][-1+iw] | in[5][0+iw]) ) >= thres ) output[ 7][iw_in] = 1;
  if( (  (in[0][0+iw]) +
 (in[1][0+iw] | in[1][1+iw]) +
 (in[2][0+iw]) +
 (in[3][0+iw]) +
 (in[4][-1+iw]) +
 (in[5][-1+iw] | in[5][0+iw]) ) >= thres ) output[ 7][iw_in] = 1;
  if( (  (in[0][0+iw] | in[0][1+iw]) +
 (in[1][1+iw]) +
 (in[2][0+iw]) +
 (in[3][0+iw]) +
 (in[4][-1+iw] | in[4][0+iw]) +
 (in[5][0+iw]) ) >= thres ) output[ 7][iw_in] = 1;
  if( (  (in[0][0+iw]) +
 (in[1][0+iw]) +
 (in[2][-1+iw] | in[2][0+iw]) +
 (in[3][0+iw]) +
 (in[4][-1+iw]) +
 (in[5][-1+iw] | in[5][0+iw]) ) >= thres ) output[ 7][iw_in] = 1;
  if( (  (in[0][0+iw]) +
 (in[1][0+iw] | in[1][1+iw]) +
 (in[2][0+iw]) +
 (in[3][0+iw]) +
 (in[4][-1+iw] | in[4][0+iw]) +
 (in[5][0+iw]) ) >= thres ) output[ 7][iw_in] = 1;
  if( (  (in[0][0+iw] | in[0][1+iw]) +
 (in[1][1+iw]) +
 (in[2][0+iw]) +
 (in[3][0+iw] | in[3][1+iw]) +
 (in[4][0+iw]) +
 (in[5][0+iw]) ) >= thres ) output[ 7][iw_in] = 1;
  if( (  (in[0][-1+iw] | in[0][0+iw]) +
 (in[1][0+iw]) +
 (in[2][-1+iw]) +
 (in[3][0+iw]) +
 (in[4][-1+iw]) +
 (in[5][-1+iw] | in[5][0+iw]) ) >= thres ) output[ 7][iw_in] = 1;
  if( (  (in[0][0+iw] | in[0][1+iw]) +
 (in[1][1+iw]) +
 (in[2][0+iw]) +
 (in[3][1+iw]) +
 (in[4][0+iw]) +
 (in[5][0+iw] | in[5][1+iw]) ) >= thres ) output[ 7][iw_in] = 1;
  if( (  (in[0][-1+iw] | in[0][0+iw]) +
 (in[1][0+iw]) +
 (in[2][0+iw]) +
 (in[3][0+iw]) +
 (in[4][0+iw]) +
 (in[5][0+iw] | in[5][1+iw]) ) >= thres ) output[ 7][iw_in] = 1;
  if( (  (in[0][0+iw] | in[0][1+iw]) +
 (in[1][1+iw]) +
 (in[2][0+iw]) +
 (in[3][1+iw]) +
 (in[4][0+iw]) +
 (in[5][0+iw] | in[5][1+iw]) ) >= thres ) output[ 8][iw_in] = 1;
  if( (  (in[0][-1+iw] | in[0][0+iw]) +
 (in[1][0+iw]) +
 (in[2][0+iw]) +
 (in[3][0+iw]) +
 (in[4][0+iw]) +
 (in[5][0+iw] | in[5][1+iw]) ) >= thres ) output[ 8][iw_in] = 1;
  if( (  (in[0][0+iw] | in[0][1+iw]) +
 (in[1][1+iw]) +
 (in[2][1+iw]) +
 (in[3][1+iw]) +
 (in[4][1+iw]) +
 (in[5][1+iw] | in[5][2+iw]) ) >= thres ) output[ 8][iw_in] = 1;
  if( (  (in[0][-1+iw] | in[0][0+iw]) +
 (in[1][0+iw]) +
 (in[2][0+iw]) +
 (in[3][0+iw] | in[3][1+iw]) +
 (in[4][0+iw]) +
 (in[5][1+iw]) ) >= thres ) output[ 8][iw_in] = 1;
  if( (  (in[0][0+iw]) +
 (in[1][0+iw] | in[1][1+iw]) +
 (in[2][0+iw]) +
 (in[3][1+iw]) +
 (in[4][0+iw] | in[4][1+iw]) +
 (in[5][1+iw]) ) >= thres ) output[ 8][iw_in] = 1;
  if( (  (in[0][0+iw]) +
 (in[1][1+iw]) +
 (in[2][0+iw] | in[2][1+iw]) +
 (in[3][1+iw]) +
 (in[4][1+iw]) +
 (in[5][1+iw] | in[5][2+iw]) ) >= thres ) output[ 8][iw_in] = 1;
  if( (  (in[0][0+iw]) +
 (in[1][0+iw] | in[1][1+iw]) +
 (in[2][0+iw]) +
 (in[3][1+iw]) +
 (in[4][1+iw]) +
 (in[5][1+iw] | in[5][2+iw]) ) >= thres ) output[ 8][iw_in] = 1;
  if( (  (in[0][-1+iw] | in[0][0+iw]) +
 (in[1][0+iw]) +
 (in[2][0+iw]) +
 (in[3][1+iw]) +
 (in[4][0+iw] | in[4][1+iw]) +
 (in[5][1+iw]) ) >= thres ) output[ 8][iw_in] = 1;
  if( (  (in[0][-1+iw] | in[0][0+iw]) +
 (in[1][0+iw]) +
 (in[2][0+iw]) +
 (in[3][1+iw]) +
 (in[4][1+iw]) +
 (in[5][1+iw] | in[5][2+iw]) ) >= thres ) output[ 8][iw_in] = 1;
  if( (  (in[0][0+iw] | in[0][1+iw]) +
 (in[1][1+iw]) +
 (in[2][1+iw]) +
 (in[3][1+iw]) +
 (in[4][1+iw]) +
 (in[5][1+iw] | in[5][2+iw]) ) >= thres ) output[ 9][iw_in] = 1;
  if( (  (in[0][0+iw]) +
 (in[1][1+iw]) +
 (in[2][0+iw] | in[2][1+iw]) +
 (in[3][1+iw]) +
 (in[4][1+iw]) +
 (in[5][1+iw] | in[5][2+iw]) ) >= thres ) output[ 9][iw_in] = 1;
  if( (  (in[0][0+iw] | in[0][1+iw]) +
 (in[1][1+iw]) +
 (in[2][1+iw]) +
 (in[3][1+iw] | in[3][2+iw]) +
 (in[4][1+iw]) +
 (in[5][2+iw]) ) >= thres ) output[ 9][iw_in] = 1;
  if( (  (in[0][0+iw]) +
 (in[1][0+iw] | in[1][1+iw]) +
 (in[2][0+iw]) +
 (in[3][1+iw]) +
 (in[4][1+iw]) +
 (in[5][1+iw] | in[5][2+iw]) ) >= thres ) output[ 9][iw_in] = 1;
  if( (  (in[0][0+iw] | in[0][1+iw]) +
 (in[1][1+iw]) +
 (in[2][1+iw]) +
 (in[3][2+iw]) +
 (in[4][1+iw] | in[4][2+iw]) +
 (in[5][2+iw]) ) >= thres ) output[ 9][iw_in] = 1;
  if( (  (in[0][-1+iw] | in[0][0+iw]) +
 (in[1][0+iw]) +
 (in[2][0+iw]) +
 (in[3][1+iw]) +
 (in[4][1+iw]) +
 (in[5][1+iw] | in[5][2+iw]) ) >= thres ) output[ 9][iw_in] = 1;
  if( (  (in[0][0+iw] | in[0][1+iw]) +
 (in[1][1+iw]) +
 (in[2][1+iw]) +
 (in[3][2+iw]) +
 (in[4][2+iw]) +
 (in[5][2+iw] | in[5][3+iw]) ) >= thres ) output[ 9][iw_in] = 1;
  if( (  (in[0][-1+iw] | in[0][0+iw]) +
 (in[1][0+iw] | in[1][1+iw]) +
 (in[2][0+iw] | in[2][1+iw]) +
 (in[3][1+iw] | in[3][2+iw]) +
 (in[4][1+iw] | in[4][2+iw]) +
 (in[5][2+iw] | in[5][3+iw]) ) >= thres ) output[ 9][iw_in] = 1;
  if( (  (in[0][0+iw] | in[0][1+iw]) +
 (in[1][1+iw]) +
 (in[2][1+iw]) +
 (in[3][2+iw]) +
 (in[4][2+iw]) +
 (in[5][2+iw] | in[5][3+iw]) ) >= thres ) output[10][iw_in] = 1;
  if( (  (in[0][-1+iw] | in[0][0+iw]) +
 (in[1][0+iw] | in[1][1+iw]) +
 (in[2][0+iw] | in[2][1+iw]) +
 (in[3][1+iw] | in[3][2+iw]) +
 (in[4][1+iw] | in[4][2+iw]) +
 (in[5][2+iw] | in[5][3+iw]) ) >= thres ) output[10][iw_in] = 1;
  if( (  (in[0][0+iw] | in[0][1+iw]) +
 (in[1][1+iw] | in[1][2+iw]) +
 (in[2][1+iw] | in[2][2+iw]) +
 (in[3][2+iw] | in[3][3+iw]) +
 (in[4][2+iw] | in[4][3+iw]) +
 (in[5][3+iw] | in[5][4+iw]) ) >= thres ) output[10][iw_in] = 1;
  if( (  (in[0][-1+iw] | in[0][0+iw]) +
 (in[1][0+iw] | in[1][1+iw]) +
 (in[2][0+iw] | in[2][1+iw]) +
 (in[3][2+iw]) +
 (in[4][2+iw]) +
 (in[5][3+iw] | in[5][4+iw]) ) >= thres ) output[10][iw_in] = 1;
  if( (  (in[0][-1+iw] | in[0][0+iw]) +
 (in[1][0+iw] | in[1][1+iw]) +
 (in[2][1+iw]) +
 (in[3][2+iw]) +
 (in[4][2+iw] | in[4][3+iw]) +
 (in[5][3+iw] | in[5][4+iw]) ) >= thres ) output[10][iw_in] = 1;
  if( (  (in[0][-1+iw] | in[0][0+iw]) +
 (in[1][1+iw]) +
 (in[2][1+iw]) +
 (in[3][2+iw] | in[3][3+iw]) +
 (in[4][2+iw] | in[4][3+iw]) +
 (in[5][3+iw] | in[5][4+iw]) ) >= thres ) output[10][iw_in] = 1;
  if( (  (in[0][0+iw] | in[0][1+iw]) +
 (in[1][1+iw] | in[1][2+iw]) +
 (in[2][1+iw] | in[2][2+iw]) +
 (in[3][2+iw] | in[3][3+iw]) +
 (in[4][2+iw] | in[4][3+iw]) +
 (in[5][3+iw] | in[5][4+iw]) ) >= thres ) output[11][iw_in] = 1;
  if( (  (in[0][-1+iw] | in[0][0+iw]) +
 (in[1][0+iw] | in[1][1+iw]) +
 (in[2][0+iw] | in[2][1+iw]) +
 (in[3][2+iw]) +
 (in[4][2+iw]) +
 (in[5][3+iw] | in[5][4+iw]) ) >= thres ) output[11][iw_in] = 1;
  if( (  (in[0][-1+iw] | in[0][0+iw]) +
 (in[1][0+iw] | in[1][1+iw]) +
 (in[2][1+iw]) +
 (in[3][2+iw]) +
 (in[4][2+iw] | in[4][3+iw]) +
 (in[5][3+iw] | in[5][4+iw]) ) >= thres ) output[11][iw_in] = 1;
  if( (  (in[0][-1+iw] | in[0][0+iw]) +
 (in[1][1+iw]) +
 (in[2][1+iw]) +
 (in[3][2+iw] | in[3][3+iw]) +
 (in[4][2+iw] | in[4][3+iw]) +
 (in[5][3+iw] | in[5][4+iw]) ) >= thres ) output[11][iw_in] = 1;
  if( (  (in[0][0+iw] | in[0][1+iw]) +
 (in[1][1+iw] | in[1][2+iw]) +
 (in[2][1+iw] | in[2][2+iw]) +
 (in[3][3+iw]) +
 (in[4][3+iw]) +
 (in[5][4+iw] | in[5][5+iw]) ) >= thres ) output[11][iw_in] = 1;
  if( (  (in[0][0+iw] | in[0][1+iw]) +
 (in[1][1+iw] | in[1][2+iw]) +
 (in[2][2+iw]) +
 (in[3][3+iw]) +
 (in[4][3+iw] | in[4][4+iw]) +
 (in[5][4+iw] | in[5][5+iw]) ) >= thres ) output[11][iw_in] = 1;
  if( (  (in[0][0+iw] | in[0][1+iw]) +
 (in[1][2+iw]) +
 (in[2][2+iw]) +
 (in[3][3+iw] | in[3][4+iw]) +
 (in[4][3+iw] | in[4][4+iw]) +
 (in[5][4+iw] | in[5][5+iw]) ) >= thres ) output[11][iw_in] = 1;
  if( (  (in[0][-1+iw] | in[0][0+iw]) +
 (in[1][0+iw] | in[1][1+iw]) +
 (in[2][1+iw]) +
 (in[3][2+iw] | in[3][3+iw]) +
 (in[4][2+iw] | in[4][3+iw]) +
 (in[5][4+iw]) ) >= thres ) output[11][iw_in] = 1;
  if( (  (in[0][0+iw]) +
 (in[1][1+iw] | in[1][2+iw]) +
 (in[2][1+iw] | in[2][2+iw]) +
 (in[3][3+iw]) +
 (in[4][3+iw] | in[4][4+iw]) +
 (in[5][4+iw] | in[5][5+iw]) ) >= thres ) output[11][iw_in] = 1;
  if( (  (in[0][-1+iw] | in[0][0+iw]) +
 (in[1][0+iw] | in[1][1+iw]) +
 (in[2][1+iw] | in[2][2+iw]) +
 (in[3][2+iw] | in[3][3+iw]) +
 (in[4][3+iw] | in[4][4+iw]) +
 (in[5][4+iw] | in[5][5+iw]) ) >= thres ) output[11][iw_in] = 1;
  if( (  (in[0][0+iw] | in[0][1+iw]) +
 (in[1][1+iw] | in[1][2+iw]) +
 (in[2][1+iw] | in[2][2+iw]) +
 (in[3][3+iw]) +
 (in[4][3+iw]) +
 (in[5][4+iw] | in[5][5+iw]) ) >= thres ) output[12][iw_in] = 1;
  if( (  (in[0][0+iw] | in[0][1+iw]) +
 (in[1][1+iw] | in[1][2+iw]) +
 (in[2][2+iw]) +
 (in[3][3+iw]) +
 (in[4][3+iw] | in[4][4+iw]) +
 (in[5][4+iw] | in[5][5+iw]) ) >= thres ) output[12][iw_in] = 1;
  if( (  (in[0][0+iw] | in[0][1+iw]) +
 (in[1][2+iw]) +
 (in[2][2+iw]) +
 (in[3][3+iw] | in[3][4+iw]) +
 (in[4][3+iw] | in[4][4+iw]) +
 (in[5][4+iw] | in[5][5+iw]) ) >= thres ) output[12][iw_in] = 1;
  if( (  (in[0][0+iw]) +
 (in[1][1+iw] | in[1][2+iw]) +
 (in[2][1+iw] | in[2][2+iw]) +
 (in[3][3+iw]) +
 (in[4][3+iw] | in[4][4+iw]) +
 (in[5][4+iw] | in[5][5+iw]) ) >= thres ) output[12][iw_in] = 1;
  if( (  (in[0][0+iw] | in[0][1+iw]) +
 (in[1][1+iw] | in[1][2+iw]) +
 (in[2][2+iw]) +
 (in[3][3+iw] | in[3][4+iw]) +
 (in[4][3+iw] | in[4][4+iw]) +
 (in[5][5+iw]) ) >= thres ) output[12][iw_in] = 1;
  if( (  (in[0][-1+iw] | in[0][0+iw]) +
 (in[1][0+iw] | in[1][1+iw]) +
 (in[2][1+iw] | in[2][2+iw]) +
 (in[3][2+iw] | in[3][3+iw]) +
 (in[4][3+iw] | in[4][4+iw]) +
 (in[5][4+iw] | in[5][5+iw]) ) >= thres ) output[12][iw_in] = 1;
  if( (  (in[0][0+iw] | in[0][1+iw]) +
 (in[1][1+iw] | in[1][2+iw]) +
 (in[2][2+iw] | in[2][3+iw]) +
 (in[3][3+iw] | in[3][4+iw]) +
 (in[4][4+iw] | in[4][5+iw]) +
 (in[5][5+iw] | in[5][6+iw]) ) >= thres ) output[12][iw_in] = 1;
  if( (  (in[0][-1+iw] | in[0][0+iw]) +
 (in[1][1+iw] | in[1][2+iw]) +
 (in[2][1+iw] | in[2][2+iw]) +
 (in[3][3+iw] | in[3][4+iw]) +
 (in[4][3+iw] | in[4][4+iw]) +
 (in[5][5+iw] | in[5][6+iw]) ) >= thres ) output[12][iw_in] = 1;
  if( (  (in[0][0+iw] | in[0][1+iw]) +
 (in[1][1+iw] | in[1][2+iw]) +
 (in[2][2+iw] | in[2][3+iw]) +
 (in[3][3+iw] | in[3][4+iw]) +
 (in[4][4+iw] | in[4][5+iw]) +
 (in[5][5+iw] | in[5][6+iw]) ) >= thres ) output[13][iw_in] = 1;
  if( (  (in[0][-1+iw] | in[0][0+iw]) +
 (in[1][1+iw] | in[1][2+iw]) +
 (in[2][1+iw] | in[2][2+iw]) +
 (in[3][3+iw] | in[3][4+iw]) +
 (in[4][3+iw] | in[4][4+iw]) +
 (in[5][5+iw] | in[5][6+iw]) ) >= thres ) output[13][iw_in] = 1;
  if( (  (in[0][0+iw] | in[0][1+iw]) +
 (in[1][2+iw] | in[1][3+iw]) +
 (in[2][2+iw] | in[2][3+iw]) +
 (in[3][4+iw] | in[3][5+iw]) +
 (in[4][4+iw] | in[4][5+iw]) +
 (in[5][6+iw] | in[5][7+iw]) ) >= thres ) output[13][iw_in] = 1;
  if( (  (in[0][0+iw] | in[0][1+iw]) +
 (in[1][2+iw] | in[1][3+iw]) +
 (in[2][2+iw] | in[2][3+iw]) +
 (in[3][4+iw] | in[3][5+iw]) +
 (in[4][4+iw] | in[4][5+iw]) +
 (in[5][6+iw] | in[5][7+iw]) ) >= thres ) output[14][iw_in] = 1;

  return;
}


/*xc7vx550tffg1158-1*/


int
CheckBitWord1(Word112Ptr hw, int n)
{
#pragma HLS INLINE

  unsigned short *w = (unsigned short *)hw->words;
  int whatword, whatbit;
  unsigned short mask = 1;

  whatword = n / SHORT_BIT;
  whatbit = n % SHORT_BIT;
  mask <<= whatbit;

  return((w[whatword] & mask) == mask);
}



/* 2.33/59/60/0%/0%/~0%/~0% */

void
SegmentSearch1(Word112 axial[6], ap_uint<1> output[NOFFSETS][NWIRES])
{
#pragma HLS PIPELINE

  ap_uint<1> input[6][NWIRESTOT];

  int i, j, stat;

  stat = 0;
  for(i=0; i<6; i++)
  {
    for(j=0; j<NWIRESTOT; j++) input[i][j]=0;
  }
  for(i=0; i<NOFFSETS; i++)
  {
    for(j=0; j<NWIRES; j++) output[i][j]=0;
  }
  for(i=0; i<6; i++)
  {
    for(j=0; j<NWIRES; j++)
	{
      if(CheckBitWord1(&axial[i], j)) input[i][j+NBLEED] = 1;
	}
  }

  for(j=0; j<NWIRES; j++) dcsegfinder(4, j, input, output);


#ifdef DEBUG

  /* print output */
  printf("\nTRIGSIM INPUT\n");
  for(i=5; i>=0; i--)
  {
    printf("%3d> ",112);
    for(j=NWIRES-1; j>0; j--)
	{
      if(!((j+1)%16) && j<111) printf(" ");
      if(input[i][j+NBLEED]) printf("X");
      else                   printf("-");
	}
    printf("\n");
  }
  printf("\n");

  printf("TRIGSIM OUTPUT\n");
  for(i=0; i<NOFFSETS; i++)
  {
    printf("%3d> ",(15-i)-8);
    for(j=NWIRES-1; j>=0; j--)
	{
      if(!((j+1)%16) && j<111) printf(" ");
      if(output[i][j]) printf("X");
      else              printf("-");
	}
    printf("\n");
  }

#endif

  return;
}

