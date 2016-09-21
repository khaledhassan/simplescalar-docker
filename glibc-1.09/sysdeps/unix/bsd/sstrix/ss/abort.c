#include <ansidecl.h>
#include <sys/cdefs.h>

/* Cause an abnormal program termination with core-dump.  */
__NORETURN
void
DEFUN_VOID(abort)
{
  asm("break 0");
}

