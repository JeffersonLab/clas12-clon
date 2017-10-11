//  clas_epics_server

// CLAS portable ca server cross-posts assorted DAQ/Online info 
//   from ipc and files as epics channels 

//  ejw, 3-jul-2003



// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__


#define USE_ACTIVEMQ


// for ca 
#include <casdef.h>
#include <fdManager.h>
#include <gdd.h>

void setDebug(int val);


#include "ipc_lib.h"
#include "MessageActionControl.h"
#include "MessageActionEPICS.h"


// misc
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#ifdef SunOS
#include <thread.h>
#endif

using namespace std;
#include <iostream>
#include <fstream>
#include <iomanip>



char *pvNames[MAX_PVS];
aitEnum pvTypes[MAX_PVS];
int pvSizes[MAX_PVS];

//class myPV;
static int npvs = 0;
static myPV *pPV[MAX_PVS];


// misc variables
static char *application       	    = (char*)"clastest0";
static char *unique_id         	    = (char*)"clas_epics_server";
static char *clon_parms             = getenv("CLON_PARMS");
static int ca_pend_time             = 5;
static int data_sleep_time          = 30;
static int archive_sleep_time       = 60;
static char *config_file            = (char*)"epics/clas_epics_server.cfg";
static char *archive_file           = (char*)"epics/clas_epics_server.dat";
static int ipc_pend_time            = 5;
static int callback_count           = 0;
static int done                	    = 0;
static int debug                    = 0;
static int run_number               = 0;
static char temp[4096];
static char filename[256];
static char line[1024];



// other prototypes
void decode_command_line(int argc, char **argv);
void read_archive();
void read_pv_info(ifstream &i, int *a, int len);
int find_tag_line(ifstream &file, const char *tag, char buffer[], int buflen);
int get_next_line(ifstream &file, char buffer[], int buflen);

extern "C" {
  //void *ipc_thread(void *param);
void *archive_thread(void *param);
void quit_callback(int sig);
  //void status_poll_callback(T_IPC_MSG msg);
}


// ref to IPC server (connection created later)
IpcServer &server = IpcServer::Instance();

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------


class myServer : public caServer {
  

public:
    
  pvExistReturn pvExistTest(const casCtx &ctx, const char *pPVName)
  {
    
    int PVLen = strcspn(pPVName,".");

    for(int i=0; i<npvs; i++)
    {
      if((PVLen==strlen(pvNames[i]))&&(strncasecmp(pPVName,pvNames[i],PVLen)==0))
	 return pverExistsHere;
    }
    return pverDoesNotExistHere;
  }


  pvCreateReturn createPV(const casCtx &ctx, const char *pPVName)
  //pvAttachReturn attachPV(const casCtx &ctx, const char *pPVName)
  {
    if(debug!=0)cout << "createPV for " << pPVName << endl;

    int PVNameLen      = strlen(pPVName);
    int PVLen          = strcspn(pPVName,"."); /* length before '.' (removing trailing VAL etc) */
    const char *pattr  = pPVName+PVLen+1;
    int lattr          = PVNameLen-PVLen-1;

    for(int i=0; i<npvs; i++)
    {
	  if((PVLen==strlen(pvNames[i]))&&(strncasecmp(pPVName,pvNames[i],PVLen)==0))
      {
	    if( (PVNameLen==PVLen) || ((lattr==3)&&(strncasecmp(pattr,"VAL",3)==0)) )
        {
          //printf("createPV returns pPV[%d]\n",i);
		  return(*pPV[i]);
	    }
        else
        {
          printf("ERROR: createPV returns myAttrPV[%d] !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n",i);
          printf("ERROR: createPV returns myAttrPV[%d] !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n",i);
          printf("ERROR: createPV returns myAttrPV[%d] !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n",i);
          exit(0);
	    }
	  }
    }
    printf("createPV returns S_casApp_pvNotFound\n");
    return(S_casApp_pvNotFound);
  }


  ~myServer()
  {
    if(debug!=0)cout << "myServer destructor" << endl;
    return;
  }
    
};


#define STRLEN    250       /* length of str_tmp */
#define ROCLEN     80       /* length of ROC_name */


