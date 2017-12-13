/*
  epics_parse_config_file.cc
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__

/*
// for smartsockets
#include <rtworks/cxxipc.hxx>
// CLAS ipc
#include <clas_ipc_prototypes.h>
*/

/*#include "clonutil.h"*/
#include "epicsutil.h"

// misc
using namespace std;
#include <strstream>
#include <fstream>
#include <iostream>
#include <iomanip>


int find_tag_line(ifstream &file, const char *tag, char buffer[], int buflen);
int get_next_line(ifstream &file, char buffer[], int buflen);

int
epics_parse_config_file(char *clon_parms, char *epics_config_name, int max_epics, EPICS_CONFIG *epics)
{
  int nepics;
  char buffer[256];
  char temp[256];
  char *p1,*p2;
  strstream fn; 

  printf("epics_parse_config_file REACHED\n");

  /* open channel config file */
  fn << clon_parms << "/" << epics_config_name << ends; 
  ifstream file(fn.str());
  if(!file.is_open())
  {
    cerr << "?unable to open " << fn << endl;
    exit(EXIT_FAILURE);
  }
  else
  {
    cout << "epics_parse_config_file: Use config file " << fn.str() << endl;
  }
  
  // find beginning of channel list
  if(find_tag_line(file,"*CHANNELS*",buffer,sizeof(buffer))!=0)
  {
    cerr << "Can't find *CHANNELS* tag in " << fn.str() << endl;
    exit(EXIT_FAILURE);
  }

  /* read in all channels */
  nepics = 0;
  while (get_next_line(file,buffer,sizeof(buffer))==0)
  {
    nepics ++;

    p1=strchr(buffer,'"')+1;  p2=strchr(p1,'"');  
    strncpy(temp,p1,p2-p1);   temp[p2-p1]=NULL;
    epics[nepics-1].name = strdup(temp);

    p1=strchr(p2+1,'"')+1;    p2=strchr(p1,'"');  
    strncpy(temp,p1,p2-p1);   temp[p2-p1]=NULL;
    epics[nepics-1].chan = strdup(temp);

    p1=strchr(p2+1,'"')+1;    p2=strchr(p1,'"');  
    strncpy(temp,p1,p2-p1);   temp[p2-p1]=NULL;
    epics[nepics-1].get = strdup(temp);

    printf("epics_parse_config_file: nepics = %d, str >%s< >%s< >%s<\n",nepics,epics[nepics-1].name,epics[nepics-1].chan,epics[nepics-1].get);

    if(nepics >= max_epics)
    {
      cerr << "Too many epics channels in " << fn << ", excess ignored" << endl;
      break;
    }
  }

  return(nepics);
}
