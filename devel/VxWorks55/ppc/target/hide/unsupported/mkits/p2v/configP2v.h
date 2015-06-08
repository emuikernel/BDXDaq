
/* Library table limits */

#define P2V_MAX_FIX_Q 	30
#define P2V_MAX_VAR_Q 	30
#define P2V_MAX_SEM 	50
#define P2V_MAX_PART 	10
#define P2V_MAX_RGN 	10
#define P2V_MAX_TIMERS 	25

/* Add this to total stack size request in t_create to account for
 * vxWorks using a bit more stack space.
 */

#define P2V_STACK_BUMP 	2048

#define P2V_STRICT_MODE 0
