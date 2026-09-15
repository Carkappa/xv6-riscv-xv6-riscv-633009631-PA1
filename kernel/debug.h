#ifndef DEBUG_H
#define DEBUG_H

#define DBG_PROC (1UL << 0) /* process lifecycle */
#define DBG_SCHED (1UL << 1) /* context switching */
#define DBG_SYSCALL (1UL << 2) /* system call entry/exit */
#define DBG_TRAP (1UL << 3) /* traps, interrupts, page faults */
#define DBG_VM (1UL << 4) /* virtual memory */
#define DBG_FS (1UL << 5) /* file system */
#define DBG_DEV (1UL << 6) /* drivers */
#define DBG_ALL (~0UL)
extern unsigned long debug_mask;


#define DBG_ERR 1
#define DBG_WARN 2
#define DBG_INFO 3
#define DBG_TRACE 4
extern int debug_level;

#endif /* DEBUG_H */


