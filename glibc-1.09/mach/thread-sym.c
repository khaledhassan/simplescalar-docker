#include <mach.h>
#include <gnu-stabs.h>

function_alias (mach_setup_thread, __mach_setup_thread,
		kern_return_t, (task, thread, pc),
		mach_setup_thread (task_t task, thread_t thread, void *pc))
