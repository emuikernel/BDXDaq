/* xdrawServer.c - VxWorks/UNIX RPC demonstration draw server */

#ifndef LINT
static char *copyright = "Copyright 1988-1991, Wind River Systems, Inc.";
#endif	/* LINT */

/*
modification history
--------------------
01c,31oct91,rrr  passed through the ansification filter
		  -fixed #else and #endif
		  -changed VOID to void
		  -changed copyright notice
01b,07aug91,gae  WRS-ized; added UDP/TCP transport option; moved README here.
01a,10dec90,ajm  written based on SunView version.
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
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#ifdef	HOST_MIPS
#include <bsd/sys/types.h>
#endif	/* HOST_MIPS */

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

LOCAL short ball_dat[] = {
#include "ball.pr"
};

extern int errno;

/*
TCP version by default
#define	UDP_DRAW_SERVER
#define	UDP_RAMA_SERVER
*/

Display *display;
int screen;
LOCAL BOOL Quit;
Window win;
GC gcontext;
LOCAL char progname [100];

/* forward declarations */

LOCAL void drawServer ();
LOCAL void draw_sprite ();
LOCAL void redrawSprite ();
LOCAL void svc_stuff ();
void svc_doit ();

/*******************************************************************************
*
* main -
*
* drawServer accepts rpc requests to draw "sprites" in a window.
* RamaServer issues draw requests each
* time a sprite updates its position.
*/

main (argc, argv)
    int argc;
    char **argv;

    {
    int win_width;
    int win_height;
    int x;
    int y;
    int border_width = 4;
    int display_width;
    int display_height;
    unsigned int icon_width;
    unsigned int icon_height;
    unsigned long valuemask = 0;
    char *window_name = "Sprites";
    Pixmap icon_pixmap;
    XSizeHints size_hints;
    XGCValues *values;
    char *icon_name = "Sprites";

    strncpy (progname, argv [0], sizeof (progname));

    /* Connect to X Server */

    if ((display = XOpenDisplay(NULL)) == NULL)
	{
	fprintf (stderr, "basic: can't connect to X Server %s",
		XDisplayName (NULL));
	}

    /* get screen size from display structures macro */
    screen = DefaultScreen (display);
    display_width = DisplayWidth (display, screen);
    display_height = DisplayHeight (display, screen);

    x = display_width / 3;
    y = display_height / 3;

    win_width = 500;
    win_height = 500;

    /* Create window with parent attributes */

#ifdef	HOST_DEC
    win = XCreateSimpleWindow (display, RootWindow( display, screen),
               x, y, win_width, win_height, border_width,
               WhitePixel (display, screen), BlackPixel (display, screen));
#else
    win = XCreateSimpleWindow (display, RootWindow( display, screen),
               x, y, win_width, win_height, border_width,
               BlackPixel (display, screen), WhitePixel (display, screen));
#endif	/* HOST_DEC */

    /* Set icon width and height */

    icon_width  = 32;
    icon_height = 32;

#ifdef	HOST_DEC
    /* Create icon pixmap for icon */

    icon_pixmap = MakePixmap (display, screen, RootWindow( display, screen ),
                              icon_image, icon_width, icon_height);
#endif	/* HOST_DEC */

    /* Set resize hints */

    size_hints.flags      = PPosition | PSize | PMinSize ;
    size_hints.x          = x;
    size_hints.y          = y;
    size_hints.width      = win_width;
    size_hints.height     = win_height ;
    size_hints.min_width  = 150 ;
    size_hints.min_height = 150 ;

    /* Set standard properties for window manager */

    XSetStandardProperties (display, win, window_name, icon_name,
                             icon_pixmap, argv, argc, &size_hints);

    /* Create a graphic context for drawing sprites */

    gcontext = XCreateGC (display, win, valuemask, values);
    XSetFunction (display, gcontext, GXxor);

#ifdef	HOST_DEC
    XSetForeground (display, gcontext, WhitePixel (display, screen));
#else
    XSetForeground (display, gcontext, BlackPixel (display, screen));
#endif	/* HOST_DEC */

    /* Display window */

    XMapWindow (display, win);

    /* Select event types looking for */

    XSelectInput (display, win, ExposureMask | ButtonPressMask);

    /* initialize defaults */

    Quit = FALSE;

    /* initialize graphics */

    svc_stuff ();
    }

/*******************************************************************************
*
* draw_sprite - redraw the sprite to the input x, y position
*/

LOCAL void draw_sprite (x, y)
    int x;
    int y;

    {
    unsigned int width  = 16;
    unsigned int height = 16;
    int angle1          = 0;
    int angle2          = 360 * 64;

    XFillArc (display, win, gcontext, x, y, width, height, angle1, angle2);
    }
