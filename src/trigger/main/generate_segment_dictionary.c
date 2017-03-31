
/* generate_segment_dictionary.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static char *filename = "segmdict.h";

int
main()
{
  FILE *fd;
  char buf[1000];
  int u,v,w;
  float fu,fv,fw;
  int nlines;

  hitpattern();

#if 0
  printf("/* pcal_atten.h */\n\n");
  printf("/* table containing exp(-path[axis]/atten), where axis can be [0..2] so we have 3 tables; arguments are 'u' and 'v' strip number */\n\n");

  /* process file counting the number of entries */
  fd = fopen(filename,"r");
  nlines=0;
  while (fgets(buf,256,fd) != NULL)
  {
    nlines++;
  }
  fclose(fd);
  printf("#define PCATTEN %4d\n",nlines);
  printf("static int filecontent[PCATTEN][6] = {\n");

  /* process file again */
  fd = fopen(filename,"r");
  nlines=0;
  while (fgets(buf,256,fd) != NULL)
  {
    sscanf(buf,"%d %d %d %f %f %f",&u,&v,&w,&fu,&fv,&fw);
    if(nlines>0) printf(",\n");
    printf("%2d, %2d, %2d, %3d, %3d, %3d",u,v,w,(int)(fu*1000.),(int)(fv*1000.),(int)(fw*1000.));
    nlines++;
  }
  fclose(fd);

  printf("\n};\n");
#endif

  exit(0);
}
