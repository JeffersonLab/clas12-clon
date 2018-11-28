
/* sgutil.cc */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

using namespace std;
#include <strstream>

#include "dclib.h"
#include "sgutil.h"

#ifndef READ_FILE
#include "segmdict.h"
#endif


//#define DEBUG

/*xc7vx550tffg1158-1*/

/*********************/
/* lookup table part */

#define STRLEN 128

static void
AxialPlusStereoInit(int8_t shift[MAXSEGM][5], int8_t angle[MAXSEGM])
{
  int i, j;
  unsigned long k;
  unsigned char *p;
  char ch, str_tmp[STRLEN], keyword[STRLEN];
#ifdef READ_FILE
  FILE *fd;
  int nsegment6;
  int segment6[MAXSEGM][6];
  /* get segments from dictionary */
  fd = fopen("segmdict.txt","r");
  if(fd <= NULL)
  {
    printf("ERROR: cannot open segment dictionary file - exit\n");
    exit(0);
  }

  /* Parsing of segment dictionary file */
  while ((ch = getc(fd)) != EOF)
  {
    if ( ch == '#' || ch == ' ' || ch == '\t' )
    {
      while (getc(fd) != '\n') {}
    }
    else if( ch == '\n' ) {}
    else
    {
      ungetc(ch,fd);
      fgets(str_tmp, STRLEN, fd);
      sscanf (str_tmp, "%s", keyword);

      if(strcmp(keyword,"SEGM_DICT") == 0)
      {
        sscanf (str_tmp, "%*s %d", &nsegment6);
#ifdef DEBUG
        printf("nsegment6=%d\n",nsegment6);
#endif
        for(i=0; i<nsegment6; i++)
		{
          fgets(str_tmp, STRLEN, fd);
          sscanf (str_tmp, "%d %d %d %d %d %d",&segment6[i][0],&segment6[i][1],
				  &segment6[i][2],&segment6[i][3],&segment6[i][4],&segment6[i][5]);
#ifdef DEBUG
          printf(" [%3d] %2d %2d %2d %2d %2d %2d\n",i,segment6[i][0],segment6[i][1],
				  segment6[i][2],segment6[i][3],segment6[i][4],segment6[i][5]);
#endif
		}
	  }
	}
  }

#endif /* READ_FILE */


  for(i=0; i<MAXSEGM; i++) for(j=0; j<5; j++) shift[i][j] = 0;
  for(i=0; i<MAXSEGM; i++)
  {
    for(j=0; j<5; j++)
    {
      shift[i][j] = segment6[i][j+1] + MAXSHIFT;
      if(shift[i][j]<0 || shift[i][j]>=NOFFSETS)
	  {
        printf("ERROR: MAXSHIFT=%d too small - exit\n",MAXSHIFT);
        exit(0);
	  }
	}
    angle[i] = shift[i][4];
  }

#ifdef DEBUG_NO
  printf("\n\n Shift table:\n");
  for(i=0; i<MAXSEGM; i++)
  {
    printf("%3d >>> %2d %2d %2d %2d %2d (angle=%2d)\n",i,shift[i][0],
		   shift[i][1],shift[i][2],shift[i][3],shift[i][4],angle[i]);
  }
  printf("\n\n");
#endif

  return;
}


/* lookup table part */
/*********************/


void
Word16Print(int num, word16_t w)
{
  int i;
  ap_uint<1> test;
  printf("%3d> ",num);
  for(i=(16-1); i>=0; i--)
  {
    if(!((i+1)%16) && i<15) printf(" ");
    test = (w>>i) & 1;
    if(test) printf("X");
    else     printf("-");
  }
  printf("\n");

  return;
}

void
Word96Print(int num, word96_t w)
{
  int i;
  ap_uint<1> test;
  printf("%3d> ",num);
  for(i=95; i>=0; i--)
  {
    if(!((i+1)%16) && i<95) printf(" ");
    test = (w>>i) & 1;
    if(test) printf("X");
    else     printf("-");
  }
  printf("\n");

  return;
}

void
Word112Print(int num, word112_t w)
{
  int i;
  ap_uint<1> test;
  printf("%3d> ",num);
  for(i=(NWIRES-1); i>=0; i--)
  {
    if(!((i+1)%16) && i<111) printf(" ");
    test = (w>>i) & 1;
    if(test) printf("X");
    else     printf("-");
  }
  printf("\n");

  return;
}

