
/* msg_to_epics_server_recv.cc */

#define USE_ACTIVEMQ

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


using namespace std;
#include <strstream>
#include <fstream>
#include <iomanip>

#include <string>
#include <iostream>



// for ca 
#include <casdef.h>
#include <fdManager.h>
#include <gdd.h>

//#include "epicsutil.h"
#include "ipc_lib_test.h"
#include "MessageActionEPICS.h"

static int npvs = 0;
char *pvNames[MAX_PVS];
int pvSizes[MAX_PVS];
static myPV *pPV[MAX_PVS];

IpcServer &server = IpcServer::Instance();

int
main()
{
  int debug = 1;
  int done = 0;

  printf("use IPC_HOST >%s<\n",getenv("IPC_HOST"));

  // connect to ipc server
  //server.init(getenv("EXPID"), NULL, NULL, "*", NULL, "*");
  server.init(NULL, NULL, NULL, "HallB_DAQ", NULL, "HallB_DAQ");

  MessageActionEPICS *epics = new MessageActionEPICS(npvs,pvNames,pvSizes,pPV);
  epics->set_debug(debug);
  server.addActionListener(epics);

  while(done==0)
  {
    sleep(1);
  }


}
