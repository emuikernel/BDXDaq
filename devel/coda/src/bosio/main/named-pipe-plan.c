/*
 * named-pipe-plan.c  - an example of using a named pipe, in order to make
 *                      the finger daemon tell whoever fingers our account,
 *                      how many times we were fingered recently.
 *                      uses a named pipe instead of a regular '.plan' file.
 */

#include <stdio.h>
#include <unistd.h>

#define PLAN_FILE "/home/choo/.plan"	/* full path to my '.plan' file */

void main()
{
    FILE* plan;
    int count = 0;


    /* run an infinite loop of opening the named pipe, writing */
    /* our message into it, and closing it.                    */
    while (1) {
        /* open the '.plan' file. This blocks until someone,       */
        /* (such as the finger daemon) opens the file for reading. */
        plan = fopen(PLAN_FILE, "w");
        if (!plan) {
	    perror("fopen");
	    exit(1);
        }

	count++;
	fprintf(plan, "I have been fingered %d times today\n", count);

        /* close the named pipe. This will cause the reader to get an EOF */
	fclose(plan);

	/* suspend execution, give the reader process enough time to */
        /* read the message, get the EOF sign, and close the pipe.   */
        /* otherwise, we might get to re-open the pipe before the    */
	/* reader closes it, and thus write several messages to the  */
	/* same reader.                                              */
	sleep(1);
    }
}
