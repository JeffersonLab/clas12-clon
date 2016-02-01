//
//  run_log_begin
//
//  collects and inserts run log begin info into database, datastream, info_server,
//     and file
//
// Usage: run_log_begin -a clasprod (from rcscript)
//        run_log_begin -a clasprod -debug (debugging)
//        run_log_begin -a clasprod -s clasprod -debug 59908 (if recovering and debugging)
//        run_log_begin -a clasprod -s clasprod 59908 (if recovering)
//
//   still to do:
//       tagger trigger words
//
//  ejw, 21-sep-2000
// sergey 29-oct-2007 migrate from cdev to ca
// sergey may 2009 added recovery mode
//
//

// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

// system stuff

using namespace std;
#include <strstream>
#include <iostream>
#include <iomanip>
#include <fstream>



// for ipc
#include <rtworks/cxxipc.hxx>


// online and coda stuff
extern "C" {
#include <clas_ipc_prototypes.h>
}

#include "epicsutil.h"


// flags to inhibit event recording, etc.
static int no_dbr    = 0;
static int debug     = 0;


// control params
static char *uniq_subj           = (char*)"run_log_begin";
static char *id_string         	 = (char*)"run_log_begin";
static char *msql_database     	 = (char*)"clasrun";
static int gmd_time              = 3;
static int filep               	 = 0;

static char *application = (char*)"clastest";
static char expid[128]   = "";
static char session[128] = "";


//  run start data
int run;
static char config[128]   = "";
static char conffile[128] = "";
static char datafile[128] = "";
static char *configin     = NULL;
static char *conffilein   = NULL;
static char *datafilein   = NULL;

static char startdate[30];
static int prescale[6];





#define MAXEPICS 300
static char *db_name[MAXEPICS];
static char *epics_chan[MAXEPICS];
EPICS_CONFIG epics[MAXEPICS];




static int ncallback = 0;
static int nepics    = sizeof(epics_chan)/sizeof(char *);
static float epics_val[sizeof(epics_chan)/sizeof(char *)];




// misc
static char filename[256];
//static char line[4096];


// prototypes
void decode_command_line(int argc, char **argv);
void collect_data(void);
void create_sql(strstream &rlb_string);

void get_epics_data(void);
void insert_into_database(const char *entry);
char *env_name(const char *env, const char *name);

int find_tag_line(ifstream &file, const char *tag, char buffer[], int buflen);
int get_next_line(ifstream &file, char buffer[], int buflen);
int epics_parse_config_file(char *, char *, int, EPICS_CONFIG *);

char *name_only(char *x);
extern "C" {
  void get_run_config(const char *msql_database, const char *session, int *run,
                      char **config, char **conffile, char **datafile);
  char *get_ts_name(char *mysql_database, char *session);
  int tcpClientCmd(char *roc, char *cmd, char *buf);
}


// program start time
static time_t start=time(NULL);


// ipc connection
TipcSrv &server=TipcSrv::Instance();


//--------------------------------------------------------------------------

int
main(int argc,char **argv)
{
  strstream rlb_string, rlr_string;
  int ret;


  // decode command line
  decode_command_line(argc,argv);


  if(filep==0)  // normal mode; set session name if not specified via env variable or on command line
  {
    if(strlen(expid)==0) strcpy(expid,getenv("EXPID"));
    printf("Expid >%s<\n",expid);fflush(stdout);

    if(strlen(session)==0) strcpy(session,getenv("SESSION"));
    printf("Session >%s<\n",session);fflush(stdout);

    // collect data
    collect_data();

    // create sql strings
    create_sql(rlb_string);

    // make entries
    if(debug==0)
    {

      // connect to server
      dbr_init(uniq_subj,application,id_string);

      // ship to database router
      insert_into_database(rlb_string.str());

      // close ipc connection
      dbr_close();

    }
    else
    {
      // just print sql strings
      cout << "\nrlb for run " << run << " is:\n\n" << rlb_string.str() << endl << endl;
    }
  }
  else  // recovery mode; no ipc or file in recovery mode
  {
    printf("RECOVERY MODE - NOT IMPLEMENTED YET !!!\n");
  }

printf("0\n");fflush(stdout);


  // done
  exit(EXIT_SUCCESS);

}
       

//----------------------------------------------------------------


