/* usrHtml.c - configuration file for the eNavigator and HTMLWorks products */

/* Copyright 1984-1999 Wind River htmlSystems, Inc. */

/*
modification history
--------------------
01f,25mar99,gmk  fixing class and function names
01e,24mar99,lcs  Add calls to htmlEnavigatorLibInit() & htmlWorksLibInit() for 
		 pagesign support, add declaration of <htmlSSLInclude>, 
		 add 3 new macros to define the application's name, 
		 release no & version. 
01d,09mar99,rlp  relocated home page.
01c,08mar99,rlp  break our htmlSECInit to a func. pointer.
01b,25feb99,rlp  added htmlNetTLModules table for the SSL support.
		 removed header file section.
01a,18feb99,rlp  written.
*/

/*
DESCRIPTION
This file configures and initializes both eNavigator and HTMLWorks products.
The only user callable routine is usrHtmlInit().

NOMANUAL
*/

/* define */

/*
 * set the margins around the HTML display area based on the product
 * we're building.
 */

#ifdef	INCLUDE_HTMLWORKS
#define	HTML_TOP_MARGIN			HTML_WORKS_TOP_MARGIN
#define	HTML_LEFT_MARGIN		HTML_WORKS_LEFT_MARGIN
#define	HTML_RIGHT_MARGIN		HTML_WORKS_RIGHT_MARGIN
#define	HTML_BOTTOM_MARGIN		HTML_WORKS_BOTTOM_MARGIN
#else	/* INCLUDE_HTMLWORKS */
#define	HTML_TOP_MARGIN			HTML_ENAV_TOP_MARGIN
#define	HTML_LEFT_MARGIN		HTML_ENAV_LEFT_MARGIN
#define	HTML_RIGHT_MARGIN		HTML_ENAV_RIGHT_MARGIN
#define	HTML_BOTTOM_MARGIN		HTML_ENAV_BOTTOM_MARGIN
#endif	/* INCLUDE_HTMLWORKS */

/* Screen size -- used to select the UI layout definition and splash screen. */

#define	HTML_SCREEN_HALF_VGA		 3200240
#define	HTML_SCREEN_VGA			 6400480
#define	HTML_SCREEN_600			 8000600
#define	HTML_SCREEN_768			10240768
#define	HTML_SCREEN_864			11520864
#define	HTML_SCREEN_900			11520900
#define	HTML_SCREEN_1024		12001024

/* Make a meaningful name for the screen size.  */

/* Make a meaningful name for the screen size.  */
#if     HTML_SCREEN_SIZE == HTML_SCREEN_HALF_VGA
#define HTML_SCREEN_SIZE_NAME           "320x240"
#elif   HTML_SCREEN_SIZE == HTML_SCREEN_VGA
#define HTML_SCREEN_SIZE_NAME           "640x480"
#elif   HTML_SCREEN_SIZE == HTML_SCREEN_600
#define HTML_SCREEN_SIZE_NAME           "800x600"
#elif   HTML_SCREEN_SIZE == HTML_SCREEN_768
#define HTML_SCREEN_SIZE_NAME           "1024x768"
#else
#error "No valid screen size defined."
#endif

/* Base URL for the application's UI files. */

#ifdef	INCLUDE_HTMLWORKS
#define	HTML_UI_BASE		HTML_LOCAL_CONTENT_URL "htmlworks/" \
							HTML_SCREEN_SIZE_NAME
#define	HTML_SCREENSAVER_BASE	HTML_LOCAL_CONTENT_URL "htmlworks/screensaver/"
#define	HTML_START_URL		HTML_WORKS_START_URL
#else	/* INCLUDE_HTMLWORKS */
#define	HTML_UI_BASE		HTML_LOCAL_CONTENT_URL HTML_SCREEN_SIZE_NAME
#define	HTML_SCREENSAVER_BASE	HTML_LOCAL_CONTENT_URL "screensaver/"
#define	HTML_START_URL		HTML_ENAV_START_URL
#endif	/* INCLUDE_HTMLWORKS */

