/*****
* XmHTML Help widget
*
* This file Version	$Revision: 1.7 $
*
* Last modification: 	$Date: 1999/10/25 17:30:26 $
* By:			$Author: abbottd $
* Current State:	$State: Exp $
*
* Author:		Graham
*
*
*****/
/*****
* ChangeLog 
* $Log: htmlHelp.c,v $
* Revision 1.7  1999/10/25 17:30:26  abbottd
* Remove Linux WCHAR_T define
*
* Revision 1.6  1998/11/06 20:07:03  timmer
* Linux port wchar_t stuff
*
* Revision 1.5  1998/09/01 18:48:35  heyes
* satisfy Randy's lust for command line options
*
* Revision 1.4  1998/08/25 18:22:36  rwm
* Added comments about rsd_strdup which is not used now.
*
* Revision 1.3  1998/06/18 12:20:26  heyes
* new GUI ready I think
*
* Revision 1.2  1998/05/28 17:46:48  heyes
* new GUI look
*
* Revision 1.1  1998/04/09 17:45:15  heyes
* add
*
*
*****/ 
#define VERSION 1103
#define HAVE_LIBJPEG
#define HAVE_REGEX 
#define NDEBUG

/*Sergey: does not work for SunOS_ipc86
#define HAVE_GIF_CODEC
*/

#include <stdio.h>
#ifdef __STDC__
#include <stdarg.h>
#else
#include <varargs.h>
#endif

#include <X11/Xos.h>
#include <Xm/CascadeB.h>
#include <Xm/FileSB.h>
#include <Xm/MainW.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/SeparatoG.h>
#include <Xm/RowColumn.h>
#include <Xm/LabelG.h>
#include <Xm/List.h>
#include <Xm/Text.h>
#include <Xm/ToggleBG.h>
#include <Xm/SelectioB.h>
#include <Xm/MessageB.h>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>		/* isspace */
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>		/* also for getwd on non-POSIX systems */

#define APP_CLASS		"HTMLDemos"

/*****
* We want to have the full XmHTML instance definition available 
* when debugging this stuff, so include XmHTMLP.h
* Note:
* 	Including XmHTMLP.h normally doesn't pull in any of the internal XmHTML
*	functions. If you want to do this anyway, you need to include *both*
*	XmHTMLP.h and XmHTMLI.h (in that order).
* Note:
*	When being compiled with the provided or imake generated Makefile, the
*	symbol VERSION is defined. This is a private define which must not be used
*	when compiling applications: it sets other defines which pull in
*	other, private, header files normally not present after installation
*	of the library. Therefore we undefine this symbol *before* including
*	the XmHTMLP.h header file. This is not required when XmHTML.h is used.
*****/
#if defined(VERSION)
#undef VERSION
#endif /* VERSION */
#include <XmHTMLP.h>
#include <Balloon.h>

/* bunch of common routines */ 
#include "misc.h"
/* imagecache stuff */
#include "cache.h"

/* string find */
#include "find.h"

/* menu build routines */
#include "menu.h"

/* catch NULL strdup's for debug builds */
/* You must define DEBUG for XmHTML also. */
/* rsd_strdup is in XmHTML-x.x.x/src/StringUtil.c */
#if 0

#if defined(DEBUG) && !defined(DMALLOC)
extern char *__rsd_strdup(const char *s1, char *file, int line);
#define strdup(PTR) __rsd_strdup(PTR, __FILE__, __LINE__)
#endif

#endif

#ifdef NEED_STRCASECMP
# include <sys/types.h>
extern int my_strcasecmp(const char *s1, const char *s2);
extern int my_strncasecmp(const char *s1, const char *s2, size_t n);
#define strcasecmp(S1,S2) my_strcasecmp(S1,S2)
#define strncasecmp(S1,S2,N) my_strncasecmp(S1,S2,N)
#endif

/*** External Function Prototype Declarations ***/
/* from visual.c */
extern int getStartupVisual(Widget shell, Visual **visual, int *depth,
	Colormap *colormap);

/* can be found in XmHTML */
extern char *my_strcasestr(const char *s1, const char *s2);

#ifdef DEBUG
extern void _XmHTMLUnloadFonts(XmHTMLWidget);
extern void _XmHTMLAddDebugMenu(Widget, Widget, String);
#endif

/*** Public Variable Declarations ***/

/*** Private Datatype Declarations ****/
#define MAX_HISTORY_ITEMS	100		/* save up to this many links */
#define MAX_PATHS			25		/* size of visited path cache */
#define MAX_IMAGE_ITEMS		512		/* max. no of images per document */
#define MAX_HTML_WIDGETS	10		/* max. no of HTML widgets allowed */

/* Link defines */
#define LINK_MADE			0
#define LINK_HOMEPAGE		1
#define LINK_TOC			2
#define LINK_INDEX			3
#define LINK_GLOSSARY		4
#define LINK_COPYRIGHT		5
#define LINK_PREVIOUS		6
#define LINK_UP				7
#define LINK_DOWN			8
#define LINK_NEXT			9
#define LINK_HELP			10
#define LINK_LAST			11

/* document cache */
typedef struct{
	String path;		/* path to this document */
	String file;		/* full filename of this document */
	int current_ref;	/* last activated hyperlink */
	int nrefs;			/* total no of activated hyperlinks in this document */
	String refs[MAX_HISTORY_ITEMS];	/* list of activated hyperlinks */
	String visited[MAX_HISTORY_ITEMS];	/* list of visited hyperlins */
	int nvisited;		/* total no of visited hyperlinks */
	int nimages;		/* no of images in this document */
	String images[MAX_IMAGE_ITEMS];	/* image urls for this document */
}DocumentCache;

/* List of all HTML widgets (especially for frames) */
typedef struct{
	Boolean active;		/* is this an active frame? */
	Boolean used;		/* is this frame currently being used? */
	String name;		/* name of this frame */
	String src;			/* source file for this frame */
	Widget html;		/* XmHTMLWidget id for this frame */
	Boolean has_popup;	/* do we have a popup menu? */
}HTMLWidgetList;

typedef struct{
	int link_type;		/* 0 = rev, 1 = rel, which means fetch it */
	Boolean have_data;
	String href;
	String title;
}documentLinks;

typedef struct{
	Widget w;			/* menu button Widget ID			*/
	int id;				/* menu button ID					*/
	String name;		/* XmHTML resource name (if any)	*/
	Boolean value;		/* current value					*/
}optionsStruct;

/*** Private Function Prototype Declarations ****/
static DocumentCache *getDocFromCache(String file);
static void storeDocInCache(String file);
static void storeInHistory(String file, String loc);
static void storeAnchor(String href);
static void removeDocFromCache(int doc);
static void flushImages(Widget w);
static void killImages(void);

/* XmHTMLWidget callbacks */
static void anchorCB(Widget w, XtPointer arg1, XmHTMLAnchorPtr href_data);
static void trackCB(Widget w, XtPointer arg1, XmHTMLAnchorPtr href_data);
static void docCB(Widget w, XtPointer arg1, XmHTMLDocumentPtr cbs);
static void linkCB(Widget w, XtPointer arg1, XmHTMLLinkPtr cbs);
static void frameCB(Widget w, XtPointer arg1, XmHTMLFramePtr cbs);
static void formCB(Widget w, XtPointer loadWidget,
	XmHTMLFormCallbackStruct *cbs);

/* misc functions */
static void callClient(URLType url_type, String url);
static void readPipe(Widget w, char *cmd);
static void infoCB(Widget parent, Widget popup, XButtonPressedEvent *event);

/* XmHTMLWidget functions */
static XmImageInfo *loadImage(Widget w, String url);
static int testAnchor(Widget w, String href);
static void jumpToFrame(String filename, String loc, String target,
	Boolean store);
static int getImageData(XmHTMLPLCStream *stream, XtPointer buffer);
static void endImageData(XmHTMLPLCStream *stream, XtPointer data, int type,
	Boolean ok);

/* button bar callbacks */
static void linkButtonCB(Widget w, XtPointer arg1, XtPointer arg2);
static void infoPopupCB(Widget w, int item);
static void historyCB(Widget w, int button);
static void progressiveButtonCB(Widget w, int reset);
static void showImageInfo(XmImageInfo *info);

/* menu callbacks */
static void fileCB(Widget w, XtPointer client_data, XtPointer call_data);
static void viewCB(Widget w, XtPointer client_data, XtPointer call_data);
static void editCB(Widget w, XtPointer client_data, XtPointer call_data);
static void optionsCB(Widget w, XtPointer client_data, XtPointer call_data);
static void windowCB(Widget w, XtPointer client_data, XtPointer call_data);
static void helpCB(Widget w, XtPointer client_data, XtPointer call_data);
static void warningCB(Widget w, XtPointer client_data, XtPointer call_data);

/* Callback helper routines */
static void viewSource(Widget parent, int what);
static void viewDocumentInfo(Widget parent);
static void findString(Widget w);

/*** Private Variable Declarations ***/
static XtAppContext context;
static Widget back, forward, load_images, label, toplevel = NULL, reload;
static Widget link_button, link_dialog, html32, verified, info_dialog;
static Widget prg_button, image_dialog;
static Widget info_popup;
static documentLinks document_links[LINK_LAST];
static XmImage *preview_image;
static Widget link_buttons[LINK_LAST];
static char default_font[128], current_font[128];
static char default_charset[128], current_charset[128];
static String link_labels[LINK_LAST] = {"Mail Author", "Home", "TOC",
	"Index", "Glossary", "Copyright", "Prev", "Up", "Down", "Next",
	"Help"};
static String image_types[] = {"(error occured)", "Unknown Image type",
	"X11 Pixmap", "X11 Bitmap", "CompuServe(C) Gif87a or Gif89a",
	"Animated Gif89a", "Animated Gif89a with NETSCAPE2.0 loop extension",
	"CompuServe(C) Compatible Gzf87a or Gzf89a", "Gif89a Compatible animation",
	"Gif89a compatible animation with NETSCAPE2.0 loop extension",
	"JPEG", "PNG", "Fast Loadable Graphic"};

/*****
* global XmHTML configuration. Elements must be in the same order as the
* OPTIONS_ defines in menuItems.h
*****/
/* and the menu definitions */
#include "menuItems.h"

static optionsStruct html_config[OPTIONS_LAST] = {
	{ NULL, OPTIONS_GENERAL, "generalOptions", True },
	{ NULL, OPTIONS_DOCUMENT, "documentOptions", True },
	{ NULL, OPTIONS_ANCHOR, "anchorOptions", True },
	{ NULL, OPTIONS_IMAGES, "imageOptions", True },
	{ NULL, OPTIONS_FONTS, "fontOptions", True },

	/* toggable anchor resources */
	{ NULL, OPTIONS_ANCHOR_BUTTONS, XmNanchorButtons, True },
	{ NULL, OPTIONS_ANCHOR_HIGHLIGHT, XmNhighlightOnEnter, True },
	{ NULL, OPTIONS_ANCHOR_TRACKING, "imageAnchorTracking", False },
	{ NULL, OPTIONS_ANCHOR_TIPS, "anchorTips", True },

	/* toggable body resources */
	{ NULL, OPTIONS_BODY_COLORS, XmNenableBodyColors, True },
	{ NULL, OPTIONS_BODY_IMAGES, XmNenableBodyImages, True },
	{ NULL, OPTIONS_ALLOW_COLORS, XmNenableDocumentColors, True },
	{ NULL, OPTIONS_ALLOW_FONTS, XmNenableDocumentFonts, True },
	{ NULL, OPTIONS_JUSTIFY, XmNenableOutlining, True },

	/* toggable parser resources */
	{ NULL, OPTIONS_STRICTHTML, XmNstrictHTMLChecking, True },
	{ NULL, OPTIONS_BADHTML, XmNenableBadHTMLWarnings, XmHTML_ALL },

	/* toggable image resources */
	{ NULL, OPTIONS_ANIMATION_FREEZE, XmNfreezeAnimations, False },
	{ NULL, OPTIONS_ENABLE_IMAGES, XmNimageEnable, True },
	{ NULL, OPTIONS_AUTOLOAD_IMAGES, "autoImageLoad", True },

	{ NULL, OPTIONS_SAVE, "optionsSave", True },
};

/* Command line options */
static Boolean root_window, noframe, external_client;
static Boolean progressive_images, allow_exec;
static int animation_timeout = 175;
#define MAX_PROGRESSIVE_DATA_SKIP	2048

static int progressive_data_skip = MAX_PROGRESSIVE_DATA_SKIP;
static int progressive_data_inc  = 0;
#define DEBUG
#ifdef DEBUG
static Boolean debug = False;
#define Debug(MSG) do { \
	if(debug) { printf MSG ; fflush(stdout); } }while(0)
#else
#define Debug(MSG)	/* emtpy */
#endif

/* document cache */
static DocumentCache doc_cache[MAX_HISTORY_ITEMS];
static int current_doc, last_doc;

/*****
* List of all html widgets. The first slot is the toplevel HTML widget and
* is never freed. All other slots are used by frames.
*****/
static HTMLWidgetList html_widgets[MAX_HTML_WIDGETS];

/* visited paths */
static String paths[MAX_PATHS][1024];
static int max_paths;

/* default settings */
static String appFallbackResources[] = {
"*fontList:			   *-adobe-helvetica-bold-r-*-*-*-120-*-*-p-*-*-*",
"*sourceView.width:	   550",
"*sourceView.height:	  500",
"*XmHTML.width:		   575",
"*XmHTML.height:		  600",
"*XmBalloon*background:  yellow",
"*file.labelString: File",
"*fileMenu*open.labelString: Open File...",
"*fileMenu*open.mnemonic: O",
"*fileMenu*open.accelerator: Ctrl<Key>O",
"*fileMenu*open.acceleratorText: Ctrl+O",
"*fileMenu*saveas.labelString: Save File As...",
"*fileMenu*saveas.mnemonic: S",
"*fileMenu*reload.labelString: Reload File",
"*fileMenu*reload.mnemonic: R",
"*fileMenu*reload.accelerator: Ctrl<Key>R",
"*fileMenu*reload.acceleratorText: Ctrl+R",
"*fileMenu*quit.labelString: Exit",
"*fileMenu*quit.mnemonic: x",
"*fileMenu*quit.accelerator: Ctrl<Key>X",
"*fileMenu*quit.acceleratorText: Ctrl+X",
"*fileMenu*view.labelString: View",
"*fileMenu*view.mnemonic: V",
"*viewMenu*viewInfo.labelString: Document Info",
"*viewMenu*viewInfo.mnemonic: I",
"*viewMenu*viewSource.labelString: Document Source",
"*viewMenu*viewSource.mnemonic: S",
"*viewMenu*viewFonts.labelString: Font Cache Info",
"*viewMenu*viewFonts.mnemonic: F",
"*edit.labelString: Edit",
"*editMenu*find.labelString: Find...",
"*editMenu*find.mnemonic: F",
"*editMenu*findAgain.labelString: Find Again",
"*editMenu*findAgain.mnemonic: A",
"*option.labelString: Options",
"*optionMenu*anchorButtons.labelString: Buttoned Anchors",
"*optionMenu*anchorButtons.mnemonic: B",
"*optionMenu*highlightOnEnter.labelString: Highlight Anchors",
"*optionMenu*highlightOnEnter.mnemonic: H",
"*optionMenu*imageAnchorTracking.labelString: Track Image Anchors",
"*optionMenu*imageAnchorTracking.mnemonic: I",
"*optionMenu*anchorTips.labelString: Anchor tooltips",
"*optionMenu*anchorTips.mnemonic: t",
"*optionMenu*enableBodyColors.labelString: Body Colors",
"*optionMenu*enableBodyColors.mnemonic: C",
"*optionMenu*enableBodyImages.labelString: Body Image",
"*optionMenu*enableBodyImages.mnemonic: o",
"*optionMenu*enableDocumentColors.labelString: Allow Document Colors",
"*optionMenu*enableDocumentColors.mnemonic: l",
"*optionMenu*enableDocumentFonts.labelString: Allow Document Fonts",
"*optionMenu*enableDocumentFonts.mnemonic: F",
"*optionMenu*enableOutlining.labelString: Text Justification",
"*optionMenu*enableOutlining.mnemonic: J",
"*optionMenu*strictHTMLChecking.labelString: Strict HTML Checking ",
"*optionMenu*strictHTMLChecking.mnemonic: S",
"*optionMenu*warning.labelString: HTML Warnings",
"*optionMenu*warning.mnemonic: W",
"*optionMenu*freezeAnimations.labelString: Freeze Animations",
"*optionMenu*freezeAnimations.mnemonic: r",
"*optionMenu*imageEnable.labelString: Enable Image Support",
"*optionMenu*imageEnable.mnemonic: E",
"*optionMenu*autoImageLoad.labelString: Autoload Images",
"*optionMenu*autoImageLoad.mnemonic: u",
"*optionMenu*save.labelString: Save Options",
"*optionMenu*save.mnemonic: v",
"*warningMenu*none.labelString: Disable",
"*warningMenu*none.mnemonic: D",
"*warningMenu*all.labelString: Show All Warnings",
"*warningMenu*all.mnemonic: A",
"*warningMenu*unknownElement.labelString: Unknown HTML element",
"*warningMenu*unknownElement.mnemonic: U",
"*warningMenu*bad.labelString: Badly placed tags",
"*warningMenu*bad.mnemonic: B",
"*warningMenu*openBlock.labelString: Bad block aparture",
"*warningMenu*openBlock.mnemonic: p",
"*warningMenu*closeBlock.labelString: Bad block closure",
"*warningMenu*closeBlock.mnemonic: c",
"*warningMenu*openElement.labelString: Unbalanced Terminators",
"*warningMenu*openElement.mnemonic: T",
"*warningMenu*nested.labelString: Improper Nested Tags",
"*warningMenu*nested.mnemonic: I",
"*warningMenu*violation.labelString: HTML 3.2 Violations",
"*warningMenu*violation.mnemonic: V",
"*window.labelString: Window",
"*windowMenu*lower.labelString: Lower Window",
"*windowMenu*lower.mnemonic: L",
"*windowMenu*raise.labelString: Raise Window",
"*windowMenu*raise.mnemonic: R",
"*help.labelString: Help",
"*helpMenu*about.labelString: About XmHTML",
"*helpMenu*about.mnemonic: A",
NULL};

/*****
* Name: 		addPath
* Return Type: 	void
* Description: 	adds a path to the list of visited paths if it hasn't been
*				stored yet.
* In: 
*	path:		path to be stored;
* Returns:
*	nothing.
*****/
static void
addPath(String path)
{
	int i = 0;

	/* see if the path has already been added */
	for(i = 0; i < max_paths; i++)
		if(!(strcmp((char*)(paths[i]), path)))
			return;

	/* store this path */
	if(max_paths < MAX_PATHS)
	{
		strcpy((char*)(paths[max_paths]), path);
		max_paths++;
	}
}

/*****
* Name: 		resolveFile
* Return Type: 	String
* Description: 	checks if the given file exists on the local file system
* In: 
*	filename:	file to check
* Returns:
*	a full filename when the file exists. NULL if it doesn't.
* Note:
*	This routine tries three things to check if a file exists on the local
*	file system:
*	1. if "filename" is absolute, it is assumed the file exists and is
*	   accessible;
*	2. checks whether "filename" can be found in the path of the current
*	   document;
*	3. sees if "filename" can be found in the list of stored paths.
*	When a file has been found, it is checked if this is a regular file,
*	and if so it is transformed into a fully qualified pathname (with
*	relative paths fully resolved).
*****/
static String
resolveFile(String filename)
{
	static String ret_val;
	char tmp[1024];

	/* throw out http:// stuff */
	if(!(strncasecmp(filename, "http://", 7)))
		return(NULL);

	Debug(("resolveFile, looking for %s\n", filename));

	ret_val = NULL;

	/*****
	* If this is an absolute path, check if it's really a valid file
	* (or directory). This allows us to recognize chrooted files when
	* browsing the local web directory.
	*****/
	if(filename[0] == '/')
	{
		if(!(access(filename, R_OK)))
			ret_val = strdup(filename); 
		else	/* a fake path, strip of the leading / */
			sprintf(tmp, "%s", &filename[1]);
	}
	else
	{
		strcpy(tmp, filename);
		tmp[strlen(filename)] = '\0';	/* NULL terminate */
	}

	if(ret_val == NULL)
	{
		char real_file[1024];
		int i;

		/*****
		* search the paths visited so far. Do it top to bottom as the
		* last visited path is inserted in the last slot. Quite usefull
		* when looking for images or links in the current document.
		*****/
		for(i = max_paths-1; i >= 0 ; i--)
		{
			sprintf(real_file, "%s%s", (char*)(paths[i]), tmp);

			/* check if we have access to this thing */
			if(!(access(real_file, R_OK)))
			{
				struct stat statb;
				/*****
				* We seem to have some access rights, make sure this
				* is a regular file
				*****/
				if(stat(real_file, &statb) == -1)
				{
					perror(filename);
					break;
				}
				else if(S_ISDIR(statb.st_mode))
				{
					/*****
					* It's a dir. First check for index.html then
					* for Welcome.html.
					*****/
					int len = strlen(real_file)-1;

					strcat(real_file, real_file[len] == '/' ?
						"index.html\0" : "/index.html\0");
					if(!(access(real_file, R_OK)))
					{
						ret_val = strdup(real_file);
						break;
					}
					real_file[len+1] = '\0';
					strcat(real_file, real_file[len] == '/' ?
						"Welcome.html\0" : "/Welcome.html\0");
					if(!(access(real_file, R_OK)))
					{
						ret_val = strdup(real_file);
						break;
					}
					/* no file in here, too bad */
					break;
				}
				else if(!S_ISREG(statb.st_mode))
				{
					fprintf(stderr, "%s: not a regular file\n", filename);
					break;
				}
				ret_val = strdup(real_file);
				break;
			}
		}
	}

	if(ret_val == NULL)
	{
		sprintf(tmp, "%s:\ncannot display: unable to locate file.", filename);
		XMessage(toplevel, tmp);
	}
	else
	{
		/* clean out relative path stuff and add the path to the path index. */
		char fname[1024], pname[1024];

		(void)parseFilename(ret_val, fname, pname);
		addPath(pname);

		/*
		* resolve symbolic links as well (prevents object cache from going
		* haywire by having two different objects with cross-linked
		* mappings)
		*/
#if 0
		(void)followSymLinks(pname);
#endif
		/*****
		* big chance parseFilename compressed relative paths out of ret_val,
		* do it again. We need to reallocate as the size of the fully
		* resolved path can exceed the current length.
		*****/
		ret_val = (String)realloc(ret_val, strlen(pname) + strlen(fname) + 1);
		sprintf(ret_val, "%s%s", pname, fname);
	}

	Debug(("resolveFile, found as %s\n", ret_val ? ret_val : "(not found)"));
	return(ret_val);
}

