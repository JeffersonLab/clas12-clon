//  get_run_status
//
//  gets run status for this session
//


// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__

#include <stdlib.h>

using namespace std;
#include <strstream>

// online and coda stuff
extern "C"{
#include "libdb.h"
}


//--------------------------------------------------------------------------

extern "C" {

char *
get_run_status(char *mysql_database, char *session)
{
  MYSQL *connNum;
  MYSQL_RES *result;
  MYSQL_ROW row_out;
  ostrstream query;
  

  // connect to mysql database
  connNum = dbConnect(getenv("MYSQL_HOST"), mysql_database);
  

  // form mysql query, execute, then close mysql connection
  query << "select log_name from sessions where name='"
	<< session << "'" << ends;
  mysql_query(connNum,query.str());
  result = mysql_store_result(connNum);
  row_out = mysql_fetch_row(result);
  

  // get run status 
  if(row_out[0]==NULL)
  {
    mysql_free_result(result);
    dbDisconnect(connNum);
    return(NULL);
  }
  else
  {
    mysql_free_result(result);
    dbDisconnect(connNum);

    return(row_out[0]);
  }

}

} // extern "C"


//--------------------------------------------------------------------------
