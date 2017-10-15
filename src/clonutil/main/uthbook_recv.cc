
/* uthbook_test.cc */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


using namespace std;
#include <strstream>
#include <fstream>
#include <iomanip>

#include <string>
#include <iostream>

#include "uthbook.h"

#include "ipc_lib.h"
#include "MessageActionHist.h"

IpcServer &server = IpcServer::Instance();

int
main()
{
  int debug = 1;
  int done = 0;

  // connect to ipc server
  server.init(getenv("EXPID"), NULL, NULL, "*", NULL, "*");

  MessageActionHist *hist = new MessageActionHist((char *)"uthbook_recv",debug);
  server.addActionListener(hist);

  while(done==0)
  {
    sleep(1);
  }

  /*
  Hbook hbook;

  hbook.hbook1(1, "test1", 100, 0.0, 100.0);
  for(int i=0; i<100; i++)
  {
    hbook.hfill(1, ((float)i), 0.0, ((float)i)*2.);
  }
  hbook.hprint(1);
  */

}