/*****
* Name: 		getAndSetFile
* Return Type: 	void
* Description: 	reads the given file, sets the contents of this
*				file in the HTML widget. Also sets the title of the application
*				to the title of the document loaded.
* In: 
*	file:		name of file to load
*	loc			location file file
*	store:		history storage
* Returns:
*	True upon success, False on failure.
*****/
static int
getAndSetFile(String file, String loc, Boolean store)
{
	String buf = NULL, title, mime = NULL;
	Arg args[5];
	int argc = 0;
	XmString xms;

	setBusy(toplevel, True);

	/* load the file */
	if(file == NULL || (buf = loadFile(file, &mime)) == NULL)
	{
		if(mime != NULL)
			XMessage(toplevel, buf);
		setBusy(toplevel, False);
		return(False);
	}
	/* set name of current file in the label */
	xms = XmStringCreateLocalized(file);
	XtVaSetValues(label,
		XmNlabelString, xms,
		NULL);
	XmStringFree(xms);

	/* kill of any outstanding progressive image loading contexts */
	XmHTMLImageProgressiveKill(html_widgets[0].html);

	/* reset/unmanage progressive image load button */
	if(prg_button)
		progressiveButtonCB(prg_button, 1);

	if(html_config[OPTIONS_ENABLE_IMAGES].value)
		XtSetSensitive(load_images, True);

	/* store this document in the history */
	if(store)
		storeInHistory(file, loc);

	/* set mime type */
	XtSetArg(args[argc], XmNmimeType, mime); argc++;

	/* and set values */
	XtSetValues(html_widgets[0].html, args, argc);

	/*****
	* Now set the text directly into the widget. XmHTMLTextSetString
	* works a lot faster than using XtVaSetValues: it causes a XmHTML widget
	* to update it's display immediatly, and this is the behaviour we want
	* to have: due to the asynchronous behavior of X, the widget might not
	* have parsed and set the text when we want to set or retrieve resources
	* from the new text. Using XmHTMLTextSetString *ensures* that the new
	* text is parsed and loaded before the widget returns control to X.
	* XmUpdateDisplay *might* work also, but that has been untested.
	*****/
	XmHTMLTextSetString(html_widgets[0].html, buf);

	/* free it, XmHTML makes a copy of the text to work with. */
	free(buf);

	/*****
	* See if the current text has got a title.
	* Note that one can also get the document title by using
	* XmHTMLGetHeadAttributes() with the HeadTitle flag set.
	*****/
	if((title = XmHTMLGetTitle(html_widgets[0].html)) != NULL)
	{
		/* it has, set it */
		XtVaSetValues(toplevel,
			XtNtitle, title,
			XtNiconName, title,
			NULL);
		XtFree(title);
	}
	else
	{
		XtVaSetValues(toplevel,
			XtNtitle, "<Untitled>",
			XtNiconName, "<Untitled>",
			NULL);
	}
	XtSetSensitive(reload, True);
	setBusy(toplevel, False);
	return(True);
}

/*****
* Name: 		getInfoSize
* Return Type: 	int
* Description: 	returns the size of the given XmImageInfo structure.
* In: 
*	call_data:	ptr to a XmImageInfo structure;
*	client_data: data registered when we called initCache.
* Returns:
*	size of the given XmImageInfo structure.
* Note:
*	This function is used both by us and the caching routines.
*****/
static int
getInfoSize(XtPointer call_data, XtPointer client_data)
{
	int size = 0;
	XmImageInfo *frame = (XmImageInfo*)call_data;

	while(frame != NULL)
	{
		size += sizeof(XmImageInfo);
		size += frame->width*frame->height;		/* raw image data */

		/* clipmask size. The clipmask is a bitmap of depth 1 */
		if(frame->clip)
		{
			int clipsize;
			clipsize = frame->width;
			/* make it byte-aligned */
			while((clipsize % 8))
				clipsize++;
			/* this many bytes on a row */
			clipsize /= 8;
			/* and this many rows */
			clipsize *= frame->height;
			size += clipsize;
		}
		/* reds, greens and blues */
		size += 3*frame->ncolors*sizeof(Dimension);
		frame = frame->frame;	/* next frame of this image (if any) */
	}
	return(size);
}

/*****
* Name: 		getDocFromCache
* Return Type: 	DocumentCache*
* Description: 	retrieves a document from the document cache.
* In: 
*	file:		filename of document to be retrieved.
* Returns:
*	nothing.
*****/
static DocumentCache*
getDocFromCache(String file)
{
	int i;
	for(i = 0; i < last_doc; i++)
	{
		if(!(strcmp(doc_cache[i].file, file)))
			return(&doc_cache[i]);
	}
	return(NULL);
}

/*****
* Name: 		storeDocInCache
* Return Type: 	void
* Description: 	stores the given document in the document cache.
* In: 
*	file:		filename of document to be stored;
* Returns:
*	nothing.
*****/
static void
storeDocInCache(String file)
{
	char foo[128], pname[1024];

	if(last_doc == MAX_HISTORY_ITEMS)
	{
		int i;
		/* free hrefs */
		for(i = 0; i < doc_cache[0].nrefs; i++)
		{
			if(doc_cache[0].refs[i])
				free(doc_cache[0].refs[i]);
			doc_cache[0].refs[i] = NULL;
		}

		/* free image url's */
		for(i = 0; i < doc_cache[0].nimages; i++)
		{
			free(doc_cache[0].images[i]);
			doc_cache[0].images[i] = NULL;
		}

		/* free visited anchor list */
		for(i = 0; i < doc_cache[0].nvisited; i++)
		{
			if(doc_cache[0].visited[i])
				free(doc_cache[0].visited[i]);
			doc_cache[0].visited[i] = NULL;
		}
		/* free file and path fields */
		free(doc_cache[0].file);
		free(doc_cache[0].path);

		/* move everything downward */
		for(i = 0; i < MAX_HISTORY_ITEMS-1; i++)
			doc_cache[i] = doc_cache[i+1];
		last_doc = MAX_HISTORY_ITEMS - 1;
	}

	Debug(("Storing document %s in document cache\n", file));

	current_doc = last_doc;
	doc_cache[current_doc].nrefs	= 0;
	doc_cache[current_doc].nvisited = 0;
	doc_cache[current_doc].nimages  = 0;
	doc_cache[current_doc].file = strdup(file);

	/* get path to this file */
	(void)parseFilename(file, foo, pname);
	/* and store it */
	doc_cache[current_doc].path = strdup(pname);
	last_doc++;
}

/*****
* Name: 		removeDocFromCache
* Return Type: 	void
* Description: 	removes a document from the document cache
* In: 
*	doc:		id of document to be removed;
* Returns:
*	nothing.
*****/
static void
removeDocFromCache(int doc)
{
	DocumentCache *this_doc;
	int i;

	this_doc = &doc_cache[doc];

	Debug(("Removing document %s from document cache\n", this_doc->file));

	/* remove all document url's */
	for(i = 0; i < this_doc->nrefs; i++)
	{
		if(this_doc->refs[i])
			free(this_doc->refs[i]);
		this_doc->refs[i] = NULL;
	}

	/* free visited anchor list */
	for(i = 0; i < this_doc->nvisited; i++)
	{
		if(this_doc->visited[i])
			free(this_doc->visited[i]);
		this_doc->visited[i] = NULL;
	}

	/* and remove all image url's */
	for(i = 0; i < this_doc->nimages; i++)
	{
		/* remove image cache entry for this image */
		removeURLObjectFromCache(this_doc->images[i]);
		free(this_doc->images[i]);
	}
	/*****
	* Update image cache (clears out all objects with a reference count
	* of zero).
	*****/
	pruneObjectCache();

	this_doc->nrefs	= 0;
	this_doc->nvisited = 0;
	this_doc->nimages  = 0;
	free(this_doc->file);
	free(this_doc->path);
}

/*****
* Name: 		storeInHistory
* Return Type: 	void
* Description: 	stores the given href in the history list
* In: 
*	file:		name of document
*	loc:		value of named anchor in file.
* Returns:
*	nothing.
*****/
static void
storeInHistory(String file, String loc)
{
	int i;
	DocumentCache *this_doc = NULL;

	/* sanity check */
	if(file == NULL && loc == NULL)
		return;

	/* if file is NULL we are for sure in the current document */
	if(file == NULL)
		this_doc = &doc_cache[current_doc];
	else 
	{
		/****
		* This is a new file. If we have any documents on the stack,
		* remove them.
		****/
		if(last_doc)
		{
			for(i = current_doc+1; i < last_doc; i++)
				removeDocFromCache(i);
			last_doc = current_doc+1;
		}
		/* update refs for current document */
		this_doc = &doc_cache[current_doc];
		if(this_doc->nrefs)
		{
			/* remove any existing references above the current one */
			for(i = this_doc->current_ref+1; i < this_doc->nrefs; i++)
			{
				if(this_doc->refs[i])
				{
					free(this_doc->refs[i]);
					this_doc->refs[i] = NULL;
				}
			}
			this_doc->nrefs = this_doc->current_ref + 1;
		}
		if((this_doc = getDocFromCache(file)) == NULL)
		{
			storeDocInCache(file);
			this_doc = &doc_cache[current_doc];
		}
	}

	if(this_doc->nrefs == MAX_HISTORY_ITEMS)
	{
		/* free up the first item */
		if(this_doc->refs[0])
			free(this_doc->refs[0]);
		/* move everything downward */
		for(i = 0; i < MAX_HISTORY_ITEMS - 1; i++)
			this_doc->refs[i] = this_doc->refs[i+1];
		this_doc->nrefs = MAX_HISTORY_ITEMS-1;
	}

	/* sanity check */
	if(loc)
		this_doc->refs[this_doc->nrefs] = strdup(loc);
	else
		this_doc->refs[this_doc->nrefs] = NULL;
	this_doc->current_ref = this_doc->nrefs;
	this_doc->nrefs++;

	/* set button sensitivity */
	XtSetSensitive(back, current_doc || this_doc->current_ref ? True : False);
	XtSetSensitive(forward, False);
}

/*****
* Name: 		storeAnchor
* Return Type: 	void
* Description: 	stores the given href in the visited anchor list of current
*				document.
* In: 
*	href:		value to store.
* Returns:
*	nothing.
*****/
static void
storeAnchor(String href)
{
	int i;
	DocumentCache *this_doc = NULL;

	/* sanity check */
	if(href == NULL)
		return;

	/* pick up current document */
	this_doc = &doc_cache[current_doc];

	/* check if this location is already present in the visited list */
	for(i = 0; i < this_doc->nvisited; i++)
		if(!(strcmp(this_doc->visited[i], href)))
			return;
	/* not present yet, store in visited anchor list */

	/* move everything down if list is full */
	if(this_doc->nvisited == MAX_HISTORY_ITEMS)
	{
		/* free up the first item */
		if(this_doc->visited[0])
			free(this_doc->visited[0]);
		/* move everything downward */
		for(i = 0; i < MAX_HISTORY_ITEMS - 1; i++)
			this_doc->visited[i] = this_doc->visited[i+1];
		this_doc->nvisited = MAX_HISTORY_ITEMS-1;
	}
	/* store this item */
	this_doc->visited[this_doc->nvisited] = strdup(href);
	this_doc->nvisited++;
}

/*****
* Name: 		loadOrJump
* Return Type: 	void
* Description: 	checks the given href for a file and a possible jump to
*				a named anchor in this file.
* In: 
*	file:		name of file to load
*	loc:		location in file to jump to.
*	store:		history storage
* Returns:
*	True upon success, False on failure (file load failed)
*****/
Boolean
loadAndOrJump(String file, String loc, Boolean store)
{
	static String prev_file;
	if (file) {
	  /* only load a file if it isn't the current one */
	  if(prev_file == NULL || (file && strcmp(file, prev_file)))
	    {
	      /* do nothing more if the load fails */
	      if(!(getAndSetFile(file, loc, store)))
		return(False);
	    }
	  if(prev_file) 
	    free(prev_file);
	  prev_file = strdup(file);
	}
	/* jump to the requested anchor in this file or to top of the document */
	if(loc) {
	  printf("loc is %s\n",loc);
		XmHTMLAnchorScrollToName(html_widgets[0].html, loc);
		printf("Id: %d\n",XmHTMLAnchorGetId(html_widgets[0].html, loc));
	} else
		XmHTMLTextScrollToLine(html_widgets[0].html, 0);
	return(True);
}

Boolean
loadTextAndOrJump(String title, String text, String loc, Boolean store)
{
  Arg args[5];
  int argc = 0;
  XmString xms;
  
  setBusy(toplevel, True);
  
  /* set name of current file in the label */
  if (title) {
    xms = XmStringCreateLocalized(title);
    XtVaSetValues(label,
		  XmNlabelString, xms,
		  NULL);
    XmStringFree(xms);
  }
  
  /* kill of any outstanding progressive image loading contexts */
  XmHTMLImageProgressiveKill(html_widgets[0].html);
  
  /* reset/unmanage progressive image load button */
  if(prg_button)
    progressiveButtonCB(prg_button, 1);
  
  if(html_config[OPTIONS_ENABLE_IMAGES].value)
    XtSetSensitive(load_images, True);
  
  XmHTMLTextSetString(html_widgets[0].html, text);
  
  if((title = XmHTMLGetTitle(html_widgets[0].html)) != NULL)
    {
      /* it has, set it */
      XtVaSetValues(toplevel,
		    XtNtitle, title,
		    XtNiconName, title,
		    NULL);
      XtFree(title);
    }
  else
    {
      XtVaSetValues(toplevel,
		    XtNtitle, "<Untitled>",
		    XtNiconName, "<Untitled>",
		    NULL);
    }
  XtSetSensitive(reload, True);
  setBusy(toplevel, False);
  
  /* jump to the requested anchor in this file or to top of the document */
  if(loc) {
	  printf("loc is %s\n",loc);
    XmHTMLAnchorScrollToName(html_widgets[0].html, loc);
  } else
    XmHTMLTextScrollToLine(html_widgets[0].html, 0);
  return(True);
}

/*****
* Name: 		readFile
* Return Type: 	void
* Description: 	XmNokCallback handler for the fileSelectionDialog: retrieves
*				the entered filename and loads it.
* In: 
*	w:			widget
*	dialog:		widget id of the fileSelectionDialog
*	cbs:		callback data
* Returns:
*	nothing.
*****/
static void
readFile(Widget widget, Widget dialog, XmFileSelectionBoxCallbackStruct *cbs)
{
	String filename, file;
	int item;

	/* remove the fileSelectionDialog */
	XtPopdown(XtParent(dialog));

	/* get the entered filename */
	XmStringGetLtoR(cbs->value, XmSTRING_DEFAULT_CHARSET, &filename);

	/* sanity check */
	if(!filename || !*filename)
	{
		if(filename)
			XtFree(filename);
		return;
	}

	/* get item data */
	XtVaGetValues(dialog, XmNuserData, &item, NULL);

	if(item == FILE_OPEN)
	{
		/* find the file */
		file = resolveFile(filename);
		XtFree(filename);

		if(file == NULL)
			return;

		/* load the file, will also update the document cache */
		loadAndOrJump(file, NULL, True);

		XFlush(XtDisplay(widget));
		free(file);
	}
	else if(item == FILE_SAVEAS)
	{
		FILE *fp;
		/* get parser output */
		String buffer = XmHTMLTextGetString(html_widgets[0].html);

		if(buffer)
		{
			if((fp = fopen(filename, "w")) == NULL)
				perror(filename);
			else
			{
				fputs(buffer, fp);
				fputs("\n", fp);
				fclose(fp);
			}
			XtFree(buffer);
		}
		XtFree(filename);
	}
}

/*****
* Name: 		fileDialog
* Return Type: 	void
* Description: 	creates & displays a fileSelectionDialog to open or save
*				a file
* In: 
*	parent:		parent for the fileSelectionDialog;
*	what:		indentifies type of dialog to be used;
* Returns:
*	nothing.
*****/
static void
fileDialog(Widget parent, int what)
{
	static Widget dialog;

	if(!dialog)
	{
		Arg args[1];
		XmString xms;

		/* set the file selection pattern */
		xms = XmStringCreateLocalized("*.html");
		XtSetArg(args[0], XmNpattern, xms);

		/* create the dialog */
		dialog = XmCreateFileSelectionDialog(parent, "fileOpen", args, 1);

		/* no longer needed, free it */
		XmStringFree(xms);

		/* 
		* register the dialog itself as callback data, so we can pop it down
		* when pressed on the OK button.
		*/
		XtAddCallback(dialog, XmNokCallback, 
			(XtCallbackProc)readFile, dialog);
		XtAddCallback(dialog, XmNcancelCallback, 
			(XtCallbackProc)XtUnmanageChild, NULL);
	}
	if(what == FILE_SAVEAS)
		XtVaSetValues(XtParent(dialog), XmNtitle, "Save Document As", NULL);
	else
		XtVaSetValues(XtParent(dialog), XmNtitle, "Open a File", NULL);

	/* save item no as user data */
	XtVaSetValues(dialog, XmNuserData, (XtPointer)what, NULL);

	XtManageChild(dialog);
	XtPopup(XtParent(dialog), XtGrabNone);
	XMapRaised(XtDisplay(dialog), XtWindow(XtParent(dialog)));
}

/*****
* Name: 		fileCB
* Return Type: 	void
* Description: 	callback for the File->open menu item. Pops up a 
*				fileSelection dialog.
* In: 
*	widget:		widget id
*	item:		id of selected menu item.
* Returns:
*	nothing.
*****/
static void
fileCB(Widget widget, XtPointer client_data, XtPointer call_data)
{
	int item_no = MenuButtonGetId(widget);

	switch(item_no)
	{
		case FILE_QUIT:
			{
				int i, j;
				/* clear all text */
				XmHTMLTextSetString(html_widgets[0].html, "");

				/*
				* Just in case we have been using a palette, clear it by
				* disabling dithering.
				*/
				XtVaSetValues(html_widgets[0].html,
					XmNimageMapToPalette, XmDISABLED, NULL);

				/* kill frame widgets */
				for(i = 1; i < MAX_HTML_WIDGETS;i++)
				{
					if(html_widgets[i].html)
						XtDestroyWidget(html_widgets[i].html);
					if(html_widgets[i].name)
						free(html_widgets[i].name);
					if(html_widgets[i].src)
						free(html_widgets[i].src);
				}
#ifdef DEBUG
				/* clear fonts */
				_XmHTMLUnloadFonts((XmHTMLWidget)html_widgets[0].html);
#endif

				/* kill all doc stuff */
				for(j = 0; j < last_doc; j++)
				{
					/* free hrefs */
					for(i = 0; i < doc_cache[j].nrefs; i++)
					{
						if(doc_cache[j].refs[i])
							free(doc_cache[j].refs[i]);
						doc_cache[j].refs[i] = NULL;
					}

					/* free image url's */
					for(i = 0; i < doc_cache[j].nimages; i++)
					{
						free(doc_cache[j].images[i]);
						doc_cache[j].images[i] = NULL;
					}

					/* free visited anchor list */
					for(i = 0; i < doc_cache[j].nvisited; i++)
					{
						if(doc_cache[j].visited[i])
							free(doc_cache[j].visited[i]);
						doc_cache[j].visited[i] = NULL;
					}
					/* free file and path fields */
					free(doc_cache[j].file);
					free(doc_cache[j].path);
				}
				/* kill all images */
				killImages();

				XtDestroyWidget(html_widgets[0].html);
			
				printf("Bye!\n");
				exit(EXIT_SUCCESS);
			}
			break;
		case FILE_RELOAD:
			{
				Cardinal topline = 0;

				/* get current text position */
				XtVaGetValues(html_widgets[0].html, XmNtopLine, &topline, NULL);

				/* clear current contents to force a *real* reload */
				XmHTMLTextSetString(html_widgets[0].html, NULL);

				/* reload the current file */
				getAndSetFile(doc_cache[current_doc].file, NULL, False);

				/* and restore the current text position. */
				XmHTMLTextScrollToLine(html_widgets[0].html, topline);
			}
			return;
		case FILE_OPEN:
		case FILE_SAVEAS:
			fileDialog(XtParent(widget), item_no);
			break;
		default:
			break;
	}
}

