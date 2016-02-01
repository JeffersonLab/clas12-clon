
#define REMOTE_ET

//  ipcbank2et
//
//  inserts bos bank events into data stream 
//
//  ejw, 19-oct-00


// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__


#define EVIO_RECORD_HEADER(myptr, mylen) \
  myptr[0] = mylen + 8; /* Block Size including 8 words header + mylen event size  */ \
  myptr[1] = 1; /* block number */ \
  myptr[2] = 8; /* Header Length = 8 (EV_HDSIZ) */ \
  myptr[3] = 1; /* event count (1 in our case) */ \
  myptr[4] = 0; /* Reserved */ \
  myptr[5] = 0x204; /*evio version (10th bit indicates last block ?)*/ \
  myptr[6] = 1; /* Reserved */ \
  myptr[7] = 0xc0da0100; /* EV_MAGIC */



#define MAXBUF 100000
static int maxbufbytes = MAXBUF*4;;
static unsigned int buf[MAXBUF];
static char old_run_status[80];


#define EVENT_TYPE 31


/* following macros assume we have 'unsigned int *dabufp' set to the output buffer */

#define BANK_INIT \
  unsigned int *dabufp, *StartOfEvent, *StartOfFrag, *StartOfBank

#define EVENT_OPEN(btag) \
  StartOfEvent =  dabufp++; \
  *dabufp++ = 0x10CC | ((btag & 0xFF) << 16)

#define EVENT_CLOSE \
  *StartOfEvent = dabufp - StartOfEvent - 1

#define FRAG_OPEN(btag,btyp,bnum) \
  StartOfFrag = dabufp++; \
  *dabufp++ = ((btag)<<16) + ((btyp)<<8) + (bnum)

#define FRAG_CLOSE \
  /* writes bank length (in words) into the first word of the bank */ \
  *StartOfFrag = dabufp - StartOfFrag - 1

#define BANK_OPEN(btag,btyp,bnum) \
  StartOfBank = dabufp++; \
  *dabufp++ = ((btag)<<16) + ((btyp)<<8) + (bnum)

#define BANK_CLOSE \
  /* writes bank length (in words) into the first word of the bank */ \
  *StartOfBank = dabufp - StartOfBank - 1




// for smartsockets
#include <rtworks/cxxipc.hxx>


// CLAS ipc
#include <clas_ipc_prototypes.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// for i/o


using namespace std;
#include <iomanip>

//#include <ifstream.h>
#include <iostream>

/* BOS swap */
/*sergey: temporary
#include "/usr/local/clas/devel/coda/src/et/main/cinclude/bosRecordSwap.c"
*/

#include <evio.h>

// for et
#include <et.h>


// misc variables
static char *project       	 = (char*)"clastest";
static char *session   		 = (char*)NULL;
static char *host         	 = getenv("HOST");
static char *unique_name       	 = (char*)"ipcbank2et";
static char *unique_id           = (char *) malloc(64);
static int wait_time           	 = 5;
static char *database     	 = (char*)"clasrun";
static char *destination     	 = NULL;
static int done                	 = 0;
static int nev_rec           	 = 0;
static int nev_to_et           	 = 0;
static int nev_no_run            = 0;
static int nev_no_et           	 = 0;
static int run                   = 0;
static int debug                 = 0;
static int et_ok                 = 0;
static int lost_connection       = 0;
static time_t last_time          = time(NULL);
static int last_rec              = 0;
static int last_proc             = 0; 
static double rec_rate;
static double proc_rate;
static time_t now;
static char bank_source[132];
static char rcstate_file[128];
static char temp[256];

static char *run_status;
static int run_number = 0;
static int event_number = 0;

// et stuff
static et_sys_id et_system_id;
static et_openconfig openconfig;
static char et_filename[128];
static et_att_id et_attach_id;
static et_event *et_event_ptr;
static int et_control[ET_STATION_SELECT_INTS];
static unsigned int etbuffersize;