/* "Faves" page URL. */
#define HTML_FAVE_PAGE		HTML_LOCAL_CONTENT_URL "faves/index.html"

/* Error pages base URL */
#define HTML_ERROR_BASE		HTML_LOCAL_CONTENT_URL "error/"

/* Initial URL */

#define HTML_LOGIN_PAGE		HTML_START_URL
#define HTML_USER_APP_PAGE	HTML_START_URL

/* global */

/*
 * Margin values -- make these ints, so we can take their
 * addresses in HTML_REG_INT().
 */

static uint32 overscanTop = HTML_TOP_MARGIN;
static uint32 overscanLeft = HTML_LEFT_MARGIN;
static uint32 overscanRight = HTML_RIGHT_MARGIN;
static uint32 overscanBottom = HTML_BOTTOM_MARGIN;
static uint32 tvBarHeight = 106 + HTML_TOP_MARGIN;
static uint32 tvGridTitleHeight = 50 + HTML_TOP_MARGIN;

/* Screensaver delay, ditto. */

static uint32 ssdelay = HTML_SCREENSAVER_DELAY;

/* Default font sizes. */

static uint32 fontSize1 = HTML_FONT_SIZE_1;
static uint32 fontSize2 = HTML_FONT_SIZE_2;
static uint32 fontSize3 = HTML_FONT_SIZE_3;
static uint32 fontSize4 = HTML_FONT_SIZE_4;
static uint32 fontSize5 = HTML_FONT_SIZE_5;
static uint32 fontSize6 = HTML_FONT_SIZE_6;
static uint32 fontSize7 = HTML_FONT_SIZE_7;

/* Network link type. */

static uint32 linkType = 0;	/* 0 - ethernet; 1 - html_modem */

/* Disable both of these html_modem features. */

static uint32	modemLCSSetting = HTML_REG_ATTR_MDM_LCS_FEATURE_NONE;
static uint32	modemCWSetting  = HTML_REG_ATTR_MDM_LCS_CWAIT_NONE;

/* define the Transport Interface Switch table. */

TLITable htmlNetTLModules[] =
    {
        {"socket",
        (TLIOpenFunc) socket,
        (TLICloseFunc) close,
        (TLIConnectFunc) connect,
        (TLISendFunc) send,
        (TLIRecvFunc) recv,
        (TLIAsynSelectFunc) select,
        (TLIShutDownFunc) shutdown,
        (TLIPropsFunc) DUMMY_SSL_FUNC},

#ifdef INCLUDE_HTML_SSL
        {"ssl",
        (TLIOpenFunc) htmlSSLSocket,
        (TLICloseFunc) htmlSSLClose,
        (TLIConnectFunc) htmlSSLConnect,
        (TLISendFunc) htmlSSLSend,
        (TLIRecvFunc) htmlSSLRecv,
        (TLIAsynSelectFunc) select,
        (TLIShutDownFunc) htmlSSLShutdown,
        (TLIPropsFunc) htmlSSLPropsFunc},
#endif  /* INCLUDE_HTML_SSL */

        {0,
        (TLIOpenFunc) DUMMY_SSL_FUNC,
        (TLICloseFunc) DUMMY_SSL_FUNC,
        (TLIConnectFunc) DUMMY_SSL_FUNC,
        (TLISendFunc) DUMMY_SSL_FUNC,
        (TLIRecvFunc) DUMMY_SSL_FUNC,
        (TLIAsynSelectFunc) DUMMY_SSL_FUNC,
        (TLIShutDownFunc) DUMMY_SSL_FUNC,
        (TLIPropsFunc) DUMMY_SSL_FUNC},
    };

/* SSL initialization function: if NULL, no SSL. */