/*****
* Name: 		windowCB
* Return Type: 	void
* Description: 	callback for the Window menu. Raises or lowers the
*				application's window
* In: 
*	widget:		selected menu button
*	client_..:	data registered for this callback;
*	call_data:	callback specific data, unused.
* Returns:
*	nothing.
* Note:
*	only used when the -root_window command line option was used.
*****/
static void
windowCB(Widget w, XtPointer client_data, XtPointer call_data)
{
	Widget widget = (Widget)client_data;
	int item_no = MenuButtonGetId(widget);

	switch(item_no)
	{
		case WINDOW_RAISE:
			XRaiseWindow(XtDisplay(widget), XtWindow(widget));
			return;
		case WINDOW_LOWER:
			XLowerWindow(XtDisplay(widget), XtWindow(widget));
			return;
	}
}

/*****
* Name: 		armCB
* Return Type: 	void
* Description: 	XmHTML's XmNarmCallback handler
* In: 
*	w:			widget id;
*	arg1:		client_data, unused;
*	href_data:	callback data;
* Returns:
*	nothing.
* Note:
*	this routine is only used when this example is being run with the -root
*	option and propagates the received event (available as href_data->event)
*	to the window manager.
*****/
static void
armCB(Widget w, XtPointer arg1, XmAnyCallbackStruct *href_data)
{
	XButtonEvent *event;

	/* security check, armCallback can be added using other methods
	* than XtAddCallback (editres for example)
	*/
	if(root_window)
	{
		event = (XButtonEvent*)href_data->event;
		event->window = DefaultRootWindow(XtDisplay(w));
		event->root = DefaultRootWindow(XtDisplay(w));
		event->subwindow = DefaultRootWindow(XtDisplay(w));
		event->send_event = True;

		XUngrabPointer(XtDisplay(w), CurrentTime);
		XSendEvent(XtDisplay(w), DefaultRootWindow(XtDisplay(w)),
			True, ButtonPressMask, (XEvent *)event);
		XFlush(XtDisplay(w));
	}
}

static void
callClient(URLType url_type, String url)
{
  printf("callClient reached\n");

  /*sergeyif(external_client)*/
  {
	char cmd[1024];
	if(url_type == ANCHOR_MAILTO)
	{
	  sprintf(cmd, "mozilla -remote 'mailto(%s)'", url);
	  printf("htmlHelp.c1: executing >%s<\n",cmd);
	}
	if(url_type == ANCHOR_NEWS)
	{
	  sprintf(cmd, "mozilla -remote 'news(%s)'", url);
	  printf("htmlHelp.c2: executing >%s<\n",cmd);
	}
	else
	{
	  sprintf(cmd, "mozilla -remote 'openURL(%s)'", url);
	  printf("htmlHelp.c3: executing >%s<\n",cmd);
	}

	if(!(fork()))
	{
	  if(execl("/bin/sh", "/bin/sh", "-c", cmd, NULL) == -1)
	  {
		fprintf(stderr, "execl failed (%s)",strerror(errno));
		exit(100);
	  }
	}
  }
}

/*****
* Ricky Ralston
*****/
static void
readPipe(Widget w, char *cmd)
{
	FILE *fd;
	char *buf=NULL;
	int val, offset;

	fd = popen(cmd, "r");
	fflush(fd);

	offset = 0;
	val = 0;

	buf = calloc(522, sizeof(char));
	strcpy(buf, "<html><body>\n");
	offset += strlen(buf);
	val = fread(buf+offset, 1, 512, fd);
	offset += 512;

	if (val == 512)
	{
		buf = realloc(buf, offset+512);
		while ((val = fread(buf+offset, 1,512, fd)) == 512)
		{
			offset += 512;
			buf = realloc(buf, offset+512);
		}
	}
	pclose(fd);

	buf = realloc(buf, strlen(buf)+20);
	strcat(buf, "\n</body></html>\n");

	XmHTMLTextSetString(w, buf);
	free(buf);
}

/*****
* Name: 		anchorCB
* Return Type: 	void
* Description: 	XmNactivateCallback handler for the XmHTML widget
* In: 
*	w:			html widget
*	arg1:		client_data, unused
*	href_data:	anchor data
* Returns:
*	nothing.
*****/
static void
anchorCB(Widget w, XtPointer arg1, XmHTMLAnchorPtr href_data)
{
	/* see if we have been called with a valid reason */
	if(href_data->reason != XmCR_ACTIVATE)
		return;

	switch(href_data->url_type)
	{
		/* a named anchor */
		case ANCHOR_JUMP:
			{
				int id;
				/* see if XmHTML knows this anchor */
				if((id = XmHTMLAnchorGetId(w, href_data->href)) != -1)
				{
					DocumentCache *this_doc = &doc_cache[current_doc];

					/* store href in history and visited anchor list... */
					storeInHistory(NULL, this_doc->refs[this_doc->current_ref]);
					storeInHistory(NULL, href_data->href);
					storeAnchor(href_data->href);

					/* ...and let XmHTML jump and mark as visited */
					href_data->doit = True;
					href_data->visited = True;
					return;
				}
				return;
			}
			break;

		/* a local file with a possible ID jump? */
		case ANCHOR_FILE_LOCAL:
		case ANCHOR_FILE_REMOTE:
			{
				String chPtr, file = NULL, loc = NULL;

				if((chPtr = strstr(href_data->href, "file:")) != NULL)
				{
					/*****
					* For the following cases, the file is local:
					* file:/some/path		-- no host
					* file:///some/path		-- NULL host
					* file://localhost/some/path -- localhost
					*****/
					chPtr += 5;	/* skip past file: */
					if(!strncasecmp(chPtr, "///", 3))
						chPtr += 3;
					else if (!strncasecmp(chPtr, "//localhost", 11))
						chPtr += 11;
					else if(*chPtr == '/' && *(chPtr+1) != '/')
						chPtr += 2;
					else
					{
						fprintf(stderr, "fetch remote file: %s\n",
							href_data->href);
						callClient(href_data->url_type, href_data->href);
						break;
					}
					/* local file to load */
					file = resolveFile(chPtr);

					/* pick out a possible jump */
					chPtr = strstr(href_data->href, "#");
				}
				else
				{
					/* first see if this anchor contains a jump */
					if((chPtr = strstr(href_data->href, "#")) != NULL)
					{
						char tmp[1024];
						strncpy(tmp, href_data->href, chPtr - href_data->href);
						tmp[chPtr - href_data->href] = '\0';
						/* try to find the file */
						file = resolveFile(tmp);
					}
					else
						file = resolveFile(href_data->href);
				}

				if(file == NULL)
					return;

				/* store href in visited anchor list */
				storeAnchor(href_data->href);

				/*****
				* All members in the XmHTMLAnchorCallbackStruct are
				* *pointers* to the contents of the current document.
				* So, if we will be changing the document, any members of this
				* structure become INVALID (in this case, any jump address to
				* a location in a different file). Therefore we *must* save
				* the members we might need after the document has changed.
				******/
				if(chPtr)
					loc = strdup(chPtr);

				/*****
				* If we have a target, call the frame loader, else call the
				* plain document loader.
				*****/
				if(href_data->target)
					jumpToFrame(file, loc, href_data->target, True);
				else
					loadAndOrJump(file, loc, True); 
				free(file);
				if(loc)
					free(loc);
			}
			break;

		case ANCHOR_EXEC:
			{
				char *ptr;
				char *cmd=NULL;

				Debug(("execute: %s\n", href_data->href));

				if((ptr = strstr(href_data->href, ":")) != NULL)
				{
					ptr++;
					cmd = strdup(ptr);
					cmd[strlen(cmd)] = '\0';
					if(!(fork()))
					{
						if(execl("/bin/sh", "/bin/sh", "-c", cmd, NULL) == -1)
						{
							fprintf(stderr, "execl failed (%s)",
								strerror(errno));
							exit(100);
						}
					}
					free(cmd);
				}
			}
			break;
			case ANCHOR_PIPE:
			{
				char *ptr;
				char *cmd=NULL;

				Debug(("pipe: %s\n", href_data->href));

				if((ptr = strstr(href_data->href, ":")) != NULL)
				{
					ptr++;
					cmd = strdup(ptr);
					cmd[strlen(cmd)] = '\0';
					readPipe(html_widgets[0].html, cmd);
					free(cmd);
				}
			}
			break;

		/* all other types are unsupported */
		case ANCHOR_FTP:
			fprintf(stderr, "fetch ftp file: %s\n", href_data->href);
			callClient(href_data->url_type, href_data->href);
			break;
		case ANCHOR_HTTP:
			fprintf(stderr, "fetch http file: %s\n", href_data->href);
			callClient(href_data->url_type, href_data->href);
			break;
		case ANCHOR_GOPHER:
			fprintf(stderr, "gopher: %s\n", href_data->href);
			callClient(href_data->url_type, href_data->href);
			break;
		case ANCHOR_WAIS:
			fprintf(stderr, "wais: %s\n", href_data->href);
			callClient(href_data->url_type, href_data->href);
			break;
		case ANCHOR_NEWS:
			fprintf(stderr, "open newsgroup: %s\n", href_data->href);
			callClient(href_data->url_type, href_data->href);
			break;
		case ANCHOR_TELNET:
			fprintf(stderr, "open telnet connection: %s\n", href_data->href);
			callClient(href_data->url_type, href_data->href);
			break;
		case ANCHOR_MAILTO:
			fprintf(stderr, "email to: %s\n", href_data->href);
			callClient(href_data->url_type, href_data->href);
			break;
		case ANCHOR_UNKNOWN:
		default:
			fprintf(stderr, "don't know this type of url: %s\n", 
				href_data->href);
			break;
	}
}

/*****
* Name: 		formCB
* Return Type: 	void
* Description: 	XmHTML XmNformCallback demonstration. Only allows for the
*				PIPE action. All values for form components are put in
*				the environment before the action is carried out (readPipe does
*				that).
* In: 
*
* Returns:
*	nothing.
*****/
static void
formCB(Widget w, XtPointer loadWidget, XmHTMLFormCallbackStruct *cbs)
{
	char **env=NULL;
	char *name=NULL;
	char *value=NULL;
	int i;

	if(cbs->method != XmHTML_FORM_PIPE)
		return;

	env = (char **) realloc((char *)env, sizeof(char *)*(cbs->ncomponents+2));
	for (i = 0; i < cbs->ncomponents; i++)
	{
		name = strdup(cbs->components[i].name);
		name[strlen(name)]='\0';
		value = strdup(cbs->components[i].value);
		value[strlen(value)]='\0';
		env[i] = (String)calloc(strlen(name)+strlen(value)+4, sizeof(char));
		sprintf(env[i],"%s=%s", name, value);
		free(name);
		free(value);
		putenv(env[i]);
	}

	env[i]=NULL;

	/*****
	* carry out the requested action.
	* As readPipe sets a new document in the html widget, any values in the
	* callback structure have become *INVALID* when readPipe returns. Therefore
	* we can't use the components array to unset the environment settings but
	* pick them up from the stored environment.
	* If we don't unset the environment variables it's possible that the
	* app runs out of environment space quite easily.
	*****/
	readPipe(w, cbs->action);

	for(i=0; env[i]; i++)
	{
		/* pick up environment name */
		name = strtok(env[i], "=");
		/*****
		* SVID says that when NAME is put in the environment without a ``='',
		* it'll be removed from the environment.
		* Using strtok on the equal sign nullifies that sign so it'll be
		* removed.
		*****/
		putenv(name);
		/* remove it */
		free(env[i]);
	}
	free(env);
}

/*****
* Name: 		setFrameText
* Return Type: 	void
* Description: 	loads the given file in the given HTML frame;
* In: 
*	widget:		XmHTML widget id in which to load the file;
*	filename:	file to be loaded;
*	loc:		selected position in the file. If non-NULL this routine will
*				scroll to the given location.
* Returns:
*	nothing.
*****/
static void
setFrameText(Widget frame, String filename, String loc)
{
	setBusy(toplevel, True);

	if(filename)
	{
		XmString xms;
		String buf, file, mime;
		file = resolveFile(filename);
		if(file == NULL || (buf = loadFile(file, &mime)) == NULL)
		{
			setBusy(toplevel, False);
			return;
		}

		/* set name of current file in the label */
		xms = XmStringCreateLocalized(file);
		XtVaSetValues(label,
			XmNlabelString, xms,
			NULL);
		XmStringFree(xms);

		/* kill of any outstanding progressive image loading contexts */
		XmHTMLImageProgressiveKill(frame);

		/* set the text in the widget */
		XtVaSetValues(frame,
			XmNvalue, buf,
			XmNmimeType, mime,
			NULL);
		free(buf);
		free(file);
	}

	/* jump to the requested anchor in this file or to top of the document */
	if(loc) {
	  printf("loc is %s\n",loc);
	  XmHTMLAnchorScrollToName(frame, loc);
	} else
		XmHTMLTextScrollToLine(frame, 0);

	setBusy(toplevel, False);
}

/*****
* Name: 		jumpToFrame
* Return Type: 	void
* Description:	loads the contents of the given file in a named frame
* In: 
*	filename:	name of file to load;
*	loc:		url of file;
*	target:		name of frame in which to load the file;
*	store:		flag for history storage;
* Returns:
*	nothing.
*****/
static void
jumpToFrame(String filename, String loc, String target, Boolean store)
{
	int i;

	/* html_widgets[0] is the master XmHTML Widget, never framed */
	for(i = 1; i < MAX_HTML_WIDGETS; i++)
	{
		if(html_widgets[i].active && !(strcmp(html_widgets[i].name, target)))
		{
			/*
			* Load new file into frame if it's not the same as the current 
			* src value for this frame.
			*/
			if(html_widgets[i].src && strcmp(html_widgets[i].src, filename))
			{
				free(html_widgets[i].src);
				html_widgets[i].src = strdup(filename);
				setFrameText(html_widgets[i].html, filename, loc);
			}
			else	/* same file, jump to requested location */
				setFrameText(html_widgets[i].html, NULL, loc);
			return;
		}
	}
	/* frame not found, use the toplevel HTML widget */
	if(i == MAX_HTML_WIDGETS)
		loadAndOrJump(filename, loc, store); 
}

/*****
* Name: 		focusCB
* Return Type: 	void
* Description: 	callback for XmHTML's focusCallback and losingFocusCallback
* In: 
*	w:			widget id;
*	client_..:	data registered on this callback, unused;
*	cbs:		callback data provided by calling widget;
* Returns:
*
*****/
static void
focusCB(Widget w, XtPointer client_data, XmAnyCallbackStruct *cbs)
{
	if(cbs->reason == XmCR_FOCUS)
	{
		/* set active widget */
	}
	else if(cbs->reason == XmCR_LOSING_FOCUS)
	{

	}
}

/* external gif decoder */
#include "cinclude/gif_decode.c"

/*****
* Name: 		frameCB
* Return Type: 	void
* Description: 	callback for XmHTML's XmNframeCallback
* In: 
*	w:			widget id;
*	arg1:		client_data, unused;
*	cbs:		data about the frame being created/destroyed/notified of 
*				creation.
* Returns:
*	nothing.
*****/
static void
frameCB(Widget w, XtPointer arg1, XmHTMLFramePtr cbs)
{
	int i;

	if(cbs->reason == XmCR_HTML_FRAME)
	{
		Widget html = cbs->html;

		/* find the first free slot where we can insert this frame */
		for(i = 0; i < MAX_HTML_WIDGETS;i++)
			if(!html_widgets[i].active)
				break;

		/* a frame always has a name */
		html_widgets[i].name = strdup(cbs->name);
		if(cbs->src)
			html_widgets[i].src = resolveFile(cbs->src);

		/* anchor callback */
		XtAddCallback(html, XmNactivateCallback,
			(XtCallbackProc)anchorCB, NULL);

		/* arm callback */
		if(root_window)
			XtAddCallback(html, XmNarmCallback, (XtCallbackProc)armCB, NULL);

		/* anchor tracking callback */
		XtAddCallback(html, XmNanchorTrackCallback,
			(XtCallbackProc)trackCB, NULL);

		/* HTML document callback */
		XtAddCallback(html, XmNdocumentCallback, (XtCallbackProc)docCB, NULL);

		/* Formcallback for playing with pipe/forms within frames */
		XtAddCallback(html, XmNformCallback, (XtCallbackProc)formCB, NULL);

#if 0
		/*****
		* Doesn't work, event's don't get propagated.
		* Have to figure out why this happens.
		*****/

		/* popup info menu */
		if(!html_widgets[i].has_popup)
		{
			Widget work_win, pb, popup_menu;
			int k;

			html_widgets[i].has_popup = True;
			XtVaGetValues(html, XmNworkWindow, &work_win, NULL);
			popup_menu = XmCreatePopupMenu(work_win, "infoPopup", NULL, 0);
			for(k = 0; k < 4; k++)
			{
				pb = XtVaCreateManagedWidget("popupButton",
					xmPushButtonGadgetClass, popup_menu, NULL);
				XtAddCallback(pb, XmNactivateCallback,
					(XtCallbackProc)infoPopupCB, (XtPointer)k);
			}
			XtAddEventHandler(work_win, ButtonPressMask, 0,
				(XtEventHandler)infoCB, popup_menu);

			/* and remove the popup menu from the main HTML menu */
			if(html_widgets[0].has_popup)
			{
				XtVaGetValues(html_widgets[0].html, XmNworkWindow,
					&work_win, NULL);
				XtRemoveEventHandler(work_win, ButtonPressMask, 0,
					(XtEventHandler)infoCB, info_popup);
				html_widgets[0].has_popup = False;
			}
		}
#endif

		/* focus callbacks so we can track which widget is current */
		XtAddCallback(html, XmNfocusCallback, (XtCallbackProc)focusCB, NULL);

		XtAddCallback(html, XmNlosingFocusCallback, (XtCallbackProc)focusCB,
			NULL);

		/* set other things we want to have */
		XtVaSetValues(html,
			XmNanchorVisitedProc, testAnchor,
			XmNimageProc, loadImage,
			XmNprogressiveReadProc, getImageData,
			XmNprogressiveEndProc, endImageData,
#ifdef HAVE_GIF_CODEC
			XmNdecodeGIFProc, decodeGIFImage,
#endif
			/* propagate current defaults */
			XmNanchorButtons,
				html_config[OPTIONS_ANCHOR_BUTTONS].value,
			XmNhighlightOnEnter,
				html_config[OPTIONS_ANCHOR_HIGHLIGHT].value,
			XmNenableBadHTMLWarnings,
				html_config[OPTIONS_BADHTML].value,
			XmNstrictHTMLChecking,
				html_config[OPTIONS_STRICTHTML].value,
			XmNenableBodyColors,
				html_config[OPTIONS_BODY_COLORS].value,
			XmNenableBodyImages,
				html_config[OPTIONS_BODY_IMAGES].value,
			XmNenableDocumentColors,
				html_config[OPTIONS_ALLOW_COLORS].value,
			XmNenableDocumentFonts,
				html_config[OPTIONS_ALLOW_FONTS].value,
			XmNenableOutlining,
				html_config[OPTIONS_JUSTIFY].value,
			XmNfreezeAnimations,
				html_config[OPTIONS_ANIMATION_FREEZE].value,
			XmNimageEnable,
				html_config[OPTIONS_ENABLE_IMAGES].value,
			NULL);

		/* store widget id */
		html_widgets[i].html = html;
		html_widgets[i].active = True;

		/* set source text */
		setFrameText(html_widgets[i].html, html_widgets[i].src, NULL);

		return;
	}
	if(cbs->reason == XmCR_HTML_FRAMECREATE)
	{
		/* see if we have an inactive frame in our frame cache */
		for(i = 0; i < MAX_HTML_WIDGETS;i++)
			if(!html_widgets[i].active && !html_widgets[i].used)
				break;

		/* we have an available slot */
		if(i != MAX_HTML_WIDGETS && html_widgets[i].html != NULL)
		{
			cbs->doit = False;
			cbs->html = html_widgets[i].html;
			/* this slot is being used */
			html_widgets[i].used = True;
		}
		/*
		* this is the appropriate place for doing frame reuse: set
		* the doit field in the callback structure to False and set the
		* id of a *HTML* widget in the html field.
		*/
		return;
	}
	if(cbs->reason == XmCR_HTML_FRAMEDESTROY)
	{
		int freecount = 0;
		/*
		* this is the appropriate place for keeping this widget: just set
		* the doit field to false, update the frame cache (if this frame is
		* going to be reused, it's name and src value will probably change).
		*/
		for(i = 0; i < MAX_HTML_WIDGETS; i++)
		{
			if(!html_widgets[i].active)
				freecount++;
			if(html_widgets[i].html == cbs->html)
			{
				freecount++;
				/* a frame has always got a name */
				free(html_widgets[i].name);
				if(html_widgets[i].src)
					free(html_widgets[i].src);
				html_widgets[i].name = NULL;
				html_widgets[i].src = NULL;
				/* we keep up to three frames in memory */
				if(freecount > 3)
				{
					html_widgets[i].html = NULL;
					/* popup menu will also be destroyed then */
					html_widgets[i].has_popup = False;
				}
				html_widgets[i].active = False;
				html_widgets[i].used = False;
				break;
			}
		}
		/* and add the popup menu to the main HTML window */
		if(!html_widgets[0].has_popup)
		{
			Widget work_win;
			XtVaGetValues(html_widgets[0].html, XmNworkWindow,
				&work_win, NULL);
			XtAddEventHandler(work_win, ButtonPressMask, 0,
				(XtEventHandler)infoCB, info_popup);
			html_widgets[0].has_popup = True;
		}
		if(freecount < 4)
			cbs->doit = False;
		return;
	}
	/* do nothing */
	return;
}