/*******************************************************************************
*
* svc_stuff - startup rpc service
*/

LOCAL void svc_stuff ()

    {
    struct sockaddr_in addr;
    SVCXPRT *xprt;
    bool_t status;

    /* create a tcp/udp transport to receive requests */

#ifdef	UDP_DRAW_SERVER
    if ((xprt = svcudp_create (RPC_ANYSOCK)) == (SVCXPRT *)NULL)
#else
    if ((xprt = svctcp_create (RPC_ANYSOCK)) == (SVCXPRT *)NULL)
#endif	/* UDP_DRAW_SERVER */
	{
	fprintf (stderr, "%s: tcp/udp_create failed\n", progname);
	exit (1);
	}

    /* un-register any previous incarnations in the local portmap daemon */

    pmap_unset (DRAWSRVR, DRAWSRVRVERS);

    /* register the service with the transport we created */

    status = svc_register(xprt, DRAWSRVR, DRAWSRVRVERS, drawServer,
#ifdef	UDP_DRAW_SERVER
	    IPPROTO_UDP);
#else
	    IPPROTO_TCP);
#endif	/* UDP_DRAW_SERVER */

    if (status == FALSE)
	{
	fprintf (stderr, "%s: svc_register failed... %#x\n", progname, errno);
	exit (-1);
	}

    get_myaddress (&addr);
    printf ("%s: host address %#x (draw-%s, rama-%s)\n",
	    progname, addr.sin_addr.s_addr,
#ifdef	UDP_DRAW_SERVER
	    "udp",
#else
	    "tcp",
#endif	/* UDP_DRAW_SERVER */
#ifdef	UDP_RAMA_SERVER
	    "udp");
#else
	    "tcp");
#endif	/* UDP_RAMA_SERVER */

    svc_doit ();	/* away we go */

    svc_unregister (DRAWSRVR, DRAWSRVRVERS);
    }
/*******************************************************************************
*
* drawServer - handle RPC requests to draw a sprite
*
* drawServer is the routine registered with rpc to handle incoming
* requests.
*/

LOCAL void drawServer (rqstp, xprt)
    struct svc_req *rqstp;
    SVCXPRT *xprt;

    {
    struct sockaddr_in *caller;
    DRAW_UPDATE thisUpdate;
    DRAW_REPLY thisReply;
    XWindowAttributes windowattr;

    caller = svc_getcaller (xprt);

    XGetWindowAttributes (display, win, &windowattr);

    thisReply.width  = windowattr.width;
    thisReply.height = windowattr.height;
    thisReply.resetFlag = FALSE;

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
	    draw_sprite (newx, newy);
	    break;

	case DR_FINAL:	/* erase a dead sprite */
	    draw_sprite (oldx, oldy);
	    break;

	default:
	    fprintf (stderr, "%s: invalid drawType %d\n", progname, drawType);
	    break;
	}
    }

/*******************************************************************************
*
* svc_doit - basically svc_run with timeout to update sunwindow
*
* This is the rpc server side idle loop
* Wait for input, call server program.
*/

void svc_doit ()

    {
#ifdef	SUNOS35
extern int svc_fds;
#define	svc_fdset svc_fds
#define	fd_set int
#endif	/* SUNOS35 */

    static struct timeval tt = { 0L, 10000L};
    fd_set readFds;
    int nFds;
    XEvent report ;

    while (!Quit)
	{
        if ( XCheckTypedEvent( display, ButtonPress, &report ))
          if ( report.type == ButtonPress )
            Quit = TRUE ;

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

#ifdef	HOST_DEC

Pixmap MakePixmap (display, screen, root, data, width, height)
    Display *display;
    int screen;
    Drawable root;
    short *data;
    unsigned int width;
    unsigned int height;

    {
    XImage ximage;
    GC pgc;
    XGCValues gcv;
    Pixmap pid;

    pid = XCreatePixmap (display, root, width, height,
			 DefaultDepth (display, screen));

    gcv.foreground = BlackPixel (display, screen);
    gcv.background = WhitePixel (display, screen);

    pgc = XCreateGC (display, pid, GCForeground | GCBackground, &gcv);

    ximage.height           = height;
    ximage.width            = width;
    ximage.xoffset          = 0;
    ximage.format           = XYBitmap;
    ximage.data             = (char *)data;
    ximage.byte_order       = LSBFirst;
    ximage.bitmap_unit      = 16;
    ximage.bitmap_bit_order = MSBFirst;
    ximage.bitmap_pad       = 16;
    ximage.bytes_per_line   = (width + 15) / 16 * 2;
    ximage.depth            = 1;

    XPutImage (display, pid, pgc, &ximage, 0, 0, 0, 0, width, height);

    XFreeGC (display, pgc);

    return (pid);
    }
#endif	/* HOST_DEC */
