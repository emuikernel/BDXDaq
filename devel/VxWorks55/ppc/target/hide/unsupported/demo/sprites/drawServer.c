/* drawServer.c - VxWorks/UNIX RPC demonstration draw server */

#ifndef	LINT
static char *copyright = "Copyright 1988-1991, Wind River Systems, Inc.";
#endif	/* LINT */

/*
modification history
--------------------
01i,31oct91,rrr  passed through the ansification filter
		  -fixed #else and #endif
		  -changed VOID to void
		  -changed copyright notice
01h,07aug91,gae  removed sprite.h; cleanup; added UDP/TCP compile options.
01g,25oct90,dnw  fixed forward declarations.
01f,31may89,dab  can now delete sprites from host.
		 changed rama transport from udp to tcp.
            rdc  fixed erroneous accessing of sockaddr structure.
01e,10may89,gae  checked for macro svc_fds for SunOS3.5 backward compatibility.
01d,14apr89,gae  used new RPC conventions (fd_set's); extra debugging.
		   used rpc error reporting routines; lint.
01c,04jun88,gae  removed alternate sprites (face and box).
01b,12apr88,gae  rewritten to use sunview stuff better.
01a,10feb88,rdc  written.
*/

/*
DESCRIPTION
These programs demonstrate the use of Remote Procedure Calls on
VxWorks and UNIX.  The programs implement a set of "sprites", or
"balls", which bounce around in a suntools window.  The demo is
comprised of three distinct entities which communicate with each other
using RPC.

(1) xdrawServer/drawServer - run from the UNIX workstation in a X11 or
Sunview window.  It erases and draws "sprites" in the window upon
receipt of draw requests from ramaServer.  Must started before any of
the corresponding VxWorks programs.

	 % drawServer &
or
	% xdrawServer &

(2) ramaServer - run on VxWorks or UNIX system.  It keeps track of all
active sprites.  Any time a sprite update's its position, it informs
ramaServer; it checks to see if the sprite has collided with any other
sprite or with a wall, and if so, informs the sprite.  ramaServer
redraws the sprite by making requests to drawServer.

	 -> ld < sprites.o
	 -> sp ramaServer, "drawServerHostName"
     or
	 % ramaServer ["drawServerHostName"] &

where drawServerHostName is the hostname of the UNIX system where the
drawServer is running.  The hostname is optional for UNIX version, it
defaults to the same host.  On VxWorks the UNIX host may be added
with:

             -> hostAdd "drawServerHostName", "90.3"

(3) simpleSprite - run on VxWorks or UNIX system.  Any number of
simpleSprite's may be spawned on any VxWorks or UNIX system on the
net- work.  The routine moves a sprite in drawServer's window by
periodi- cally updating its position with ramaServer.

	 -> sp simpleSprite, "ramaHostName", xVel, yVel
     or
	 % simpleSprite ["ramaHostName"] xVel yVel

where ramaHostName is the hostname of the VxWorks or UNIX system where
ramaServer is running.  Using "localhost" is fine, otherwise on
VxWorks use:
             -> hostAdd "ramaHostName", "90.50"

xVel is the initial X velocity of the sprite, and yVel is the initial
Y velocity of the sprite.  Reasonable values range between -20 and 20.
*/

#include <stdio.h>
#include <suntool/sunview.h>
#include <suntool/canvas.h>
#include <suntool/panel.h>
#include <rpc/rpc.h>
#include <errno.h>
#include <signal.h>

#include "vxWorks.h"
#include "rama.h"
#include "draw.h"

#ifndef	svc_fds
#define	SUNOS35		/* is SunOS 3.5 */
#endif	/* svc_fds */

LOCAL short icon_image[] = {
#include "draw.icon"
};
DEFINE_ICON_FROM_IMAGE(draw_icon, icon_image);

LOCAL short ball_dat[] = {
#include "ball.pr"
};
mpr_static(ball,     16, 16, 1, ball_dat);