/*****
* Name: 		trackCB
* Return Type: 	void
* Description: 	displays the URL when the pointer is moved over an anchor
* In: 
*	html:		owner of this callback
*	arg1:		client_data, unused
*	href_data:	call_data, anchor callback structure.
* Returns:
*	nothing
*****/
static void
trackCB(Widget w, XtPointer arg1, XmHTMLAnchorPtr href_data)
{
	XmString xms;
	static char anchor_label[128];
	static Boolean cleared = False;
	static Widget balloon;
	XmHTMLInfoPtr info;
	XButtonEvent event = href_data->event->xbutton;
	int len;

	/* see if we have been called with a valid reason */
	if(href_data->reason != XmCR_HTML_ANCHORTRACK)
		return;

	/* a valid anchor, eg, moving into an anchor */
	if(href_data->href)
	{
		/* see if the new label isn't the previous label */
		if(anchor_label[0] != '\0')
		{
			/* first see if an alternate title has been given */
			if(href_data->title && !strcmp(anchor_label, href_data->title))
				return;
			if(!strcmp(anchor_label, href_data->href))
				return;
		}

		/* create a new label */
		if(href_data->title)
		{
			strncpy(anchor_label, href_data->title, 128);
			len = strlen(href_data->title);

			/*****
			* Create an XmBalloonWidget to display the title in a small
			* popup window.
			*****/
			if(html_config[OPTIONS_ANCHOR_TIPS].value)
			{
				if(balloon == NULL)
				{
					Arg args[5];
					/* automatically popdown after 4 seconds */
					XtSetArg(args[0], XmNpopdownDelay, 4000);
					balloon = XmCreateBalloon(w, "balloonHelp", args, 1);
				}
				/*****
				* Using 0,0 as position will place the balloon under the
				* current pointer position.
				*****/
				XmBalloonPopup(balloon, 0, 0, href_data->title);
			}
		}
		else
		{
			strncpy(anchor_label, href_data->href, 128);
			len = strlen(href_data->href);
			/*****
			* Create an XmBalloonWidget to display the title in a small
			* popup window.
			*****/
			if(html_config[OPTIONS_ANCHOR_TIPS].value)
			{
				if(balloon == NULL)
				{
					Arg args[5];
					/* automatically popdown after 4 seconds */
					XtSetArg(args[0], XmNpopdownDelay, 4000);
					balloon = XmCreateBalloon(w, "balloonHelp", args, 1);
				}
				/*****
				* Using 0,0 as position will place the balloon under the
				* current pointer position.
				*****/
				XmBalloonPopup(balloon, 0, 0, anchor_label);
			}
			/* no title on current label, pop balloon down 
			if(html_config[OPTIONS_ANCHOR_TIPS].value && balloon)
				XmBalloonPopdown(balloon);*/
		}
		/* don't overflow */
		anchor_label[len > 127 ? 127 : len] = '\0';
		xms = XmStringCreateLocalized(anchor_label);
		XtVaSetValues(label,
			XmNlabelString, xms,
			NULL);
		XmStringFree(xms);
		cleared = False;
	}
	/* a valid anchor, eg, moving away from an anchor */
	else
	{
		anchor_label[0] = '\0'; /* invalidate previous anchor selection */
		if(!cleared)
		{
			/* remove previous label */
			xms = XmStringCreateLocalized("  ");
			XtVaSetValues(label,
				XmNlabelString, xms,
				NULL);
			XmStringFree(xms);
		}
		cleared = True;

		if(html_config[OPTIONS_ANCHOR_TIPS].value && balloon)
			XmBalloonPopdown(balloon);
	}
	/* image dialog not available, ignore */
	if(!html_config[OPTIONS_ANCHOR_TRACKING].value ||
		!image_dialog || !XtIsManaged(image_dialog))
		return;

	/* get the info for the selected position */
	event = href_data->event->xbutton;
	info = XmHTMLXYToInfo(w, event.x, event.y); 

	/* not over an image anchor or still the same anchor, ignore */
	if(info == NULL || info->image == NULL ||
		(preview_image && !strcmp(info->image->url, preview_image->file)))
		return;

	/* We moved onto a new image, flip it */
	showImageInfo(info->image);
}

/*****
* Name: 		metaListCB
* Return Type: 	void
* Description: 	callback for the list in the document information dialog.
*				Displays the data associated with the selected item in the
*				list.
* In: 
*	w:			list widget id;
*	edit:		widget id of text widget in which data will be displayed.
* Returns:
*	nothing.
*****/
static void
metaListCB(Widget w, Widget edit)
{
	int *pos_list;	/* selected list position */
	int pos_cnt, selected;	/* no of selected items */
	XmHTMLMetaDataPtr meta_data = NULL;

	if(!(XmListGetSelectedPos(w, &pos_list, &pos_cnt)))
		/* no item selected */
		return;

	selected = pos_list[0];
	/* list positions start at 1 instead of zero, so be sure to adjust */
	selected--;

	/* get meta data out of the list's userData */
	XtVaGetValues(w, XmNuserData, &meta_data, NULL);

	/* very serious error */
	if(meta_data == NULL)
	{
		fprintf(stderr, "Could not retrieve meta-data from userData field!\n");
		free(pos_list);
		return;
	}

	/* and put the selected string in the edit field */
	XmTextSetString(edit, meta_data[selected].content);
	free(pos_list);
}

int
#ifdef __STDC__
	my_sprintf(String *dest, int *size, int *max_size, String fmt, ...)
#else /* ! __STDC__ */
	my_sprintf(String *dest, int *size, int *max_size, String fmt, va_alist)
	String *dest;
	int *size;
	int *max_size;
	String fmt;
	va_dcl
#endif /* __STDC__ */
{
	int len;
	String s;
#ifdef __STDC__
	va_list	arg_list;
#else
	va_alist arg_list
#endif

	if(*max_size - *size < 1024) 
	{
		*max_size += 1024;
		/* realloc(NULL, size) ain't exactly portable */
		if(*max_size == 1024)
			s = (char *)malloc(*max_size);
		else
			s = (char *)realloc(*dest, *max_size);
		*dest = s;
	}
#ifdef __STDC__
	va_start(arg_list, fmt);
#else
	va_start(arg_list);
#endif
	len = vsprintf(*dest + *size, fmt, arg_list);
	va_end(arg_list);

	/* new size of destination buffer */
	if(len != 0) 
		*size += strlen(*dest + *size);
	return(len);
}

static void
viewSource(Widget parent, int what)
{
	static Widget viewer, html;
	String source = NULL;

	if(viewer == NULL)
	{
		viewer = XmCreateFormDialog(parent, "DocSource",
			NULL, 0);
		html = XtVaCreateManagedWidget("sourceView", 
			xmHTMLWidgetClass, viewer,
			XmNtopAttachment, XmATTACH_FORM,
			XmNleftAttachment, XmATTACH_FORM,
			XmNbottomAttachment, XmATTACH_FORM,
			XmNrightAttachment, XmATTACH_FORM,
			XmNmarginWidth, 5,
			XmNmarginHeight, 5,
			NULL);
	}
	if(what == VIEW_SOURCE)
	{
		if((source = XmHTMLTextGetSource(html_widgets[0].html)) == NULL)
			return;

		XtVaSetValues(html, XmNvalue, source, XmNmimeType, "text/plain", NULL);

		XtVaSetValues(XtParent(viewer),
			XtNtitle, "Document Source",
			NULL);
	}
	else if(what == VIEW_FONTS)
	{
		XmHTMLFontCacheInfo *info;
		int i, curr_size = 0, max_size = 0;

		if((info = XmHTMLGetFontCacheInfo(html_widgets[0].html)) == NULL)
			return;

		XtVaSetValues(XtParent(viewer),
			XtNtitle, "XmHTML Font Cache Information",
			NULL);

		/* create HTML document */
		my_sprintf(&source, &curr_size, &max_size, "<html><head><title>XmHTML "
			"Font Cache Info</title></head><body>");
		my_sprintf(&source, &curr_size, &max_size, "<h3>Font Cache "
			"Statistics</h3><blockquote>Widgets refererring this cache: %i<br>",
			info->nwidgets);
		my_sprintf(&source, &curr_size, &max_size, "%i fonts cached "
			"(%i mappings)<br>", info->nentries, info->nmaps);
		my_sprintf(&source, &curr_size, &max_size, "requests : %i<br>",
			info->nrequests);
		my_sprintf(&source, &curr_size, &max_size, "lookups  : %i<br>",
			info->nlookups);
		my_sprintf(&source, &curr_size, &max_size, "hits	 : %i<br>",
			info->hits);
		my_sprintf(&source, &curr_size, &max_size, "misses   : %i<br>",
			info->misses);
		my_sprintf(&source, &curr_size, &max_size, "hit ratio: %.2f%%<br>",
			(float)(info->hits*100./(float)info->nrequests));
		my_sprintf(&source, &curr_size, &max_size, "Average lookups per font: "
			"%.2f<br>", (float)(info->nlookups/(float)info->hits));
		my_sprintf(&source, &curr_size, &max_size, "</blockquote><p><h3>List "
			"of cached fonts</h3><blockquote><pre>\n");
		for(i = 0; i < info->nentries; i++)
		{
			if(info->mapping[i] != NULL)
			{
				my_sprintf(&source, &curr_size, &max_size, "%s\n\tmapped to: "
					"%s\n", info->fonts[i], info->mapping[i]);
			}
			else
			{
				my_sprintf(&source, &curr_size, &max_size, "%s\n",
					info->fonts[i]);
			}
		}
		my_sprintf(&source, &curr_size, &max_size, "</pre></blockquote>"
			"</body></html>");

		/* set text */
		XtVaSetValues(html, XmNmimeType, "text/html", NULL);
		XmHTMLTextSetString(html, source);

		/* free source, no longer needed */
		free(source);

		/* release cache info */
		XmHTMLFreeFontCacheInfo(info);
	}
	else
		return;

	/* put on screen */
	XtManageChild(viewer);
	XMapRaised(XtDisplay(viewer), XtWindow(viewer));
}

/*****
* Name: 		viewDocumentInfo
* Return Type: 	void
* Description: 	shows a dialog with document information. This routine gets
*				activated when the file->document info menu item is selected.
* In: 
*	parent:		parent for this dialog
* Returns:
*	nothing.
*****/
static void
viewDocumentInfo(Widget parent)
{
	static Widget title_label, author_label, base_label, doctype_label;
	static Widget list, edit;
	XmString xms;
	static XmHTMLHeadAttributes head_info;
	Boolean have_info = False;
	String tmp;
	int i;

	/*****
	* Get <head></head> information from the current document. XmHTML will
	* take care of replacing the requested members when they have been used
	* before. When no <head></head> is available this function returns false
	* Note: the value of the <!DOCTYPE> member is always returned if there
	* is one, even if GetHeadAttributes() returns False.
	*****/
	have_info = XmHTMLGetHeadAttributes(html_widgets[0].html, &head_info,
					HeadDocType|HeadTitle|HeadBase|HeadMeta);

	if(!info_dialog)
	{
		Widget sep, rc1, rc2, fr1, fr2;
		Arg args[20];
		int argc = 0;

		info_dialog = XmCreateFormDialog(parent, "documentAttributes",
			NULL, 0);

		XtVaSetValues(XtParent(info_dialog),
			XtNtitle, "Document Attributes",
			NULL);
		fr1 = XtVaCreateManagedWidget("documentTitleForm",
			xmFormWidgetClass, info_dialog,
			XmNtopAttachment, XmATTACH_FORM,
			XmNleftAttachment, XmATTACH_FORM,
			XmNrightAttachment, XmATTACH_FORM,
			NULL);

		sep = XtVaCreateManagedWidget("documentSeperator",
			xmSeparatorGadgetClass, info_dialog,
			XmNorientation, XmHORIZONTAL,
			XmNtopAttachment, XmATTACH_WIDGET,
			XmNtopWidget, fr1,
			XmNleftAttachment, XmATTACH_FORM,
			XmNrightAttachment, XmATTACH_FORM,
			XmNleftOffset, 10,
			XmNrightOffset, 10,
			XmNtopOffset, 10,
			NULL);

		fr2 = XtVaCreateManagedWidget("documentTitleForm",
			xmFormWidgetClass, info_dialog,
			XmNtopAttachment, XmATTACH_WIDGET,
			XmNtopWidget, sep,
			XmNleftAttachment, XmATTACH_FORM,
			XmNrightAttachment, XmATTACH_FORM,
			XmNbottomAttachment, XmATTACH_FORM,
			NULL);

		rc1 = XtVaCreateManagedWidget("documentTitleLeftRow",
			xmRowColumnWidgetClass, fr1,
			XmNtopAttachment, XmATTACH_FORM,
			XmNleftAttachment, XmATTACH_FORM,
			XmNbottomAttachment, XmATTACH_FORM,
			XmNpacking, XmPACK_COLUMN,
			XmNorientation, XmVERTICAL,
			XmNnumColumns, 1,
			XmNtopOffset, 10,
			XmNleftOffset, 10,
			XmNrightOffset, 10,
			NULL);
		XtVaCreateManagedWidget("Document Title:",
			xmLabelGadgetClass, rc1,
			XmNalignment, XmALIGNMENT_BEGINNING,
			NULL);
		XtVaCreateManagedWidget("Author:",
			xmLabelGadgetClass, rc1,
			XmNalignment, XmALIGNMENT_BEGINNING,
			NULL);
		XtVaCreateManagedWidget("Document type:",
			xmLabelGadgetClass, rc1,
			XmNalignment, XmALIGNMENT_BEGINNING,
			NULL);
		XtVaCreateManagedWidget("Base Location:",
			xmLabelGadgetClass, rc1,
			XmNalignment, XmALIGNMENT_BEGINNING,
			NULL);

		rc2 = XtVaCreateManagedWidget("documentTitleRightRow",
			xmRowColumnWidgetClass, fr1,
			XmNtopAttachment, XmATTACH_FORM,
			XmNleftAttachment, XmATTACH_WIDGET,
			XmNleftWidget, rc1,
			XmNrightAttachment, XmATTACH_FORM,
			XmNbottomAttachment, XmATTACH_FORM,
			XmNpacking, XmPACK_COLUMN,
			XmNorientation, XmVERTICAL,
			XmNnumColumns, 1,
			XmNtopOffset, 10,
			XmNleftOffset, 10,
			XmNrightOffset, 10,
			NULL);
		title_label = XtVaCreateManagedWidget("docTitle",
			xmLabelGadgetClass, rc2,
			XmNalignment, XmALIGNMENT_BEGINNING,
			NULL);
		author_label = XtVaCreateManagedWidget("docAuthor",
			xmLabelGadgetClass, rc2,
			XmNalignment, XmALIGNMENT_BEGINNING,
			NULL);
		doctype_label = XtVaCreateManagedWidget("docType",
			xmLabelGadgetClass, rc2,
			XmNalignment, XmALIGNMENT_BEGINNING,
			NULL);
		base_label = XtVaCreateManagedWidget("docBase",
			xmLabelGadgetClass, rc2,
			XmNalignment, XmALIGNMENT_BEGINNING,
			NULL);
		XtSetArg(args[argc], XmNlistSizePolicy, XmRESIZE_IF_POSSIBLE); argc++;
		XtSetArg(args[argc], XmNvisibleItemCount, 5); argc++;
		XtSetArg(args[argc], XmNleftAttachment, XmATTACH_FORM); argc++;
		XtSetArg(args[argc], XmNtopAttachment, XmATTACH_FORM); argc++;
		XtSetArg(args[argc], XmNbottomAttachment, XmATTACH_FORM); argc++;
		XtSetArg(args[argc], XmNleftOffset, 10); argc++;
		XtSetArg(args[argc], XmNtopOffset, 10); argc++;
		XtSetArg(args[argc], XmNbottomOffset, 10); argc++;
		list = XmCreateScrolledList(fr2, "metaList", args, argc);

		argc = 0;
		XtSetArg(args[argc], XmNscrollBarDisplayPolicy, XmAS_NEEDED); argc++;
		XtSetArg(args[argc], XmNscrollingPolicy, XmAUTOMATIC); argc++;
		XtSetArg(args[argc], XmNleftAttachment, XmATTACH_WIDGET); argc++;
		XtSetArg(args[argc], XmNleftWidget, list); argc++;
		XtSetArg(args[argc], XmNtopAttachment, XmATTACH_FORM); argc++;
		XtSetArg(args[argc], XmNbottomAttachment, XmATTACH_FORM); argc++;
		XtSetArg(args[argc], XmNrightAttachment, XmATTACH_FORM); argc++;
		XtSetArg(args[argc], XmNleftOffset, 10); argc++;
		XtSetArg(args[argc], XmNrightOffset, 10); argc++;
		XtSetArg(args[argc], XmNtopOffset, 10); argc++;
		XtSetArg(args[argc], XmNbottomOffset, 10); argc++;
		edit = XmCreateScrolledText(fr2, "metaEdit", args, argc);

		argc = 0;
		XtSetArg(args[argc], XmNeditable, False); argc++;
		XtSetArg(args[argc], XmNcolumns, 35); argc++;
		XtSetArg(args[argc], XmNrows, 5); argc++;
		XtSetArg(args[argc], XmNwordWrap, True); argc++;
		XtSetArg(args[argc], XmNeditMode, XmMULTI_LINE_EDIT); argc++;
		XtSetArg(args[argc], XmNscrollHorizontal, False); argc++;
		XtSetArg(args[argc], XmNscrollVertical, False); argc++;
		XtSetValues(edit, args, argc);

		/* single selection callback on the list */
		XtAddCallback(list, XmNbrowseSelectionCallback,
			(XtCallbackProc)metaListCB, edit);

		XtManageChild(list);
		XtManageChild(edit);
	}

	/* delete all list items */
	XmListDeleteAllItems(list);
	/* clear all text in the edit window */
	XmTextSetString(edit, NULL);

	if(head_info.doctype)
		xms = XmStringCreateLocalized(head_info.doctype);
	else
		xms = XmStringCreateLocalized("<Unspecified>");
	XtVaSetValues(doctype_label, XmNlabelString, xms, NULL);
	XmStringFree(xms);

	if(have_info)
	{
		if(head_info.title)
			xms = XmStringCreateLocalized(head_info.title);
		else
			xms = XmStringCreateLocalized("<Untitled>");
		XtVaSetValues(title_label, XmNlabelString, xms, NULL);
		XmStringFree(xms);

		if(head_info.base)
			xms = XmStringCreateLocalized(head_info.base);
		else
			xms = XmStringCreateLocalized("<Not specified>");
		XtVaSetValues(base_label, XmNlabelString, xms, NULL);
		XmStringFree(xms);
		xms = NULL;

		/*
		* No need to check for font or charset in meta info, that's
		* already been done from within the documentCallback.
		*/
		if(head_info.num_meta)
		{
			for(i = 0; i < head_info.num_meta; i++)
			{
				tmp = (head_info.meta[i].http_equiv ? 
					head_info.meta[i].http_equiv : head_info.meta[i].name);

				/* pick out author */
				if(!strcmp(tmp, "author"))
					xms = XmStringCreateLocalized(head_info.meta[i].content);
			}
		}
		if(xms == NULL)
			xms = XmStringCreateLocalized("<Unknown>");
		XtVaSetValues(author_label, XmNlabelString, xms, NULL);
		XmStringFree(xms);

		/* string table */
		if(head_info.num_meta)
		{
			XmStringTable strs;
			strs =(XmStringTable)malloc(head_info.num_meta*sizeof(XmString*));
			
			for(i = 0; i < head_info.num_meta; i++)
			{
				if(head_info.meta[i].http_equiv)
					strs[i] =
						XmStringCreateLocalized(head_info.meta[i].http_equiv);
				else
					strs[i] =
						XmStringCreateLocalized(head_info.meta[i].name);
			}
			XtVaSetValues(list,
				XmNitemCount, head_info.num_meta,
				XmNitems, strs,
				XmNuserData, (XtPointer)head_info.meta,
				NULL);
			for(i = 0; i < head_info.num_meta; i++)
				XmStringFree(strs[i]);
			free(strs);
		}
	}
	else
	{
		xms = XmStringCreateLocalized("<Untitled>");
		XtVaSetValues(title_label, XmNlabelString, xms, NULL);
		XmStringFree(xms);

		xms = XmStringCreateLocalized("<Unknown>");
		XtVaSetValues(author_label, XmNlabelString, xms, NULL);
		XmStringFree(xms);

		xms = XmStringCreateLocalized("<Not specified>");
		XtVaSetValues(base_label, XmNlabelString, xms, NULL);
		XmStringFree(xms);
	}
	/* put on screen */
	XtManageChild(info_dialog);
	XMapRaised(XtDisplay(info_dialog), XtWindow(info_dialog));
}