void
collect_data(void)
{
  int i,ind,flen,ret;
  char *p;
  char line[20000];
  char need[128];
  char *ts_name = NULL;

  // get all epics data

  get_epics_data();

  // get run number and config
  get_run_config(msql_database, session, &run, &configin, &conffilein, &datafilein);

  if(strlen(session)==0) strcpy(session,"No_session!");

  if(configin == NULL) strcpy(config,"No_configuration!");
  else                 strcpy(config,configin);

  if(conffilein == NULL) strcpy(conffile,"No_conffile!");
  else                   strcpy(conffile,conffilein);

  if(datafilein == NULL) strcpy(datafile,"No_datafile!");
  else                   strcpy(datafile,datafilein);

  printf("run_log_begin: session >%s< configuration >%s<\n",session,config);fflush(stdout);

  // form start date
  tm *tstruct = localtime(&start);

  strftime(startdate,25,"%Y-%m-%d %H:%M:%S",tstruct);
  
  ts_name = get_ts_name(msql_database, session);

  printf("run_log_begin: ts_name >%s<\n",ts_name);

  /* get TS data */
  ret = tcpClientCmd((char*)ts_name,(char*)"tiUploadAllPrint",(char*)line);
  if(ret<0)
  {
    printf("run_log_begin: tcpClientCmd returns %d - skip clastrig2 request\n",ret);
  }
  else
  {
    /*printf(">%s<\n",line);*/

    for(i=0; i<6; i++)
	{
      sprintf(need,"TI_INPUT_PRESCALE %d",i+1);
      p = strstr(line,need);
      if(p)
	  {
        prescale[i] = atoi((char *)&p[strlen(need)]);
	  }
	  else
	  {
        prescale[i] = -1;
	  }
      printf("prescale[%d] = %d\n",i,prescale[i]);
	}
  }
 
  return;
}



/*************************************************************************/













/* get epics channels in according to list provided */

void
get_epics_data()
{
  int i, result;
  char fname[256];

  printf("get_epics_data reached\n");
  
  for(i=0; i<MAXEPICS; i++)
  {
    epics_val[i]=-9999.0;
  }

  /* get channels list from config file */
  sprintf(fname,"run_log/%s/%s_log_end.cfg",expid,session);
  printf("run_log_begin: fname >%s<\n",fname);
  nepics = epics_parse_config_file(getenv("CLON_PARMS"), fname, MAXEPICS, epics);

  printf("get_epics_data: nepics = %d\n",nepics);
  for(i=0; i<nepics; i++)
  {
    printf("get_epics_data: [%d] str >%s< >%s< >%s<\n",i,epics[i].name,epics[i].chan,epics[i].get);
    epics_chan[i] = epics[i].chan;
    db_name[i] = epics[i].name;
  }
	
  result = getepics(nepics, epics_chan, epics_val);

  return;
}




/* create sql request */

void
create_sql(strstream &rlb)
{
  int i;
  char tablename[256];
  const char *comma = ",", *prime = "'";

  sprintf(tablename,"%s_%s_log_begin",expid,session);

  // run_log_begin
  rlb << "insert into " << tablename << " ("
      << "run_number,start_date,coda_config,"
      << "trig_config,data_file";

  // prescale registers
  for (i=0; i<sizeof(prescale)/sizeof(int); i++)
  {
    rlb << comma << "prescale_" << i+1;
  }
  
  // epics data
  for (i=0; i<nepics; i++)
  {
    rlb << comma << db_name[i];
  }

  rlb << ") values ("
      << run   	  
      << comma	<< prime << startdate  << prime
      << comma	<< prime << config     << prime
      << comma	<< prime << conffile   << prime
      << comma	<< prime << datafile   << prime;

  for (i=0; i<sizeof(prescale)/sizeof(int); i++)
  {
    rlb << comma << prescale[i];
  }

  for (i=0; i<nepics; i++)
  {
    rlb << comma << epics_val[i];
  }

  rlb << ")" << ends;  

  return;
}


  

/* send message to 'dbrouter' */

void
insert_into_database(const char *entry)
{
  if(no_dbr==0)
  {
    printf("run_log_begin: QUERY >%s<\n",entry);

    // disable gmd timeout
    T_OPTION opt = TutOptionLookup((T_STR)"Server_Delivery_Timeout");
    TutOptionSetNum(opt,0.0);

    TipcMsg dbr = TipcMsg((T_STR)"dbr_request");
    dbr.Dest((T_STR)"dbrouter");
    dbr.Sender(uniq_subj);
    dbr.UserProp(0.0);
    dbr.DeliveryMode(T_IPC_DELIVERY_ALL);
    dbr << (T_INT4) 1 << (T_STR)entry;

    // send and flush, wait for gmd
    server.Send(dbr);
    server.Flush();
    dbr_check((double) gmd_time);
  }

  return;
}


  




/***************************************************************************/

void
decode_command_line(int argc, char **argv)
{
  int i=1;
  const char *help="\nusage:\n\n  run_log_begin [-a application] [-u uniq_subj] [-i id_string]\n"
       "              [-debug] [-m msql_database]  [-s session] [-no_dbr]\n"
       "              [-g gmd_time] file1 file2 ... \n\n\n";

  while(i<argc)
  {    
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
    else if (strncasecmp(argv[i],"-no_dbr",7)==0){
      no_dbr=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-a",2)==0){
      application=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-u",2)==0){
      uniq_subj=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-i",2)==0){
      id_string=strdup(argv[i+1]);
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
  }
}




char *
env_name(const char *env, const char *name)
{
  static char bigname[200];
  char *e=getenv(env);

  if(e!=NULL) {
    strcpy(bigname,e);
    strcat(bigname,"/");
    strcat(bigname,name);
  } else {
    strcpy(bigname,name);
  }
  
  return(bigname);
}




char *
name_only(char *fullname)
{
  char *p;

  p=strstr(fullname,"/parms/");
  if(p!=NULL) {
    return(p+7);
  } else {
    return(fullname);
  }
}