void
Word128Print(int num, word128_t w)
{
  int i;
  ap_uint<1> test;
  printf("%3d> ",num);
  for(i=(128-1); i>=0; i--)
  {
    if(!((i+1)%16) && i<127) printf(" ");
    test = (w>>i) & 1;
    if(test) printf("X");
    else     printf("-");
  }
  printf("\n");

  return;
}

void
Word224Print(int num, word224_t w)
{
  int i;
  ap_uint<1> test;
  printf("%3d> ",num);
  for(i=223; i>=0; i--)
  {
    if(!((i+1)%16) && i<223) printf(" ");
    test = (w>>i) & 1;
    if(test) printf("X");
    else     printf("-");
  }
  printf("\n");

  return;
}


/****************************************************************************/
/****************************************************************************/
/****************************************************************************/



#define SLMIN2 (b1|b2)
#define SLMIN3 ((b0&b1)|b2)
#define SLMIN4 (b2)
#define SLMIN5 ((b0&b2)|(b1&b2))
#define SLMIN6 (b1&b2)



void
word112_to_shorts7(word112_t in, Word112Ptr out)
{
  int i, j, k, ii;
  unsigned short *w = (unsigned short *)out->words;
  ap_uint<1> m;

  for(i=0; i<7; i++) w[i] = 0;

  for(i=0; i<7; i++)
  {
    for(k=0; k<16; k++)
	{
      ii = 16*i+k;
      m = (in>>ii)&((ap_uint<1>)1);
      //cout<<"i,k,m="<<+i<<" "<<+k<<" "<<+m<<endl;
      if(m) {w[i] |= 1<<k; /*printf("out.words[%d]=0x%08x\n",i,w[i]);*/}
	}
  }

  //i=2;
  //printf("out.words[%d]=0x%08x\n",i,w[i]);
  return;
}

void
shorts7_to_word112(Word112Ptr in, word112_t *outout)
{
  int i, j, k, m, ii;
  unsigned short *w = (unsigned short *)in->words;
  word112_t out;

  out = (word112_t)0;
  //Word112Print(31,out);
  for(i=0; i<7; i++)
  {
    for(k=0; k<16; k++)
	{
      ii = 16*i+k;
      m = (w[i]>>k)&1;
      if(m) {out |= (((word112_t)1)<<ii); /*printf("i=%d k=%d ii=%d\n",i,k,ii); Word112Print(32,out);*/}
	}
  }

  //Word112Print(33,out);
  *outout = out;
  return;
}



/* 3.36/5/2/0%/0%/1%/15% MAXSEGM=315 */

void
SegmentSearch21(ap_uint<3> n, word112_t in[6], word16_t out[NOFFSETS])
{
#pragma HLS ARRAY_PARTITION variable=in complete dim=1
#pragma HLS ARRAY_PARTITION variable=out complete dim=1
#pragma HLS PIPELINE II=2

  int i, j, k, stat;
  int8_t itmp, ishift;
  word16_t b0, b1, b2, bb;
  word16_t data[6];


  /****************/
  /* init section */
  static int8_t shift[MAXSEGM][5];
  static int8_t angle[MAXSEGM];
  AxialPlusStereoInit(shift, angle);
  /* init section */
  /****************/

  /* reset output */
  for(i=0; i<NOFFSETS; i++) out[i] = (word16_t)0;

  /* copy first layer once, it will not be shifted */
  ishift = n*16;
  data[0] = (in[0]>>ishift);

  stat = 0;
  for(j=0; j<MAXSEGM; j++)
  {
    for(i=1; i<6; i++) /* loop over layers 1-5 */
    {
      itmp = shift[j][i-1];
      if(itmp>=0) data[i] = (in[i]>>ishift) << itmp;
      else        data[i] = (in[i]>>ishift) >> (-itmp);
    }

    b1 = data[0]&data[1];
    b0 = data[0]^data[1];

    b1 = (b1)^((b0)&data[2]);
    b0 = (b0)^data[2];

    bb = (b0)&data[3];
    b2 = (b1)&bb;
    b1 = (b1)^bb;
    b0 = (b0)^data[3];

    bb = (b0)&data[4];
    b2 = (b2)^((b1)&bb);
    b1 = (b1)^bb;
    b0 = (b0)^data[4];

    bb = (b0)&data[5];
    b2 = (b2)^((b1)&bb);
    b1 = (b1)^bb;
    b0 = (b0)^data[5];

    out[angle[j]] |= SLMIN4;

  }

  return;
}



/* 3.36/17/14/0%/0%/1%/15% MAXSEGM=315 */