static void
viewCB(Widget w, XtPointer client_data, XtPointer call_data)
{
	int item_no = MenuButtonGetId(w);

	switch(item_no)
	{
		case VIEW_INFO:
			viewDocumentInfo((Widget)client_data);
			break;
		case VIEW_SOURCE:
		case VIEW_FONTS:
			viewSource((Widget)client_data, item_no);
			break;
	}
}

static void
editCB(Widget w, XtPointer client_data, XtPointer call_data)
{
	int item_no = MenuButtonGetId(w);

	switch(item_no)
	{
		case EDIT_FIND:
		case EDIT_FIND_AGAIN:
			findString((Widget)client_data);
			break;
		default:
			break;
	}
}

static String
getCharset(String content)
{
	String ptr, start;
	int len = 0;
	static char this_set[128];

	/*
	* We possible have a charset spec in here. Check
	* for it
	*/
	if((ptr = strstr(content, "charset")) != NULL)
	{
		ptr+= 7;	/* move past "charset" */

		/* skip until we hit = */
		for(;*ptr && *ptr != '='; ptr++);
			ptr++;

		/* skip all spaces */
		for(;*ptr && isspace(*ptr); ptr++);

		/*
		* now count how many chars we have. The charset spec is terminated
		* by a a quote
		*/
		start = ptr;
		while(*ptr && *ptr != '\"')
		{
			len++;
			ptr++;
		}
		if(len)
		{
			strncpy(this_set, start, len);
			this_set[len] = '\0'; /* nullify */
			/*
			* if we don't have a - in charset, we *must* append -* to make
			* it a valid XmNcharset spec.
			*/
			if(!(strstr(this_set, "-")))
				strcat(this_set, "-*");

			return(this_set);
		}
	}
	return(NULL);
}

static void
checkFonts(Widget html)
{
	char this_font[128];
	String this_charset = NULL;
	static XmHTMLHeadAttributes head_info;
	String tmp;
	int i, argc = 0;
	Arg args[5];

	this_font[0] = '\0';

	/* get meta info for a charset and/or font spec */
	if(XmHTMLGetHeadAttributes(html, &head_info, HeadMeta))
	{
		if(head_info.num_meta)
		{
			for(i = 0; i < head_info.num_meta; i++)
			{
				tmp = (head_info.meta[i].http_equiv ? 
					head_info.meta[i].http_equiv : head_info.meta[i].name);

				if(!strcmp(tmp, "font"))
				{
					sprintf(this_font, "*-%s-normal-*",
						head_info.meta[i].content);
				}
				else if(!strcmp(tmp, "content-type") &&
					this_charset == NULL)
				{
					this_charset = getCharset(head_info.meta[i].content);
				}
			}
		}
	}

	/*****
	* have we been told to set a new font?
	* Please note that in a real world application it should be checked
	* if the requested font is available. XmHTML silently ignores this
	* kind of errors.
	*****/
	if(*this_font)
	{
		/* font changed */
		if(strcmp(this_font, current_font))
		{
			strcpy(current_font, this_font);
			current_font[strlen(this_font)] = '\0';

			Debug(("checkFonts, setting XmNfontFamily to %s\n",
				current_font));
			XtSetArg(args[argc], XmNfontFamily, current_font); argc++;
		}
		/* still the same, don't touch it */
	}
	else if(strcmp(current_font, default_font))
	{
		/* reset default font */
		strcpy(current_font, default_font);
		current_font[strlen(default_font)] = '\0';
		Debug(("checkFonts, setting XmNfontFamily to %s\n", current_font));
		XtSetArg(args[argc], XmNfontFamily, current_font); argc++;
	}

	/*****
	* have we been told to set a new character set?
	* Please note that a real world app *MUST* check if the requested
	* character set is available. XmHTML silently ignores any errors
	* resulting from an invalid charset, it just falls back to whatever
	* the X server provides.
	* To make these checks really consistent, it should also be verified
	* that the current font family is still valid if the charset is
	* changed. XmHTML's font allocation routines will almost *never*
	* fail on font allocations: if a font can not be found in the requested
	* character set it will use the default charset supplied by the X
	* server. If font allocation still fails after this, it will wildcard
	* the fontfamily and try again (this is done so XmHTML will always have
	* a default font available). If this also fails (which is almost
	* impossible) XmHTML will exit your application: if it can't find
	* any fonts at all, why keep on running?
	*****/
	if(this_charset)
	{
		/* charset changed */
		if(strcmp(current_charset, this_charset))
		{
			/* we currently only known koi8 and iso8859-1 */
			if(strstr(this_charset, "koi8"))
			{
				/* save charset */
				strcpy(current_charset, this_charset);
				current_charset[strlen(this_charset)] = '\0';

				/* koi8 has cronyx for its foundry */
				strcpy(current_font, "cronyx-times-*-*\0");
				argc = 0;		/* overrides a fontspec */

				Debug(("checkFonts, setting XmNcharset to %s\n",
					current_charset));
				XtSetArg(args[argc], XmNfontFamily, current_font); argc++;
				XtSetArg(args[argc], XmNcharset, current_charset); argc++;
			}
			else if(!strstr(this_charset, "iso-8859-1") &&
				!strstr(this_charset, "iso8859-1"))
			{
				fprintf(stderr, "Warning: character set %s unsupported\n",
					this_charset);
			}
		}
		/* still the same, don't touch it */
	}
	else if(strcmp(current_charset, default_charset))
	{
		strcpy(current_charset, default_charset);
		current_charset[strlen(default_charset)] = '\0';
		XtSetArg(args[argc], XmNcharset, current_charset); argc++;
	}

	/* plop'em in */
	if(argc)
		XtSetValues(html, args, argc);

	/*****
	* Tell XmHTML to clear everything. This is not really required but
	* adviseable since XmHTML will only clear the fields that have
	* been requested. For example, in all subsequent calls to the above
	* XmHTMLGetHeadAttributes call, XmHTML will first clear the stored
	* meta info before filling it again.
	*****/
	XmHTMLGetHeadAttributes(html, &head_info, HeadClear);
}

/*****
* Name: 		docCB
* Return Type: 	void
* Description: 	displays current document state as given by XmHTML.
* In: 
*	html:		owner of this callback
*	arg1:		client_data, unused
*	cbs:		call_data, documentcallback structure.
* Returns:
*	nothing
* Note:
*	the XmNdocumentCallback is an excellent place for setting several
*	formatting resources: XmHTML triggers this callback when the parser
*	has finished, but *before* doing any formatting. As an example, we
*	check the information contained in the document head for a font
*	specification: we call checkFonts.
*****/
static void
docCB(Widget w, XtPointer arg1, XmHTMLDocumentPtr cbs)
{
	XmString xms;
	char doc_label[128];
	Pixel my_pix = (Pixel)0;
	static Pixel red, green;

	/* see if we have been called with a valid reason */
	if(cbs->reason != XmCR_HTML_DOCUMENT)
		return;

	/*
	* If we are being notified of the results of another pass on the loaded
	* document, only check whether the generated parser tree is balanced and
	* don't update the labels since the callback data is the result of a
	* modified document.
	*
	* XmHTML's document verification and repair routines are capable of
	* creating a verified, properly balanced and HTML conforming document
	* from even the most horrible non-HTML conforming documents!
	*
	* And when the XmNstrictHTMLChecking resource has been set to True, these
	* routines are also bound to make the document HTML 3.2 conformant as well.
	*/
	if(cbs->pass_level)
	{
		/*
		* Allow up to two iterations on the current document (remember that
		* the document has already been checked twice when pass_level == 1).
		* XmHTML sets the redo field to True whenever the parser tree is
		* unbalanced, so it needs to be set to False when the allowed number
		* of iterations has been reached.
		*
		* The results of displaying a document with an unbalanced parser tree
		* are undefined however and can lead to some weird markup results.
		*/
		if(!cbs->balanced && cbs->pass_level < 2)
			return;
		cbs->redo = False;

		/* done parsing, check if a font is given in the meta spec. */
		checkFonts(w);
		return;
	}

	if(!red)
		red = XmHTMLAllocColor((Widget)html_widgets[0].html, "Red",
			BlackPixelOfScreen(XtScreen(toplevel)));

	if(!green)
		green = XmHTMLAllocColor((Widget)html_widgets[0].html, "Green",
			WhitePixelOfScreen(XtScreen(toplevel)));

	if(cbs->html32)
	{
		sprintf(doc_label, "HTML 3.2");
		my_pix = green;
	}
	else
	{
		sprintf(doc_label, "Bad HTML 3.2");
		my_pix = red;
	}
	xms = XmStringCreateLocalized(doc_label);
	XtVaSetValues(html32,
		XmNbackground, my_pix,
		XmNlabelString, xms,
		NULL);
	XmStringFree(xms);

	if(cbs->verified)
	{
		sprintf(doc_label, "Verified");
		my_pix = green;
	}
	else
	{
		sprintf(doc_label, "Unverified");
		my_pix = red;
	}
	xms = XmStringCreateLocalized(doc_label);
	XtVaSetValues(verified,
		XmNbackground, my_pix,
		XmNlabelString, xms,
		NULL);
	XmStringFree(xms);

	/*
	* default processing here. If the parser tree isn't balanced
	* (cbs->balanced == False) and you want to prevent XmHTML from making
	* another pass on the current document, set the redo field to false.
	*/
	if(cbs->balanced)
	{
		/* check meta info for a possible font spec */
		checkFonts(w);
	}
}

/*****
* Name: 		linkCB
* Return Type: 	void
* Description: 	XmHTML's XmNlinkCallback handler
* In: 
*	w:			widget id;
*	arg1:		client_data, unused;
*	cbs:		link data found in current document.
* Returns:
*	nothing, but copies the link data to an internal structure which is used
*	for displaying a site-navigation bar.
*****/
static void
linkCB(Widget w, XtPointer arg1, XmHTMLLinkPtr cbs)
{
	int i, j;

	/* free previous document links */
	for(i = 0; i < LINK_LAST; i++)
	{
		if(document_links[i].have_data)
		{
			/* we always have a href */
			free(document_links[i].href);
			/* but title is optional */
			if(document_links[i].title)
				free(document_links[i].title);
			document_links[i].href= NULL;
			document_links[i].title = NULL;
		}
		document_links[i].have_data  = False;
	}

	/*****
	* Since this callback gets triggered for every document, this is also
	* the place for updating the info dialog (if it's up that is).
	*****/
	if(info_dialog && XtIsManaged(info_dialog))
		viewDocumentInfo(toplevel);

	/*****
	* if the current document doesn't have any links, unmanage the button
	* and dialog and return.
	*****/
	if(cbs->num_link == 0)
	{
		/* might not have been created yet */
		if(link_dialog != NULL)
			XtUnmanageChild(link_dialog);
		XtUnmanageChild(link_button);
		return;
	}

	/* store links in this document */
	for(i = 0; i < LINK_LAST; i++)
	{
		for(j = 0; j < cbs->num_link; j++)
		{
			/* kludge for the mailto */
			String tmp = (i == 0 ? "made" : link_labels[i]);

			/*****
			* url is mandatory and so is one of rel or rev. Although both
			* of them can be present, we prefer a rel over a rev.
			*****/
			if(!cbs->link[j].url || (!cbs->link[j].rel && !cbs->link[j].rev))
				continue;

			if((cbs->link[j].rel && my_strcasestr(cbs->link[j].rel, tmp)) ||
				(cbs->link[j].rev && my_strcasestr(cbs->link[j].rev, tmp)))
			{
				document_links[i].have_data = True;
				document_links[i].link_type = cbs->link[j].rel ? 1 : 0;
				/* we always have this */
				document_links[i].href = strdup(cbs->link[j].url);
				/* title is optional */
				if(cbs->link[j].title)
					document_links[i].title = strdup(cbs->link[j].title);
			}
		}
	}
	/* if the dialog is already up, update the buttons */
	if(link_dialog && XtIsManaged(link_dialog))
	{
		for(i = 0; i < LINK_LAST; i++)
		{
			if(document_links[i].have_data)
				XtSetSensitive(link_buttons[i], True);
			else
				XtSetSensitive(link_buttons[i], False);
		}
		/* and make sure everything is up and displayed */
		XmUpdateDisplay(link_dialog);
	}
	/* manage the button so user can see the site structure of this doc. */
	XtManageChild(link_button);
}

/*****
* Name: 		imageViewUnmapCB
* Return Type: 	void
* Description: 	callback for the XmNunmapCallback callback resource in the
*				image preview dialog (popped up by showImageInfo)
* In: 
*	w:			widget being unmapped.
* Returns:
*	nothing.
*****/
static void
imageViewUnmapCB(Widget w)
{
	if(preview_image)
		XmImageDestroy(preview_image);
	preview_image = NULL;
}

/*****
* Name: 		showImageInfo
* Return Type: 	void
* Description: 	display a dialog with some information about the selected
*				image
* In: 
*	info:		info to be displayed
* Returns:
*	nothing.
* Note:
*	This routine also demonstrates how to use the XmImage data type for
*	creating a preview image from the given XmImageInfo structure.
*****/
static void
showImageInfo(XmImageInfo *info)
{
	/* various labels */
	static Widget url_label, size_label, dimension_label, npixels_label;
	static Widget type_label, pixmap_label, trans_label, depth_label;
	static Widget cache_label;
	XmString xms;
	Dimension w, h;
	char tmp[64];
	static XmImageConfig config;	/* for setting XmImage options */
	Pixel bg_pixel = 0;
	int cache_size, nobjects;

	setBusy(toplevel, True);

	if(!image_dialog)
	{
		Widget rc1, rc2;
		image_dialog = XmCreateFormDialog(toplevel, "imageView", NULL, 0);
		
		/* add an unmap callback so we can destroy the preview_image */
		XtAddCallback(image_dialog, XmNunmapCallback,
			(XtCallbackProc)imageViewUnmapCB, NULL);

		XtVaSetValues(XtParent(image_dialog),
			XtNtitle, "View Image Details",
			NULL);

		/* a rowcol for the labels */
		rc1 = XtVaCreateManagedWidget("imageRowColumn",
			xmRowColumnWidgetClass, image_dialog,
			XmNtopAttachment, XmATTACH_FORM,
			XmNleftAttachment, XmATTACH_FORM,
			XmNbottomAttachment, XmATTACH_FORM,
			XmNspacing, 0,
			XmNpacking, XmPACK_COLUMN,
			XmNorientation, XmVERTICAL,
			XmNnumColumns, 1,
			XmNtopOffset, 10,
			XmNleftOffset, 10,
			XmNbottomOffset, 10,
			NULL);

		XtVaCreateManagedWidget("Location:",
				xmLabelGadgetClass, rc1,
				XmNalignment, XmALIGNMENT_BEGINNING,
				NULL);
		XtVaCreateManagedWidget("Image Type:",
				xmLabelGadgetClass, rc1,
				XmNalignment, XmALIGNMENT_BEGINNING,
				NULL);
		XtVaCreateManagedWidget("Uncompressed Size:",
				xmLabelGadgetClass, rc1,
				XmNalignment, XmALIGNMENT_BEGINNING,
				NULL);
		XtVaCreateManagedWidget("Image Dimensions:",
				xmLabelGadgetClass, rc1,
				XmNalignment, XmALIGNMENT_BEGINNING,
				NULL);
		XtVaCreateManagedWidget("Number of Colors:",
				xmLabelGadgetClass, rc1,
				XmNalignment, XmALIGNMENT_BEGINNING,
				NULL);
		XtVaCreateManagedWidget("Depth:",
				xmLabelGadgetClass, rc1,
				XmNalignment, XmALIGNMENT_BEGINNING,
				NULL);
		XtVaCreateManagedWidget("Transparent:",
				xmLabelGadgetClass, rc1,
				XmNalignment, XmALIGNMENT_BEGINNING,
				NULL);

		XtVaCreateManagedWidget("Image Cache:",
				xmLabelGadgetClass, rc1,
				XmNalignment, XmALIGNMENT_BEGINNING,
				NULL);

		/* a rowcol for the fill-in fields */
		rc2 = XtVaCreateManagedWidget("imageRowColumn",
			xmRowColumnWidgetClass, image_dialog,
			XmNtopAttachment, XmATTACH_FORM,
			XmNleftAttachment, XmATTACH_WIDGET,
			XmNleftWidget, rc1,
			XmNbottomAttachment, XmATTACH_FORM,
			XmNspacing, 0,
			XmNpacking, XmPACK_COLUMN,
			XmNorientation, XmVERTICAL,
			XmNnumColumns, 1,
			XmNtopOffset, 10,
			XmNleftOffset, 10,
			XmNbottomOffset, 10,
			NULL);

		xms = XmStringCreateLocalized("");

		url_label = XtVaCreateManagedWidget("url",
				xmLabelGadgetClass, rc2,
				XmNalignment, XmALIGNMENT_END,
				XmNlabelString, xms,
				NULL);
		type_label = XtVaCreateManagedWidget("type",
				xmLabelGadgetClass, rc2,
				XmNalignment, XmALIGNMENT_END,
				XmNlabelString, xms,
				NULL);
		size_label = XtVaCreateManagedWidget("size",
				xmLabelGadgetClass, rc2,
				XmNalignment, XmALIGNMENT_END,
				XmNlabelString, xms,
				NULL);
		dimension_label = XtVaCreateManagedWidget("dimensions",
				xmLabelGadgetClass, rc2,
				XmNalignment, XmALIGNMENT_END,
				XmNlabelString, xms,
				NULL);
		npixels_label = XtVaCreateManagedWidget("npixels",
				xmLabelGadgetClass, rc2,
				XmNalignment, XmALIGNMENT_END,
				XmNlabelString, xms,
				NULL);
		depth_label = XtVaCreateManagedWidget("depth",
				xmLabelGadgetClass, rc2,
				XmNalignment, XmALIGNMENT_END,
				XmNlabelString, xms,
				NULL);
		trans_label = XtVaCreateManagedWidget("transparent",
				xmLabelGadgetClass, rc2,
				XmNalignment, XmALIGNMENT_END,
				XmNlabelString, xms,
				NULL);
		cache_label = XtVaCreateManagedWidget("cacheInfo",
				xmLabelGadgetClass, rc2,
				XmNalignment, XmALIGNMENT_END,
				XmNlabelString, xms,
				NULL);
		XmStringFree(xms);

		/* and the display area */
		pixmap_label = XtVaCreateManagedWidget("imageViewer",
				xmLabelGadgetClass, image_dialog,
				XmNtopAttachment, XmATTACH_FORM,
				XmNleftAttachment, XmATTACH_WIDGET,
				XmNleftWidget, rc2,
				XmNrightAttachment, XmATTACH_FORM,
				XmNbottomAttachment, XmATTACH_FORM,
				XmNtopOffset, 10,
				XmNleftOffset, 10,
				XmNrightOffset, 10,
				XmNbottomOffset, 10,
				XmNlabelType, XmPIXMAP,
				NULL);
	}
	/* destroy an existing image. */
	if(preview_image)
	{
		XmImageDestroy(preview_image);
		preview_image = NULL;
	}
	/*****
	* create the image we want to display. Try to scale it down to a 128x128
	* icon and keep the aspect ratio. 
	* swidth & sheight are the *real* image dimensions while
	* width & height are the current image dimensions. If they differ the
	* image has been scaled by XmHTML. For our preview image we use the
	* current image dimensions and show the scaling factor in the dialog box
	* itself. We could use the real dimensions but that would lead to a little
	* loss of data as the original data is no longer available.
	*
	* Note that we always set an explicit value for width & height: if they
	* left at zero, XmImageCreate will attempt to use the real image dimensions.
	*****/
	w = info->width;
	h = info->height;

	/* sanity check */
	if(h)
	{
		if(w > 128 || h > 128)
		{
			if(w > h)
			{
				h = (Dimension)(h*128/w);
				w = 128;
			}
			else
			{
				w = (Dimension)(w*128/h);
				h = 128;
			}
		}
	}

	/*
	* create an XmImage from the obtained XmImageInfo. We don't run animations
	* in the image info dialog, so we just want an image from the first frame
	* in an animation. Saves time and resources.
	*
	* XmImage configuration stuff we set in here:
	* - maximum image colors;
	* - enable quantization of images with more than allowed colors, and allow
	*   Floyd-Steinberg dithering if it should be required;
	* - animation frame selection;
	* - background substitution;
	*/
	/* get background pixel */
	XtVaGetValues(image_dialog, XmNbackground, &bg_pixel, NULL);

	/* set XmImageConfig options */
	config.ncolors	  = 216;		/* has no effect for CreateFromInfo */
	config.which_frames = FirstFrame;
	config.bg_color	 = bg_pixel;
	config.flags		= ImageQuantize|ImageFSDither|ImageBackground|
							ImageFrameSelect;

	/* create preview image */
	preview_image = XmImageCreateFromInfo(toplevel, info, w, h, &config);

	/* create the various labels */

	/* name/location of this image */
	xms = XmStringCreateSimple(collapseURL(info->url));
	XtVaSetValues(url_label, XmNlabelString, xms, NULL);
	XmStringFree(xms);

	/* type of image */
	xms = XmStringCreateSimple(image_types[info->type]);
	XtVaSetValues(type_label, XmNlabelString, xms, NULL);
	XmStringFree(xms);

	/* in-memory size of this image/animation */
	sprintf(tmp, "%i bytes", getInfoSize((XtPointer)info, NULL));
	xms = XmStringCreateSimple(tmp);
	XtVaSetValues(size_label, XmNlabelString, xms, NULL);
	XmStringFree(xms);

	/*
	* Show image dimensions as specified within the image itself.
	* image->width and image->height contain the dimensions of the generated
	* pixmap, which will be the same as swidth and sheight *unless* we've
	* requested scaling in either direction.
	*/
	/* image was scaled */
	if(info->swidth != info->width ||
		info->sheight != info->height)
		sprintf(tmp, "%ix%i pixels (scaled from %ix%i)",
			info->width, info->height,
			info->swidth, info->sheight);
	else
		sprintf(tmp, "%ix%i pixels", info->swidth,
			info->sheight);

	xms = XmStringCreateSimple(tmp);
	XtVaSetValues(dimension_label, XmNlabelString, xms, NULL);
	XmStringFree(xms);

	if(preview_image)
		/* image has been quantized? */
		if(preview_image->ncolors != preview_image->scolors)
			sprintf(tmp, "%i (reduced from %i)", preview_image->ncolors,
				preview_image->scolors);
		else
			sprintf(tmp, "%i", preview_image->ncolors);
	else
		sprintf(tmp, "%i", info->ncolors);

	xms = XmStringCreateSimple(tmp);
	XtVaSetValues(npixels_label, XmNlabelString, xms, NULL);
	XmStringFree(xms);

	/* image depth (bits per pixel) and colorclass */
	switch(info->colorspace)
	{
		case XmIMAGE_COLORSPACE_GRAYSCALE:
			sprintf(tmp, "%i, Grayscale", info->depth);
			break;
		case XmIMAGE_COLORSPACE_INDEXED:
			sprintf(tmp, "%i, Indexed color", info->depth);
			break;
		case XmIMAGE_COLORSPACE_RGB:
			sprintf(tmp, "%i, TrueColor", info->depth);
			break;
		default:
			sprintf(tmp, "%i", info->depth);
			break;
	}
	xms = XmStringCreateSimple(tmp);
	XtVaSetValues(depth_label, XmNlabelString, xms, NULL);
	XmStringFree(xms);

	/* transparency */
	switch(info->transparency)
	{
		case XmNONE:
			sprintf(tmp, "No");
			break;
		case XmIMAGE_TRANSPARENCY_BG:
			sprintf(tmp, "Yes, using background substitution "
				"(pixel index %i)\n", info->bg);
			break;
		case XmIMAGE_TRANSPARENCY_ALPHA:
			sprintf(tmp, "Yes, using alpha channel");
			break;
		default:
			sprintf(tmp, "Unknown");
	}
	xms = XmStringCreateSimple(tmp);
	XtVaSetValues(trans_label, XmNlabelString, xms, NULL);
	XmStringFree(xms);

	/* cache info */
	getCacheInfo(&cache_size, &nobjects);
	sprintf(tmp, "%i images cached (%i bytes)", nobjects, cache_size);
	xms = XmStringCreateSimple(tmp);
	XtVaSetValues(cache_label, XmNlabelString, xms, NULL);
	XmStringFree(xms);

	/* and the actual image */
	if(preview_image)
	{
		XtVaSetValues(pixmap_label,
			XmNlabelType, XmPIXMAP,
			XmNlabelPixmap, preview_image->pixmap,
			NULL);
	}
	else
	{
		xms = XmStringCreateSimple("(unable to create image)");
		XtVaSetValues(pixmap_label,
			XmNlabelType, XmSTRING,
			XmNlabelString, xms,
			NULL);
		XmStringFree(xms);
	}

	setBusy(toplevel, False);

	/* put on screen */
	XtManageChild(image_dialog);
	XMapRaised(XtDisplay(image_dialog), XtWindow(image_dialog));
}

