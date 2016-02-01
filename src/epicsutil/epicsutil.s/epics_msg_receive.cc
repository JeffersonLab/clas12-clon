/*
 epics_msg_receive.cc - receives ipc epics messages
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


static void
epics_msg_callback(T_IPC_CONN conn, T_IPC_CONN_PROCESS_CB_DATA data, T_CB_ARG arg)
{
  int status, ii;
  T_STR sender, host, user;
  T_INT4 time;
  T_STR caname, catype;
  T_INT4 nelem;
  T_INT4  d_int[MAX_ELEM];
  T_INT4 d_uint[MAX_ELEM];
  T_REAL4 d_float[MAX_ELEM];
  T_REAL8 d_double[MAX_ELEM];
  T_UCHAR d_uchar[MAX_ELEM];
  T_STR   d_string[MAX_ELEM];

  printf("\n\nepics_msg_callback reached\n");

  TipcMsg msg(data->msg);

  msg.Current(0);
  printf("numfields=%d\n",msg.NumFields());
  printf("destination >%s<\n",msg.Dest());
  printf("sender >%s<\n",msg.Sender());

  printf("Message:\n");

  msg >> sender >> host >> user >> time;
  printf("  Sender >%s<\n",sender);
  printf("  Host >%s<\n",host);
  printf("  User >%s<\n",user);
  printf("  Unixtime >%d<\n",time);

  msg >> caname >> catype >> nelem;

  printf("  caname >%s<\n",caname);
  printf("  catype >%s<\n",catype);
  printf("  nelem >%d<\n",nelem);
  if(nelem > MAX_ELEM)
  {
    printf("WARN: nelem > %d, will set nelem=%d\n",MAX_ELEM,MAX_ELEM);
    nelem = MAX_ELEM;
  }

  if( !strcmp(catype,"int"))         for(ii=0; ii<nelem; ii++) {msg >> d_int[ii];printf(" %d",d_int[ii]);}
  else if( !strcmp(catype,"uint"))   for(ii=0; ii<nelem; ii++) {msg >> d_uint[ii];printf(" %d",d_uint[ii]);}
  else if( !strcmp(catype,"float"))  for(ii=0; ii<nelem; ii++) {msg >> d_float[ii];printf(" %f",d_float[ii]);}
  else if( !strcmp(catype,"double")) for(ii=0; ii<nelem; ii++) {msg >> d_double[ii];printf(" %f",d_double[ii]);}
  else if( !strcmp(catype,"uchar"))  for(ii=0; ii<nelem; ii++) {msg >> d_uchar[ii];printf(" %d",d_uchar[ii]);}
  else if( !strcmp(catype,"string")) for(ii=0; ii<nelem; ii++) {msg >> d_string[ii];printf(" %s",d_string[ii]);}
  else
  {
    printf("epics_msg_receive: ERROR: unknown catype >%s<\n",catype);
    return;
  }
  printf("\n");

  return;
}



int
epics_msg_receiver_init(char *application)
{
  int status;
  int wait_time = 5;
  char *unique_id = (char*)"epics_msg_receiver";

  // set ipc parameters and connect to ipc system
  ipc_set_application(application);
  /*
  ipc_set_user_status_poll_callback(status_poll_callback);
  ipc_set_control_message_callback(control_message_callback);
  ipc_set_quit_callback(quit_callback);
  */
  status = ipc_init(unique_id,"epics_msg_receiver");
  if(status<0)
  {
    cerr << "\n?Unable to connect to server...probably duplicate unique id\n"
	 << "   ...check for another epics_msg_receiver  using ipc_info\n"
	 << "   ...only one connection allowed!" << endl << endl;
    exit(EXIT_FAILURE);
  }

  // ref to IPC server (connection created later)
  TipcSrv &server=TipcSrv::Instance();

  TipcMt mt((T_STR)"epics_message");
  server.ProcessCbCreate(mt,epics_msg_callback,0);

  /* get everything
  server.SubjectSubscribe("/...",TRUE);
  */

  /* get epics messages only */
  server.SubjectSubscribe("/epics_msg",TRUE);

  server.Flush();
 
  while(1/*done==0*/)
  {
    server.MainLoop((double)wait_time);
  }

  return(0);
}
