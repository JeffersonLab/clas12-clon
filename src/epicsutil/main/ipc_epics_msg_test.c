/*
  ipc_epics_msg_test.c - test program for 'ipc_epics_msg' calls

to see all messages:
  java clonjava/ipc_monitor -a clastest0 -dump

*/

#define USE_ACTIVEMQ

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__


#include "epicsutil.h"


int
main(int argc, char **argv)
{
  int i;

  if(argc==1) /* sender */
  {
    int id[100];
    int status = 0;
    /*unsigned char dd[10] = {100,101,102,3,4,5,6,7,8,9};*/
    char *dd[10] = {"a120","a121","a122","a123","a124","a125","a126","a127","a128","a129"};


    printf("SENDER %s\n",__FILE__);
    epics_msg_sender_init(getenv("EXPID"), "bla"); /* SECOND ARG MUST BE UNIQUE !!! */
    //while(1)
	{
	  printf("2222222222222222222\n");fflush(stdout);
      /*status = epics_msg_send("channel_name","string",10,dd);*/
      /*id[0] = get_run_number(getenv("EXPID"),getenv("SESSION"));*/

      id[0] = get_run_number("clasrun","clasprod");
      status = epics_msg_send("hallb_run_number","int",1,id);

	 
      dd[0] = get_run_operators("clasrun","clasprod");
      status = epics_msg_send("hallb_run_operators","string",1,dd);
	 

      for(i=0; i<100; i++) id[i] = i;
      status = epics_msg_send("hallb_dsc2_hps2_slot2","uint",72,id);
	  

      sleep(1);
	}
  }
  else /* receiver */
  {
    printf("RECEIVER\n");
    epics_msg_receiver_init("clastest0"/*getenv("EXPID")*/);
  }

  exit(0);
}