/*****
* Name: 		infoPopupCB
* Return Type: 	void
* Description: 	callback for the menu-items in the popup menu
* In: 
*	w:			widget id;
*	item:		id of selected menu item.
* Returns:
*	nothing.
*****/
static void
infoPopupCB(Widget w, int item)
{
	XtPointer ref = NULL;
	
	/* get href, image name or image data from the widget's userdata */
	XtVaGetValues(w, XmNuserData, &ref, NULL);

	/* fourth popup menu item doesn't carry userData */
	if(ref == NULL && item != 3)
	{
		fprintf(stderr, "Failed to retrieve userData field from popup "
			"button %i\n", item);
		return;
	}

	switch(item)
	{
		case 0:
			/* follow link */
			{
				/*****
				* We just compose a XmHTMLAnchorCallbackStruct and let anchorCB
				* do the loading.
				*****/
				XmHTMLAnchorCallbackStruct cbs;
				cbs.reason = XmCR_ACTIVATE;
				cbs.event  = NULL;
				cbs.url_type = XmHTMLGetURLType((String)ref);
				cbs.href   = (String)ref;
				cbs.title  = NULL;
				cbs.line   = 0;
				cbs.target = NULL;
				cbs.doit   = False;
				cbs.visited= False;
				/* and call the activate callback */
				anchorCB(html_widgets[0].html, NULL, &cbs);
			}
			break;
		case 1:
			/* open image */
			{
				String filename;
				if((filename = resolveFile((String)ref)) != NULL)
				{
					loadAndOrJump(filename, NULL, True);
					free(filename);
				}
			}
			break;
		case 2:
			/* view image details */
			showImageInfo((XmImageInfo*)ref);
			break;
		default:
			fprintf(stderr, "Impossible popup menu selection!\n");
	}
}

/*****
* Name: 		infoCB
* Return Type: 	void
* Description: 	ButtonPressed handler for the workArea of a XmHTML widget.
*				In this case, a possible use of the XmHTMLXYToInfo resource
*				is demonstrated.
* In: 
*	w:			widget id;
*	popup:		popup menu widget id
*	event:		location of button press.
* Returns:
*	nothing.
*****/
static void
infoCB(Widget parent, Widget popup, XButtonPressedEvent *event)
{
	XmString xms;
	char tmp[84];	/* max label width */
	XmHTMLInfoPtr info;
	WidgetList children;
	Widget html_w;

	XUngrabPointer(XtDisplay(parent), CurrentTime);

	/* only button 3 */
	if(event->button != 3)
		return;

	html_w = XtParent(parent);

	Debug(("In infoCB for Widget %s\n", XtName(html_w)));

	if(html_w == NULL || !XmIsHTML(html_w))
	{
		fprintf(stderr, "%s parent gotten from XtParent(%s)\n",
			html_w == NULL ? "NULL" : "Invalid", XtName(parent));
		return;
	}

	/* get the info for the selected position */
	info = XmHTMLXYToInfo(html_w, event->x, event->y);

	/* no popup if no image and anchor */
	if(info == NULL || (info->image == NULL && info->anchor == NULL))
	{
		Debug(("XmHTMLXYToInfo returned nothing usefull for location "
			"(%i,%i) (minus core offsets: %i,%i)\n",
			(int)event->x, (int)event->y,
			(int)event->x - html_w->core.x, (int)event->y - html_w->core.y));
		return;
	}

	XtVaGetValues(popup, XmNchildren, &children, NULL);

	/* unmanage all buttons */
	XtUnmanageChild(children[0]);
	XtUnmanageChild(children[1]);
	XtUnmanageChild(children[2]);
	XtUnmanageChild(children[3]);

	/*****
	* Note on how to convey the href or image url's to the popup callbacks:
	*
	* All strings provided in the anchor and/or image field of this callback
	* are internal to XmHTML and are guarenteed to exist as long as the
	* current document is up. Knowing this, we can safely store these strings
	* in the userData field of the popup menu buttons.
	****/

	/* if we have an anchor, we copy the url to the label */
	if(info->anchor)
	{
		sprintf(tmp, "Follow this link (%s)", collapseURL(info->anchor->href));
		xms = XmStringCreateLocalized(tmp);
		XtVaSetValues(children[0],
			XmNlabelString, xms,
			XmNuserData, info->anchor->href,
			NULL);
		XmStringFree(xms);

		/* manage it */
		XtManageChild(children[0]);
	}
	if(info->image)
	{
		sprintf(tmp, "Open this image (%s)", collapseURL(info->image->url));
		xms = XmStringCreateLocalized(tmp);
		XtVaSetValues(children[1],
			XmNlabelString, xms,
			XmNuserData, info->image->url,
			NULL);
		XmStringFree(xms);

		/* manage it */
		XtManageChild(children[1]);

		xms = XmStringCreateLocalized("View Image details");
		XtVaSetValues(children[2],
			XmNlabelString, xms,
			XmNuserData, info->image,
			NULL);
		XmStringFree(xms);

		/* manage it */
		XtManageChild(children[2]);
	}
	/* set correct menu position */
	XmMenuPosition(popup, event);
	/* and show it */
	XtManageChild(popup);
}

/*****
* Name: 		navCB
* Return Type: 	void
* Description: 	callback for the buttons in the link dialog (displayed 
*				by the linkButtonCB routine)
* In: 
*	w:			widget id of selected button;
*	item:		id of selected item;
* Returns:
*	nothing.
* Note:
*	This routine simply creates a XmHTMLAnchorCallbackStruct and calls
*	the anchorCB routine to let it handle navigation of the document
*	(which can include loading a new local or remote document, call a mail
*	application to mail something, download something, whatever).
*****/
static void
navCB(Widget w, int item)
{
	static XmHTMLAnchorCallbackStruct cbs;

	/*****
	* We just compose a XmHTMLAnchorCallbackStruct and let anchorCB do the
	* loading.
	*****/
	cbs.reason = XmCR_ACTIVATE;
	cbs.event  = NULL;
	cbs.url_type = XmHTMLGetURLType(document_links[item].href);
	cbs.href   = document_links[item].href;
	cbs.title  = document_links[item].title;
	cbs.line   = 0;
	cbs.target = NULL;
	cbs.doit   = False;
	cbs.visited= False;
	if(document_links[item].link_type == 0)
	{
		cbs.rev = link_labels[item];
		cbs.rel = NULL;
	}
	else
	{
		cbs.rel = link_labels[item];
		cbs.rev = NULL;
	}

	/* and call the activate callback */
	anchorCB(html_widgets[0].html, NULL, &cbs);
}

/*****
* Name: 		linkButtonCB
* Return Type: 	void
* Description: 	displays a dialog with buttons to allow navigation of a
*				document using the information contained in the <link></link>
*				section of a HTML document.
* In: 
*	w:			widget id, unused;
*	arg1:		client_data, unused;
*	arg2:		call_data, unused;
* Returns:
*	nothing.
*****/
static void
linkButtonCB(Widget w, XtPointer arg1, XtPointer arg2)
{
	int i;

	if(!link_dialog)
	{
		Widget rc;
		link_dialog = XmCreateFormDialog(toplevel, "Preview", NULL, 0);
		
		XtVaSetValues(XtParent(link_dialog),
			XtNtitle, "Site Structure",
			NULL);

		/* a rowcol for the buttons */
		rc = XtVaCreateManagedWidget("rowColumn",
			xmRowColumnWidgetClass, link_dialog,
			XmNtopAttachment, XmATTACH_FORM,
			XmNleftAttachment, XmATTACH_FORM,
			XmNrightAttachment, XmATTACH_FORM,
			XmNspacing, 0,
			XmNpacking, XmPACK_COLUMN,
			XmNorientation, XmVERTICAL,
			XmNnumColumns, 1,
			NULL);
		/* all buttons */
		for(i = 0; i < LINK_LAST; i++)
		{
			link_buttons[i] = XtVaCreateManagedWidget(link_labels[i],
				xmPushButtonWidgetClass, rc,
				NULL);
			XtAddCallback(link_buttons[i], XmNactivateCallback,
				(XtCallbackProc)navCB, (XtPointer)i);
		}
	}
	/* Now see what buttons to activate. */
	for(i = 0; i < LINK_LAST; i++)
	{
		if(document_links[i].have_data)
			XtSetSensitive(link_buttons[i], True);
		else
			XtSetSensitive(link_buttons[i], False);
	}
	/* 
	* Keep the focus on the html widget so the navigation keys always
	* work.
	*/
	XmProcessTraversal(html_widgets[0].html, XmTRAVERSE_CURRENT);

	/* put on screen */
	XtManageChild(link_dialog);
	XMapRaised(XtDisplay(link_dialog), XtWindow(link_dialog));

	/* and make sure everything is up and displayed */
	XmUpdateDisplay(link_dialog);
}

/*****
* Name: 		historyCB
* Return Type: 	void
* Description: 	XmNactivateCallback handler for the back & forward buttons.
*				moves back or forward in the history.
* In: 
*	w:			widget
*	button:		button the triggered this callback.
* Returns:
*	nothing.
*****/
static void
historyCB(Widget w, int button)
{
	DocumentCache *this_doc = &doc_cache[current_doc];

	/* back button has been pressed */
	if(button == 0)
	{
		if(!XtIsSensitive(back))
			return;

		/* current_ref == 0 -> top of file */
		if(this_doc->current_ref)
			this_doc->current_ref--;
		/* reached bottom of current document, move to the previous one */
		else
		{
			current_doc--;
			this_doc = &doc_cache[current_doc];
		}
		if(loadAndOrJump(this_doc->file, this_doc->refs[this_doc->current_ref],
			False))
		{
			XtSetSensitive(forward, True);
			XtSetSensitive(back, 
				current_doc || this_doc->current_ref ? True : False);
		}
		else
		{
			if(current_doc)
			{
				current_doc--;
				historyCB(w, button);
			}
		}
	}
	/* forward button has been pressed */
	else
	{
		if(!XtIsSensitive(forward))
			return;

		this_doc->current_ref++;
		/* reached max of this document, move to the next doc */
		if(this_doc->current_ref == this_doc->nrefs)
		{
			this_doc->current_ref--;
			current_doc++;
			this_doc = &doc_cache[current_doc];
		}

		if(loadAndOrJump(this_doc->file, this_doc->refs[this_doc->current_ref],
			False))
		{
			XtSetSensitive(back, True);

			if(current_doc == last_doc - 1 && 
				this_doc->current_ref == this_doc->nrefs - 1)
				XtSetSensitive(forward, False);
		}
		else
		{
			if(current_doc != last_doc-1)
			{
				current_doc++;
				historyCB(w, button);
			}
		}
	}
	/* 
	* Keep the focus on the html widget so the navigation keys always
	* work.
	*/
	XmProcessTraversal(html_widgets[0].html, XmTRAVERSE_CURRENT);
}

/*****
* Name: 		destroyCacheObject
* Return Type: 	void
* Description: 	gets called by the object caching routines when it wants to
*				destroy a cached object.
* In: 
*	call_data:	object to be destroyed;
*	client_data:data we registered ourselves when we made the initObjectCache
*				call.
* Returns:
*	nothing.
*****/
static void
destroyCacheObject(XtPointer call_data, XtPointer client_data)
{
	Debug(("destroyCacheObject, called for %s\n",
		((XmImageInfo*)call_data)->url));

	XmHTMLImageFreeImageInfo((Widget)client_data, (XmImageInfo*)call_data);
}

/*****
* Name: 		loadAnimation
* Return Type: 	XmImageInfo
* Description: 	load an animation consisting of multiple images
* In: 
*	names:		comma separated list of images
* Returns:
*	a list of XmImageInfo composing the animation
* Note:
*	this is a fairly simple example on how to add support for images
*	that are not supported by the XmHTMLImageDefaultProc convenience
*	function.
*****/
XmImageInfo*
loadAnimation(char *names)
{
	static XmImageInfo *all_frames;
	XmImageInfo *frame = NULL;
	String chPtr, name_buf, filename;
	int nframes = 0;

	name_buf = strdup(names);
	for(chPtr = strtok(name_buf, ","); chPtr != NULL; 
		chPtr = strtok(NULL, ","))
	{
		if((filename = resolveFile(chPtr)) == NULL)
		{
			free(name_buf);
			return(NULL);
		}
		if(nframes)
		{
			frame->frame = XmHTMLImageDefaultProc(html_widgets[0].html,
				filename, NULL, 0);
			frame = frame->frame;
		}
		else
		{
			all_frames = XmHTMLImageDefaultProc(html_widgets[0].html,
				filename, NULL, 0);
			frame = all_frames;
		}
		frame->timeout = animation_timeout;
		nframes++;
		free(filename);
	}
	free(name_buf);

	/* nframes is total no of frames in this animation */
	all_frames->nframes = nframes;
	all_frames->timeout = animation_timeout;
	return(all_frames);
}

/*****
* Name: 		flushImages
* Return Type: 	void
* Description: 	flushes all images (normal *and* delayed) to the currently
*				loaded document.
* In: 
*	w:			widget id, unused;
* Returns:
*	nothing.
* Note:
*	This routine simply flushes all images in the currently loaded document.
*	When it finds an image that has been delayed, it loads the real image and
*	replaces the delayed image in the current document with the real image.
*	When all images have been updated, it calls XmHTMLRedisplay to force
*	XmHTML to do a re-computation of the document layout.
*****/
static void 
flushImages(Widget w)
{
	int i;
	static XmImageInfo *image, *new_image;
	String url, filename;
	DocumentCache *this_doc;
	XmImageStatus status = XmIMAGE_OK, retval = XmIMAGE_OK;
	
	setBusy(toplevel, True);

	/* get current document */
	this_doc = &doc_cache[current_doc];

	for(i = 0; i < this_doc->nimages; i++)
	{
		url = this_doc->images[i];

		/* get image to update */
		if((image = (XmImageInfo*)getURLObjectFromCache(url)) == NULL)
			continue;

		/* only do this when the image hasn't been loaded yet */
		if(image->options & XmIMAGE_DELAYED &&
			(filename = resolveFile(url)) != NULL)
		{
			if(strstr(url, ","))
				new_image = loadAnimation(url); 
			else
			{
				new_image = XmHTMLImageDefaultProc(html_widgets[0].html,
					filename, NULL, 0);
				free(filename);
			}
			if(new_image)
			{
				/* don't let XmHTML free it */
				new_image->options &= ~(XmIMAGE_DEFERRED_FREE) &
					~(XmIMAGE_DELAYED);
				/*****
				* Replace it. XmHTMLImageReplace returns a statuscode 
				* indicating success of the action. If XmIMAGE_OK is returned,
				* no call to XmHTMLRedisplay is necessary, the dimensions are
				* the same as specified in the document. It returns
				* XmIMAGE_ALMOST if a recomputation of document layout is
				* necessary, and something else if an error occured:
				* XmIMAGE_ERROR: the widget arg is not a XmHTML widget or
				*	pixmap creation failed;
				* XmIMAGE_BAD if image and/or new_image is NULL;
				* XmIMAGE_UNKNOWN if image is unbound to an internal image.
				*****/
				retval = XmHTMLImageReplace(html_widgets[0].html, image,
					new_image);

				/* update private cache */
				replaceObjectInCache((XtPointer)image, (XtPointer)new_image);

				/* and destroy previous image data */
				XmHTMLImageFreeImageInfo((Widget)html_widgets[0].html, image);
			}
		}
		else	/* same note as above applies */
			retval = XmHTMLImageUpdate(html_widgets[0].html, image);

		/* store return value of ImageReplace and/or ImageUpdate */
		if(retval == XmIMAGE_ALMOST)
			status = retval;
	}
	/* force a reformat and redisplay of the current document if required */
	if(status == XmIMAGE_ALMOST)
		XmHTMLRedisplay(html_widgets[0].html);

	/* keep focus on the html widget */
	XmProcessTraversal(html_widgets[0].html, XmTRAVERSE_CURRENT);

	setBusy(toplevel, False);
}

/*****
* Name: 		killImages
* Return Type: 	void
* Description: 	kills all images (normal *and* delayed) of all documents.
*				Should be called at exit.
* In: 
*	nothing.
* Returns:
*	nothing.
*****/
static void 
killImages(void)
{
	setBusy(toplevel, True);

	destroyObjectCache();

	setBusy(toplevel, False);
}