#ifdef INCLUDE_HTML_SSL
int 	htmlSSLInclude = 1;
Status (*htmlSECInitFunc)() = htmlSECInit;
#else   /* INCLUDE_HTML_SSL */
int 	htmlSSLInclude = 0;
Status (*htmlSECInitFunc)() = NULL;
#endif  /* INCLUDE_HTML_SSL */

/* forward static declarations */

#ifdef	INCLUDE_ENAVIGATOR

LOCAL STATUS	usrENavigatorInit ();

extern void htmlStart (void);

#endif	/* INCLUDE_ENAVIGATOR */

#ifdef	INCLUDE_HTMLWORKS

LOCAL STATUS	usrHtmlWorksInit ();

extern void htmlWorksStart (void);

#endif	/* INCLUDE_HTMLWORKS */


regDefEntry html_regDefTable[] =
    {

    /* html_modem */
    
	{ 0, HTML_REG_OBJ_MODEM, HTML_REG_ATTR_ISP_MDM_LCS_FEATURE, 
	REG_DATATYPE_INT32, HTML_REG_INT(modemLCSSetting)},

	{ 0, HTML_REG_OBJ_MODEM, HTML_REG_ATTR_ISP_MDM_CWAIT,
	REG_DATATYPE_INT32, HTML_REG_INT(modemCWSetting)},

	{ 0, HTML_REG_OBJ_MODEM, HTML_REG_ATTR_ISP_MDM_DEVICENAME, 
	REG_DATATYPE_CHARSTRING, HTML_REG_STRING("/tyCo/0")},
    
    /* link */
    
	{ 0, HTML_REG_OBJ_LINK, HTML_REG_ATTR_LINK, REG_DATATYPE_INT32,
	HTML_REG_INT(linkType)},

    /* curisp */
    
	{ 0, HTML_REG_OBJ_CURISP, HTML_REG_ATTR_CURISP_INFO, 
	REG_DATATYPE_CHARSTRING, HTML_REG_STRING(HTML_REG_OBJ_REG800)},

	{ 0, HTML_REG_OBJ_CURISP, HTML_REG_ATTR_CURISP_HW,
	REG_DATATYPE_CHARSTRING, HTML_REG_STRING(HTML_REG_OBJ_MODEM)},

    /* reg800 */

	{ 0, HTML_REG_OBJ_REG800, HTML_REG_ATTR_ISP_LOGIN_NAME, 
	REG_DATATYPE_CHARSTRING, HTML_REG_STRING("yourLoginName")},

	{ 0, HTML_REG_OBJ_REG800, HTML_REG_ATTR_ISP_LOGIN_PASSWORD, 
	REG_DATATYPE_CHARSTRING, HTML_REG_STRING("yourPassword")},

	{ 0, HTML_REG_OBJ_REG800, HTML_REG_ATTR_ISP_PRIMARY_PHONE, 
	REG_DATATYPE_CHARSTRING, HTML_REG_STRING("1234567")},

	{ 0, HTML_REG_OBJ_REG800, HTML_REG_ATTR_ISP_DNSIP,
	REG_DATATYPE_CHARSTRING, HTML_REG_STRING("1.2.3.4")},

	{ 0, HTML_REG_OBJ_REG800, HTML_REG_ATTR_ISP_DNSIP2,
	REG_DATATYPE_CHARSTRING, HTML_REG_STRING("5.6.7.8")},

	{ 0, HTML_REG_OBJ_REG800, HTML_REG_ATTR_ISP_NSP_URL, 
	REG_DATATYPE_CHARSTRING, HTML_REG_STRING("http://www.wrs.com")},

	{ 0, HTML_REG_OBJ_REG800, HTML_REG_ATTR_ISP_NSP_UPDATE_URL, 
	REG_DATATYPE_CHARSTRING,
	HTML_REG_STRING("http://nspbox2.navio.com/SoftwareUpdate.nvf")},

	{ 0, HTML_REG_OBJ_REG800, HTML_REG_ATTR_ISP_NSP_ERROR_URL, 
	REG_DATATYPE_CHARSTRING,
	HTML_REG_STRING("http://nspbox2.navio.com/LogError.nve")},

    /* roots */

	{ 0, HTML_REG_OBJ_ROOTS, HTML_REG_ATTR_ROOTS_OVERSCAN_LEFT, 
	REG_DATATYPE_INT32, HTML_REG_INT(overscanLeft)},

	{ 0, HTML_REG_OBJ_ROOTS, HTML_REG_ATTR_ROOTS_OVERSCAN_TOP, 
	REG_DATATYPE_INT32, HTML_REG_INT(overscanTop)},

	{ 0, HTML_REG_OBJ_ROOTS, HTML_REG_ATTR_ROOTS_OVERSCAN_RIGHT, 
	REG_DATATYPE_INT32, HTML_REG_INT(overscanRight)},

	{ 0, HTML_REG_OBJ_ROOTS, HTML_REG_ATTR_ROOTS_OVERSCAN_BOTTOM, 
	REG_DATATYPE_INT32, HTML_REG_INT(overscanBottom)},

	{ 0, HTML_REG_OBJ_ROOTS, HTML_REG_ATTR_ROOTS_TV_BAR_HEIGHT, 
	REG_DATATYPE_INT32, HTML_REG_INT(tvBarHeight)},

	{ 0, HTML_REG_OBJ_ROOTS, HTML_REG_ATTR_ROOTS_TV_GRID_TITLE_HEIGHT, 
	REG_DATATYPE_INT32, HTML_REG_INT(tvGridTitleHeight)},

	{ 0, HTML_REG_OBJ_ROOTS, HTML_REG_ATTR_ROOTS_LOCAL,
	REG_DATATYPE_CHARSTRING, HTML_REG_STRING(HTML_LOCAL_CONTENT_URL)},

	{ 0, HTML_REG_OBJ_ROOTS, HTML_REG_ATTR_ROOTS_INIT,
	REG_DATATYPE_CHARSTRING, HTML_REG_STRING(HTML_START_URL)},

	{ 0, HTML_REG_OBJ_ROOTS, HTML_REG_ATTR_ROOTS_QUIT,
	REG_DATATYPE_CHARSTRING, HTML_REG_STRING(HTML_START_URL)},

	{ 0, HTML_REG_OBJ_ROOTS, HTML_REG_ATTR_ROOTS_TV_BAR,
	REG_DATATYPE_CHARSTRING,
	HTML_REG_STRING(HTML_LOCAL_CONTENT_URL"tvbar/tvbar.htm")},

	{ 0, HTML_REG_OBJ_ROOTS, HTML_REG_ATTR_ROOTS_BAR_CONTENT,
	REG_DATATYPE_CHARSTRING,
	HTML_REG_STRING(HTML_LOCAL_CONTENT_URL"tvbar/body.htm")},

	{ 0, HTML_REG_OBJ_ROOTS, HTML_REG_ATTR_ROOTS_TV_GRID_TITLE,
	REG_DATATYPE_CHARSTRING,
	HTML_REG_STRING(HTML_LOCAL_CONTENT_URL"tvgrid/gridbnr.htm")},

	{ 0, HTML_REG_OBJ_ROOTS, HTML_REG_ATTR_ROOTS_SHUTDOWN, 
	REG_DATATYPE_CHARSTRING,
	HTML_REG_STRING(HTML_LOCAL_CONTENT_URL"accntman/shutdown.htm")},

	{ 0, HTML_REG_OBJ_ROOTS, HTML_REG_ATTR_ROOTS_PKGS,
	REG_DATATYPE_CHARSTRING,
	HTML_REG_STRING(HTML_LOCAL_CONTENT_URL"pkgs/")},

	{ 0, HTML_REG_OBJ_ROOTS, HTML_REG_ATTR_ROOTS_LOCALE, 
	REG_DATATYPE_CHARSTRING, 
	HTML_REG_STRING(HTML_LOCAL_CONTENT_URL"locale/")},

	{ 0, HTML_REG_OBJ_APP, HTML_REG_ATTR_APP_UI_BASE,
	REG_DATATYPE_CHARSTRING, HTML_REG_STRING(HTML_UI_BASE)},

	{ 0, HTML_REG_OBJ_APP, HTML_REG_ATTR_APP_ERROR_BASE, 
	REG_DATATYPE_CHARSTRING, HTML_REG_STRING(HTML_ERROR_BASE)},

	{ 0, HTML_REG_OBJ_APP, HTML_REG_ATTR_APP_DEMOS_FLAG, 
	REG_DATATYPE_CHARSTRING, HTML_REG_STRING("false")},

#ifdef  INCLUDE_HTMLWORKS
        { 0, HTML_REG_OBJ_APP, HTML_REG_ATTR_APP_NAME,
        REG_DATATYPE_CHARSTRING, HTML_REG_STRING(HTML_WORKS_APP_NAME)},

        { 0, HTML_REG_OBJ_APP, HTML_REG_ATTR_APP_RELEASE_NO,
        REG_DATATYPE_CHARSTRING, HTML_REG_STRING(HTML_WORKS_RELEASE_NO)},

        { 0, HTML_REG_OBJ_APP, HTML_REG_ATTR_APP_RELEASE_VERSION,
        REG_DATATYPE_CHARSTRING, HTML_REG_STRING(HTML_WORKS_RELEASE_VERSION)},
#else
        { 0, HTML_REG_OBJ_APP, HTML_REG_ATTR_APP_NAME,
        REG_DATATYPE_CHARSTRING, HTML_REG_STRING(HTML_ENAV_APP_NAME)},

        { 0, HTML_REG_OBJ_APP, HTML_REG_ATTR_APP_RELEASE_NO,
        REG_DATATYPE_CHARSTRING, HTML_REG_STRING(HTML_ENAV_RELEASE_NO)},

        { 0, HTML_REG_OBJ_APP, HTML_REG_ATTR_APP_RELEASE_VERSION,
        REG_DATATYPE_CHARSTRING, HTML_REG_STRING(HTML_ENAV_RELEASE_VERSION)},
#endif /* INCLUDE_HTMLWORKS */
    
    /* Default locale. Last resort after user and system locale tried.*/

	{ 0, HTML_REG_OBJ_LOCALE_DFLT, HTML_REG_ATTR_LOCALE, 
	REG_DATATYPE_CHARSTRING, HTML_REG_STRING("en-US")},

    /*
     * These are the bare minimum strings the system needs using the default
     * locale. These are here just in case the file system is not accessible
     * where all the strings in all locales are kept (scorched earth case).
     * Never compile in strings for more than one locale.
     */

	{ 0, HTML_REG_OBJ_LOCALE_STRS_SYS"en-US",
	HTML_REG_ATTR_LOCALE_STRS_SYS_1, REG_DATATYPE_CHARSTRING,
	HTML_REG_STRING("OK")},

	{ 0, HTML_REG_OBJ_LOCALE_STRS_SYS"en-US",
	HTML_REG_ATTR_LOCALE_STRS_SYS_2, REG_DATATYPE_CHARSTRING,
	HTML_REG_STRING("Cancel")},

#ifdef	INCLUDE_HTML_MAIL
	{ 0, HTML_REG_OBJ_MAIL, HTML_REG_ATTR_MAIL_POP3,
	REG_DATATYPE_CHARSTRING, HTML_REG_STRING(HTML_MAIL_SERVER)},

	{ 0, HTML_REG_OBJ_MAIL, HTML_REG_ATTR_MAIL_SMTP,
	REG_DATATYPE_CHARSTRING, HTML_REG_STRING(HTML_MAIL_SERVER)},

	{ 0, HTML_REG_OBJ_MAIL, HTML_REG_ATTR_MAIL_DIR,
	REG_DATATYPE_CHARSTRING, HTML_REG_STRING(HTML_MAIL_BASE_DIR)},

	{ 0, HTML_REG_OBJ_MAIL, HTML_REG_ATTR_MAIL_NFS_HOST,
	REG_DATATYPE_CHARSTRING, HTML_REG_STRING(HTML_NFS_HOST)},
#endif	/* INCLUDE_HTML_MAIL */

	{ 0, HTML_REG_OBJ_SCREENSAVER, HTML_REG_ATTR_SCREENSAVER_DELAY, 
	REG_DATATYPE_INT32, HTML_REG_INT(ssdelay)},

	{ 0, HTML_REG_OBJ_SCREENSAVER, HTML_REG_ATTR_SCREENSAVER_URL, 
	REG_DATATYPE_CHARSTRING, HTML_REG_STRING (HTML_SCREENSAVER_BASE)}

    };