//---------------------------------------------------------------------------
// fills npvs, pvNames[npvs], pvTypes[npvs], pvSizes[npvs]
int
read_config_file()
{
  FILE *fd;
  int   ii, jj, ch, kk = 0;
  char  str_tmp[STRLEN];
  char  pvname[ROCLEN], pvtype[ROCLEN], pvsize[ROCLEN];
  int   i1, i2, i3, i4;
  int   slot, slot1, slot2, chan;
  char fname[STRLEN];

  sprintf(fname,"%s/%s",clon_parms,config_file);

  if((fd=fopen(fname,"r")) == NULL)
  {
    printf("\nread_config_file: Can't open config file >%s<\n",fname);
    return(-1);
  }

  printf("\nread_config_file: Using configuration file >%s<\n",fname);

  npvs = 0;
  while ((ch = getc(fd)) != EOF)
  {
    if ( ch == '#' || ch == ' ' || ch == '\t' )
    {
      while (getc(fd) != '\n') {}
    }
    else if( ch == '\n' ) {}
    else
    {
      ungetc(ch,fd);
      fgets(str_tmp, STRLEN, fd);
      sscanf (str_tmp, "%s %s %s", pvname, pvtype, pvsize);

      pvNames[npvs] = strdup(pvname);

      if(!strcmp(pvtype,"char"))        pvTypes[npvs] = aitEnumInt8;
      else if(!strcmp(pvtype,"uchar"))  pvTypes[npvs] = aitEnumUint8;
      else if(!strcmp(pvtype,"short"))  pvTypes[npvs] = aitEnumInt16;
      else if(!strcmp(pvtype,"ushort")) pvTypes[npvs] = aitEnumUint16;
      else if(!strcmp(pvtype,"int"))    pvTypes[npvs] = aitEnumInt32;
      else if(!strcmp(pvtype,"uint"))   pvTypes[npvs] = aitEnumUint32;
      else if(!strcmp(pvtype,"float"))  pvTypes[npvs] = aitEnumFloat32;
      else if(!strcmp(pvtype,"double")) pvTypes[npvs] = aitEnumFloat64;
      else if(!strcmp(pvtype,"string")) pvTypes[npvs] = aitEnumString; /*aitEnumFixedString ?*/
      else
	  {
        printf("\nread_config_file: unknown pvtype >%s<\n",pvtype);
        return(-2);
	  }

      pvSizes[npvs] = atoi(pvsize);

      printf("\nread_config_file: [%2d] name >%s< type=%d size=%d\n",npvs,pvNames[npvs],pvTypes[npvs],pvSizes[npvs]);

      npvs ++;
	}
  }

  fclose(fd);


  return(0);
}


//---------------------------------------------------------------------------

int
main(int argc,char **argv)
{
  int status, i;
  pthread_t t1,t2,t3;


  // synch with stdio
  ios::sync_with_stdio();


  // decode command line...
  decode_command_line(argc,argv);


  status = server.init(getenv("EXPID"), NULL, NULL, (char *)"epics_server", NULL, "*");
  if(status<0) {
    cerr << "\n?Unable to connect to server...probably duplicate unique id\n"
	 << "   ...check for another epics_server  using ipc_info\n"
	 << "   ...only one connection allowed!" << endl << endl;
    exit(EXIT_FAILURE);
  }

  // set ipc parameters and connect to ipc system
  //if(!TipcInitThreads())
  //{
  //  cerr << "Unable to init IPC thread package" << endl;
  //  exit(EXIT_FAILURE);
  // }
  //ipc_set_application(application);
  //ipc_set_user_status_poll_callback(status_poll_callback);
  //ipc_set_quit_callback(quit_callback);
  //status = ipc_init(unique_id,"Epics Server");
  /*sergey: callback to accept epics messages from epics_msg_send */
  //server.SubjectSubscribe((T_STR)"/epics_msg",TRUE);
  //server.SubjectCbCreate((T_STR)"/epics_msg",NULL,epics_msg_callback,NULL);


  // set epics server debug flag
  setDebug(debug);

  read_config_file();
  if(npvs<=0)
  {
    printf("No PVs - exit\n");
    exit(0);
  }

  // read pv alarm info from archive file
  read_archive();



  // create array of pv objects
  printf("Create %d pPV's\n",npvs);
  for(int i=0; i<npvs; i++)
  {
    pPV[i] = new myPV(pvNames[i],pvTypes[i],pvSizes[i]);
  }



  // create ca server
  myServer *cas = new myServer();

  // launch threads
  MessageActionControl *control = new MessageActionControl((char *)"epics_server");
  server.addActionListener(control);

  MessageActionEPICS *epics = new MessageActionEPICS(npvs,pvNames,pvSizes,pPV);
  epics->set_debug(debug);
  server.addActionListener(epics);
  //pthread_create(&t2,NULL,ipc_thread,(void*)NULL);

  pthread_create(&t3,NULL,archive_thread,(void*)NULL);


  // post startup message
  sprintf(temp,"Process startup:    clas_epics_server starting in %s",application);
  status = insert_msg("clas_epics_server","online",unique_id,"status",0,"START",0,temp);

  // flush output to log files, etc
  fflush(NULL);


  /*******************/
  /*  ca server loop */
  while(done==0)
  {
    fileDescriptorManager.process((double)ca_pend_time); // need it
    done = control->getDone();
  }
  /*******************/
  /*******************/



  // post shutdown message
  sprintf(temp,"Process shutdown:  clas_epics_server");
  status = insert_msg("clas_epics_server","online",unique_id,"status",0,"STOP",0,temp);

  // done...clean up
  server.close();
  //ipc_close();


  exit(EXIT_SUCCESS);
}
       



