
/* msg_to_epics_server_send.cc */

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

#include "json/json.hpp"
using json = nlohmann::json;

#include "uthbook.h"

#include "ipc_lib_test.h"
IpcServer &server = IpcServer::Instance();


int
main()
{
  strstream message;

  printf("use IPC_HOST >%s<\n",getenv("IPC_HOST"));

  // connect to ipc server
  //server.init(getenv("EXPID"), NULL, NULL, "*", NULL, "*");
  server.init(NULL, NULL, NULL, "HallB_TALK", NULL, "HallB_TALK");

  int iarray[5] = {6,7,8,9,0};
  float farray[5] = {1.,2.,3.,4.,5.};

  json j0 = {
	        {"EventRate",77766}
  };

  json j1 = {
            {"TestScalers", iarray}
  };

  json j2 = {
            {"TestVals", farray}
  };


  json j3;
  j3.push_back(j0);
  j3.push_back(j1);
  j3.push_back(j2);

  message << "{\"items\": " << j3.dump() << "}" <<ends;

  cout << "will send >" << message.str() << "<" << endl;

  server << clrm << message.str() << endm;

}