static void
progressiveButtonCB(Widget w, int reset)
{
	int curr_state = 0;
	String label;
	XmString xms;

	if(reset)
	{
		if(XtIsManaged(prg_button))
			XtUnmanageChild(prg_button);

		/* set new label and global PLC state */
		xms = XmStringCreateLocalized("Suspend Image Load");
		XtVaSetValues(prg_button,
			XmNlabelString, xms,
			XmNuserData, (XtPointer)STREAM_OK,
			NULL);
		XmStringFree(xms);
		return;
	}

	/* get current progressive image loading state */
	XtVaGetValues(w, XmNuserData, &curr_state, NULL);

	switch(curr_state)
	{
		case STREAM_OK:
			XmHTMLImageProgressiveSuspend(html_widgets[0].html);
			curr_state = STREAM_SUSPEND;
			label = "Continue Image Load";
			break;
		case STREAM_SUSPEND:
			XmHTMLImageProgressiveContinue(html_widgets[0].html);
			curr_state = STREAM_OK;
			label = "Suspend Image Load";
			break;
		default:
			fprintf(stderr, "Oops, unknown button state in "
				"progressiveButtonCB\n");
			return;
	}

	/* set new label and global PLC state */
	xms = XmStringCreateLocalized(label);
	XtVaSetValues(prg_button,
		XmNlabelString, xms,
		XmNuserData, (XtPointer)curr_state,
		NULL);
	XmStringFree(xms);
}

/*****
* Name:			getImageData
* Return Type: 	int
* Description: 	XmHTMLGetDataProc method. Called when we are to
*				load images progressively.
* In: 
*	stream:		Progressive Load Context stream object
*	buffer:		destination buffer.
* Returns:
*	STREAM_END when we have run out of data, number of bytes copied into the
*	buffer otherwise.
* Note:
*	This routine is an example implementation of how to write a
*	XmHTMLGetDataProc method. As this program doesn't have networking
*	capabilities, we mimic a connection by providing the data requested in
*	small chunks (which is a command line option: prg_skip [number of bytes]).
*	The stream argument is a structure containing a minimum and maximum byte
*	count (the min_out and max_out fields), a number representing the number
*	of bytes used by XmHTML (the total_in field) and user_data registered
*	with the object that is being loaded progressively.
*
*	If this routine returns data, it must *always* be a number between
*	min_out and max_out (including min_out and max_out). Returning less is
*	not an advisable thing to do (it will cause an additional call immediatly)
*	and returning more *can* cause an error (by overflowing the buffer).
*
*	You can let XmHTML expand it's internal buffers by setting the max_out
*	field to the size you want the buffer to have and returning STREAM_RESIZE.
*	XmHTML will then try to resize its internal buffers to the requested size
*	and call this routine again immediatly. When a buffer has been resized, it
*	is very likely that XmHTML will backtrack to an appropriate starting point,
*	so be sure to check and use the total_in field of the stream arg when 
*	returning data.
*
*	If you want to abort progressive loading, you can return STREAM_ABORT.
*	This will cause XmHTML to terminate the progressive load for the given
*	object (which involves a call to any installed XmHTMLProgressiveEndData
*	method). Example use of this could be an ``Abort'' button. An alternative
*	method is to use the XmHTMLImageProgressiveKill() convenience routine as
*	shown in the getAndSetFile() routine above.
*
*	Also note that returning 0 is equivalent to returning STREAM_END (which
*	is defined as being 0).
*
*	As a final note, XmHTML will ignore any bytes copied into the buffer
*	if you return any of the STREAM_ codes.
*****/
static int
getImageData(XmHTMLPLCStream *stream, XtPointer buffer)
{
	ImageBuffer *ib = (ImageBuffer*)stream->user_data;
	int len;

	Debug(("getImageData, request made for %s\n", ib->file));
	Debug(("getImageData, XmHTML already has %i bytes\n", stream->total_in));

	/* no more data available, everything has been copied */
	if(ib->next >= ib->size)
		return(STREAM_END);

	/*
	* Maximum no of bytes we can return. ib->size contains the total size of
	* the image data, and total_in contains the number of bytes that have
	* already been used by XmHTML so far.
	* total_in may differ from ib->next due to backtracking of the calling PLC.
	*/
	len = ib->size - stream->total_in;

	/*
	* If you want to flush all data you've got to XmHTML but max_out is too
	* small, you can do something like this:
	* if(len > stream->max_out)
	* {
	*	stream->max_out = len;
	*	return(STREAM_RESIZE);
	* }
	* As noted above, XmHTML will then resize it's internal buffers to fit
	* the requested size and call this routine again. Before I forget, the
	* default size of the internal buffers is 2K.
	* And no, setting max_out to 0 will not cause XmHTML to choke. It will
	* simply ignore it (and issue a blatant warning message accusing you of
	* being a bad programmer :-).
	*/
	if(progressive_data_inc)
	{
		/* increment if not yet done for this pass */
		if(!ib->may_free)
		{
			progressive_data_skip += progressive_data_inc;

			Debug(("getImageData, incrementing buffer size to %i bytes\n",
				progressive_data_skip));

			stream->max_out = progressive_data_skip;
			ib->may_free = True;
			return(STREAM_RESIZE);
		}
		else	/* already incremented, copy data for this pass */
			ib->may_free = False;
	}

	/* provide the minimum if our skip is too small */
	if(len < stream->min_out || progressive_data_skip < stream->min_out)
		len = stream->min_out;
	else
		len = progressive_data_skip;

	/* final sanity */
	if(len + stream->total_in > ib->size)
		len = ib->size - stream->total_in;

	/* more bytes available than minimally requested, we can copy */
	if(len >= stream->min_out)
	{
		/* but don't exceed the maximum allowable amount to return */
		if(len > stream->max_out)
			len = stream->max_out;

		Debug(("getImageData, returning %i bytes (min_out = %i, "
			"max_out = %i)\n", len, stream->min_out, stream->max_out));

		memcpy((char*)buffer, ib->buffer + stream->total_in, len);
		ib->next = stream->total_in+len;
		return(len);
	}
	/*
	* some sort of error, XmHTML requested data beyond the end of the file,
	* so we just return STREAM_END here and let XmHTML decide what to do.
	*/
	return(STREAM_END);
}

static void
endImageData(XmHTMLPLCStream *stream, XtPointer data, int type, Boolean ok)
{
	XmImageInfo *image = (XmImageInfo*)data;
	ImageBuffer *ib;

	/*
	* XmHTML signals us that there are no more images being loaded
	* progressively. Remove ``Suspend Image Load'' button.
	* Beware: this is the only case in which stream is NULL.
	*/
	if(type == XmPLC_FINISHED)
	{
		XtSetSensitive(prg_button, False);
		XtUnmanageChild(prg_button);
		return;
	}

 	ib = (ImageBuffer*)stream->user_data;

	/*
	* To keep the cache size in sync, we update the cached image by replacing
	* it. As we will be replacing the same object, the only effect this call
	* will have is that the sizeObjectProc will be called.
	*/
	if(ok)
		replaceObjectInCache((XtPointer)image, (XtPointer)image);
	else
	{
		/* incomplete image, remove it from the image cache */
		removeObjectFromCache(ib->file);
		cleanObjectCache();
	}

	Debug(("endImageData, called for %s, ok = %s\n", ib->file,
		ok ? "True" : "False"));

	free(ib->file);
	free(ib->buffer);
	free(ib);
}

/*****
* Name: 		loadImage
* Return Type: 	XmImageInfo
* Description: 	XmHTMLimageProc handler
* In: 
*	w:			HTML widget id
*	url:		src value of an img element.
* Returns:
*	return value from the HTML widget imageDefaultProc
* Note:
*	this is a very simple example of how to respond to requests for images:
*	XmHTML calls this routine with the name (or location or whatever the
*	src value is) of an image to load. All you need to do is get the full
*	name of the image requested and call the imageDefaultProc and let XmHTML
*	handle the actual loading.
*	This is also the place to fetch remote images, implement an imagecache
*	or add support for images not supported by the imageDefaultProc.
*****/
static XmImageInfo*
loadImage(Widget w, String url)
{
	String filename = NULL;
	XmImageInfo *image = NULL;
	DocumentCache *this_doc;
	int i;

	/* get current document */
	this_doc = &doc_cache[current_doc];

	Debug(("Requested to load image %s\n", url));

	/*****
	* get full path for this url. The "," strstr is used to check if this
	* image is an animation consisting of a list of comma-separated images
	* (see examples/test-pages/animation?.html for an example)
	*****/
	if((filename = resolveFile(url)) == NULL)
		if(!strstr(url, ","))
			return(NULL);

	/* 
	* add this image to this document's image list.
	* First check if we haven't got it already, can only happen when the 
	* document is reloaded (XmHTML doesn't load identical images).
	* Use original URL for this.
	*/
	for(i = 0; i < this_doc->nimages; i++)
	{
		if(!(strcmp(this_doc->images[i], url)))
			break;
	}
	/* don't have it yet */
	if(i == this_doc->nimages)
	{
		/* see if it can hold any more images */
		if(this_doc->nimages != MAX_IMAGE_ITEMS)
		{
			this_doc->images[this_doc->nimages++] = strdup(url);
			i = this_doc->nimages;
		}
		else
		{
			char buf[128];
			sprintf(buf, "This document contains more than %i images,\n"
				"Only the first %i will be shown.", MAX_IMAGE_ITEMS,
  				MAX_IMAGE_ITEMS);
			XMessage(toplevel, buf);
			return(NULL);
		}
	}

	/* now check if we have this image already available */
	if((image = (XmImageInfo*)getObjectFromCache(filename, url)) != NULL)
	{
		/*
		* If i isn't equal to the current no of images for the current
		* document, the requested image has already been loaded once for this
		* document, so we do not have to store it again.
		*/
		/* call storeImage again as we might be using a different URL */
		if(i == this_doc->nimages)
			storeObjectInCache((XtPointer)image,
				filename ? filename : url, url);
		if(filename)
			free(filename);
		return(image);
	}

	if(filename || (strstr(url, ",")) != NULL)
	{
		/* test delayed image loading */
		if(html_config[OPTIONS_AUTOLOAD_IMAGES].value)
		{
			if(strstr(url, ","))
				image = loadAnimation(url);
			else
			{
				if(!progressive_images)
					image = XmHTMLImageDefaultProc(w, filename, NULL, 0);
				else
				{
					unsigned char img_type;

					img_type = XmHTMLImageGetType(filename, NULL, 0);

					if(img_type != IMAGE_ERROR && img_type != IMAGE_UNKNOWN &&
						img_type != IMAGE_XPM && img_type != IMAGE_PNG)
					{
						FILE *file;
						static ImageBuffer *ib;

						/* open the given file */
						if((file = fopen(filename, "r")) == NULL)
						{
							perror(filename);
							return(NULL);
						}

						/*
						* We load the image data into an ImageBuffer (which
						* we will be using in the get_data() function.
						*/
						ib = (ImageBuffer*)malloc(sizeof(ImageBuffer));
						ib->file = strdup(filename);
						ib->next = 0;

						/* see how large this file is */
						fseek(file, 0, SEEK_END);
						ib->size = ftell(file);
						rewind(file);

						/* allocate a buffer to contain the entire image */
						ib->buffer = malloc(ib->size+1);

						/* now read the contents of this file */
						if((fread(ib->buffer, 1, ib->size, file)) != ib->size)
							printf("Warning: did not read entire file!\n");
						ib->buffer[ib->size] = '\0';	/* sanity */

						/* create an empty ImageInfo */
						image = (XmImageInfo*)malloc(sizeof(XmImageInfo));
						memset(image, 0, sizeof(XmImageInfo));

						/* set the Progressive bit and allow scaling */
						image->options =XmIMAGE_PROGRESSIVE|XmIMAGE_ALLOW_SCALE;
						image->url = strdup(filename);

						/* set file buffer as user data for this image */
						image->user_data = (XtPointer)ib;

						/* make the progressive image loading button visible */
						if(!XtIsManaged(prg_button))
							XtManageChild(prg_button);

						XtSetSensitive(prg_button, True);

						/* all done! */
					}
					else
						image = XmHTMLImageDefaultProc(w, filename, NULL, 0);
				}
			}
			/* failed, too bad */
			if(!image)
				return(NULL);

			/* don't let XmHTML free it */
			image->options &= ~(XmIMAGE_DEFERRED_FREE);
		}
		else
		{
			image = (XmImageInfo*)malloc(sizeof(XmImageInfo));
			memset(image, 0, sizeof(XmImageInfo));
			image->options = XmIMAGE_DELAYED|XmIMAGE_ALLOW_SCALE;
			image->url = strdup(filename ? filename : url);
		}
		/* store in the cache */
		storeObjectInCache((XtPointer)image, filename ? filename : url, url);
	}
	if(filename)
		free(filename);
	
	return(image);
}

/*****
* Name: 		testAnchor
* Return Type: 	int
* Description: 	XmNanchorVisitedProc procedure
* In: 
*	w:			widget
*	href:		href to test
* Returns:
*	True when the given href has already been visited, false otherwise.
* Note:
*	This is quite inefficient. In fact, the whole history scheme is 
*	inefficient, but then again, this is only an example and not a full 
*	featured browser ;-)
*****/
static int
testAnchor(Widget w, String href)
{
	int i, j;

	/* walk each document */
	for(i = 0 ; i < last_doc; i++)
	{
		/* and walk the history list of each document */
		for(j = 0; j < doc_cache[i].nvisited; j++)
			if(doc_cache[i].visited[j] &&
				!strcmp(doc_cache[i].visited[j], href))
				return(True);
	}
	/* we don't know it */
	return(False);
}

/*****
* Name: 		aboutCB
* Return Type: 	void
* Description: 	displays an ``About'' dialog when the help->about menu item
*				is selected.
* In: 
*	widget:		menubutton widget id
*	client_data:	unused
*	call_data:	unused
* Returns:
*	nothing
*****/
static void
helpCB(Widget w, XtPointer client_data, XtPointer call_data)
{
	int item_no = MenuButtonGetId(w);
	char label[256];

	switch(item_no)
	{
		case HELP_ABOUT:
			sprintf(label, "A Simple HTML browser using\n"
				"%s\n", XmHTMLVERSION_STRING);
			XMessage(toplevel, label);
			break;
		default:
			break;
	}
}

#define SET_STATE(STATE) { \
	if((new_state & STATE) == STATE) \
		MenuToggleSetState(menu, (int)STATE, True); \
	else \
		MenuToggleSetState(menu, (int)STATE, False); \
}

/* explicitly set toggle states for each warning menu entry */
static void
updateWarningMenu(Widget menu, unsigned char new_state)
{
	/* XmHTML_NONE == 0, so the above macro would always evaluate to True */
	if(new_state == XmHTML_NONE) 
		MenuToggleSetState(menu, (int)XmHTML_NONE, True);
	else
		MenuToggleSetState(menu, (int)XmHTML_NONE, False);

	SET_STATE(XmHTML_ALL);
	SET_STATE(XmHTML_UNKNOWN_ELEMENT);
	SET_STATE(XmHTML_BAD);
	SET_STATE(XmHTML_OPEN_BLOCK);
	SET_STATE(XmHTML_CLOSE_BLOCK);
	SET_STATE(XmHTML_OPEN_ELEMENT);
	SET_STATE(XmHTML_NESTED);
	SET_STATE(XmHTML_VIOLATION);
}
#undef SET_STATE

static void
warningCB(Widget w, XtPointer client_data, XtPointer call_data)
{
	int item_no, i;
	Boolean set;
	unsigned char new_state;
	unsigned char prev_state;

	/* get selected menu entry */
	item_no = MenuButtonGetId(w);

	/* get toggle value */
	set = MenuToggleSelected(w);

	/* previous menu state */
	prev_state = html_config[OPTIONS_BADHTML].value;

	/* none or all set/unset all other types or restore the previous state */
	if(item_no == XmHTML_NONE || item_no == XmHTML_ALL)
	{
		if(item_no == XmHTML_NONE)
		{
			if(set)
				new_state = XmHTML_NONE;
			else
				new_state = prev_state;
		}
		else
		{
			if(set)
				new_state = XmHTML_ALL;
			else
				new_state = prev_state;
		}
	}
	else
	{
		Widget menu = XtParent(w);

		new_state = XmHTML_NONE;

		/* get value of each type */
#define GET_STATE(STATE) { \
	if(MenuToggleGetState(menu, STATE)) \
		new_state |= STATE; \
	else \
		new_state &= ~STATE; \
}

		/* get value of each type */
		GET_STATE(XmHTML_UNKNOWN_ELEMENT);
		GET_STATE(XmHTML_BAD);
		GET_STATE(XmHTML_OPEN_BLOCK);
		GET_STATE(XmHTML_CLOSE_BLOCK);
		GET_STATE(XmHTML_OPEN_ELEMENT);
		GET_STATE(XmHTML_NESTED);
		GET_STATE(XmHTML_VIOLATION);
#undef GET_STATE
	}

	/* explicitly set warning menu toggle states */
	updateWarningMenu(XtParent(w), new_state);

	/* save new state */
	html_config[OPTIONS_BADHTML].value = (Boolean)new_state;

	/* propagate changes down to all active HTML widgets */
	for(i = 0; i < MAX_HTML_WIDGETS; i++)
	{
		if(html_widgets[i].active)
			XtVaSetValues(html_widgets[i].html,
				XmNenableBadHTMLWarnings, html_config[OPTIONS_BADHTML].value,
				NULL);
	}
}

static void
optionsCB(Widget w, XtPointer client_data, XtPointer call_data)
{
	XmString label;
	Boolean set = False;
	int i, argc = 0;
	Arg args[4];
	int item_no = MenuButtonGetId(w);

	switch(item_no)
	{
		/*
		* These are seven XmHTML On/Off resources so we can treat them
		* all in the same manner
		*/
		case OPTIONS_ANCHOR_BUTTONS:
		case OPTIONS_ANCHOR_HIGHLIGHT:
		case OPTIONS_STRICTHTML:
		case OPTIONS_BODY_COLORS:
		case OPTIONS_BODY_IMAGES:
		case OPTIONS_ALLOW_COLORS:
		case OPTIONS_ALLOW_FONTS:
		case OPTIONS_ENABLE_IMAGES:
		case OPTIONS_JUSTIFY:
		case OPTIONS_ANIMATION_FREEZE:

			/* get value */
			XtVaGetValues(w, XmNset, &set, NULL);

			/* check if changed */
			if(set == html_config[item_no].value)
				break;
			/* store new value */
			html_config[item_no].value = set;

			Debug(("optionsCB, setting value for resource %s to %s\n",
				html_config[item_no].name, set ? "True" : "False"));

			/* set new value */
			XtSetArg(args[argc], html_config[item_no].name,
				html_config[item_no].value);
			argc++;

			/*
			* if global image support has been toggled, toggle other buttons
			* as well.
			*/
			if(item_no == OPTIONS_ENABLE_IMAGES)
			{
				XtSetSensitive(html_config[OPTIONS_BODY_IMAGES].w, set);
				XtSetSensitive(html_config[OPTIONS_AUTOLOAD_IMAGES].w, set);
				XtSetSensitive(load_images, set);

				/* set corresponding resources */
				XtSetArg(args[argc],
					html_config[OPTIONS_BODY_IMAGES].name,
					set ? html_config[OPTIONS_BODY_IMAGES].value:False);
				argc++;
			}

			/* propagate changes down to all active HTML widgets */
			for(i = 0; i < MAX_HTML_WIDGETS; i++)
			{
				if(html_widgets[i].active)
					XtSetValues(html_widgets[i].html, args, argc);
			}
			break;

		case OPTIONS_ANCHOR_TIPS:
			/* get value */
			XtVaGetValues(w, XmNset, &set, NULL);
			/* store new value */
			html_config[item_no].value = set;
			break;

		case OPTIONS_AUTOLOAD_IMAGES:
		case OPTIONS_ANCHOR_TRACKING:
			/* get value */
			XtVaGetValues(w, XmNset, &set, NULL);

			/* check if changed */
			if(set == html_config[item_no].value)
				break;

			/* store new value */
			html_config[item_no].value = set;

			Debug(("optionsCB, setting value for %s to %s\n",
				html_config[item_no].name, set ? "True" : "False"));

			/* change label as well */
			if(set)
				label = XmStringCreateLocalized("Reload Images");
			else
				label = XmStringCreateLocalized("Load Images");
			XtVaSetValues(load_images,
				XmNlabelString, label,
				NULL);
			XmStringFree(label);
			break;

		case OPTIONS_GENERAL:
		case OPTIONS_DOCUMENT:
		case OPTIONS_ANCHOR:
		case OPTIONS_FONTS:
		case OPTIONS_IMAGES:
			fprintf(stderr, "Configure %s not ready.\n", XtName(w));
			break;
		default:
			fprintf(stderr, "optionsCB: impossible menu selection "
				"(item_no = %i)\n", item_no);
	}
}

