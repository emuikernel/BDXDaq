#ifndef __CODA_REGISTRY

#define __CODA_REGISTRY

#undef _ANSI_ARGS_
#undef CONST

#if ((defined(__STDC__) || defined(SABER)) && !defined(NO_PROTOTYPE)) || defined(__cplusplus)

#define _USING_PROTOTYPES_ 1
#define _ANSI_ARGS_(x)       x
#define CONST const

#ifdef __cplusplus

#define VARARGS(first) (first, ...)

#else

#define VARARGS(first) ()

#endif

#else

#define _ANSI_ARGS_(x) ()
#define CONST

#endif
 
#define EXTERN extern

#ifdef __cplusplus
extern "C" {
#endif





/*sergey: from codaRegister.c*/
typedef struct NameRegistry {
    Display *dispPtr;		/* Display from which the registry was
				 * read. */
    int locked;			/* Non-zero means that the display was
				 * locked when the property was read in. */
    int modified;		/* Non-zero means that the property has
				 * been modified, so it needs to be written
				 * out when the NameRegistry is closed. */
    unsigned long propLength;	/* Length of the property, in bytes. */
    char *property;		/* The contents of the property, or NULL
				 * if none.  See format description above;
				 * this is *not* terminated by the first
				 * null character.  Dynamically allocated. */
    int allocedByX;		/* Non-zero means must free property with
				 * XFree;  zero means use free. */
} NameRegistry;





/*sergey
void   *codaRegOpen _ANSI_ARGS_((Display *display, int lock));
void    codaRegClose _ANSI_ARGS_((void *regPtr));
*/
NameRegistry *codaRegOpen _ANSI_ARGS_((Display *display, int lock));
static void	  codaRegClose _ANSI_ARGS_((NameRegistry *regPtr));



char   *CODASetAppName _ANSI_ARGS_((Display *display, Window window, char *name));
char   *CODAGetAppNames _ANSI_ARGS_((Display *display));
int     CODAGetAppWindow _ANSI_ARGS_((Display *display, char *name));
int     coda_Send _ANSI_ARGS_((Display *display, char *destName, char *cmd));

  /*sergey*/
#ifndef USE_TK
int     codaSendInit _ANSI_ARGS_((Widget w, char *name));
#else
  /*int     codaSendInit(Tcl_Interp *interp, Tk_Widget *w, char *name);*/ /*sergey*/
#endif

void    codaRegisterMsgCallback _ANSI_ARGS_((void *callback));

#define coda_send coda_Send

#ifdef __cplusplus
}
#endif

#endif