// other prototypes
void decode_command_line(int argc, char **argv);
void init_et(void);
void connect_et(void);
void bosbank_callback(T_IPC_CONN,
		      T_IPC_CONN_PROCESS_CB_DATA,
		      T_CB_ARG);
extern "C" {
void status_poll_callback(T_IPC_MSG msg);
void control_message_callback(T_IPC_CONN,
			      T_IPC_CONN_PROCESS_CB_DATA,
			      T_CB_ARG);
void quit_callback(int sig);
int get_run_number(char *database, char *session);
char *get_run_status(char *database, char *session);
int create_header(int *p, int evlen, int &banksize,
                  int name1, int name2, int nrun, int nevnt, int nphys, int trig);
int add_bank(int *p2ev, int evlen, 
      const char *name, int num, const char *format, int ncol, int nrow, int ndata, int &banksize, int *data);
int va_add_bank(int *p2ev, int evlen, 
      const char *name, int num, const char *format, int ncol, int nrow, int ndata, int &banksize, ...);
int insert_msg(const char *name, const char *facility, const char *process, const char *msgclass, 
	       int severity, const char *status, int code, const char *text);
}


// ref to IPC server (connection created later)
TipcSrv &server=TipcSrv::Instance();


//--------------------------------------------------------------------------

int
main(int argc,char **argv)
{
  int status;

  // synch with stdio
  ios::sync_with_stdio();

  // decode command line
  decode_command_line(argc,argv);

  // set session name
  if(session==NULL)session=(char*)"clasprod";

  //sergey: use project name as database name
  database = project;

  // create unique_id from uniq_name, node, and et system name
  char *p;
  strcpy(unique_id,unique_name);

  /*
  strcat(unique_id,"_");
  p = strtok(host,".");
  printf("p=>%s<\n",p);
  strcat(unique_id,p);
  */


  /*sergey: used to be _10_clasprod for clon10 and clasprod, do not need it since we have only one ipcbank2et
  strcat(unique_id,"_");
  p = host + strlen(host) - 2;
  strcat(unique_id,p);
  strcat(unique_id,"_");
  strcat(unique_id,session);
  */




  // set ipc parameters and connect to ipc system
  ipc_set_application(project);
  ipc_set_user_status_poll_callback(status_poll_callback);
  ipc_set_control_message_callback(control_message_callback);
  ipc_set_quit_callback(quit_callback);
  status = ipc_init(unique_id,"ipcbank2et");
  if(status<0)
  {
    cerr << "\n?Unable to connect to server...probably duplicate unique id\n"
	 << "   ...check for another ipcbank2et  using ipc_info\n"
	 << "   ...only one connection allowed!" << endl << endl;
    exit(EXIT_FAILURE);
  }
  server.SubjectSubscribe((T_STR)"evt_system",TRUE);
  TipcMt mt((T_STR)"evt_bosbank");
  server.ProcessCbCreate(mt,bosbank_callback,0);
  sprintf(bank_source,"evt_bosbank/%s",session);
  server.SubjectSubscribe(bank_source,TRUE);

  // initialize et system
  init_et();

  // get rcstate file name
  sprintf(rcstate_file,"%s/run_log/rcstate_%s.txt",getenv("CLON_PARMS"),session);

  // post startup message
  sprintf(temp,"Process startup:  %s  in project:  %s, session: %s",unique_id,project,session);
  status = insert_msg("ipcbank2et","online",unique_id,"status",0,"START",0,temp);

  // flush messages and output
  server.Flush();
  fflush(NULL);

  strcpy(old_run_status, get_run_status(database,session));
  printf("At startup time run state is '%s' (UNIX time=%d)\n",old_run_status,time(NULL));fflush(stdout);

  // main loop
  while(done==0)
  {
    server.MainLoop((double)wait_time);

    // check et
    if((et_ok==1)&&(et_alive(et_system_id)==0))
    {
      lost_connection=1;
      done=1;
    }

    // calc rates every 60 seconds...banks come very slowly!
    now = time(NULL);
    if((now-last_time)>60)
    {
      int delta=now-last_time;
      rec_rate=(double)(nev_rec-last_rec)/delta;
      proc_rate=(double)(nev_to_et-last_proc)/delta;
      last_time=now;
      last_rec=nev_rec;
      last_proc=nev_to_et;
    }

  }

  // shutdown messages
  if(lost_connection==1)
  {
    printf("lost connection to ET - exiting\n");
    insert_msg("ipcbank2et","online",unique_id,"status",1,"WARN",0,"ipcbank2et...lost connection to ET system");
  }
  sprintf(temp,"Process shutdown:  %s",unique_id);
  status = insert_msg("ipcbank2et","online",unique_id,"status",0,"STOP",0,temp);

  // done
  if(et_ok==1) et_forcedclose(et_system_id);
  ipc_close();
  exit(EXIT_SUCCESS);
}
       

