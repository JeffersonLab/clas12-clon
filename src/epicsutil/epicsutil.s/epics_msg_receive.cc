/*
 epics_msg_receive.cc - receives ipc epics messages
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>


#define USE_ACTIVEMQ

// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__


#include "ipc_lib.h"
#include "MessageActionEPICS.h"


// misc
using namespace std;
#include <strstream>
#include <fstream>
#include <iostream>
#include <iomanip>


static int done = 0;

static IpcServer &server = IpcServer::Instance();

int
epics_msg_receiver_init(char *application)
{
  int status;
  int wait_time = 5;
  char *unique_id = (char*)"epics_msg_receiver";

  // set ipc parameters and connect to ipc system
  //ipc_set_application(application);
  /*
  ipc_set_user_status_poll_callback(status_poll_callback);
  ipc_set_control_message_callback(control_message_callback);
  ipc_set_quit_callback(quit_callback);
  */

  status = server.init(getenv("EXPID"), NULL, NULL, (char *)"epics_msg_recv", NULL, "*");
  if(status<0)
  {
    cerr << "\n?Unable to connect to server...probably duplicate unique id\n"
	 << "   ...check for another epics_msg_receiver  using ipc_info\n"
	 << "   ...only one connection allowed!" << endl << endl;
    exit(EXIT_FAILURE);
  }

  MessageActionEPICS  *epics = new MessageActionEPICS();
  server.addActionListener(epics);

  while(1)
  {
    sleep(1);
    if(done!=0) break;
  }

  /*
  // ref to IPC server (connection created later)
  TipcSrv &server=TipcSrv::Instance();
  TipcMt mt((T_STR)"epics_message");
  server.ProcessCbCreate(mt,epics_msg_callback,0);

  // get epics messages only
  server.SubjectSubscribe("/epics_msg",TRUE);

  server.Flush();
 
  while(1)
  {
    server.MainLoop((double)wait_time);
  }
  */

  server.close();

  return(0);
}
