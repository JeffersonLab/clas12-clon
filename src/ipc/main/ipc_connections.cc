//
//  ipc_connections
//
//  lists ipc client connections
//
//
//     ipc_connections  [-a application]
//
//       default application     = "clastest"
//
//  ejw, 15-may-98




#define _POSIX_SOURCE 1
#define __EXTENSIONS__


#include <rtworks/cxxipc.hxx>
#include <stdio.h>
#include <clas_ipc_prototypes.h>

using namespace std;
#include <strstream>
#include <iostream>
#include <iomanip>


// misc vars
char *app  = "clastest";
char *help = "\nusage:\n\n   ipc_connections [-a application] \n\n\n";


// prototypes
void decode_command_line(int argc, char **argv);


//-------------------------------------------------------------------------


int main(int argc, char **argv) {

  T_STR *names;
  T_STR *info;
  T_STR *serv;
  T_INT4 nclient;

  
  // decode command line
  decode_command_line(argc,argv);
  

  // inhibit output
  //  TutSetOutputFunc(ipc_output_dummy);


  // read Smartsockets license file
  strstream s;
  s << getenv("RTHOME") << "/standard/license.cm" << ends;
  TutCommandParseFile(s.str());


  // set application
  T_OPTION opt=TutOptionLookup("Application");
  if(!TutOptionSetEnum(opt,app)){TutOut("?unable to set application\n");}


  // connect to server 
  TipcSrv &server=TipcSrv::InstanceCreate(T_IPC_SRV_CONN_FULL);


  // reenable output
  TutSetOutputFunc(NULL);


  // poll
  TipcMon mon;
  mon.ClientNamesPoll();


  // get result
  TipcMsg msg = server.Next(1.0);

  
  // unpack msg
  msg >> names >> GetSize(&nclient) >> info >> GetSize(&nclient) >> serv >> GetSize(&nclient);


  // dump results
  for(int i=0; i<nclient; i++) {
    *strchr(serv[i]+2,'_')='\0';
    cout << setw(45) << names[i]+1 << "  " << setw(15) << serv[i]+2 << endl;
  }


  exit(EXIT_SUCCESS);
}


//-------------------------------------------------------------------------


void decode_command_line(int argc, char **argv) {


  int i=1;
  while(i<argc) {
    if(strncasecmp(argv[i],"-h",2)==0) {
      TutOut("%s", help);
      exit(EXIT_SUCCESS);
    }
    else if (strncasecmp(argv[i],"-a",2)==0){
      app=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-",1)==0) {
      TutWarning("Unknown command line arg: %s\n\n",argv[i]);
      i=i+1;
    }
    else {
      TutOut("%s", help);
      exit(EXIT_SUCCESS);
    }
  }

  return;
}


//-------------------------------------------------------------------------


void dummy(T_STR str, va_list arg) {

  return;

}


//-------------------------------------------------------------------
