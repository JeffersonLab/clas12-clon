void
dcsegfinder(int thres, int iw_in, unsigned char in[6][NWIRESTOT], unsigned char output[NOFFSETS][NWIRES])
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