#define	XOR	(PIX_SRC^PIX_DST)

#define draw_sprite(x, y) 	\
    pw_rop(pixwin, x, y, 16, 16, XOR, &ball, 0, 0)


extern int errno;
extern Notify_error notify_dispatch ();

/*
TCP version by default
#define	UDP_DRAW_SERVER
#define	UDP_RAMA_SERVER
*/

LOCAL Pixwin *pixwin;		/* screen pixwin */
LOCAL Panel_item MainPanel;
LOCAL Panel_item QuitButton;
LOCAL Panel_item ResetButton;
LOCAL Pixfont *small_font;

LOCAL int Xsize;
LOCAL int Ysize;
LOCAL BOOL Quit;
LOCAL BOOL Reset;
LOCAL char progname [100];
LOCAL int spriteCount = 0;

/* forward declarations */

LOCAL void drawServer ();

LOCAL void winResize ();
LOCAL void winChange ();
LOCAL Notify_value my_notice_destroy ();
LOCAL void svc_stuff ();
LOCAL void redrawSprite ();
LOCAL void svc_doit ();


/*******************************************************************************
*
* main - maintain Sunview "draw" window and accept RPC update requests
*/

main (argc, argv)
    int argc;
    char **argv;

    {
    Canvas MainCanvas;
    Frame frame;

    strncpy (progname, argv [0], sizeof (progname));

    frame = window_create (NULL, FRAME,
			   FRAME_LABEL, argv[0],
			   FRAME_ARGC_PTR_ARGV, &argc, argv,
			   FRAME_ICON, &draw_icon,
			   WIN_ERROR_MSG, "Can't create frame",
			   0);


    /* get width of the frame to scale panel items by */
    Xsize = ((Rect *) window_get (frame, WIN_RECT))->r_width;
    Ysize = ((Rect *) window_get (frame, WIN_RECT))->r_height;


    MainPanel = window_create (frame, PANEL,
				PANEL_LABEL_BOLD, TRUE,
				0);

    QuitButton = panel_create_item (MainPanel, PANEL_BUTTON,
				PANEL_LABEL_IMAGE,
				 panel_button_image (MainPanel, "Quit", 5, 0),
				PANEL_CHOICE_STRINGS, "", 0,
				PANEL_NOTIFY_PROC, winChange,
				PANEL_TOGGLE_VALUE, 0, FALSE,
				0);

    ResetButton = panel_create_item (MainPanel, PANEL_BUTTON,
				PANEL_LABEL_IMAGE,
				 panel_button_image (MainPanel, "Reset", 5, 0),
				PANEL_CHOICE_STRINGS, "", 0,
				PANEL_NOTIFY_PROC, winChange,
				PANEL_TOGGLE_VALUE, 0, FALSE,
				0);

    window_fit_height (MainPanel); /* squeeze everything together */

    MainCanvas =
    window_create (frame, CANVAS,
	CANVAS_WIDTH,		Xsize,
	CANVAS_HEIGHT,		Ysize,
	CANVAS_RESIZE_PROC,	winResize,
	WIN_ERROR_MSG, "Can't create canvas",
	0);

    if (MainCanvas == NULL)
	{
	fprintf (stderr, "%s: couldn't create canvas\n", progname);
	exit (0);
	}

    pixwin = canvas_pixwin (MainCanvas);

    if (pixwin == NULL)
	{
	fprintf (stderr, "%s: couldn't create pixwin\n", progname);
	exit (0);
	}


    if (!(small_font = pf_default()))
	{
	perror("can't open default font");
	exit (1);
	}


    (void) notify_interpose_destroy_func (frame, my_notice_destroy);

    /* initialize defaults */

    Quit = FALSE;
    Reset = FALSE;

    /* initialize graphics */

    window_set (frame, WIN_SHOW, TRUE, 0);

    pw_use_fast_monochrome (pixwin);
    pw_writebackground (pixwin, 0, 0, Xsize, Ysize, (PIX_SRC));

    svc_stuff ();
    }
