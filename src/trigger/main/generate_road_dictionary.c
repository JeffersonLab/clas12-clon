/* dc_dict.c - reads drift chamber dictionary file and generate hardware image */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *filename = "DCdictionary.txt";

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

int roads[2000000][6];


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
  /* sort from sl5 to sl0 */
  for(k=5; k>=0; k--)
  {
    if(p1[k]>p2[k]) return(1);
    if(p1[k]<p2[k]) return(-1);
  }
  return(0);
}


int
main()
{
  int ii, jj, iprev;
  FILE *fd, *frd;
  char fname[256], frname[256];
  char buf[1000];
  int iw[6];
  float mom,tet,phi;
  int dif[6][6], min[6][6], max[6][6];
  int nbits[6], shift[6];
  int addr[6];
  unsigned int address;
  int nlines, nroads, exist;

  sprintf(fname,"%s/ssp/%s",getenv("CLON_PARMS"),filename);

  for(ii=0; ii<6; ii++)
  {
    for(jj=0; jj<6; jj++)
    {
      min[ii][jj] = 200;
      max[ii][jj] = -200;
    }
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
  while (fgets(buf,256,fd) != NULL)
  {
    sscanf(buf,"%f %f %f %d %d %d %d %d %d",&mom,&tet,&phi,&iw[0],&iw[1],&iw[2],&iw[3],&iw[4],&iw[5]);
    for(ii=0; ii<6; ii++) iw[ii]--; /* start wire number from 0 */
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
		/*printf("exist\n");*/
        break;
      }
    }

    if(exist==0)
	{
      for(ii=0; ii<6; ii++) roads[nroads][ii] = iw[ii];
      nroads ++;
      /*printf("new, nroads=%d\n",nroads);*/
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

    nlines++;
  }






  /* sort nroads */
  /*
  for(ii=0; ii<nroads; ii++) printf("B [%4d] %3d %3d %3d %3d %3d %3d\n",ii,
	roads[ii][0],roads[ii][1],roads[ii][2],roads[ii][3],roads[ii][4],roads[ii][5]);
  */
  qsort((void *)roads, nroads, 6*sizeof(int), road6compare);
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

    fprintf(frd,"#define NROADS %d\n",nroads);
    fprintf(frd,"#define ROADS_SEARCH \\\n");
 
	/*output to [112] single bit array */
	iprev = -1;
    for(ii=0; ii<nroads; ii++)
    {
      if(iprev!=roads[ii][5])
      {
        iprev = roads[ii][5];
        fprintf(frd,"  res[%3d]  = ",roads[ii][5]);
	  }
      else
	  {
        fprintf(frd,"  res[%3d] |= ",roads[ii][5]);
	  }
      fprintf(frd," sl[5](%3d,%3d)&sl[4](%3d,%3d)&sl[3](%3d,%3d)&sl[2](%3d,%3d)&sl[1](%3d,%3d)&sl[0](%3d,%3d)",
           roads[ii][5],roads[ii][5],roads[ii][4],roads[ii][4],roads[ii][3],roads[ii][3],
		   roads[ii][2],roads[ii][2],roads[ii][1],roads[ii][1],roads[ii][0],roads[ii][0]);

      if(ii==(nroads-1)) fprintf(frd,";\n");
      else               fprintf(frd,"; \\\n");
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
    sscanf(buf,"%f %f %f %d %d %d %d %d %d",&mom,&tet,&phi,&iw[0],&iw[1],&iw[2],&iw[3],&iw[4],&iw[5]);
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




  exit(0);
}