//--------------------------------------------------------------------------


void
read_pv_info(ifstream &i, int *a, int len)
{
  for(int j=0; j<len; j++) i >> a[j];
}

//--------------------------------------------------------------------------



//  reads archive file and fills pvXXXX arrays prior to pv initialization
void
read_archive()
{

  int len;

  sprintf(temp,"%s/%s",clon_parms,archive_file);
  if(debug!=0)cout << "reading archive file " << temp << endl;
  ifstream file(temp);
  if(!file.is_open())
  {
    cerr << "?Unable to open archive file: " << temp << endl;
    return;
  }
    
  /*
  file >> len;
  if(len!=sizeof(pvNames)/sizeof(char*))
  {
    cerr << "?Unable to read archive, lengths disagree, program/archive are: " 
	   << sizeof(pvNames)/sizeof(char*) << "/" << len << endl;
    file.close();
    return;
  }
  */

  /*
  int tmp[40];
  read_pv_info(file,tmp,sizeof(pvNames)/sizeof(char*)); 
  read_pv_info(file,tmp,sizeof(pvNames)/sizeof(char*)); 
  read_pv_info(file,tmp,sizeof(pvNames)/sizeof(char*)); 
  read_pv_info(file,tmp,sizeof(pvNames)/sizeof(char*)); 
  read_pv_info(file,tmp,sizeof(pvNames)/sizeof(char*)); 
  read_pv_info(file,tmp,sizeof(pvNames)/sizeof(char*)); 
  read_pv_info(file,tmp,sizeof(pvNames)/sizeof(char*)); 
  read_pv_info(file,tmp,sizeof(pvNames)/sizeof(char*)); 
  read_pv_info(file,tmp,sizeof(pvNames)/sizeof(char*)); 
  */

  file.close();
}



//--------------------------------------------------------------------------


//  archives epics alarm data
void *
archive_thread(void *param)
{
  int i;

  while(done==0)
  {
    sleep(archive_sleep_time);

    sprintf(temp,"%s/%s",clon_parms,archive_file);
    if(debug!=0)cout << "archiving to " << temp << endl;
    ofstream file(temp);

    file << npvs << endl;
    for(i=0; i<npvs; i++)
	{
      //file<<pPV[i]->myAlarm<<" "<<pPV[i]->myHIHI<<" "<<pPV[i]->myLOLO<<" "<<pPV[i]->myHIGH<<" "<<pPV[i]->myLOW<<" ";
      //file<<pPV[i]->myHOPR<<" "<<pPV[i]->myLOPR<<" "<<pPV[i]->myDRVH<<" "<<pPV[i]->myDRVL<<" ";

      file << endl;    
	}

    file.close();
  }  

  return((void*)NULL);
}


//--------------------------------------------------------------------------

#ifndef USE_ACTIVEMQ

//  receives online info from other programs and dispatches
//  to appropriate callbacks
void *
ipc_thread(void *param)
{
  while(done==0)
  {
    server.MainLoop((double)ipc_pend_time);
  }  
  return((void*)NULL);
}


//------------------------------------------------------------------


void
status_poll_callback(T_IPC_MSG msg)
{  
  TipcMsgAppendStr(msg,(char*)"number of epics channels served");
  TipcMsgAppendInt4(msg,npvs);
  
  TipcMsgAppendStr(msg,(char*)"number of smartsockets callbacks");
  TipcMsgAppendInt4(msg,callback_count);

  return;
}

#endif

//-------------------------------------------------------------------


void
quit_callback(int sig)
{
  done=1;

  return;
}


//--------------------------------------------------------------------------


void
decode_command_line(int argc, char**argv)
{
  const char *help = "\nusage:\n\n clas_epics_server [-a application] [-u unique_id]\n"
    "              [-ca ca_pend_time] [-ipc ipc_pend_time] [-data data_sleep_time]\n"
    "              [-archive archive_sleep_time] [-debug]\n";


  // loop over all arguments, except the 1st (which is program name)
  int i=1;
  while(i<argc)
  {
    if(strncasecmp(argv[i],"-h",2)==0)
    {
      cout << help << endl;
      exit(EXIT_SUCCESS);
    }
    else if (strncasecmp(argv[i],"-debug",6)==0)
    {
      debug=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-ca",3)==0)
    {
      ca_pend_time=atoi(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-ipc",4)==0)
    {
      ipc_pend_time=atoi(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-data",5)==0)
    {
      data_sleep_time=atoi(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-archive",8)==0)
    {
      archive_sleep_time=atoi(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-a",2)==0)
    {
      application=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-u",2)==0)
    {
      unique_id=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-",1)==0)
    {
      cout << "Unknown command line arg: " << argv[i] << argv[i+1] << endl << endl;
      i=i+1;
    }
  }

  return;
}