void
SegmentSearch2(word112_t in[6], word112_t out[NOFFSETS])
{
#pragma HLS ARRAY_PARTITION variable=in complete dim=1
#pragma HLS ARRAY_PARTITION variable=out complete dim=1
#pragma HLS PIPELINE II=14
  int i;
  ap_uint<3> n;
  word16_t outout[NOFFSETS];

#ifdef DEBUG
  printf("\nINPUT DATA:\n");
  for(i=NLAY-1; i>=0; i--) Word112Print(i,in[i]);
#endif

  /* reset output */
  for(i=0; i<NOFFSETS; i++) out[i] = (word112_t)0;

  for(n=0; n<7; n++)
  {
    SegmentSearch21(n, in, outout);
    for(i=0; i<NOFFSETS; i++) out[i] |= ((word112_t)outout[i])<<(n*16);
  }

#ifdef DEBUG
  printf("\nOUTPUT ARRAY:\n\n");
  for(i=(NOFFSETS-1); i>=0; i--)
  {
    Word112Print(i-8,out[i]);
  }

#endif

  return;
}
















void
SegmentSearch31(word16_t in[6], word1_t out[NOFFSETS], ap_uint<3> hit_threshold)
{
#pragma HLS ARRAY_PARTITION variable=in complete dim=1
#pragma HLS ARRAY_PARTITION variable=out complete dim=1
#pragma HLS PIPELINE II=1

  int i, j, k, stat;
  int8_t itmp;
  word1_t b0, b1, b2, bb;
  word1_t data[6];
  ap_uint<3> sum;

  /****************/
  /* init section */
  static int8_t shift[MAXSEGM][5];
  static int8_t angle[MAXSEGM];
  AxialPlusStereoInit(shift, angle);
  /* init section */
  /****************/

#ifdef DEBUG_NO
  printf("\n16BIT DATA:\n");
  for(i=NLAY-1; i>=0; i--) Word16Print(i,in[i]);
#endif

  /* reset output */
  for(i=0; i<NOFFSETS; i++) out[i] = (word1_t)0;

  /* copy first layer once, it will not be shifted */
  data[0] = (word1_t)in[0](MAXSHIFT,MAXSHIFT);

  stat = 0;
#ifdef DEBUG_NO
  cout<<endl;
#endif
  for(j=0; j<MAXSEGM; j++)
  {
#ifdef DEBUG_NO
    cout<<"#segm="<<+j<<" itmp[5..1]="<<+shift[j][4]<<","<<+shift[j][3]<<","<<+shift[j][2]<<","<<+shift[j][1]<<","<<+shift[j][0]<<" MAXSHIFT="<<+MAXSHIFT<<endl;
#endif
    for(i=5; i>0; i--) /* loop over layers 5-1 shifting them in according to segment dictionary */
    {
      itmp = shift[j][i-1];
      data[i] = ((in[i]) >> itmp)&((ap_uint<1>)1);
#ifdef DEBUG_NO
      Word16Print(i,(((in[i]<<MAXSHIFT)>>itmp)) );
#endif
    }
#ifdef DEBUG_NO
    Word16Print(0,in[0]);
	for(i=0; i<20-MAXSHIFT; i++) printf(" ");
    printf("^\n");
#endif

    /*3.48/2/1/0%/0%/~0%(364)/~0%(2471)*/

    sum = data[0]+data[1]+data[2]+data[3]+data[4]+data[5];
#ifdef DEBUG_NO
    cout<<"??? sum="<<sum<<" data = "<<data[0]<<" "<<data[1]<<" "<<data[2]<<" "<<data[3]<<" "<<data[4]<<" "<<data[5]<<endl;
#endif
    if(sum>=hit_threshold)
	{
#ifdef DEBUG_NO
      cout<<"!!! sum="<<sum<<endl;
#endif
      out[angle[j]] |= 1;
	}
#ifdef DEBUG_NO
	cout<<endl;
#endif

    /*3.36/3/2/0%/0%/~0%(671)/~0%(2290)*/
/*
    b1 = data[0]&data[1];
    b0 = data[0]^data[1];

    b1 = (b1)^((b0)&data[2]);
    b0 = (b0)^data[2];

    bb = (b0)&data[3];
    b2 = (b1)&bb;
    b1 = (b1)^bb;
    b0 = (b0)^data[3];

    bb = (b0)&data[4];
    b2 = (b2)^((b1)&bb);
    b1 = (b1)^bb;
    b0 = (b0)^data[4];

    bb = (b0)&data[5];
    b2 = (b2)^((b1)&bb);
    b1 = (b1)^bb;
    b0 = (b0)^data[5];

    out[angle[j]] |= SLMIN4;
*/

  }

  return;
}




