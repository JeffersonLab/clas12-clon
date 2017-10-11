
/* htcctest.cc */

#include <stdio.h>

#include "evio.h"
#include "evioBankUtil.h"

#include "htcclib.h"


static int handler; /* evio file handler */

int
main(int argc, char **argv)
{
  int status, iev;
  int batch = 0;

  printf("--> %d %s %s %s\n",argc,argv[0],argv[1],argv[2]);fflush(stdout);


  printf("argc=%d\n",argc);fflush(stdout);
  if(argc != 2 && argc != 3)
  {
    printf("Usage: trigview <evio_filename>\n");
    exit(1);
  }
  else if(argc==3 && !strcmp(argv[2],"batch"))
  {
    batch = 1;
    printf("running in batch mode\n");fflush(stdout);
  }

  status = evOpen(argv[1],"r",&handler);
  if(status < 0)
  {
    printf("evOpen error %d - exit\n",status);
    exit(0);
  }

  printf("running, data file >%s<\n",argv[1]);fflush(stdout);




  //if(batch)
  {
    for(iev=0; iev<12; iev++)
    {
      htcclib(handler);
	}
  }




  status = evClose(handler);

  exit(0);
}