/*******************************************************************************
*
* svc_stuff - startup RPC service
*/

LOCAL void svc_stuff ()

    {
    struct sockaddr_in addr;
    SVCXPRT *xprt;
    bool_t status;

    /* create a tcp/udp transport to receive requests */

#ifdef	UDP_DRAW_SERVER
    if ((xprt = svcudp_create (RPC_ANYSOCK, 0, 0)) == (SVCXPRT *)NULL)
#else
    if ((xprt = svctcp_create (RPC_ANYSOCK, 0, 0)) == (SVCXPRT *)NULL)
#endif	/* UDP_DRAW_SERVER */
	{
	fprintf (stderr, "%s: tcp_create failed\n", progname);
	exit (1);
	}

    /* un-register any previous incarnations in the local portmap daemon */

    pmap_unset (DRAWSRVR, DRAWSRVRVERS);

    /* register the service with the transport we created */

    status = svc_register (xprt, DRAWSRVR, DRAWSRVRVERS, drawServer,
#ifdef	UDP_DRAW_SERVER
			    IPPROTO_UDP);
#else
			    IPPROTO_TCP);
#endif	/* UDP_DRAW_SERVER */

    if (status == FALSE)
	{
	fprintf (stderr, "%s: svc_register failed, errno=%x\n",
		progname, errno);
	exit (-1);
	}

    get_myaddress (&addr);
    printf ("%s: host address %#x\n", progname, addr.sin_addr.s_addr);

    svc_doit ();	/* away we go */

    svc_unregister (DRAWSRVR, DRAWSRVRVERS);
    }
/*******************************************************************************
*
* drawServer - handle RPC requests to draw a sprite
*
* ARGSUSED0
*/

LOCAL void drawServer (rqstp, xprt)
    struct svc_req *rqstp;
    SVCXPRT *xprt;

    {
    struct sockaddr_in *caller;
    DRAW_UPDATE thisUpdate;
    DRAW_REPLY thisReply;

    caller = svc_getcaller (xprt);

    thisReply.width  = Xsize;
    thisReply.height = Ysize;
    thisReply.resetFlag = Reset;

    /* get the update request */

    if (!svc_getargs (xprt, xdr_DRAW_UPDATE, &thisUpdate))
	svcerr_decode (xprt);

    else if (thisUpdate.drawType == DR_GET_WINDOW_SIZE)
	{
	/* return the size of the window we're running in */

	if (!svc_sendreply(xprt, (xdrproc_t) xdr_DRAW_REPLY,
			   (caddr_t) &thisReply))
	    {
	    fprintf (stderr, "%s: couldn't reply to RPC call (1)\n", progname);
	    exit (1);
	    }

	printf ("\n%s: client from %#x has connected\n",
		progname, caller->sin_addr.s_addr);
	}

    else	/* DR_INITIAL, DR_UPDATE, DR_FINAL */
	{
	/* return the size of the window we're running in */

	if (!svc_sendreply(xprt, (xdrproc_t) xdr_DRAW_REPLY,
			   (caddr_t) &thisReply))
	    {
	    fprintf (stderr, "%s: couldn't reply to RPC call (2)\n", progname);
	    exit (1);
	    }

	switch (thisUpdate.drawType)
	    {
	    case DR_GET_WINDOW_SIZE:
	    case DR_UPDATE:
		break;

	    case DR_FINAL:	/* erase a dead sprite */
	    case DR_INITIAL:	/* draw a new sprite */
		{
		struct sockaddr_in *caller;
		caller = svc_getcaller (xprt);
		printf ("\n%s: client from %#x has %s\n",
			progname, caller->sin_addr.s_addr,
			thisUpdate.drawType == DR_FINAL ? "left" : "joined");
		break;

	    default:
                fprintf (stderr,"\n%s: invalid drawType <%d> from client %#x\n",
                        progname, thisUpdate.drawType, caller->sin_addr.s_addr);
                break;
		}
	    }
	redrawSprite (thisUpdate.drawType,
		      thisUpdate.oldx, thisUpdate.oldy,
		      thisUpdate.newx, thisUpdate.newy);
	}
    }