/*****
* Name:	     HTMhelp
* Return Type:	int
* Description:	main for example 2
* In:
*	argc:		no of command line arguments
*	argv:		array of command line arguments
* Returns:
*	EXIT_FAILURE when an error occurs, EXIT_SUCCESS otherwise
*****/
int
HTMLhelp(Widget parent,char *use_file)
{
	Widget main_w, menubar, menu, rc, form, sep, fr, pb;
	Widget option_menu;
	Widget frame = NULL;	/* shutup compiler */
	Display *display = NULL; /* shutup compiler */
	XmString xms;
	int i;
	String font = NULL, charset = NULL;
	Visual *visual = NULL;
	int depth = 0;
	Colormap colormap = 0;
	Boolean arg_error = False;
	Arg arg[20];
	int ac;

	root_window = False;
	progressive_images = False;

	html_config[OPTIONS_ANCHOR_TIPS].value = True;

	/* set the debugging levels */
	/*_XmHTMLSetDebugLevels(&argc, argv);*/

	/* set current working directory as the first path to search */
#ifdef _POSIX_SOURCE
	getcwd((char*)(paths[0]), sizeof(paths[0]));
#else
	getwd((char*)(paths[0]));
#endif
	strcat((char*)(paths[0]), "/");
	max_paths = 1;

	toplevel = parent;

	/* check if visual, depth or colormap have been given 
	   if(getStartupVisual(parent, &visual, &depth, &colormap))
	   XtVaSetValues(parent,
	   XmNvisual, visual,
	   XmNdepth, depth,
	   XmNcolormap, colormap,
	   NULL);
	*/ 
 
	/* create mainWindow */

	main_w = toplevel;

	/* simple menubar with a File and Help menu */
	ac = 0;
	XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
	XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg (arg[ac], XmNresizePolicy, XmRESIZE_ANY); ac++;
	menubar = XmCreateMenuBar(main_w, "menubar", arg, ac);

	/* the File and View Submenu have the toplevel widget id as callback data */
	for(i = 0; i < XtNumber(fileMenu); i++)
		fileMenu[i].client_data = (XtPointer)toplevel;
	for(i = 0; i < XtNumber(viewMenu); i++)
		viewMenu[i].client_data = (XtPointer)toplevel;

	menu = MenuBuild(menubar, XmMENU_PULLDOWN, "file", 'F', fileMenu);
	reload = MenuFindButtonById(menu, FILE_RELOAD);

	/* the Edit menu has the toplevel widget id as callback data */
	for(i = 0; i < XtNumber(editMenu); i++)
		editMenu[i].client_data = (XtPointer)toplevel;

	/* now create Edit menu */
	menu = MenuBuild(menubar, XmMENU_PULLDOWN, "edit", 'E', editMenu);

	/* create Options menu */
	option_menu = MenuBuild(menubar, XmMENU_PULLDOWN, "option", 'O',
					optionMenu);

	/* and store menu button Widget ID's */
	for(i = OPTIONS_START; i < OPTIONS_SAVE + 1; i++)
		html_config[i].w = MenuFindButtonById(option_menu, i);

	/* menubar is done */
	XtManageChild(menubar);

	/* form as the workarea */
	form = XtVaCreateWidget("form",
				xmFormWidgetClass, main_w,
				XmNtopAttachment, XmATTACH_WIDGET,
				XmNtopWidget, menubar,
				XmNleftAttachment, XmATTACH_FORM,
				XmNrightAttachment, XmATTACH_FORM,
				XmNbottomAttachment, XmATTACH_FORM,
				XmNresizePolicy, XmRESIZE_ANY,
				NULL);

	/* a rowcol for the buttons */
	rc = XtVaCreateWidget("rowColumn",
			      xmRowColumnWidgetClass, form,
			      XmNtopAttachment, XmATTACH_FORM,
			      XmNleftAttachment, XmATTACH_FORM,
			      XmNrightAttachment, XmATTACH_FORM,
			      XmNadjustLast, False,
			      XmNspacing, 0,
			      XmNpacking, XmPACK_COLUMN,
			      XmNorientation, XmVERTICAL,
			      XmNnumColumns, 4,
			      NULL);
	
	/* the back button */
	back  = XtVaCreateManagedWidget("Back",
		xmPushButtonWidgetClass, rc,
		XmNsensitive, False,
		NULL);

	/* callback */
	XtAddCallback(back , XmNactivateCallback,
		(XtCallbackProc)historyCB, (XtPointer)0);
		
	/* the forward button */
	forward  = XtVaCreateManagedWidget("Forward",
		xmPushButtonWidgetClass, rc,
		XmNsensitive, False,
		NULL);

	/* callback */
	XtAddCallback(forward, XmNactivateCallback,
		(XtCallbackProc)historyCB, (XtPointer)1);

	/* load images button */
	load_images = XtVaCreateManagedWidget(
		(html_config[OPTIONS_AUTOLOAD_IMAGES].value ? "Reload Images" :
			"Load Images"),
		xmPushButtonWidgetClass, rc,
		XmNsensitive, False,
		NULL);

	/* callback */
	XtAddCallback(load_images, XmNactivateCallback,
		(XtCallbackProc)flushImages, NULL);

	/* the Link button */
	link_button = XtCreateWidget("Document Links",
		xmPushButtonWidgetClass, rc, NULL, 0);

	/* callback */
	XtAddCallback(link_button, XmNactivateCallback,
		(XtCallbackProc)linkButtonCB, NULL);

	/* suspend/activate/kill progressive image loading */
	if(progressive_images)
	{
		prg_button = XtVaCreateWidget("Suspend Image Load",
			xmPushButtonWidgetClass, rc,
			XmNuserData, (XtPointer)STREAM_OK,
			XmNsensitive, False,
			NULL);

		/* callback */
		XtAddCallback(prg_button, XmNactivateCallback,
			(XtCallbackProc)progressiveButtonCB, (XtPointer)0);
	}

	/* done with buttonbar, manage it */
	XtManageChild(rc);

	/* a separator between the rc and the html widget */
	sep = XtVaCreateManagedWidget("separator",
		xmSeparatorGadgetClass, form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, rc,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		XmNorientation, XmHORIZONTAL,
		NULL);

	/* create a frame as the html container */
	if(!noframe)
		frame = XtVaCreateManagedWidget("frame",
			xmFrameWidgetClass, form,
			XmNtopAttachment, XmATTACH_WIDGET,
			XmNtopWidget, sep,
			XmNtopOffset, 10,
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, 10,
			XmNrightAttachment, XmATTACH_FORM,
			XmNrightOffset, 10,
			XmNshadowType, XmSHADOW_IN,
			NULL);

	/* create the HTML widget using the default resources */
	if(root_window)
	{
		/* create the HTML widget using the default resources */
		html_widgets[0].html = XtVaCreateManagedWidget("helpHtml",
			xmHTMLWidgetClass, (noframe ? form : frame),
			XmNanchorVisitedProc, testAnchor,
			XmNimageProc, loadImage,
			XmNimageEnable, True,
			XmNprogressiveReadProc, getImageData,
			XmNprogressiveEndProc, endImageData,
#ifdef HAVE_GIF_CODEC
			XmNdecodeGIFProc, decodeGIFImage,
#endif
			/*
			* Is isn't the way to this, but then again ...
			* it's just an example! 
			*/
			XmNheight, DisplayHeight(display, DefaultScreen(display))-125,
			XmNwidth, DisplayWidth(display, DefaultScreen(display))-50,
			NULL);

			/* arm callback for passing events to the window manager */
			XtAddCallback(html_widgets[0].html, XmNarmCallback,
				(XtCallbackProc)armCB, NULL);
	}
	else
	{
		html_widgets[0].html = XtVaCreateManagedWidget("helpHtml",
			xmHTMLWidgetClass, (noframe ? form : frame),
			XmNanchorVisitedProc, testAnchor,
			XmNimageProc, loadImage,
			XmNimageEnable, True,
			XmNprogressiveReadProc, getImageData,
			XmNprogressiveEndProc, endImageData,
			XmNscrollBarDisplayPolicy, XmAS_NEEDED,
#ifdef HAVE_GIF_CODEC
			XmNdecodeGIFProc, decodeGIFImage,
#endif
			NULL);
	}
	html_widgets[0].active = True;
	html_widgets[0].used   = True;
	XtVaSetValues(noframe ? html_widgets[0].html: frame,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, sep,
		XmNtopOffset, 10,
		XmNleftAttachment, XmATTACH_FORM,
		XmNleftOffset, 10,
		XmNrightAttachment, XmATTACH_FORM,
		XmNrightOffset, 10,
		NULL);

	/* anchor activation callback */
	XtAddCallback(html_widgets[0].html, XmNactivateCallback,
		(XtCallbackProc)anchorCB, NULL);

	/* anchor tracking callback */
	XtAddCallback(html_widgets[0].html, XmNanchorTrackCallback,
		(XtCallbackProc)trackCB, NULL);

	/* HTML frame callback */
	XtAddCallback(html_widgets[0].html, XmNframeCallback,
		(XtCallbackProc)frameCB, NULL);

	/* set the HTML document callback */
	XtAddCallback(html_widgets[0].html, XmNdocumentCallback,
		(XtCallbackProc)docCB, NULL);

	/* link callback for site structure */
	XtAddCallback(html_widgets[0].html, XmNlinkCallback,
		(XtCallbackProc)linkCB, NULL);

	/* form callback for HTML forms */
	XtAddCallback(html_widgets[0].html, XmNformCallback,
		(XtCallbackProc)formCB, NULL);

	/* focus callbacks so we can track which widget is current */
	XtAddCallback(html_widgets[0].html, XmNfocusCallback,
		(XtCallbackProc)focusCB, NULL);

	XtAddCallback(html_widgets[0].html, XmNlosingFocusCallback,
		(XtCallbackProc)focusCB, NULL);
	/*
	* now create the togglebuttons for XmHTML On/Off resources:
	* XmNenableBadHTMLWarnings
	* XmNstrictHTMLChecking
	* XmNenableBodyColors
	* XmNenableBodyImages
	* XmNenableDocumentColors
	* XmNenableDocumentFonts
	* XmNimageEnable
	* XmNenableOutlining
	* XmNfreezeAnimations
	* Also get the current default values for fontFamily and charset, we
	* want to honor the meta HTTP-EQUIV Content-Type info when it contains
	* a charset specification, and we also treat a meta NAME font spec
	* to set the basefont of a document (a kludge for some sort of style
	* control).
	*/
	/* get widget defaults */
	XtVaGetValues(html_widgets[0].html,
		XmNanchorButtons, &html_config[OPTIONS_ANCHOR_BUTTONS].value,
		XmNhighlightOnEnter, &html_config[OPTIONS_ANCHOR_HIGHLIGHT].value,
		XmNenableBadHTMLWarnings, &html_config[OPTIONS_BADHTML].value,
		XmNstrictHTMLChecking, &html_config[OPTIONS_STRICTHTML].value,
		XmNenableBodyColors, &html_config[OPTIONS_BODY_COLORS].value,
		XmNenableBodyImages, &html_config[OPTIONS_BODY_IMAGES].value,
		XmNenableDocumentColors,
			&html_config[OPTIONS_ALLOW_COLORS].value,
		XmNenableDocumentFonts,
			&html_config[OPTIONS_ALLOW_FONTS].value,
		XmNenableOutlining, &html_config[OPTIONS_JUSTIFY].value,
		XmNfreezeAnimations, &html_config[OPTIONS_ANIMATION_FREEZE].value,
		XmNimageEnable, &html_config[OPTIONS_ENABLE_IMAGES].value,
		XmNfontFamily, &font,
		XmNcharset, &charset,
		NULL);

	/* and update warning menu state */
	pb = MenuFindButtonById(option_menu, OPTIONS_BADHTML);
	updateWarningMenu(pb, html_config[OPTIONS_BADHTML].value);

	/* save default font information */
	strcpy(default_font, font);
	strcpy(current_font, font);

	/* save default charset information */
	strcpy(default_charset, charset);
	strcpy(current_charset, charset);

	/* if image support is disabled, make all image related buttons insens. */
	if(!html_config[OPTIONS_ENABLE_IMAGES].value)
	{
		XtSetSensitive(html_config[OPTIONS_BODY_IMAGES].w, False);
		XtSetSensitive(html_config[OPTIONS_AUTOLOAD_IMAGES].w, False);
		XtSetSensitive(load_images, False);
	}

	/* a frame in which to display current hrefs */
	fr = XtVaCreateManagedWidget("docInfoForm",
		xmFrameWidgetClass, form,
		XmNshadowType,	XmSHADOW_IN,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		XmNbottomAttachment,XmATTACH_FORM,
		XmNrightOffset, 10,
		XmNleftOffset, 10,
		XmNbottomOffset, 10,
		NULL);

	pb = XtVaCreateManagedWidget("anchorRefFrame",
		xmFormWidgetClass, fr,
		NULL);

	/* create a label to contain name of current anchor */
	xms = XmStringCreateLocalized("<no file loaded>");
	label = XtVaCreateManagedWidget("anchorRef",
		xmLabelGadgetClass, pb,
		XmNalignment, XmALIGNMENT_BEGINNING,
		XmNlabelString, xms,
		XmNtopAttachment, XmATTACH_FORM,
		XmNleftAttachment, XmATTACH_FORM,
		NULL);
	XmStringFree(xms);

	/* a label to indicate whether parser succeeded with verification */
	xms = XmStringCreateLocalized("			");
	verified = XtVaCreateManagedWidget("documentVerified",
		xmLabelGadgetClass, pb,
		XmNalignment, XmALIGNMENT_CENTER,
		XmNlabelString, xms,
		XmNtopAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		NULL);

	/* a label to indicate HTML 3.2 conformance */
	html32 = XtVaCreateManagedWidget("documentHTML32",
		xmLabelGadgetClass, pb,
		XmNalignment, XmALIGNMENT_CENTER,
		XmNlabelString, xms,
		XmNtopAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_WIDGET,
		XmNrightWidget, verified,
		XmNrightOffset, 5,
		XmNleftOffset, 10,
		NULL);
	XmStringFree(xms);

	/* set final constraints on the html container */
	XtVaSetValues((noframe ? html_widgets[0].html : frame),
		XmNbottomAttachment, XmATTACH_WIDGET,
		XmNbottomWidget, fr,
		XmNbottomOffset, 10,
		NULL);

	/* store the html widget as callback data for the file menu */
	XtVaSetValues(menu, XmNuserData, html_widgets[0].html, NULL);

	/* all widgets have been created, manage them */
	XtManageChild(menubar);
	XtManageChild(form);

	/*****
	* Popup menu for displaying image/link information
	* button 0: follow this link
	* button 1: open this image
	* button 2: show image info
	* button 3: enable/disable auto image tracking
	*****/
	XtVaGetValues(html_widgets[0].html, XmNworkWindow, &pb, NULL);
	info_popup = XmCreatePopupMenu(pb, "infoPopup", NULL, 0);
	for(i = 0; i < 4; i++)
	{
		pb = XtVaCreateManagedWidget("popupButton", xmPushButtonGadgetClass,
			info_popup, NULL);
		XtAddCallback(pb, XmNactivateCallback, (XtCallbackProc)infoPopupCB,
			(XtPointer)i);
	}
	/*
	* An event handler that needs something of XmHTML should attach
	* itself to the workWindow
	*/
	pb = NULL;
	XtVaGetValues(html_widgets[0].html, XmNworkWindow, &pb, NULL);

	/*
	* Initialize object caching which we will be using for image caching.
	* We set a cache of 5MB (= 5242880 bytes).
	* Since the objects will all be of type XmImageInfo we need to have a
	* handle to the HTML widget using this cache.
	* Currently, the cache is global for *all* HTML widgets which could
	* give problems when images in frames are cached: the widget id provided
	* will *not* be the one which was used to create the XmImage for a frame.
	* Maybe I should move the client_data argument to the cleanObjectCache
	* function instead of having it here, or provide a sort of cacheContext.
	* (or even make it into an X Object).
	*/
	initCache(5*1024*1024, (cleanObjectProc)destroyCacheObject,
		(sizeObjectProc)getInfoSize, (XtPointer)html_widgets[0].html);

	/* and the event handler */
	XtAddEventHandler(pb, ButtonPressMask, 0,
		(XtEventHandler)infoCB, info_popup);
	html_widgets[0].has_popup = True;

	/* if we have a file, load it */
	
	{
		String filename;
		/* get full filename */
		if((filename = resolveFile(use_file)) != NULL)
		{
			/* load the file, will also update the document cache */
			loadAndOrJump(filename, NULL, True);
			free(filename);
		}
	}

	/* never reached, but keeps compiler happy */
	return 0;
}

/*****
* Find code by David Lewis, dbl@ics.com
*****/
static void setTitle(Widget dialog);
static void findOkCB();
static void findCancelCB();
static void findClearCB();

static void *cacheData = NULL;

static void
findString(Widget w)
{
	static Widget dialog;

	if(!dialog)
	{
		Widget pb;
		Arg args[5];
		int n = 0;
		XmString xms;

		XtSetArg (args[n], XmNautoUnmanage, False); n++;
		XtSetArg (args[n], XmNtitle, "Find Dialog"); n++;
		dialog = XmCreatePromptDialog(w, "prompt", args, n);

		XtAddCallback(dialog, XmNokCallback, findOkCB, (XtPointer)dialog);

		/* Configure OK button */
		pb = XmSelectionBoxGetChild(dialog, XmDIALOG_OK_BUTTON);
		xms = XmStringCreateLocalized("Find");
		XtVaSetValues(pb , XmNlabelString, xms, NULL);
		XmStringFree(xms);

		/* Configure Cancel button */
		pb = XmSelectionBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON);
		xms = XmStringCreateLocalized("Cancel");
		XtVaSetValues(pb, XmNlabelString, xms, NULL);
		XmStringFree(xms);
		XtAddCallback(pb, XmNactivateCallback, findCancelCB, dialog);

		/* Configure Help button */
		pb = XmSelectionBoxGetChild(dialog, XmDIALOG_HELP_BUTTON);
		xms = XmStringCreateLocalized("Clear");
		XtVaSetValues(pb, XmNlabelString, xms, NULL);
		XmStringFree(xms);
		XtAddCallback(pb, XmNactivateCallback, findClearCB, dialog);
	}
	setTitle(dialog);

	XtManageChild(dialog);
	XtPopup(XtParent(dialog), XtGrabNone);
	XMapRaised(XtDisplay(dialog), XtWindow(XtParent(dialog)));
}

static void
setTitle(Widget dialog)
{
	XmString xms;

	if(cacheData != NULL)
		xms = XmStringCreateLocalized("Find Again: ");
	else
		xms = XmStringCreateLocalized("Find (from beginning): ");

	XtVaSetValues(dialog,XmNselectionLabelString, xms, NULL); 

	XmStringFree(xms);
}

static void
setString(Widget dialog, char *string)
{
	Widget text = XmSelectionBoxGetChild(dialog, XmDIALOG_TEXT);
	XmString xms = NULL; 

	if(string)
		xms = XmStringCreateLocalized(string);
	XtVaSetValues(dialog, XmNtextString, xms, NULL);

	XmProcessTraversal(text, XmTRAVERSE_CURRENT);

	if(xms)
		XmStringFree(xms);
}

/* FIND: find a word [non-space-containing] (can be a pattern) in a word
** (space-delimited), and highlight the entire word containing that pattern
** temporarily
** TBC: handle space delimiting better
** TBC: should ensure scroll sideways, too
** TBC: use current frame not main widget 
*/ 
static void
findOkCB(Widget widget, XtPointer client_data, XtPointer call_data)
{
	XmSelectionBoxCallbackStruct *cbs;
	Widget dialog;
	String s;

	cbs = (XmSelectionBoxCallbackStruct*)call_data;
	dialog = (Widget)client_data;

	if(XmStringGetLtoR(cbs->value, XmFONTLIST_DEFAULT_TAG, &s))
	{
		if(s && (strcmp("",s)))
		{
			Widget html = html_widgets[0].html;
			void *data;
			char *windex;

			windex = index(s,' ');
			if(windex)
			{
				windex[0] = '\0';
				setString(dialog, s);
			} 

			if((data = FindString(html, s, cacheData)) != NULL)
			{
				XRectangle rect;
				int line = FindLineFromData(data);

				XmHTMLTextScrollToLine(html, line);
				XmUpdateDisplay(html);
				if(FindCoordsFromData(data, &rect))
				{
					XmHTMLWidget htmlW = (XmHTMLWidget)html;
					GC gc;
					XGCValues gcv;

					gcv.foreground = BlackPixelOfScreen(XtScreen(html));
					gc = XtGetGC(html, GCForeground, &gcv);
					XDrawRectangle(XtDisplay(html),
						XtWindow(htmlW->html.work_area), gc,
						rect.x - htmlW->html.scroll_x,
						rect.y - htmlW->html.scroll_y,
					rect.width, rect.height);
					XtReleaseGC(html, gc);
				}
			}
			else
			{
				/* failure dialog */
				Widget popup;
				XmString tcs;
				Arg args[2]; int n = 0;

				tcs = XmStringCreateLocalized("No more occurrences found.");

				XtSetArg(args[n], XmNmessageString, tcs); n++;
				XtSetArg(args[n], XmNtitle, "Out of Data"); n++;

				popup = XmCreateInformationDialog(dialog, "zil", args, n);

				XtUnmanageChild(XmMessageBoxGetChild(popup,
					XmDIALOG_CANCEL_BUTTON));
				XtUnmanageChild(XmMessageBoxGetChild(popup,
					XmDIALOG_HELP_BUTTON));
				XtManageChild(popup);
				XmStringFree(tcs);
			}
			cacheData = data;
			setTitle(dialog);
		}
		XtFree(s);	
	}
}

static void
findClearCB(Widget widget, XtPointer client_data, XtPointer call_data)
{
	Widget dialog = (Widget) client_data;

	cacheData = NULL;
	setTitle(dialog);
	setString(dialog,NULL);
}

static void
findCancelCB(Widget widget, XtPointer client_data, XtPointer call_data)
{
	cacheData = NULL;
}