/* 3.48/ 2/ 1/0%/0%/5%/79% ii=1 */
/* 3.48/ 3/ 2/0%/0%/3%/41% ii=2 */
/* 3.48/ 4/ 3/0%/0%/3%/28% ii=3 */
/* 3.48/ 5/ 4/0%/0%/2%/20% ii=4 */

/*     /  /  / %/ %/ %/ % ii=32 */

/* 3.48/15/14/0%/0%/2%/6% II=14 */

void
SegmentSearch3(word112_t in[6], word112_t out[NOFFSETS], ap_uint<3> hit_threshold)
{
#pragma HLS ARRAY_PARTITION variable=in complete dim=1
#pragma HLS ARRAY_PARTITION variable=out complete dim=1
#pragma HLS PIPELINE II=32
  int i;
  int8_t n;
  word1_t outout[NOFFSETS];
  word128_t inin[6];
  word16_t ininin[6];

#ifdef DEBUG
  printf("\nSegmentSearch3: IN DATA:\n");
  for(i=NLAY-1; i>=0; i--) {printf("         ");Word112Print(i,in[i]);}
#endif

  for(i=0; i<NLAY; i++)
  {
	inin[i](127,112+MAXSHIFT) = 0;
    inin[i](111+MAXSHIFT,MAXSHIFT) = in[i];
    inin[i](MAXSHIFT-1,0) = 0;
  }

#ifdef DEBUG
  printf("\nSegmentSearch3: ININ DATA:\n");
  for(i=NLAY-1; i>=0; i--) Word128Print(i,inin[i]);
#endif

  /* reset output */
  for(i=0; i<NOFFSETS; i++) out[i] = (word112_t)0;

  for(n=0; n<112; n++)
  {
#ifdef DEBUG_NO
	printf("nnn=%d\n",n);
#endif
    for(i=0; i<6; i++) ininin[i] = inin[i]>>n;
    /*
    for(i=0; i<6; i++)
	{
      inin[i] = inin[i]>>1;
      ininin[i] = inin[i];
	}
    */
    SegmentSearch31(ininin, outout, hit_threshold);
    for(i=0; i<NOFFSETS; i++) out[i] |= ((word112_t)outout[i])<<n;
  }

#ifdef DEBUG
  printf("\nSegmentSearch3: OUTPUT ARRAY:\n");
  for(i=(NOFFSETS-1); i>=0; i--)
  {
    printf("         ");
    Word112Print(i-8,out[i]);
  }
#endif

  return;
}









/* 3.48/ 5/ 4/0%/0%/5%/41% ii=4 - out1&out2*/
/* 3.48/ 7/ 4/0%/0%/7%/51% ii=4 - <<5 >>5 */
/* 3.48/ 10/ 4/0%/0%/7%/52% ii=4 - <<5 >>5 with return */

/* region (2 superlayers) */

ap_uint<1>
RegionSearch1(word112_t in1[6], word112_t in2[6], word112_t out[NOFFSETS], ap_uint<3> hit_threshold)
{
#pragma HLS ARRAY_PARTITION variable=in1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=in2 complete dim=1
#pragma HLS ARRAY_PARTITION variable=out complete dim=1
#pragma HLS PIPELINE II=4
  word112_t out1[NOFFSETS], out2[NOFFSETS];
  int i, j;
  ap_uint<1> ret;

  SegmentSearch3(in1, out1, hit_threshold);
  SegmentSearch3(in2, out2, hit_threshold);

  for(i=0; i<NOFFSETS; i++)
  {
    out[i] = out1[i] & out2[i];
    for(j=1; j<=10; j++) out[i] |= out1[i] & (out2[i]<<j);
    for(j=1; j<=10; j++) out[i] |= out1[i] & (out2[i]>>j);
  }

  ret = 0;
  for(i=0; i<NOFFSETS; i++) for(j=0; j<NWIRES; j++) ret |= out[i](j,j);

#ifdef DEBUG
  cout<<endl<<"REGION OUTPUT ARRAY, ret="<<+ret<<endl;
  for(i=(NOFFSETS-1); i>=0; i--)
  {
    printf("        ");
    Word112Print(i-8,out[i]);
  }

#endif

  return(ret);
}