/*******************************************************************************
*
* redrawSprite - redraw a sprite
*/

LOCAL void redrawSprite (drawType, oldx, oldy, newx, newy)
    DRAW_TYPE drawType;
    int oldx;
    int oldy;
    int newx;
    int newy;

    {
    switch (drawType)
	{
	case DR_UPDATE:	/* erase the old sprite, draw a new one */
	    draw_sprite (oldx, oldy);
	    draw_sprite (newx, newy);
	    break;

	case DR_INITIAL:	/* draw a new sprite */
	    /* check for sprites that have exited ungracefully */

	    if (Reset)
		{
		Reset = FALSE;
		pw_writebackground (pixwin, 0, 0, Xsize, Ysize, (PIX_SRC));
		spriteCount = 0;
		}

	    draw_sprite (newx, newy);
	    spriteCount++;
	    break;

	case DR_FINAL:	/* erase a dead sprite */
	    draw_sprite (oldx, oldy);
	    spriteCount--;
	    break;

	default:
	    fprintf (stderr, "%s: invalid drawType %d\n", progname, drawType);
	    break;
	}

    if (spriteCount == 0)
	{
	/* reset Reset and clear the screen */

	Reset = FALSE;
	pw_writebackground (pixwin, 0, 0, Xsize, Ysize, (PIX_SRC));
	}
    }

/*******************************************************************************
*
* winChange -
*
* ARGSUSED0
*/

LOCAL void winChange (item, value, event)
    Panel_item item;
    int value;
    Event *event;

    {
    if (item == QuitButton)
	Quit = value;
    else if (item == ResetButton)
	{
	if (spriteCount > 0)
	    Reset = value;
	}
    else
	{
	fprintf (stderr, "%s: winChange?\n", progname);
	pw_vector (pixwin, 0, 0, Xsize, Ysize, PIX_SRC^PIX_DST, 1);
	}
    }
/*******************************************************************************
*
* winResize -
*
* ARGSUSED0
*/

LOCAL void winResize (canvas, w, h)
    Canvas canvas;
    int w;
    int h;

    {
    Xsize = w;
    Ysize = h;
    }
/*******************************************************************************
*
* my_notice_destroy -
*/

LOCAL Notify_value my_notice_destroy (frame, status)
    Frame frame;
    Destroy_status status;

    {
    if (status != DESTROY_CHECKING)
	{
	Quit = TRUE;
	fprintf (stderr, "%s: game over!\n", progname);
	(void) notify_stop ();
	}

    return (notify_next_destroy_func (frame, status));
    }

/*******************************************************************************
*
* svc_doit - basically svc_run with timeout to update sunwindow
*
* This is the RPC server side idle loop
* Wait for input, call server program.
*/

LOCAL void svc_doit ()

    {
#ifdef	SUNOS35
extern int svc_fds;
#define	svc_fdset svc_fds
#define	fd_set int
#endif	/* SUNOS35 */

    static struct timeval tt = { 0L, 10000L};
    fd_set readFds;
    int nFds;

    while (!Quit)
	{
	(void) notify_dispatch ();	/* any window shit */

	readFds = svc_fdset;

	nFds = select (32, &readFds, (int *)NULL, (int *)NULL,
			(struct timeval *)&tt);

	switch (nFds)
	    {
	    case -1:
		if (errno != EINTR)
		    {
		    perror ("svc_doit: select failed");
		    return;
		    }

	    case 0:
		break;

	    default:
#ifdef	SUNOS35
		svc_getreq (readFds);
#else
		svc_getreqset (&readFds);
#endif	/* SUNOS35 */
		break;
	    }
	}
    }
