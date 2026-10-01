//
//  run_log_go
//
//  update run stat time only
//
// Usage: run_log_go -a clasrun (from rcscript)
//        run_log_go -a clasrun -s clasprod -debug (if recovering and debugging)
//
// from rcscripts: run_log_go -a clasrun >>& $CLON_LOG/run_log/run_log_go.log
//

// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__

#define USE_RCDB
#define USE_ACTIVEMQ

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

// system stuff

using namespace std;
#include <strstream>
#include <iostream>
#include <iomanip>
#include <fstream>



// for ipc
#include "ipc_lib.h"
#include "MessageActionControl.h"

#ifdef USE_RCDB

#include "RCDB/WritingConnection.h"

#include "json/json.hpp"
using json = nlohmann::json;

#define MAXLABELS 16
int nlabels; /* the number of lines in .cfg file */
char *labels[MAXLABELS];
char *dbnames[MAXLABELS];
char *values[MAXLABELS];
char *actions[MAXLABELS];

char vals[MAXLABELS][256];

#endif


// flags to inhibit event recording, etc.
static int no_dbr    = 0;
static int no_info   = 0;
static int no_file   = 0;
static int debug     = 0;
static int done      = 0;


// misc variables
static char *application      	 = (char*)"clastest";
static char *uniq_dgrp        	 = (char*)"run_log_go";
static char *id_string        	 = (char*)"run_log_go";
static char *dest             	 = (char*)"dbrouter";
static char *msql_database    	 = (char*)NULL;
static char expid[128]           = "";
static char session[128]         = "";
static int gmd_time           	 = 5;
static int filep              	 = 0;
static int force              	 = 0;
static int min_file_age          = 5;   // hours
static int wait_time           	 = 5;
static time_t now                = time(NULL);

static char *archive_file_name   = (char*)"run_log/archive/end_%s_%06d.txt";
static char *run_file_summary 	 = (char*)"run_files/runfile_%s_%06d.txt";
/*
static char *scaler_file_name    = (char*)"scalers/archive/scalers_%s_%06d.txt";
*/

static char line[1024];
static char filename[128];
static char temp[128];


#include "epicsutil.h"

#define MAXEPICS       300
#define MAXVECTOR      128
static char *db_name[MAXEPICS];
static char *epics_chan[MAXEPICS];
EPICS_CONFIG epics[MAXEPICS];

static int ncallback = 0;
static int nepics    = sizeof(epics_chan)/sizeof(char *);
static float epics_val[sizeof(epics_chan)/sizeof(char *)];


// end data
static int run_number;
static int nfiles;
static int nevents;
static int nevents_old = 100;
static int ndata;

static char config[128]   = "";
static char conffile[128] = "";
static char datafile[128] = "";
static char *configin     = NULL;
static char *conffilein   = NULL;
static char *datafilein   = NULL;

static char end_date[30];
static char location[80];
static long nfile  = 0;
static long nevent = 0;
//static long ndata  = 0;
static long nerror = 0;


// scalers
static unsigned long eor_scalers[64];
static unsigned long clock_all, clock_live;
static unsigned long fcup_all, fcup_live;
static unsigned long trig_event[6];


// prototypes
void decode_command_line(int argc, char **argv);
int epics_parse_config_file(char *, char *, int, EPICS_CONFIG *);
int collect_data(strstream &sql_string, int recover, char *recovery_filename);
void insert_into_ipc(char *sql);
int find_tag_line(ifstream &file, const char *tag, char buffer[], int buflen);
int get_next_line(ifstream &file, char buffer[], int buflen);


#include "codautil.h"


extern "C" {
  void get_run_config(char *msql_database, char *session, int *run,
                      char **config, char **conffile, char **datafile);
}


// program start time
static time_t start_time = time(NULL);


// ref to server (connection created later)
IpcServer &server = IpcServer::Instance();



//--------------------------------------------------------------------------

