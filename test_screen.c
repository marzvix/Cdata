/* -------- test screen --------------------------------- */

#include <errno.h>
#include "cdata.h"
#include "screen.h"

void (*database_message)(void);

void test_msg(void)
{
  puts("Testing screen...");  
}

void main(void)
{
  int i;
  void *vp;
  clear_screen();
  cursor(0, 0);
  init_screen("Teste", vp, vp);
  for (i=1; i<8; i++)    {
    errno = i;
    if (database_message)
      (*database_message)();
    else
      puts("db msg not defined");
  }
}