//--------------------------------------------------------------------------


void
init_et()
{  
  char ch[256], *chptr;
  et_ok=0;

  // create et file name
  sprintf(et_filename,"/tmp/et_sys_%s",session);
  et_open_config_init(&openconfig);

#ifdef REMOTE_ET

  if(destination==NULL)
  {
    printf("destination host not specified, trying env var CLON_EB\n");
    chptr = getenv("CLON_EB");
    if(chptr==NULL)
    {
      printf("ERROR: env var 'CLON_EB' is not set - exit\n");
      exit(0);
    }
	else
	{
      destination = strdup(chptr);
      printf("CLON_EB set to '%s', use it as destination host\n",destination);
	}
  }
  else
  {
    printf("destination host specified as '%s'\n",destination);
  }


  et_open_config_sethost(openconfig, destination);
  et_open_config_gethost(openconfig, ch);

  printf("will use destination (remote) host '%s', et system '%s'\n",ch,et_filename);

  /* do not need it: Carl fixed problem
  et_open_config_setmode(openconfig, ET_DIRECT);
  */


#endif

  return;
}


//---------------------------------------------------------------------


void
connect_et()
{  
  int status;
  sigset_t sigblock;

  et_ok=0;

  printf("trying to connect to ET system\n");

  // open et system
  if(et_open(&et_system_id,et_filename,openconfig)!=ET_OK)
  {
    printf("ERROR: Cannot connect to ET - return\n");
    exit(0);/*return; - will exit assuming that process manager will start us soon and we'll try to connect again*/
  }

  // get max normal event size
  et_system_geteventsize(et_system_id,&etbuffersize);
  printf("INFO: event size = %d\n",etbuffersize);


  // block signals to THIS thread and any thread created by this thread
  // needed to keep signals from et threads
  sigfillset(&sigblock);
  pthread_sigmask(SIG_BLOCK,&sigblock,NULL);


  // attach to existing station
  status = et_station_attach(et_system_id,ET_GRANDCENTRAL,&et_attach_id);
  if(status!=ET_OK)
  {
    et_forcedclose(et_system_id);
    cerr << "Unable to attach to grandcentral station" << endl;
    done=1;
    return;
  }
  

  // unblock signals
  pthread_sigmask(SIG_UNBLOCK,&sigblock,NULL);


  // success
  et_ok=1; 
  cout << "...now connected to ET system: " << et_filename 
       << ",   station: grandcentral" << endl;


  return;
}


//---------------------------------------------------------------------


