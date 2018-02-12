
/* comment.cc - run command 'run_log_comment -fix' */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int
main()
{
  char temp[1024];

  sprintf(temp,"(echo \"Fixing last run comment\";run_log_comment -fix )&");
  /*printf("Executing >%s<\n",temp);*/
  system(temp);
}
