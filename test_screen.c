/* -------- test screen --------------------------------- */
#include <errno.h>
#include <ncurses.h>
#include "cdata.h"
#include "screen.h"

void (*database_message)(void);

void test_msg(void)
{
  puts("Testing screen...");  
}

int main(void)
{
  int i;
  void *vp;

  init_screen();

  for (i=1; i<8; i++)    {
    errno = i;
    if (database_message)
      (*database_message)();
    else
      puts("db msg not defined");
  }

  end_screen();
  return (0);
}