void
bosbank_callback(T_IPC_CONN conn,
		         T_IPC_CONN_PROCESS_CB_DATA data,
		         T_CB_ARG arg)
{  
  int status;
  size_t len, ii, jj;
  int *p,*pstart,i,nused,banksize,nhead,buflen;
  T_STR msgtype;
  T_STR bankname,bankformat;
  T_INT4 banknumber,ncol,nrow,nwrds,ndatawords;
  T_INT4* datawords;
  char line[128];
  
  BANK_INIT;

  TipcMsg msg(data->msg);

  // for record segment header
  int nevnt  = 0;
  int nphys  = 0;
  int trig   = 0;

  // constants for head bank
  int nvers  = 0;
  int type   = EVENT_TYPE;
  int rocst  = 0;
  int evcls  = 0;
  int presc  = 0;


  // total events received
  nev_rec++;

  // check et
  if(et_ok==0)
  {
    connect_et();
    if(et_ok==0)
    {
      nev_no_et++;
      return;
    }
  }
  else if(et_alive(et_system_id)==0)
  {
    nev_no_et++;
    lost_connection = 1;
    done=1;
    return;
  }

  /* sergey: get run state */
  run_status = get_run_status(database,session);

  if(strcasecmp(run_status,old_run_status)!=0)
  {
    printf("Run state changed from '%s' to '%s' (UNIX time=%d)\n",old_run_status,run_status,time(NULL));fflush(stdout);
    strcpy(old_run_status,run_status);
  }

  // no entry unless in 'active' state
  if(strncasecmp(run_status,"active",6)!=0)
  {
    if(debug) printf("bosbank_callback: session '%s' is in state '%s', state is not 'active' - set nev_no_run=%d and return\n",
      session,run_status,nev_no_run);
    nev_no_run++;
	return;
  }
  


  // get run number
  if(debug) printf("Geting run number from database >%s< session >%s<\n",database,session);
  run = get_run_number(database,session);
  if(debug) printf("Obtained run numbr %d\n",run);
  run_number = run;


  // get free event 
  status = et_event_new(et_system_id,et_attach_id,&et_event_ptr,ET_ASYNC,NULL,etbuffersize);
  if(status!=ET_OK)
  {
	printf("error in et_event_new - return\n");
    if(debug!=0) cerr << "?unable to get event, status is: " << status << endl;
    nev_no_et++;
    return;
  }


  // set control words
  for (int ii=0; ii<ET_STATION_SELECT_INTS; ii++) et_control[ii] = 0;
  et_control[0] = type;
  et_event_setcontrol(et_event_ptr,et_control,ET_STATION_SELECT_INTS);


  // set pointer, reset counts, etc.
  et_event_getdata(et_event_ptr,(void**)&p);
  pstart = p;
  nused=0;
  nhead=0;





  /**************************/
  /* creating evio fragment */

  int handle1;
  unsigned int *ptr;

  ptr = (unsigned int *)et_event_ptr->pdata;
  et_event_getlength(et_event_ptr, &len); /*get event length from et*/
  /*printf("len1=%d\n",len);*/


  status = evOpenBuffer((char *)pstart, MAXBUF, "w", &handle1);
  if(status!=0) printf("evOpenBuffer returns %d\n",status);


  pstart += 8; /* skip record header */
  dabufp = (unsigned int *)pstart;

  if(debug) printf("Open event\n");

  EVENT_OPEN(EVENT_TYPE);

  if(debug) printf("Open fragment tag=129\n");

  FRAG_OPEN(129,0xe,0); /* 'rocid' = 129 */

  /*
  // create segment header, then update pointer and counters
  status = create_header(p,etbuffersize-nused,nhead,'RUNP','ARMS',run,nevnt,nphys,trig);
  if(status==0)
  {
    p+=nhead;
    nused+=nhead;
  }  
  */

  if(debug) printf("Open bank tag=0xe112\n");

  BANK_OPEN(0xe112,1,0);

	/*
 // head bank (pstart' points to data area)
  status = va_add_bank(p,etbuffersize-nused,"HEAD",0,"I",8,1,8,banksize,
	      nvers,run,nevnt,(int)time(NULL),type,rocst,evcls,presc);
  if(status==0)
  {
    p+=banksize;
    nused+=banksize;
  }
  */

  int nwords = 5; /* UPDATE THAT IF THE NUMBER OF WORDS CHANGED BELOW !!! */
  int event_type = EVENT_TYPE;
  event_number ++;

  //
  // does not need headers - there are no rol2
  //
  //  *dabufp ++ = (0x12<<27)+(event_number&0x7FFFFFF); /*event header*/
  //  *dabufp ++ = (0x14<<27)+nwords; /*head data*/
  //

  *dabufp ++ = 0; /*version  number */
  *dabufp ++ = run_number; /*run  number */
  *dabufp ++ = event_number; /*event number */
  *dabufp ++ = time(0); /*event unix time */
  *dabufp ++ = event_type; /*event type */

  if(debug) printf("Close bank tag=0xe112\n");

  BANK_CLOSE;

  // extract banks from message and insert into event
  int fptr = 1;
  msg.Current(0);
  int banknum = 0;
  while(fptr<msg.NumFields())
  {
    msg >> bankname >> banknumber >> bankformat >> ncol >> nrow >> nwrds
		>> datawords >> GetSize(&ndatawords) /*>> Check(msg)*/;
    if((debug==1)&&(nwrds!=ndatawords))
    {
      cerr << "Bank data inconsistent...nwrds,ndatawords are: " << nwrds << ", " << ndatawords << endl;
    }

	/*
    status = add_bank(p,etbuffersize-nused,bankname,banknumber,bankformat,ncol,nrow,nwrds,banksize,
		    (int*)&datawords[0]);
    if(status==0)
    {
      p+=banksize;
      nused+=banksize;
    }
	*/

    if(debug) printf("Open bank tag=0xe114 num=%d\n",banknum);

    BANK_OPEN(0xe114,3,banknum);

    char *str = (char *)dabufp;
    char sss[1024];
    str[0] = '\n'; /* one 'cr' in the beginning to make evio2xml output looks better ... */
    str[1] = '\0';
    int nch;

    float *ptr;
    char *ch;
    ptr = (float *)datawords;
    ch = (char *)(ptr+1);

    for(ii=0; ii<nrow; ii++)
    {
      //printf("val=%f, name >",ptr[0]);

      sprintf(sss,"%16.6f %32.32s\n",ptr[0],(char *)&ptr[1]);
      strcat(str,sss);

      //for(jj=0; jj<(ncol-1)*4; jj++)
      //{
      //  printf("%c",ch[jj]);
      //}
      //printf("<\n");
      ptr += 9;
      ch = (char *)(ptr+1);

      //printf("in loop str >%s<\n",str);
    }

    //printf("final str >%s<\n",str);

    nch = strlen(str);
    str[nch+1] = '\n';
    str[nch+2] = '\n';
    str[nch+3] = '\n';
    dabufp += (nch+4)/4;

    //printf("banknum=%d\n",banknum);
    //printf("  ncol%d nrow=%d nwrds=%d\n",ncol,nrow,nwrds);

    if(debug) printf("Close bank tag=0xe114 num=%d\n",banknum);

    BANK_CLOSE;

    banknum ++;

    fptr+=7;
  }

  FRAG_CLOSE;

  EVENT_CLOSE;

  status = evWrite(handle1, (uint32_t *)pstart);
  if(status!=0) printf("evWrite returns %d\n",status);
  evGetBufferLength(handle1,&len);
  /*printf("len=%d\n",len);*/
  status = evClose(handle1);
  if(status!=0) printf("evClose returns %d\n",status);

  /* pstart[0] contains exclusive event length, add 1 for itself and 8 for record header */
  et_event_setlength(et_event_ptr,(pstart[0]+1+8)<<2);


#ifdef REMOTE_ET


  /*****************************************************************/
  /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
  /* sergey: we assume that local host is sparc and destination is */
  /* intel, so we'll swap BOS record and set ET endian equal to    */
  /* ET_ENDIAN_LITTLE=1; in future it must be done such way so it  */
  /* will check the necessity of swapping                          */
  /*****************************************************************/


  /****************** do it only if necessary !!!!!!!!!!!!!!!!!!!! 
  BOSrecordSwap((unsigned int *)pstart, (unsigned int *)pstart);

  {
    int endian;
    et_event_getendian(et_event_ptr,&endian);
    et_event_setendian(et_event_ptr,ET_ENDIAN_LITTLE);
    et_event_getendian(et_event_ptr,&endian);
  }
  */


#endif


  // insert event into ET system
  if(et_alive(et_system_id)==1)
  {
    status = et_event_put(et_system_id,et_attach_id,et_event_ptr);
    if(status==ET_OK)
    {
      nev_to_et++;
    }
    else
    {
      if(debug==1) cerr << "?unable to put event, status is: " << status << endl;
      nev_no_et++;
    }
  }

  return;
}

  
//----------------------------------------------------------------


