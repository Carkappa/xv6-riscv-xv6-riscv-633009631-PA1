#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"
#include "debug.h"

// Width of the longest category name ("syscall"), so prefixes line up.
#define DBG_NAMEWIDTH 7


unsigned long debug_mask = 0;
int debug_level = DBG_INFO;

static char *catnames[] = DBG_CATNAMES;

// Write the name of the lowest category bit set in cat into buf
// (at least 16 bytes), padded to DBG_NAMEWIDTH.
char *
dbg_catname(unsigned long cat, char *buf)
{
  char *name = "?";
  int i, n;

  for (i = 0; i < NELEM(catnames); i++) {
    if (cat & (1UL << i)) {
      name = catnames[i];
      break;
    }
  }
  for (n = 0; name[n]; n++)
    buf[n] = name[n];
  for (; n < DBG_NAMEWIDTH; n++)
    buf[n] = ' ';
  buf[n] = 0;
  return buf;
}

// Write the current pid into buf (at least 16 bytes), right-aligned
// to 2 columns, since printk has no field widths. Writes " -" when no
// process is running, e.g. during boot or in the scheduler.
char *
dbg_pidstr(char *buf)
{
  struct proc *p = myproc();
  char tmp[16];
  int pid, n = 0, i = 0;

  if (p == 0) {
    buf[0] = ' ';
    buf[1] = '-';
    buf[2] = 0;
    return buf;
  }

  pid = p->pid;
  do {
    tmp[n++] = '0' + pid % 10;
    pid /= 10;
  } while (pid > 0);
  while (n < 2)
    tmp[n++] = ' ';
  while (n > 0)
    buf[i++] = tmp[--n];
  buf[i] = 0;
  return buf;
}
