
/* coda_scripts.c - inserts or removes scripts from CODA configuration */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>

#define MY_CHECK \
  printf("queryString >%s<\n",queryString); \
  if (mysql_query(mysql, queryString) != 0) {printf ("Insert error: %s\n", mysql_error(mysql));exit(1);}

static MYSQL* mysql = NULL;           /* connection socket to the database */
static char  config[128];            /* current configuration name        */

int
main(int argc, char *argv[])
{
  char queryString[1024];
  char action[10];
 
  if(argc != 2 && argc != 3)
  {
    printf("Usage: coda_scripts <config_name> [insert/remove]\n");
    exit(0);
  }
  strcpy (config, argv[1]);
  printf(" coda_scripts: database >daq_%s< config >%s<\n",getenv("EXPID"),config);
  if(argc == 3)
  {
    strncpy(action, argv[2],9);
    printf(" coda_scripts: action >%s<\n",action);
    if( strcmp(action,"insert") && strcmp(action,"remove") )
	{
      printf("Usage: coda_scripts <config_name> [insert/remove]\n");
      exit(0);
	}
  }


  mysql = dbConnect("clondb1", getenv("EXPID"));


  if(!strcmp(action,"insert"))
  {
    sprintf (queryString, "delete from `%s_option` WHERE CONVERT(`%s_option`.`name` USING utf8) = 'Prestart'\n",config,config); MY_CHECK;
    sprintf(queryString, "insert into %s_option ( `name` , `value` ) VALUES ( 'Prestart', '$CODA/common/rcscripts/run_dispatch prestart end main' )\n",config); MY_CHECK;
	
    sprintf (queryString, "delete from `%s_option` WHERE CONVERT(`%s_option`.`name` USING utf8) = 'End'\n",config,config); MY_CHECK;
    sprintf(queryString, "insert into %s_option ( `name` , `value` ) VALUES ( 'End', '$CODA/common/rcscripts/run_dispatch end end main' )\n",config); MY_CHECK;
	
    sprintf (queryString, "delete from `%s_script` WHERE CONVERT(`%s_script`.`name` USING utf8) = 'ERDAQ6'\n",config,config); MY_CHECK;
    sprintf(queryString, "insert into %s_script ( `name` , `state` , `script` ) VALUES ( 'ERDAQ6', 'prestart', '$CODA/common/rcscripts/run_dispatch prestart er main' )\n",config); MY_CHECK;
  }
  
  if(!strcmp(action,"remove"))
  {
    sprintf (queryString, "delete from `%s_option` WHERE CONVERT(`%s_option`.`name` USING utf8) = 'Prestart'\n",config,config); MY_CHECK;
	
    sprintf (queryString, "delete from `%s_option` WHERE CONVERT(`%s_option`.`name` USING utf8) = 'End'\n",config,config); MY_CHECK;
	
    sprintf (queryString, "delete from `%s_script` WHERE CONVERT(`%s_script`.`name` USING utf8) = 'ERDAQ6'\n",config,config); MY_CHECK;
  }

  
  dbDisconnect(mysql);

  exit(0);
}
