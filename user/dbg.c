#include "kernel/types.h"
#include "kernel/debug.h"
#include "user/user.h"


static char *catnames[] = DBG_CATNAMES;
static char *levelnames[] = DBG_LEVELNAMES;

#define NCAT   ((int)(sizeof(catnames) / sizeof(catnames[0])))
#define NLEVEL ((int)(sizeof(levelnames) / sizeof(levelnames[0])))

static void
usage(void)
{
  int i;

  fprintf(2, "usage:\n");
  fprintf(2, "  dbg                 print current mask and level\n");
  fprintf(2, "  dbg on <cat>...     enable categories\n");
  fprintf(2, "  dbg off <cat>...    disable categories\n");
  fprintf(2, "  dbg only <cat>...   set mask to exactly these\n");
  fprintf(2, "  dbg level <level>   set the level threshold\n");
  fprintf(2, "categories:");
  for (i = 0; i < NCAT; i++)
    fprintf(2, " %s", catnames[i]);
  fprintf(2, " all\n");
  fprintf(2, "levels:");
  for (i = DBG_ERR; i < NLEVEL; i++)
    fprintf(2, " %s", levelnames[i]);
  fprintf(2, "\n");
}

// Return the bit for a category name, or 0 if the name is unknown.
static uint64
catbit(const char *name)
{
  int i;

  if (strcmp(name, "all") == 0)
    return DBG_ALL;
  for (i = 0; i < NCAT; i++)
    if (strcmp(name, catnames[i]) == 0)
      return 1UL << i;
  return 0;
}

// Fold argv[first..argc-1] into one bit set. Returns -1 without
// setting *out if any name is unknown, so a bad argument leaves the
// kernel untouched.
static int
parsecats(int argc, char *argv[], int first, uint64 *out)
{
  uint64 bits = 0, b;
  int i;

  if (first >= argc) {
    fprintf(2, "dbg: expected at least one category\n");
    return -1;
  }
  for (i = first; i < argc; i++) {
    if ((b = catbit(argv[i])) == 0) {
      fprintf(2, "dbg: unknown category '%s'\n", argv[i]);
      return -1;
    }
    bits |= b;
  }
  *out = bits;
  return 0;
}

static void
show(void)
{
  uint64 mask = debugctl(DBGCTL_GETMASK, 0);
  uint64 level = debugctl(DBGCTL_GETLEVEL, 0);
  int i, n = 0;

  printf("mask  0x%lx (", mask);
  for (i = 0; i < NCAT; i++)
    if (mask & (1UL << i))
      printf("%s%s", n++ ? " " : "", catnames[i]);
  if (n == 0)
    printf("none");
  printf(")\n");

  if (level < (uint64)NLEVEL)
    printf("level %d (%s)\n", (int)level, levelnames[level]);
  else
    printf("level %d\n", (int)level);
}

int
main(int argc, char *argv[])
{
  uint64 mask, bits;
  int i, level;

  if (argc == 1) {
    show();
    exit(0);
  }

  if (strcmp(argv[1], "level") == 0) {
    if (argc != 3) {
      fprintf(2, "dbg: level takes exactly one argument\n");
      usage();
      exit(1);
    }
    level = -1;
    for (i = DBG_ERR; i < NLEVEL; i++)
      if (strcmp(argv[2], levelnames[i]) == 0)
        level = i;
    if (level < 0) {
      fprintf(2, "dbg: unknown level '%s'\n", argv[2]);
      usage();
      exit(1);
    }
    if (debugctl(DBGCTL_SETLEVEL, level) == (uint64)-1) {
      fprintf(2, "dbg: kernel rejected level %s\n", argv[2]);
      exit(1);
    }
    show();
    exit(0);
  }

  if (strcmp(argv[1], "on") != 0 && strcmp(argv[1], "off") != 0 &&
      strcmp(argv[1], "only") != 0) {
    fprintf(2, "dbg: unknown command '%s'\n", argv[1]);
    usage();
    exit(1);
  }

  if (parsecats(argc, argv, 2, &bits) < 0) {
    usage();
    exit(1);
  }

  mask = debugctl(DBGCTL_GETMASK, 0);
  if (strcmp(argv[1], "on") == 0)
    mask |= bits;
  else if (strcmp(argv[1], "off") == 0)
    mask &= ~bits;
  else
    mask = bits;

  debugctl(DBGCTL_SETMASK, mask);
  show();
  exit(0);
}