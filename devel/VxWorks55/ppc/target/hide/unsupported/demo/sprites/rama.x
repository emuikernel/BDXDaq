/* rama.x - RPCL for ramaServer */

/*
modification history
--------------------
01d,31may89,dab  added resetFlag to RAMA_REPLY.
01c,23apr88,gae  changed #define back to const -- funny?
01b,19apr88,gae  added modification history; changed 'const' to #define.
		 added DELETE_SPRITE; moved SPRITE definition to sprite.h.
01a,01jan88,rdc  written.
*/


enum RAMA_REQUEST_COMMAND
	{
	ADD_SPRITE,
	UPDATE_SPRITE,
	DELETE_SPRITE
	} ;

struct addSprite
    {
    int initialXPos;
    int initialYPos;
    int initialXVelocity;
    int initialYVelocity;
    } ;

struct updateSprite
    {
    int spriteId;
    int newXPos;
    int newYPos;
    int newXVelocity;
    int newYVelocity;
    } ;

struct deleteSprite
    {
    int spriteId;
    } ;

union RAMA_REQUEST switch (enum RAMA_REQUEST_COMMAND command)
    {
    case ADD_SPRITE:
	addSprite addSprite;
    case UPDATE_SPRITE:
	updateSprite updateSprite;
    case DELETE_SPRITE:
	deleteSprite deleteSprite;
    } ;

struct RAMA_REPLY
    {
    int status;
    int xVelocity;
    int yVelocity;
    int resetFlag;
    } ;

const RAMA = 123457;

