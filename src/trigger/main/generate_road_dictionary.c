/* generate_road_dictionary.c - reads drift chamber dictionary file(s) and vhdl file(s) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*static char *filein = "DCdictionary.txt";*/
/*static char *filein = "/work/boiarino/data/Sec1TorNeg1SolN1.txt";*/
static char *filein = "/usr/clas12/release/1.3.2/clon/src/trigger/data/TracksDic_n20000000_newformat.txt";

static char *headername = "roaddict.h";
static char *vhdlname = "roadfinder.vhd";

static char *filename1 = "roadchargedict.h";
static char *filename2 = "roadftofdict.h";

static int BIN_SIZE[6] = {
    2,  /* SL1 */
    2,  /* SL2 */
    3,  /* SL3 */
    3,  /* SL4 */
    3,  /* SL5 */
    3   /* SL6 */
  };

#define MULTIPLICITY  5

char *
get_lut_init()
{
  int i, j;
  static char buf[100];
  unsigned long long lut = 0;

  for(i=0;i<64;i++)
  {
    int ones = 0;
    for(j=0;j<6;j++)
    {
      if(i & (1<<j))
        ones++;
    }
    if(ones >= MULTIPLICITY)
      lut |= ((unsigned long long)1<<i);
  }
  sprintf(buf, "x\"%llX\"", lut);
  return buf;
}

/*

32bit  4294967296
31bit  2147483648
30bit  1073741824
29bit   536870912
28bit   268435456
27bit   134217728
26bit    67108864
25bit    33554432
24bit    16777216

*/

#define NLOOKUP 268435456
static unsigned char lookup[NLOOKUP];

#define ICHARGE 6
#define IFTOF   7
#define NCOL    8
int roads[2000000][NCOL];

#define NFTOF 62

static int
road6compare(const void *segm1, const void *segm2)
{
  int *p1 = (int *)segm1;
  int *p2 = (int *)segm2;
  int k;

  /* sort from sl0 to sl5 
  for(k=0; k<6; k++)
  {
    if(p1[k]>p2[k]) return(1);
    if(p1[k]<p2[k]) return(-1);
  }
  */

  /* sort from sl5 to sl0
  for(k=5; k>=0; k--)
  {
    if(p1[k]>p2[k]) return(1);
    if(p1[k]<p2[k]) return(-1);
  }
  */

  k=5;
  if(p1[k]>p2[k]) return(1);
  if(p1[k]<p2[k]) return(-1);
  k=3;
  if(p1[k]>p2[k]) return(1);
  if(p1[k]<p2[k]) return(-1);
  k=1;
  if(p1[k]>p2[k]) return(1);
  if(p1[k]<p2[k]) return(-1);
  k=4;
  if(p1[k]>p2[k]) return(1);
  if(p1[k]<p2[k]) return(-1);
  k=2;
  if(p1[k]>p2[k]) return(1);
  if(p1[k]<p2[k]) return(-1);
  k=0;
  if(p1[k]>p2[k]) return(1);
  if(p1[k]<p2[k]) return(-1);


  return(0);
}


