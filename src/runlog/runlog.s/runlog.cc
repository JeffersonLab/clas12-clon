
/* runlog.cc - IPC and other utils */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

using namespace std;
#include <strstream>
#include <iomanip>
#include <fstream>
#include <string>
#include <iostream>

#include "ipc.h"

int library_initialized_counter = 0; /* see comments in ipc_lib.h */

#include "ipc_lib.h"
static IpcServer &server = IpcServer::Instance();

#include "runlog.h"

#define STRLEN 128

int
runlogMsgInit(const char *expid_, const char *session_, const char *unique_id_, const char *topic_)
{
  int status = 0;
  pthread_t t1;
  strstream temp;
  char expid[STRLEN+1];
  char session[STRLEN+1];
  char unique_id[STRLEN+1];
  char topic[STRLEN+1];

  printf("use IPC_HOST >%s<\n",getenv("IPC_HOST"));

  if(expid_==NULL || strlen(expid_)==0)
  {
    strcpy(expid,(char*)getenv("EXPID"));
  }
  else
  {
    strncpy(expid,expid_,STRLEN);
  }

  if(session_==NULL || strlen(session_)==0)
  {
    strcpy(session,(char*)getenv("SESSION"));
  }
  else
  {
    strncpy(session,session_,STRLEN);
  }

  if(unique_id_==NULL || strlen(unique_id_)==0)
  {
    strcpy(unique_id,(char*)"runlogMsg");
  }
  else
  {
    strncpy(unique_id,unique_id_,STRLEN);
  }

  if(topic_==NULL || strlen(topic_)==0)
  {
    strcpy(topic,(char*)"HallB_DAQ");
  }
  else
  {
    strncpy(topic,topic_,STRLEN);
  }

  printf("runlogMsgInit: expid >%s<, session >%s<, unique_id >%s<, topic >%s<\n",expid,session,unique_id,topic);

  server.AddSendTopic(expid, session, (char*)"control", topic);
  server.AddRecvTopic(expid, session, (char*)"control", (char*)"*");

  server.AddSendTopic(expid, session, unique_id, topic);

  status = server.Open();
  if(status<0)
  {
    printf("runlogMsgInit: unable to connect to ipc server on host %s\n",getenv("IPC_HOST"));
    return(-1);
  }

  return(0);
}

int
runlogMsgClose()
{
  server.Close();

  return(0);
}

int
runlogMsgSend(const char *msg)
{
  server << clrm << "runlog" << (char *)msg << endm;

  return(0);
}
