/***  for testing only ***/

#include <rtworks/ipc.h>
#include <clas_ipc.h>
#include <stdlib.h>

void myquitcallback(int sig)
{
  TutOut("myquitcallback called with signal %d\n",sig);
  ipc_close();
  exit(0);
}

void mystatuspollcallback(T_IPC_MSG msg)
{
  TipcMsgAppendStr(msg,"Current");
  TipcMsgAppendReal4(msg,50.0*((float)rand())/((float)RAND_MAX));
  TipcMsgAppendStr(msg,"Voltage");
  TipcMsgAppendReal4(msg,100.0*((float)rand())/((float)RAND_MAX));
  TipcMsgAppendStr(msg,"Calibration file");
  TipcMsgAppendStr(msg,"/clas/u2/data/calib.dat");
}


main()
{
  int err;
  char *name;
  T_IPC_MT mt;
  char *monitor_group;
  char *uniq_dgrp;
  T_OPTION opt;


  err=ipc_set_quit_callback(myquitcallback); 
  if(err!=0)ipc_perror(err);

  err=ipc_set_user_status_poll_callback(mystatuspollcallback);

  err=ipc_set_alarm_poll_time(1);
  if(err!=0)ipc_perror(err);

  err=ipc_init("","ipc system test");
  if(err!=0)ipc_perror(err);

  opt=TutOptionLookup("Unique_Datagroup");
  err=TutOptionGetEnum(opt,&uniq_dgrp);

  err=tclipc_init(NULL);

  err=ipc_get_application(&name);

  err=ipc_get_unique_datagroup(&name);

  err=ipc_get_status_poll_group(&monitor_group);


  for(;;){
    TutSleep(1000.0);
  }
}



