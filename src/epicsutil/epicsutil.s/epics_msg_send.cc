/*
 ipc_epics_msg_send.cc - sends ipc epics message 
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__

// for smartsockets
#include <rtworks/cxxipc.hxx>


// CLAS ipc
#include <clas_ipc_prototypes.h>
#include "epicsutil.h"

// misc
using namespace std;
#include <strstream>
#include <fstream>
#include <iostream>
#include <iomanip>


int
epics_msg_sender_init(char *application, char *unique_id)
{
  int status = 0;
  pthread_t t1;
  strstream temp;

  // synch with stdio
  ios::sync_with_stdio();

  /*
  // set ipc parameters and connect to ipc system
  if(!TipcInitThreads())
  {
    cerr << "Unable to init IPC thread package" << endl;
    exit(EXIT_FAILURE);
  }
  */

  ipc_set_application(application);
  /*
  ipc_set_user_status_poll_callback(status_poll_callback);
  ipc_set_quit_callback(quit_callback);
  */

  if(strlen(unique_id)==0) unique_id = (char*)"epics_msg_sender";

  printf("epics_msg_sender_init: unique_id set to >%s<\n",unique_id);

  status = ipc_init(unique_id,"epics_msg_sender");
  if(status<0)
  {
    cerr << "\n?Unable to connect to server...probably duplicate unique id\n"
	 << "   ...check for another epics_msg_sender  using ipc_info\n"
	 << "   ...only one connection allowed!" << endl << endl;
    return(EXIT_FAILURE);
  }

  // post startup message
  temp << "Process startup: ipc_epics_msg starting in " << application << ends;
  status = insert_msg("ipc_epics_msg","online",unique_id,"status",0,"START",0,temp.str());
  

  // flush output to log files, etc
  fflush(NULL);

  return(status);
}



int
epics_msg_send(const char *caname, const char *catype, int nelem, void *data)
{
  int ii;
  T_STR  domain   = (char*) "epics_msg_send";
  T_STR  user     = getenv("USER");
  T_INT4 msgtime = time(NULL);
  T_STR  host     = getenv("HOST");
  

  /* params check */
  if(strlen(host)==0)
  {
    printf("epics_msg_send: ERROR: host undefined\n");
    return(-1);
  }

  if(caname==NULL)
  {
    printf("epics_msg_send: ERROR: caname undefined\n");
    return(-1);
  }

  if(catype==NULL)
  {
    printf("epics_msg_send: ERROR: catype undefined\n");
    return(-1);
  }


  // get ref to server
  TipcSrv &server=TipcSrv::Instance();

  // form and send message
  TipcMsg msg((char*)"epics");

  msg.Dest((char*)"epics_msg"); /* receiver will subscribe for that with '/' in front of it !!! */
  msg.Sender((char*)caname); 

  /* standard section */
  msg << (T_STR)  domain
      << (T_STR)  host
      << (T_STR)  user
      << (T_INT4) msgtime;

	/* epics section */
  msg << (T_STR)  caname
      << (T_STR)  catype
      << (T_INT4) nelem;

  if( !strcmp(catype,"int"))         for(ii=0; ii<nelem; ii++) msg << (T_INT4)((int *)data)[ii];
  else if( !strcmp(catype,"uint"))   for(ii=0; ii<nelem; ii++) msg << (T_INT4)((int *)data)[ii];
  else if( !strcmp(catype,"float"))  for(ii=0; ii<nelem; ii++) msg << (T_REAL4)((float *)data)[ii];
  else if( !strcmp(catype,"double")) for(ii=0; ii<nelem; ii++) msg << (T_REAL8)((double *)data)[ii];
  else if( !strcmp(catype,"uchar"))  for(ii=0; ii<nelem; ii++) msg << (T_UCHAR)((char *)data)[ii];
  else if( !strcmp(catype,"string")) for(ii=0; ii<nelem; ii++) msg << (T_STR)((char **)data)[ii];
  else
  {
    printf("epics_msg_send: ERROR: unknown catype >%s<\n",catype);
    return(-1);
  }

  server.Send(msg);
  server.Flush();

  return(0);
}