int
main()
{
  int ii, jj, kk, iprev, sl_min, sl_max;
  FILE *fd, *frd;
  char fname[256], frname[256];
  char buf[1000];
  int iw[6], w[36];
  float mom,tet,phi,f1;
  int charge, ftof;
  int npositives, nnegatives, nftofs[NFTOF];
  int dif[6][6], min[6][6], max[6][6];
  int sldif[6], slmin[6], slmax[6];
  int nbits[6], shift[6];
  int addr[6];
  unsigned int address;
  int nlines, nroads, exist, ret, nunmatched, nmatched, matched_sl;

  /*sprintf(fname,"%s/ssp/%s",getenv("CLON_PARMS"),filein);*/
  sprintf(fname,"%s",filein);
  
  npositives = 0;
  nnegatives = 0;
  for(ii=0; ii<NFTOF; ii++) nftofs[ii] = 0;
  
  for(ii=0; ii<6; ii++)
  {
    for(jj=0; jj<6; jj++)
    {
      min[ii][jj] = 200;
      max[ii][jj] = -200;
    }
  }

  for(jj=0; jj<6; jj++)
  {
    slmin[jj] = 200;
    slmax[jj] = -200;
  }

  /* process file doing ... */
  fd = fopen(fname,"r");
  if(fd==NULL)
  {
    printf("\nCannot open dictionary file >%s< - exit\n",fname);
    exit(0);
  }
  else
  {
    printf("\nUsing file %s\n",fname);
  }
  nlines = 0;
  nroads = 0;
  nmatched = 0;
  nunmatched = 0;
  while (fgets(buf,256,fd) != NULL)
  {
    if(buf[0] == '#')
      continue;

    nlines++;
    if(!(nlines % 100000))
    {
      printf("lines = %u, nroads = %u, nnegatives = %u, npositives = %u, nmatched = %u, nunmatched = %u\n", nlines, nroads, nnegatives, npositives, nmatched, nunmatched);
      nmatched = 0;
      nunmatched = 0;
    }

    ret = sscanf(buf,"%f %f %f %f %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
				 &charge,&mom,&tet,&phi,
				 &w[0],&w[1],&w[2],&w[3],&w[4],&w[5],&w[6],&w[7],&w[8],&w[9],&w[10],&w[11],
				 &w[12],&w[13],&w[14],&w[15],&w[16],&w[17],&w[18],&w[19],&w[20],&w[21],&w[22],&w[23],
				 &w[24],&w[25],&w[26],&w[27],&w[28],&w[29],&w[30],&w[31],&w[32],&w[33],&w[34],&w[35],
                 &ftof);

    if(w[0]<=0) continue;
    if(w[6]<=0) continue;
    if(w[12]<=0) continue;
    if(w[18]<=0) continue;
    if(w[24]<=0) continue;
    if(w[30]<=0) continue;

    for(ii=0; ii<6; ii++)
      iw[ii] = (w[ii*6]-1) / BIN_SIZE[ii];

	/*
    printf("mom=%4.1f tet=%4.1f phi=%4.1f -> iw = %2d %2d %2d %2d %2d %2d\n",
      mom,tet,phi,iw[0],iw[1],iw[2],iw[3],iw[4],iw[5]);
	*/


    exist = 0;
    for(jj=0; jj<nroads; jj++)
    {
      if(roads[jj][0]==iw[0]&&roads[jj][1]==iw[1]&&roads[jj][2]==iw[2]&&roads[jj][3]==iw[3]&&roads[jj][4]==iw[4]&&roads[jj][5]==iw[5])
      {
        exist = 1;
        break;
      }
    }

    for(jj=0; jj<nroads; jj++)
    {
      matched_sl = 0;

      for(kk=0; kk<6; kk++)
      {
        if(roads[jj][kk]==iw[kk])
          matched_sl++;
      }
      if(matched_sl >= 5)
      {
        nmatched++;
        break;
      }
    }

    if(matched_sl < 5)
      nunmatched++;

	/*
if(iw[5]==56)
	*/


    if(exist==0)
	{
      ftof --; /* in original table ftof starts from 1, make it start from 0 */

      for(ii=0; ii<6; ii++) roads[nroads][ii] = iw[ii];
      roads[nroads][ICHARGE] = charge;
      roads[nroads][IFTOF] = ftof;

      nroads ++;
      if(charge>0) npositives ++;
      if(charge<0) nnegatives ++;
      if(ftof >= 0) nftofs[ftof] ++; /* negative ftof means no ftof match (?) */

     /*printf("new, nroads=%d\n",nroads);*/


      for(ii=0; ii<6; ii++)
	  {
        if(slmin[ii]>iw[ii]) slmin[ii]=iw[ii];
        if(slmax[ii]<iw[ii]) slmax[ii]=iw[ii];
	  }
      

	}


    for(ii=0; ii<6; ii++)
	{
      for(jj=0; jj<6; jj++)
	  {
        dif[ii][jj] = iw[jj]-iw[ii];

        if( dif[ii][jj] < min[ii][jj] ) min[ii][jj] = dif[ii][jj];
        if( dif[ii][jj] > max[ii][jj] ) max[ii][jj] = dif[ii][jj];
	  }
	}

  }



  printf("slmin, slmax, diff:\n");
  for(ii=5; ii>=0; ii--)
  {
    sldif[ii] = slmax[ii]-slmin[ii]+1;
    printf("[%d] %4d %4d %4d\n",ii,slmin[ii],slmax[ii],sldif[ii]);
  }


  /* sort nroads */
  /*
  for(ii=0; ii<nroads; ii++) printf("B [%4d] %3d %3d %3d %3d %3d %3d\n",ii,
	roads[ii][0],roads[ii][1],roads[ii][2],roads[ii][3],roads[ii][4],roads[ii][5]);
  */
  qsort((void *)roads, nroads, NCOL*sizeof(int), road6compare);
  /*  
  for(ii=0; ii<nroads; ii++) printf("A [%4d] %3d %3d %3d %3d %3d %3d\n",ii,
	roads[ii][0],roads[ii][1],roads[ii][2],roads[ii][3],roads[ii][4],roads[ii][5]);
  */
  /***************/

  /* generate 'cc' code for HLS */
  sprintf(frname,"%s","dcroadfinder.h");
  frd = fopen(frname,"w");
  if(frd==NULL)
  {
    printf("\nCannot open file >%s< for writing\n",frname);
    exit(0);
  }
  else
  {
    printf("\nWill write road finder file >%s<\n",frname);

    fprintf(frd,"static int BIN_SIZE[6] = {\n");
    fprintf(frd, "    %d,  /* SL1 */\n", BIN_SIZE[0]);
    fprintf(frd, "    %d,  /* SL2 */\n", BIN_SIZE[1]);
    fprintf(frd, "    %d,  /* SL3 */\n", BIN_SIZE[2]);
    fprintf(frd, "    %d,  /* SL4 */\n", BIN_SIZE[3]);
    fprintf(frd, "    %d,  /* SL5 */\n", BIN_SIZE[4]);
    fprintf(frd, "    %d,  /* SL6 */\n", BIN_SIZE[5]);
    fprintf(frd, "  };\n");

    fprintf(frd,"#define NROADS %d\n\n",nroads);
    fprintf(frd,"#define MIN_SL5 %d\n",slmin[5]);
    fprintf(frd,"#define MIN_SL4 %d\n",slmin[4]);
    fprintf(frd,"#define MIN_SL3 %d\n",slmin[3]);
    fprintf(frd,"#define MIN_SL2 %d\n",slmin[2]);
    fprintf(frd,"#define MIN_SL1 %d\n",slmin[1]);
    fprintf(frd,"#define MIN_SL0 %d\n\n",slmin[0]);
    fprintf(frd,"#define NBIT_SL5 %d\n",sldif[5]);
    fprintf(frd,"#define NBIT_SL4 %d\n",sldif[4]);
    fprintf(frd,"#define NBIT_SL3 %d\n",sldif[3]);
    fprintf(frd,"#define NBIT_SL2 %d\n",sldif[2]);
    fprintf(frd,"#define NBIT_SL1 %d\n",sldif[1]);
    fprintf(frd,"#define NBIT_SL0 %d\n\n",sldif[0]);
    fprintf(frd,"#define ROADS_SEARCH \\\n");
 
	/*output to [112] single bit array */
	iprev = -1;
    for(ii=0; ii<nroads; ii++)
    {
      if(iprev!=roads[ii][5])
      {
        iprev = roads[ii][5];
		/*
if(roads[ii][5]==56)
		*/
        fprintf(frd,"  sum = ");
	  }
      else
	  {
		/*
if(roads[ii][5]==56)
		*/
        fprintf(frd,"  sum = ");
	  }
	  /*
if(roads[ii][5]==56)
	  */
{
      fprintf(frd,"sl5+sl3(%3d,%3d)+sl1(%3d,%3d)+sl4(%3d,%3d)+sl2(%3d,%3d)+sl0(%3d,%3d)",
           roads[ii][3]-slmin[3],roads[ii][3]-slmin[3],
           roads[ii][1]-slmin[1],roads[ii][1]-slmin[1],
		   roads[ii][4]-slmin[4],roads[ii][4]-slmin[4],
           roads[ii][2]-slmin[2],roads[ii][2]-slmin[2],
           roads[ii][0]-slmin[0],roads[ii][0]-slmin[0]);

      if(ii==(nroads-1)) fprintf(frd,"; if(sum>=sl_threshold) res = 1;\n");
      else               fprintf(frd,"; if(sum>=sl_threshold) res = 1; \\\n");
}
    }

    fclose(frd);
  }

  for(ii=0; ii<6; ii++)
  {
    for(jj=0; jj<6; jj++)
	{
      dif[ii][jj] = max[ii][jj]-min[ii][jj];
	}
  }


  printf("\n\n#bits for different superlayer couples:\n\n  ");
  for(ii=0; ii<6; ii++) printf("          w[%d]",ii);
  printf("\n");
  for(ii=0; ii<6; ii++)
  {
    printf("w[%d]",ii);
    for(jj=0; jj<ii; jj++) printf("              ");
    for(jj=ii; jj<6; jj++)
	{
      printf(" %4d[%3d:%3d]",dif[ii][jj],min[ii][jj],max[ii][jj]);
	}
    printf("\n");
  }
  printf("\n");
  printf("nroads=%d\n\n",nroads);

  fclose(fd);




#if 1
  /* write road dictionary as header file*/
  fd = fopen(headername,"w");
  if(fd!=NULL)
  {
    fprintf(fd,"/* roaddict.h */\n\n");
    fprintf(fd,"/* table containing road dictionary, 6 columns - wire numbers in first layers of every superlayer, from sl0 to sl5 */\n");
    fprintf(fd,"/* sorting in following order: sl5, sl3, sl1, sl4,sl2, sl0 */\n\n");

    fprintf(fd,"#define NROADS %4d\n",nroads);
    fprintf(fd,"static int roaddict[NROADS][6] = {\n");

    for(nlines=0; nlines<nroads; nlines++)
    {
      if(nlines>0) fprintf(fd,",\n");
      fprintf(fd,"%3d, %3d, %3d, %3d, %3d, %3d",roads[nlines][0],roads[nlines][1],roads[nlines][2],roads[nlines][3],roads[nlines][4],roads[nlines][5]);
    }
    fprintf(fd,"\n};\n");

    fclose(fd);
    printf("Recorded file >%s<\n",headername);
  }
  else
  {
    printf("ERROR: cannot open file >%s<\n",headername);
  }
#endif


#if 1
  /* write road dictionary as vhdl file*/
  fd = fopen("roadfinder.vhd", "wt");
  if(fd!=NULL)
  {
    char *lut = get_lut_init();
  
    fprintf(fd,
      "library ieee;\n"\
      "use ieee.std_logic_1164.all;\n"\
      "use ieee.std_logic_misc.all;\n"\
      "\n"\
      "library utils;\n"\
      "use utils.utils_pkg.all;\n"\
      "\n"\
      "library unisim;\n"\
      "use unisim.vcomponents.all;\n"\
      "\n"\
      "entity roadfinder is\n"\
      "  port(\n"\
      "    SEGS_SL1         : in std_logic_vector(111 downto 0);\n"\
      "    SEGS_SL2         : in std_logic_vector(111 downto 0);\n"\
      "    SEGS_SL3         : in std_logic_vector(111 downto 0);\n"\
      "    SEGS_SL4         : in std_logic_vector(111 downto 0);\n"\
      "    SEGS_SL5         : in std_logic_vector(111 downto 0);\n"\
      "    SEGS_SL6         : in std_logic_vector(111 downto 0);\n"\
      "    ROAD_VALID       : out std_logic;\n"\
      "    P_ROAD_VALID     : out std_logic;\n"\
      "    N_ROAD_VALID     : out std_logic\n"
      "  );\n"\
      "end roadfinder;\n"\
      "\n"\
      "architecture synthesis of roadfinder is\n"
    );
    fprintf(fd, "  signal W1    : std_logic_vector(%d downto 0);\n", (112+BIN_SIZE[0]-1)/BIN_SIZE[0]-1);
    fprintf(fd, "  signal W2    : std_logic_vector(%d downto 0);\n", (112+BIN_SIZE[1]-1)/BIN_SIZE[1]-1);
    fprintf(fd, "  signal W3    : std_logic_vector(%d downto 0);\n", (112+BIN_SIZE[2]-1)/BIN_SIZE[2]-1);
    fprintf(fd, "  signal W4    : std_logic_vector(%d downto 0);\n", (112+BIN_SIZE[3]-1)/BIN_SIZE[3]-1);
    fprintf(fd, "  signal W5    : std_logic_vector(%d downto 0);\n", (112+BIN_SIZE[4]-1)/BIN_SIZE[4]-1);
    fprintf(fd, "  signal W6    : std_logic_vector(%d downto 0);\n", (112+BIN_SIZE[5]-1)/BIN_SIZE[5]-1);
  
    fprintf(fd,
      "  signal O     : std_logic_vector(0 to %d);\n", nroads-1
    );
    fprintf(fd,
      "  signal O_P   : std_logic_vector(0 to %d);\n", npositives-1
    );
    fprintf(fd,
      "  signal O_N   : std_logic_vector(0 to %d);\n", nnegatives-1
    );

    fprintf(fd,
      "begin\n"\
      "\n"\
      "  ROAD_VALID <= or_reduce(O);\n"\
      "  P_ROAD_VALID <= or_reduce(O_P);\n"\
      "  N_ROAD_VALID <= or_reduce(O_N);\n"
      "\n"
    );

    for(ii=0;ii<6;ii++)
    {
      for(jj=0;jj<(112+BIN_SIZE[ii]-1)/BIN_SIZE[ii];jj++)
      {
        sl_max = BIN_SIZE[ii]*jj+BIN_SIZE[ii]-1;
        sl_min = BIN_SIZE[ii]*jj;
        if(sl_max > 111) sl_max = 111;
        fprintf(fd,"  W%d(%d) <= or_reduce(SEGS_SL%d(%d downto %d))\n", ii+1, jj, ii+1, sl_max, sl_min);
      }
    }

    for(nlines=0; nlines<nroads; nlines++)
    {
      fprintf(fd,
        "  LUT6_inst%d: LUT6 generic map(INIT=>%s) "\
        "port map(O=>O(%d),I0=>W1(%d),I1=>W2(%d),I2=>W3(%d),"\
        "I3=>W4(%d),I4=>W5(%d),I5=>W6(%d));\n",
         nlines, lut, nlines,
         roads[nlines][0],roads[nlines][1],roads[nlines][2],
         roads[nlines][3],roads[nlines][4],roads[nlines][5]
      );
    } 

    for(nlines=0; nlines<nroads; nlines++)
    {
      if(roads[nlines][ICHARGE]>0)
	  {
        if(ii>0) fprintf(fd,",\n");
        ii++;
        kk++;
        fprintf(fd,"%3d",nlines);
	  }
    }




    fprintf(fd, "\n");
    fprintf(fd, "end synthesis;\n\n");
  
    printf("Recorded file >%s<\n",vhdlname);
    fclose(fd);
  }
  else
  {
    printf("ERROR: cannot open file >%s<\n",vhdlname);
  }
#endif









#if 1
  /* write road-charge dictionary as header file*/
  fd = fopen(filename1,"w");
  if(fd!=NULL)
  {
    fprintf(fd,"/* roadchargedict.h */\n\n");
    fprintf(fd,"/* table containing charge vs road index from road dictionary */\n");

    fprintf(fd,"#define NPOSITIVES %4d\n",npositives);
    fprintf(fd,"#define NNEGATIVES %4d\n",nnegatives);

    fprintf(fd,"\n");
    fprintf(fd,"static int roadpositivedict[NPOSITIVES] = {\n");
    kk = 0;
    ii = 0;
    for(nlines=0; nlines<nroads; nlines++)
    {
      if(roads[nlines][ICHARGE]>0)
	  {
        if(ii>0) fprintf(fd,",\n");
        ii++;
        kk++;
        fprintf(fd,"%3d",nlines);
	  }
    }
    fprintf(fd,"\n};\n");
    printf("  npositives=%d, ii=%d\n",npositives,ii);

    fprintf(fd,"\n");
    fprintf(fd,"static int roadnegativedict[NNEGATIVES] = {\n");
    ii = 0;
    for(nlines=0; nlines<nroads; nlines++)
    {
      if(roads[nlines][ICHARGE]<0)
	  {
        if(ii>0) fprintf(fd,",\n");
        ii++;
        kk++;
        fprintf(fd,"%3d",nlines);
	  }
    }
    fprintf(fd,"\n};\n");
    printf("  negatives=%d, ii=%d\n",nnegatives,ii);

    fclose(fd);

    printf("Recorded file >%s<, kk=%d\n",filename1,kk);
  }
  else
  {
    printf("ERROR: cannot open file >%s<\n",filename1);
  }
#endif




#if 1
  /* write road-ftof dictionary as header file*/
  fd = fopen(filename2,"w");
  if(fd!=NULL)
  {
    fprintf(fd,"/* roadchargedict.h */\n\n");
    fprintf(fd,"/* table containing charge vs road index from road dictionary */\n\n");

    for(jj=0; jj<NFTOF; jj++)
	{
      fprintf(fd,"#define NFTOF%02d %4d\n",jj,nftofs[jj]);
    }
    fprintf(fd,"\n");


    fprintf(fd,"static int nroadftof[] = {\n");
    for(jj=0; jj<NFTOF; jj++)
	{
      fprintf(fd,"  NFTOF%02d",jj);
      if(jj<(NFTOF-1)) fprintf(fd,",\n",jj);
	}
    fprintf(fd,"\n};\n");
    fprintf(fd,"\n");


    kk = 0;
    for(jj=0; jj<NFTOF; jj++)
	{
      fprintf(fd,"\n");
      fprintf(fd,"static int roadftofdict%02d[NFTOF%02d] = {\n",jj,jj,nftofs[jj]);
      ii = 0;
      for(nlines=0; nlines<nroads; nlines++)
      {
        if(roads[nlines][IFTOF]==jj)
	    {
          if(ii>0) fprintf(fd,",\n");
          ii++;
          kk++;
          fprintf(fd,"  %6d",nlines);
	    }
      }
      fprintf(fd,"\n};\n");
      printf("  nftofs[%2d]=%d, ii=%d\n",jj,nftofs[jj],ii);
    }


    fprintf(fd,"static int *roadftof[] = {\n");
    for(jj=0; jj<NFTOF; jj++)
	{
      fprintf(fd,"  roadftofdict%02d",jj);
      if(jj<(NFTOF-1)) fprintf(fd,",\n",jj);
	}
    fprintf(fd,"\n};\n");



    fclose(fd);

    printf("Recorded file >%s<, kk=%d\n",filename2,kk);
  }
  else
  {
    printf("ERROR: cannot open file >%s<\n",filename2);
  }
#endif
















#if 0

  /* calculate it !!!!!!!!!! */

  nbits[0] = 7;
  nbits[1] = 4; /*11*/
  nbits[2] = 3; /*14*/
  nbits[3] = 4; /*18*/
  nbits[4] = 5; /*23*/
  nbits[5] = 5; /*28*/

  shift[0] = 0;
  shift[1] = 7;
  shift[2] = 11;
  shift[3] = 14;
  shift[4] = 18;
  shift[5] = 23;

  for(ii=0; ii<NLOOKUP; ii++) lookup[ii] = 0;

  /* process file again */
  fd = fopen(fname,"r");
  nlines=0;
  while (fgets(buf,256,fd) != NULL)
  {
    ret = sscanf(buf,"%f %f %f %f %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %f",
				 &charge,&mom,&tet,&phi,
				 &w[0],&w[1],&w[2],&w[3],&w[4],&w[5],&w[6],&w[7],&w[8],&w[9],&w[10],&w[11],
				 &w[12],&w[13],&w[14],&w[15],&w[16],&w[17],&w[18],&w[19],&w[20],&w[21],&w[22],&w[23],
				 &w[24],&w[25],&w[26],&w[27],&w[28],&w[29],&w[30],&w[31],&w[32],&w[33],&w[34],&w[35],
                 &ftof);
    iw[0] = w[0];
    iw[1] = w[6];
    iw[2] = w[12];
    iw[3] = w[18];
    iw[4] = w[24];
    iw[5] = w[30];
    if(iw[0]<=0) continue;
    if(iw[1]<=0) continue;
    if(iw[2]<=0) continue;
    if(iw[3]<=0) continue;
    if(iw[4]<=0) continue;
    if(iw[5]<=0) continue;


    for(ii=0; ii<6; ii++) iw[ii]--; /* start wire number from 0 */
	/*
    printf("mom=%4.1f tet=%4.1f phi=%4.1f -> iw = %2d %2d %2d %2d %2d %2d\n",
      mom,tet,phi,iw[0],iw[1],iw[2],iw[3],iw[4],iw[5]);
	*/

    addr[0] = iw[0];
    addr[1] = (iw[1]-iw[0]);
    addr[2] = (iw[2]-iw[0]+7);
    addr[3] = (iw[3]-iw[0]+5);
    addr[4] = (iw[4]-iw[0]+23);
    addr[5] = (iw[5]-iw[0]+27);
    address = 0;
    for(ii=0; ii<6; ii++)
	{
      if(addr[ii] < 0)
      {
        printf("ERROR !!! addr[%d]=%d - exit\n",ii,addr[ii]);
        printf("mom=%4.1f tet=%4.1f phi=%4.1f -> iw = %2d %2d %2d %2d %2d %2d\n",
          mom,tet,phi,iw[0],iw[1],iw[2],iw[3],iw[4],iw[5]);
        fclose(fd);
        exit(0);
	  }
      address += addr[ii]<<shift[ii];
	}
	if(address >= NLOOKUP)
    {
      printf("ERROR: address=0x%08x (%3d %3d %3d %3d %3d %3d)\n",address,addr[0],addr[1],addr[2],addr[3],addr[4],addr[5]);
      fclose(fd);
      exit(0);
    }
    else
    {
      lookup[address] = 1;
    }

    nlines++;
  }

  fclose(fd);


  nroads = 0;
  for(ii=0; ii<NLOOKUP; ii++) if(lookup[ii]>0) nroads ++;;
  printf("lookup size = %d, nroads = %d, usage = %f percent\n",NLOOKUP,nroads,(float)nroads*100./(float)NLOOKUP);

#endif





  exit(0);
}
