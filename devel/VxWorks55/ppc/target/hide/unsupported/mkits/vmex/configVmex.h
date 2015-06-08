
/* Library table limits */

#define VMEX_MAX_FIX_Q 	30
#define VMEX_MAX_VAR_Q 	30
#define VMEX_MAX_SEM 	50
#define VMEX_MAX_PART 	10
#define VMEX_MAX_RGN 	10
#define VMEX_MAX_TIMERS 25

/* Add this to total stack size request in t_create to account for
 * vxWorks using a bit more stack space.
 */

#define VMEX_STACK_BUMP 	2048

#define VMEX_STRICT_MODE 0