int
main(int argc,char **argv)
{
  int ret;

  // decode command line
  decode_command_line(argc,argv);

  // disable GMD timeout and connect to server
  if(no_dbr==0)
  {
    //T_OPTION opt=TutOptionLookup((T_STR)"Server_Delivery_Timeout");
    //TutOptionSetNum(opt,0.0);
  }

  //dbr_init(uniq_dgrp,application,id_string);

  server.AddSendTopic(getenv("EXPID"), getenv("SESSION"), (char *)"control", (char *)"run_log_go");
  server.AddRecvTopic(getenv("EXPID"), getenv("SESSION"), (char *)"control", (char *)"*");

  server.AddSendTopic(getenv("EXPID"), getenv("SESSION"), (char *)"runlog", (char *)"run_log_go");

  server.Open();

  MessageActionControl *control = new MessageActionControl((char *)"run_log_go");
  control->setDebug(debug);
  server.AddCallback(control);


  if(strlen(expid)==0) strcpy(expid,getenv("EXPID"));
  printf("Expid >%s<\n",expid);fflush(stdout);

  if(strlen(session)==0) strcpy(session,getenv("SESSION"));
  printf("Session >%s<\n",session);fflush(stdout);


  // get run number and config
  if(msql_database==NULL) msql_database = getenv("EXPID");
  printf("Use msql_database '%s'\n",msql_database);
  get_run_config(msql_database, session, &run_number, &configin, &conffilein, &datafilein);

  if(configin == NULL) strcpy(config,"No_configuration!");
  else                 strcpy(config,configin);

  if(conffilein == NULL) strcpy(conffile,"No_conffile!");
  else                   strcpy(conffile,conffilein);

  if(datafilein == NULL) strcpy(datafile,"No_datafile!");
  else                   strcpy(datafile,datafilein);

  printf("run_log_go: session >%s<, run %d, configuration >%s<\n",session,run_number,config);fflush(stdout);


  // collect data in normal mode (returns sql string)
  strstream sql_string;
  ret = collect_data(sql_string,0,NULL);


  // ship sql string to database router and/or info_server
  if(debug==0)
  {
    if(ret) insert_into_ipc(sql_string.str());
  }
    
  // debug...just print sql string
  if(debug!=0)
  {
    cout << "\nsql string for normal run " << run_number << " is:\n\n" << sql_string.str() << endl;
    if(ret==0) cout << "NOT SENDABLE !";
    else cout << "SENDABLE !";
    cout<<endl<<endl;
  }


  // allow gmd to acknowledge receipt and close connection
  //if(no_dbr==0) dbr_check((double) gmd_time);
  //dbr_close();
  server.Close();
  

  // done
  exit(EXIT_SUCCESS);
}
       

//----------------------------------------------------------------


#ifdef USE_RCDB

int
collect_data(strstream &sql, int recover, char *recovery_filename)
{
  int ret = 1;
  struct tm *run_start_time;
  long nevt, nlng, nerr;
  const char *comma = ",", *prime = "'";
  int i;


  /* get current time */
  run_start_time = localtime(&start_time);


  /* construct json manually */

  sql << "[{\"name\":\"run_log\",\"run_number\":"<<run_number<<"";

  sql << ",\"run_start_time\":\""<<StringUtils::GetFormattedTime(*run_start_time)<<"\"";
  sql << ",\"run_end_time\":\""<<StringUtils::GetFormattedTime(*run_start_time)<<"\"";

  sql <<"}]" << ends;

  

  // print sql string
  if(debug!=0)
  {
    cout << "\n ret="<<ret<<", sql for run " << run_number << " is:\n\n" << sql.str() << endl << endl;
  }

  return(ret);
}

#endif


//--------------------------------------------------------------------------

void
insert_into_ipc(char *sql)
{
  if(sql==NULL) return;

  // dbr message
  if(no_dbr==0)
  {
    server << clrm << "runlog" << (char *)sql << endm;
  }

  // flush messages
  /*server.Flush();*/


  return;
}



//----------------------------------------------------------------
  

void
decode_command_line(int argc, char **argv)
{
  int i=1;
  const char *help="\nusage:\n\n  run_log_go [-a application] [-u uniq_dgrp] [-i id_string] [-debug]\n"
               "        [-d destination] [-m msql_database]]\n"
               "        [-no_dbr] [-no_info] [-no_file] [-mf min_file_age]\n"
               "        [-s session] [-g gmd_time] [-w wait_time] file1 file2 ...\n\n\n";

  while(i<argc) {
    
    if(strncasecmp(argv[i],"-h",2)==0){
      printf(help);
      exit(EXIT_SUCCESS);
    }
    else if (strncasecmp(argv[i],"-",1)!=0){
      filep=i;
      return;
    }
    else if (strncasecmp(argv[i],"-debug",6)==0){
      debug=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-force",6)==0){
      force=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-no_dbr",7)==0){
      no_dbr=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-no_info",8)==0){
      no_info=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-no_file",8)==0){
      no_file=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-mf",3)==0){
      min_file_age=atoi(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-a",2)==0){
      application=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-u",2)==0){
      uniq_dgrp=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-i",2)==0){
      id_string=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-d",2)==0){
      dest=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-g",2)==0){
      gmd_time=atoi(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-s",2)==0){
      strcpy(session,argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-m",2)==0){
      msql_database=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-w",2)==0){
      wait_time=atoi(argv[i+1]);
      i=i+2;
    }
  }
}


//---------------------------------------------------------------------
