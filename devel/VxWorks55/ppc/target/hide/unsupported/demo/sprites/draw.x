/* draw.x - RPCL for drawServer */

/*
modification history
--------------------
01d,31may89,dab  added resetFlag to DRAW_REPLY.
01c,23apr88,gae  changed #define back to const -- funny?
01b,19apr88,gae  added modification history; changed 'const' to #define.
01a,01jan88,rdc  written.
*/

enum DRAW_TYPE 
    {
    DR_GET_WINDOW_SIZE,
    DR_UPDATE,
    DR_INITIAL,
    DR_FINAL
    } ;

struct DRAW_UPDATE    
    {
    int oldx;
    int oldy;
    int newx;
    int newy;
    DRAW_TYPE drawType;
    } ;

struct DRAW_REPLY
    {
    int width;
    int height;
    int resetFlag;
    } ;

const DRAWSRVR = 12345;
const DRAWSRVRVERS = 0;

