#ifndef DEBUG_H
#define DEBUG_H

// Categories, one bit each. Keep DBG_CATNAMES below in the same order.
#define DBG_PROC (1UL << 0) /* process lifecycle */
#define DBG_SCHED (1UL << 1) /* context switching */
#define DBG_SYSCALL (1UL << 2) /* system call entry/exit */
#define DBG_TRAP (1UL << 3) /* traps, interrupts, page faults */
#define DBG_VM (1UL << 4) /* virtual memory */
#define DBG_FS (1UL << 5) /* file system */
#define DBG_DEV (1UL << 6) /* drivers */
#define DBG_ALL (~0UL)

// Category names, indexed by bit number. This header is shared by the
// kernel and user programs so names and bits cannot drift apart.
#define DBG_CATNAMES { "proc", "sched", "syscall", "trap", "vm", "fs", "dev" }

// Levels, ordered by decreasing severity.
#define DBG_ERR 1
#define DBG_WARN 2
#define DBG_INFO 3
#define DBG_TRACE 4

// Level names, indexed by level number.
#define DBG_LEVELNAMES { "", "err", "warn", "info", "trace" }

extern unsigned long debug_mask;
extern int debug_level;

// Prefix helpers (kernel/debug.c).
char *dbg_catname(unsigned long cat, char *buf);
char *dbg_pidstr(char *buf);

#ifdef DEBUG
// Print only when cat is set in debug_mask and level is at or below
// debug_level. The arguments sit inside the if, so they are evaluated
// only when the message is printed. fmt must be a string literal,
// since it is pasted onto the prefix format.
#define dprintf(cat, level, fmt, ...)                                  \
  do {                                                                 \
    if ((debug_mask & (cat)) && (level) <= debug_level) {              \
      char _dbg_cat[16], _dbg_pid[16];                                 \
      printk("[%s][%s][%s] " fmt, dbg_catname((cat), _dbg_cat),        \
             dbg_pidstr(_dbg_pid), __func__, ##__VA_ARGS__);           \
    }                                                                  \
  } while (0)
#else
// Built with DEBUG=0: every call compiles to nothing.
#define dprintf(cat, level, fmt, ...) do { } while (0)
#endif

#endif /* DEBUG_H */