void
status_poll_callback(T_IPC_MSG msg)
{  
  TipcMsgAppendStr(msg,(T_STR)"Bank source");
  TipcMsgAppendStr(msg,bank_source);

  TipcMsgAppendStr(msg,(T_STR)"nevent received");
  TipcMsgAppendInt4(msg,nev_rec);
  
  TipcMsgAppendStr(msg,(T_STR)"nevent sent to et");
  TipcMsgAppendInt4(msg,nev_to_et);
  
  TipcMsgAppendStr(msg,(T_STR)"nevent no et");
  TipcMsgAppendInt4(msg,nev_no_et);
  
  TipcMsgAppendStr(msg,(T_STR)"nevent no run");
  TipcMsgAppendInt4(msg,nev_no_run);
  
  TipcMsgAppendStr(msg,(T_STR)"wait_time");
  TipcMsgAppendInt4(msg,wait_time);

  return;
}


//-------------------------------------------------------------------


void
quit_callback(int sig)
{
  done=1;

  return;
}


//-------------------------------------------------------------------


void
control_message_callback(T_IPC_CONN conn,
			      T_IPC_CONN_PROCESS_CB_DATA data,
			      T_CB_ARG arg)
{

  T_STR string;


  // get first string
  TipcMsgSetCurrent(data->msg,0);
  TipcMsgNextStr(data->msg,&string);
  
  
  //  event transfer system status request
  if(strcasecmp(string,"evt_status_poll")==0) {

    T_STR srvnode = server.Node();

    TipcMsg status((T_STR)"evt_status");
    status.Dest((T_STR)"/evt_system/status");
    status.Sender(unique_id);
    status << unique_id << host << session << srvnode 
	   << (T_INT4) nev_rec << rec_rate << (T_INT4) nev_to_et << proc_rate
	   << (T_INT4) et_ok << (T_STR)"IPC->et:GRANDCENTRAL";
    server.Send(status,TRUE);
    server.Flush();


  //  don't understand message...ship to smartsockets interpreter
  } else {
    TutCommandParseStr(string);
  }

  return;
}


//----------------------------------------------------------------------


void
decode_command_line(int argc, char**argv)
{
  const char *help = "\nusage:\n\n  ipcbank2et [-a project] [-u unique_name] [-s session]\n"
    "             [-m database] [-d destination] [-w wait_time]\n"
    "             [-debug]\n";

  // loop over all arguments, except the 1st (which is program name)
  int i=1;
  while(i<argc) {
    if(strncasecmp(argv[i],"-h",2)==0){
      cout << help << endl;
      exit(EXIT_SUCCESS);
    }
    else if (strncasecmp(argv[i],"-debug",6)==0){
      debug=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-a",2)==0){
      project=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-u",2)==0){
      unique_name=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-s",2)==0){
      session=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-m",2)==0){
      database=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-d",2)==0){
      destination=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-w",2)==0){
      wait_time=atoi(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-",1)==0) {
      cout << "Unknown command line arg: " << argv[i] << argv[i+1] << endl << endl;
      i=i+1;
    }
  }

  return;
}