unsigned int html_regDefTabSize = (sizeof(html_regDefTable)/sizeof(regDefEntry));

/*
 * Most user registry values are set at login, the values here are to
 * provide default values before AccntMan has logged in a user for
 * demos and debugging
 */

regDefEntry html_regDefUserTable[] =
    {
	{ 0, HTML_REG_OBJ_USER_APP, HTML_REG_ATTR_USER_APP_LOGIN_PAGE,
	REG_DATATYPE_CHARSTRING, HTML_REG_STRING(HTML_LOGIN_PAGE)},
    
	{ 0, HTML_REG_OBJ_USER_APP, HTML_REG_ATTR_USER_TV_BAR_URL,
	REG_DATATYPE_CHARSTRING, HTML_REG_STRING("")},
    
	{ 0, HTML_REG_OBJ_USER_APP, HTML_REG_ATTR_USER_APP_PAGE,
	REG_DATATYPE_CHARSTRING, HTML_REG_STRING(HTML_USER_APP_PAGE)},
    
	{ 0, HTML_REG_OBJ_USER_APP, HTML_REG_ATTR_USER_FAV_PAGE,
	REG_DATATYPE_CHARSTRING, HTML_REG_STRING(HTML_FAVE_PAGE)},

	{ 0, HTML_REG_OBJ_USER_APP,  HTML_REG_ATTR_USER_SETTINGS_PAGE,
	REG_DATATYPE_CHARSTRING,
	HTML_REG_STRING(HTML_LOCAL_CONTENT_URL"accntman/options.htm")},

	{ 0, HTML_REG_OBJ_USER_APP, HTML_REG_ATTR_USER_ADDRBOOK_PAGE,
	REG_DATATYPE_CHARSTRING, HTML_REG_STRING("")},

	{ 0, HTML_REG_OBJ_USER_APP, HTML_REG_ATTR_USER_HELP_PAGE,
	REG_DATATYPE_CHARSTRING, HTML_REG_STRING("")},

	{ 0, HTML_REG_OBJ_USER_APP, HTML_REG_ATTR_USER_NAVIO_URL,
	REG_DATATYPE_CHARSTRING, HTML_REG_STRING("")},

	{ 0, HTML_REG_OBJ_USER_APP, HTML_REG_ATTR_USER_ITV_GUIDE_URL,
	REG_DATATYPE_CHARSTRING, HTML_REG_STRING("http://www.gist.com")},

	{ 0, HTML_REG_OBJ_USER_APP, HTML_REG_ATTR_USER_WEB_GUIDE_URL,
	REG_DATATYPE_CHARSTRING, HTML_REG_STRING("http://www.yahoo.com")},

	{ 0, HTML_REG_OBJ_USER_APP, HTML_REG_ATTR_USER_SEND_PAGE_URL,
	REG_DATATYPE_CHARSTRING, HTML_REG_STRING
	(HTML_LOCAL_CONTENT_URL"/mail/mail.htm?mailState=write")},

	{ 0, HTML_REG_OBJ_USER_APP, HTML_REG_ATTR_USER_MAIL_URL,
	REG_DATATYPE_CHARSTRING,
	HTML_REG_STRING(HTML_LOCAL_CONTENT_URL"mail/mail.htm")},

	{ 0, HTML_REG_OBJ_USER_APP, HTML_REG_ATTR_APP_FAVORITES_PNG_DIR,
	REG_DATATYPE_CHARSTRING, HTML_REG_STRING("/tmp")},

#ifdef	INCLUDE_HTML_MAIL
	{ 0, HTML_REG_OBJ_USER_UINFO, HTML_REG_ATTR_USER_UINFO_FIRSTNAME,
	REG_DATATYPE_CHARSTRING, HTML_REG_STRING("Default")},

	{ 0, HTML_REG_OBJ_USER_UINFO, HTML_REG_ATTR_USER_UINFO_LASTNAME,
	REG_DATATYPE_CHARSTRING, HTML_REG_STRING("User")},
    
	{ 0, HTML_REG_OBJ_USER_EMAIL, HTML_REG_ATTR_USER_EMAIL_PHYSICAL,
	REG_DATATYPE_CHARSTRING, HTML_REG_STRING("user@domain.com")},

	{ 0, HTML_REG_OBJ_USER_EMAIL, HTML_REG_ATTR_USER_EMAIL_PWD,
	REG_DATATYPE_CHARSTRING, HTML_REG_STRING("password")},

	{ 0, HTML_REG_OBJ_USER_EMAIL, HTML_REG_ATTR_USER_EMAIL_SIGNATURE,
	REG_DATATYPE_CHARSTRING, HTML_REG_STRING("signature")},
#endif	/* INCLUDE_HTML_MAIL */

    /* Default font sizes. */

	{ 0, HTML_REG_OBJ_USER_APP, HTML_REG_ATTR_USER_FONT_SIZE_1,
	REG_DATATYPE_INT32, HTML_REG_INT(fontSize1)},

	{ 0, HTML_REG_OBJ_USER_APP, HTML_REG_ATTR_USER_FONT_SIZE_2,
	REG_DATATYPE_INT32, HTML_REG_INT(fontSize2)},

	{ 0, HTML_REG_OBJ_USER_APP, HTML_REG_ATTR_USER_FONT_SIZE_3,
	REG_DATATYPE_INT32, HTML_REG_INT(fontSize3)},

	{ 0, HTML_REG_OBJ_USER_APP, HTML_REG_ATTR_USER_FONT_SIZE_4,
	REG_DATATYPE_INT32, HTML_REG_INT(fontSize4)},

	{ 0, HTML_REG_OBJ_USER_APP, HTML_REG_ATTR_USER_FONT_SIZE_5,
	REG_DATATYPE_INT32, HTML_REG_INT(fontSize5)},

	{ 0, HTML_REG_OBJ_USER_APP, HTML_REG_ATTR_USER_FONT_SIZE_6,
	REG_DATATYPE_INT32, HTML_REG_INT(fontSize6)},

	{ 0, HTML_REG_OBJ_USER_APP, HTML_REG_ATTR_USER_FONT_SIZE_7,
	REG_DATATYPE_INT32, HTML_REG_INT(fontSize7)}
    };

