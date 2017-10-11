
/* uthbook_test.cc */

#include <stdio.h>

#include "uthbook.h"

int
main()
{
  Hbook hbook;

  hbook.hbook1(1, "test1", 100, 0.0, 100.0);
  for(int i=0; i<100; i++)
  {
    hbook.hfill(1, ((float)i), 0.0, ((float)i)*2.);
  }
  hbook.hprint(1);

}