unsigned int html_regDefUserTabSize = (sizeof(html_regDefUserTable)/sizeof(regDefEntry));


/*******************************************************************************
*
* usrHtmlInit - configure and initialize eNavigator and HTMLWorks
*
* This routine configures and initializes both eNavigator and HTMLWorks
* products.
*
* RETURNS:
* OK, or
* ERROR if there is a problem in initializing HTML based products.
*
* NOMANUAL
*/

STATUS usrHtmlInit ()
    {
    STATUS html_status = OK;

    /* Common initialisation */

    if (memDrv () != OK)
	return ERROR;

    /* initialize HTMLWorks */

#ifdef	INCLUDE_HTMLWORKS
    {
    extern STATUS memDrvAddFiles_local_htmlworks_home (void);
    html_status = memDrvAddFiles_local_htmlworks_home ();
    }

    /* Mount the filesystem containing the appropriate UI content. */

    {
#if	HTML_SCREEN_SIZE == HTML_SCREEN_HALF_VGA
    extern STATUS memDrvAddFiles_local_htmlworks_320x240 (void);
    html_status = memDrvAddFiles_local_htmlworks_320x240 ();
#elif	HTML_SCREEN_SIZE == HTML_SCREEN_VGA
    extern STATUS memDrvAddFiles_local_htmlworks_640x480 (void);
    html_status = memDrvAddFiles_local_htmlworks_640x480 ();
#elif	HTML_SCREEN_SIZE == HTML_SCREEN_600
    extern STATUS memDrvAddFiles_local_htmlworks_800x600 (void);
    html_status = memDrvAddFiles_local_htmlworks_800x600 ();
#elif	HTML_SCREEN_SIZE == HTML_SCREEN_768
    extern STATUS memDrvAddFiles_local_htmlworks_1024x768 (void);
    html_status = memDrvAddFiles_local_htmlworks_1024x768 ();
#endif
    }
    if (html_status != OK)
	return ERROR;

    usrHtmlWorksInit ();
#endif	/* INCLUDE_HTMLWORKS */

    /* initialize eNavigator */

#ifdef	INCLUDE_ENAVIGATOR
    {
    extern STATUS memDrvAddFiles_local_home (void);
    html_status = memDrvAddFiles_local_home ();
    }

    /* Mount the filesystem containing the appropriate UI content. */

    {
#if	HTML_SCREEN_SIZE == HTML_SCREEN_HALF_VGA
    extern STATUS memDrvAddFiles_local_320x240 (void);
    html_status = memDrvAddFiles_local_320x240 ();
#elif	HTML_SCREEN_SIZE == HTML_SCREEN_VGA
    extern STATUS memDrvAddFiles_local_640x480 (void);
    html_status = memDrvAddFiles_local_640x480 ();
#elif	HTML_SCREEN_SIZE == HTML_SCREEN_600
    extern STATUS memDrvAddFiles_local_800x600 (void);
    html_status = memDrvAddFiles_local_800x600 ();
#elif	HTML_SCREEN_SIZE == HTML_SCREEN_768
    extern STATUS memDrvAddFiles_local_1024x768 (void);
    html_status = memDrvAddFiles_local_1024x768 ();
#endif
    }
    if (html_status != OK)
	return ERROR;

    usrENavigatorInit ();
#endif	/* INCLUDE_ENAVIGATOR */

    return (html_status);
    }


/*******************************************************************************
*
* usrENavigatorInit - configure and initialize eNavigator
*
* This routine configures and initializes eNavigator product.
*
* RETURNS:
* OK, or
* ERROR if there is a problem in initializing eNavigator.
*
* NOMANUAL
*/

#ifdef	INCLUDE_ENAVIGATOR
STATUS usrENavigatorInit ()
    {
    htmlEnavigatorLibInit();
    htmlStart ();
    return (OK);
    }
#endif	/* INCLUDE_ENAVIGATOR */


/*******************************************************************************
*
* usrHtmlWorksInit - configure and initialize HTMLWorks
*
* This routine configures and initializes HTMLWorks product.
*
* RETURNS:
* OK, or
* ERROR if there is a problem in initializing HTMLWorks.
*
* NOMANUAL
*/

#ifdef	INCLUDE_HTMLWORKS
STATUS usrHtmlWorksInit ()
    {
    htmlWorksLibInit ();
    htmlWorksStart ();
    return (OK);
    }
#endif	/* INCLUDE_HTMLWORKS */

