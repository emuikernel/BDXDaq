/*
 * ------------------------------------------------------------------------
 *      PACKAGE:  [incr Tcl]
 *  DESCRIPTION:  Object-Oriented Extensions to Tcl
 *
 *  [incr Tcl] provides object-oriented extensions to Tcl, much as
 *  C++ provides object-oriented extensions to C.  It provides a means
 *  of encapsulating related procedures together with their shared data
 *  in a local namespace that is hidden from the outside world.  It
 *  promotes code re-use through inheritance.  More than anything else,
 *  it encourages better organization of Tcl applications through the
 *  object-oriented paradigm, leading to code that is easier to
 *  understand and maintain.
 *
 *  This part supports the creation of "ensembles" which are like
 *  compound Tcl commands that can be arbitrarily extended.  An "ensemble"
 *  command has an associated namespace full of subcommand operations.
 *  Whenever the command is invoked, control is transferred to the
 *  subcommand namespace, and the remaining arguments are executed in
 *  that context.  Since a namespace can be arbitrarily extended with
 *  new commands, the functionality of an ensemble can grow.
 *
 *  For example, the usual Tcl "info" command could have been implemented
 *  as an ensemble.  If it were, there would be an "info" command which
 *  transfers control to an "info::" namespace.  Operations like "globals",
 *  "level", etc., would have been implemented as separate commands in
 *  the "info" namespace.
 *
 * ========================================================================
 *  AUTHOR:  Michael J. McLennan       Phone: (610)712-2842
 *           AT&T Bell Laboratories   E-mail: michael.mclennan@att.com
 *     RCS:  $Id: itcl_ensemble.c,v 1.1.1.1 1996/08/21 19:30:11 heyes Exp $
 * ========================================================================
 *             Copyright (c) 1993-1995  AT&T Bell Laboratories
 * ------------------------------------------------------------------------
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
#include <assert.h>
#include "tclInt.h"

static char rcsid[] = "$Id: itcl_ensemble.c,v 1.1.1.1 1996/08/21 19:30:11 heyes Exp $";

/*
 *  Data used to represent an ensemble:
 */
struct Ensemble;
typedef struct EnsembleOption {
    char *name;                 /* name of this option */
    int minChars;               /* chars needed to uniquely identify option */
    Command *cmdPtr;            /* command handling this option */
    int minArgs;                /* minimum args required for this option */
    int maxArgs;                /* maximum argc allowed for this option */
    char *usage;                /* usage string describing syntax */
    struct Ensemble* ensemble;  /* ensemble containing this option */
} EnsembleOption;

/*
 *  Data used to represent an ensemble:
 */
typedef struct Ensemble {
    Tcl_Interp *interp;         /* interpreter containing this ensemble */
    EnsembleOption **options;   /* list of options in this ensemble */
    int numOptions;             /* number of options in option list */
    int maxOptions;             /* current size of options list */
    Tcl_Command cmd;            /* command representing this ensemble */
    EnsembleOption* parent;     /* parent option for sub-ensembles
                                 * NULL => toplevel ensemble */
} Ensemble;

/*
 *  Data shared by ensemble access commands and ensemble parser:
 */
typedef struct EnsembleCommonInfo {
    Itcl_Namespace parserNs;      /* parser namespace */
    Ensemble* parsing;            /* add options to this ensemble */
} EnsembleCommonInfo;

/*
 *  Use this to keep track of the last option being handled
 *  by ItclHandleEnsemble().  This is needed by Itcl_EnsembleInvoc()
 *  to report the invocation path for the current option.
 */
static Ensemble* LastEnsemble = NULL;
static EnsembleOption* LastOption = NULL;

/*
 *  FLAGS used for ItclCreateEnsOption():
 *    ENS_REPLACE ......... replace any existing option with a new one
 *    ENS_USE_EXISTING .... return any existing option found
 */
#define ENS_REPLACE      0x00
#define ENS_USE_EXISTING 0x01

/*
 *  FORWARD DECLARATIONS
 */

static int ItclFindEnsemble _ANSI_ARGS_((Tcl_Interp* interp, char* name,
    Ensemble** rens));

static EnsembleOption* ItclCreateEnsOption _ANSI_ARGS_((Ensemble* ensData,
    char* name, int flags));
static void ItclDelEnsembleOption _ANSI_ARGS_((EnsembleOption* ensOpt));
static void ComputeMinChars _ANSI_ARGS_((Ensemble* ensData, int pos));

static int ItclFindEnsOption _ANSI_ARGS_((Tcl_Interp* interp,
    Ensemble* ensData, char* name, EnsembleOption **rensOpt));

static char* ItclEnsOptionUsage _ANSI_ARGS_((EnsembleOption* ensOpt));
static int ItclEnsOptionIsError _ANSI_ARGS_((EnsembleOption* ensOpt));

static void ItclDelEnsemble _ANSI_ARGS_((ClientData cdata));
static void ItclDelEnsembleCommon _ANSI_ARGS_((ClientData cdata));

static int ItclHandleEnsemble _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp* interp, int argc, char** argv));

/*
 * ------------------------------------------------------------------------
 *  ItclInitEnsemble()
 *
 *  Initializes the "ensemble" facility for the given interpreter by
 *  installing ensemble access commands.  This is usually invoked once
 *  when an interpreter is first created in Tcl_CreateInterp().
 *
 *  Returns TCL_OK on success, or TCL_ERROR (along with an error message
 *  in the interp->result) if something goes wrong.
 * ------------------------------------------------------------------------
 */
int
ItclInitEnsemble(interp)
    Tcl_Interp *interp;            /* interpreter to be updated */
{
    Itcl_Namespace ns;
    EnsembleCommonInfo *ensInfo;
    int status;

    /*
     *  Create an "ensemble-parser" namespace used to parse
     *  the body of an ensemble definition.  Add commands that
     *  are recognized within the body of the ensemble definition.
     */
    ensInfo = (EnsembleCommonInfo*)ckalloc(sizeof(EnsembleCommonInfo));
    ensInfo->parsing = NULL;

    status = Itcl_CreateNamesp(interp, "::tcl::ensemble-parser",
        Itcl_PreserveData((ClientData)ensInfo), Itcl_ReleaseData, &ns);

    Itcl_EventuallyFree((ClientData)ensInfo, ItclDelEnsembleCommon);

    ensInfo->parserNs = ns;

    if (status != TCL_OK) {
        Itcl_ReleaseData((ClientData)ensInfo);
        return TCL_ERROR;
    }
    Itcl_ClearImportNamesp(ns);  /* allow only commands in this namespace */

    Tcl_CreateCommand(interp, "::tcl::ensemble-parser::option",
        Itcl_EnsOptionCmd,
        Itcl_PreserveData((ClientData)ensInfo), Itcl_ReleaseData);

    Tcl_CreateCommand(interp, "::tcl::ensemble-parser::ensemble",
        Itcl_EnsEnsembleCmd,
        Itcl_PreserveData((ClientData)ensInfo), Itcl_ReleaseData);

    /*
     *  Install the Tcl access command for creating/updating an ensemble.
     */
    Tcl_CreateCommand(interp, "::ensemble", Itcl_EnsembleCmd,
        Itcl_PreserveData((ClientData)ensInfo), Itcl_ReleaseData);

    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_CreateEnsemble()
 *
 *  Creates a new ensemble with the given name.  This name is parsed as
 *  a space separated list of options identifying the ensemble, so this
 *  procedure can be used to create an ensemble within an ensemble as
 *  well:
 *
 *      name <=> "namespace::namespace::ensemble sub-ensemble ..."
 *
 *  If a command exists with the ensemble name, then it is destroyed.
 *  An empty ensemble is created in its place, and an access command
 *  is installed in the parent namespace.
 *
 *  If the name addresses a sub-ensemble, and if an option or sub-ensemble
 *  exists with the same name, it is replaced.
 *
 *  Returns TCL_OK on success, or TCL_ERROR (along with an error message
 *  in the interp->result) if something goes wrong.
 * ------------------------------------------------------------------------
 */
int
Itcl_CreateEnsemble(interp,name)
    Tcl_Interp *interp;            /* interpreter to be updated */
    char* name;                    /* name of the new ensemble */
{
    static Tcl_DString *buffer = NULL;

    int status;
    char *tail;
    Ensemble *ensParent;
    Ensemble *ensData;
    EnsembleOption *ensOpt;
    Command *cmdPtr;

    /*
     *  Create the data associated with the ensemble.
     */
    ensData = (Ensemble*)ckalloc(sizeof(Ensemble));
    ensData->interp = interp;
    ensData->numOptions = 0;
    ensData->maxOptions = 10;
    ensData->options = (EnsembleOption**)ckalloc(
        (unsigned)ensData->maxOptions*sizeof(EnsembleOption*)
    );
    ensData->cmd = NULL;
    ensData->parent = NULL;

    /*
     *  Find the tail component of the ensemble name path.
     */
    for (tail=name; *tail != '\0'; tail++)
        ;
    while ((tail >= name) && !isspace(*tail))
        tail--;

    tail++;

    /*
     *  If there is only one path component, then this is a
     *  toplevel ensemble.  Create the ensemble by installing
     *  its access command.
     */
    if (tail == name) {
        ensData->cmd = Tcl_CreateCommand(interp, name,
            ItclHandleEnsemble, (ClientData)ensData,
            ItclDelEnsemble);

        return TCL_OK;
    }

    /*
     *  Otherwise, this ensemble is contained within another parent.
     *  Find the data associated with the parent and install this
     *  new ensemble in its option table.
     */
    if (!buffer) {
        buffer = (Tcl_DString*)ckalloc(sizeof(Tcl_DString));
        Tcl_DStringInit(buffer);
    }
    Tcl_DStringTrunc(buffer,0);
    Tcl_DStringAppend(buffer, name, (tail-name));

    status = ItclFindEnsemble(interp, Tcl_DStringValue(buffer), &ensParent);

    if (ensParent == NULL) {
        Tcl_ResetResult(interp);
        Tcl_AppendResult(interp, "cannot create ensemble: ",
            "parent \"", Tcl_DStringValue(buffer), "\" does not exist",
            (char*)NULL);
        status = TCL_ERROR;
    }

    if (status != TCL_OK) {
        ItclDelEnsemble((ClientData)ensData);
        return TCL_ERROR;
    }

    ensOpt = ItclCreateEnsOption(ensParent, tail, ENS_REPLACE);

    ensData->cmd = ensParent->cmd;
    ensData->parent = ensOpt;

    cmdPtr = (Command*)ckalloc(sizeof(Command));
    cmdPtr->hPtr = NULL;
    cmdPtr->proc = ItclHandleEnsemble;
    cmdPtr->clientData = (ClientData)ensData;
    cmdPtr->deleteProc = ItclDelEnsemble;
    cmdPtr->deleteData = cmdPtr->clientData;
    cmdPtr->cacheInfo = NULL;

    ensOpt->cmdPtr = cmdPtr;
    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_AddEnsembleOption()
 *
 *  Adds a new option into an ensemble, or replaces an existing option.
 *  The ensemble must have been created previously by Itcl_CreateEnsemble().
 *  The parent ensemble name is represented as a space-separated path
 *  of components:
 *
 *      name <=> "namespace::namespace::ensemble sub-ensemble ..."
 *
 *  Any client data specified is automatically passed to the handling
 *  procedure whenever the option is invoked.  It is destroyed by the
 *  specified deleteProc when the option is removed.
 *
 *  Returns TCL_OK on success, or TCL_ERROR (along with an error message
 *  in the interp->result) if something goes wrong.
 * ------------------------------------------------------------------------
 */
int
Itcl_AddEnsembleOption(interp, ensName, optName, minArgs, maxArgs, usageInfo,
    proc, cdata, dproc)

    Tcl_Interp *interp;            /* interpreter to be updated */
    char* ensName;                 /* ensemble containing this option */
    char* optName;                 /* name of new option */
    int minArgs;                   /* minimum number of required args */
    int maxArgs;                   /* maximum number of required args */
    char* usageInfo;               /* usage info for argument list */
    Tcl_CmdProc *proc;             /* handling procedure for option */
    ClientData cdata;              /* client data associated with option */
    Tcl_CmdDeleteProc *dproc;      /* procedure used to destroy client data */
{
    Ensemble *ensData;
    EnsembleOption *ensOpt;
    Command *cmdPtr;

    /*
     *  Parse the ensemble name and look for a containing ensemble.
     */
    if (ItclFindEnsemble(interp, ensName, &ensData) != TCL_OK) {
        return TCL_ERROR;
    }
    else if (ensData == NULL) {
        Tcl_AppendResult(interp, "ensemble \"", ensName, "\" does not exist",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  Install the new option into the option list.
     */
    ensOpt = ItclCreateEnsOption(ensData, optName, ENS_REPLACE);

    ensOpt->minArgs = minArgs;
    ensOpt->maxArgs = maxArgs;
    if (usageInfo) {
        ensOpt->usage = ckalloc((unsigned)(strlen(usageInfo)+1));
        strcpy(ensOpt->usage, usageInfo);
    }

    cmdPtr = (Command*)ckalloc(sizeof(Command));
    cmdPtr->hPtr = NULL;
    cmdPtr->proc = proc;
    cmdPtr->clientData = cdata;
    cmdPtr->deleteProc = dproc;
    cmdPtr->deleteData = cdata;
    cmdPtr->cacheInfo = NULL;

    ensOpt->cmdPtr = cmdPtr;
    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_GetEnsembleOption()
 *
 *  Looks for an option within an ensemble, and if it is found, returns
 *  its associated data.  The ensemble must have been created previously
 *  by Itcl_CreateEnsemble().  The ensemble name is represented as a
 *  space-separated path of components:
 *
 *      name <=> "namespace::namespace::ensemble sub-ensemble ..."
 *
 *  If the option is found, its data is loaded into the cinfo buffer,
 *  and this procedure returns non-zero.  Otherwise, it returns zero,
 *  along with a possible error message in the interp->result.
 * ------------------------------------------------------------------------
 */
int
Itcl_GetEnsembleOption(interp,ensName,optName,cinfo)
    Tcl_Interp *interp;            /* interpreter to be updated */
    char* ensName;                 /* ensemble containing this option */
    char* optName;                 /* name of new option */
    Tcl_CmdInfo *cinfo;            /* returns: info associated with option */
{
    Ensemble *ensData;
    EnsembleOption *ensOpt;

    /*
     *  Parse the ensemble name and look for a containing ensemble.
     */
    if (ItclFindEnsemble(interp, ensName, &ensData) != TCL_OK ||
        ensData == NULL) {
        return 0;
    }

    /*
     *  Look for an option with the desired name.  If found, load
     *  its data into the cinfo buffer.
     */
    if (ItclFindEnsOption(interp, ensData, optName, &ensOpt) != TCL_OK ||
        ensOpt == NULL) {
        return 0;
    }

    cinfo->proc       = ensOpt->cmdPtr->proc;
    cinfo->clientData = ensOpt->cmdPtr->clientData;
    cinfo->deleteProc = ensOpt->cmdPtr->deleteProc;
    cinfo->deleteData = ensOpt->cmdPtr->deleteData;

    return 1;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_IsEnsemble()
 *
 *  Checks to see whether or not the given command represents an
 *  ensemble.  Returns 1 if it does, and 0 otherwise.
 * ------------------------------------------------------------------------
 */
int
Itcl_IsEnsemble(cinfo)
    Tcl_CmdInfo* cinfo;  /* command info from Tcl_GetCommandInfo() */
{
    if (cinfo) {
        return (cinfo->deleteProc == ItclDelEnsemble);
    }
    return 0;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_EnsembleInvoc()
 *
 *  Returns a string describing the most recent invocation of an
 *  ensemble option.  Useful for returning error messages within
 *  option handler routines:
 *
 *    Tcl_AppendResult(interp, "wrong # args: should be \"",
 *        Itcl_EnsembleInvoc(), " ?value?\"", (char*)NULL);
 *
 *  Returns a pointer to a string like "ensemble subens subens option".
 *  This string remains valid until the next call to this procedure.
 * ------------------------------------------------------------------------
 */
char*
Itcl_EnsembleInvoc()
{
    static Tcl_DString *buffer = NULL;

    char *name;
    Itcl_Stack trail;
    EnsembleOption *opt;

    /*
     *  If a buffer has not yet been allocated, do it now.
     */
    if (!buffer) {
        buffer = (Tcl_DString*)ckalloc(sizeof(Tcl_DString));
        Tcl_DStringInit(buffer);
    }
    Tcl_DStringTrunc(buffer,0);

    /*
     *  Build the trail of ensemble names leading to this option.
     */
    Itcl_InitStack(&trail);

    for (opt=LastOption; opt; opt=opt->ensemble->parent) {
        if (!ItclEnsOptionIsError(opt)) {
            Itcl_PushStack((ClientData)opt, &trail);
        }
        if (opt->ensemble->parent == NULL) {
            break;
        }
    }

    if (opt && !ItclEnsOptionIsError(opt)) {
        name = Tcl_GetCommandName(opt->ensemble->interp, opt->ensemble->cmd);
    } else if (LastEnsemble) {
        name = Tcl_GetCommandName(LastEnsemble->interp, LastEnsemble->cmd);
    } else {
        name = "";
    }
    Tcl_DStringAppendElement(buffer, name);

    while (Itcl_GetStackSize(&trail) > 0) {
        opt = (EnsembleOption*)Itcl_PopStack(&trail);
        Tcl_DStringAppendElement(buffer, opt->name);
    }
    Itcl_DeleteStack(&trail);

    return Tcl_DStringValue(buffer);
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_EnsembleUsage()
 *
 *  Returns a string describing the usage information for the most
 *  recent invocation of an ensemble option.  Useful for returning
 *  error messages within option handler routines:
 *
 *    Tcl_AppendResult(interp, "wrong # args: should be one of...\n",
 *        Itcl_EnsembleUsage(), (char*)NULL);
 *
 *  Returns a pointer to a string containing using information.
 *  This string remains valid until the next call to this procedure.
 * ------------------------------------------------------------------------
 */
char*
Itcl_EnsembleUsage()
{
    static Tcl_DString *buffer = NULL;

    int i;
    Ensemble *ensData;

    /*
     *  If a buffer has not yet been allocated, do it now.
     */
    if (!buffer) {
        buffer = (Tcl_DString*)ckalloc(sizeof(Tcl_DString));
        Tcl_DStringInit(buffer);
    }
    Tcl_DStringTrunc(buffer,0);

    /*
     *  If the current option is an ensemble, then report usage
     *  information for each of its options.
     */
    ensData = NULL;
    if (LastOption && !ItclEnsOptionIsError(LastOption)) {
        if (LastOption->cmdPtr->deleteProc == ItclDelEnsemble) {
            ensData = (Ensemble*)LastOption->cmdPtr->clientData;
        }
    }
    else if (LastEnsemble) {
        ensData = LastEnsemble;
    }

    if (ensData) {
    	int isOpenEnded = 0;

        for (i=0; i < ensData->numOptions; i++) {
            if (ItclEnsOptionIsError(ensData->options[i])) {
                isOpenEnded = 1;
            } else {
                Tcl_DStringAppend(buffer, "\n  ", -1);
                Tcl_DStringAppend(buffer,
                    ItclEnsOptionUsage(ensData->options[i]), -1);
            }
        }
        if (isOpenEnded) {
            Tcl_DStringAppend(buffer,
                "\n...and others described on the man page", -1);
        }
    }
    else if (LastOption) {
        Tcl_DStringAppend(buffer, ItclEnsOptionUsage(LastOption), -1);
    }

    return Tcl_DStringValue(buffer);
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_EnsembleErrorCmd()
 *
 *  Invoked whenever the user tries to access an unknown option for
 *  an ensemble.  Acts as the default handler for "@error".  Generates
 *  an error message like:
 *
 *      bad option "foo": should be one of...
 *        ens option ?arg arg...?
 *        ...
 *
 *  Returns TCL_ERROR to indicate failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_EnsembleErrorCmd(cdata, interp, argc, argv)
    ClientData cdata;        /* ensemble info */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    Ensemble *ensData = (Ensemble*)cdata;
    int isOpenEnded = 0;

    int i;

    Tcl_AppendResult(interp, "bad option \"", argv[0],
        "\": should be one of...",
        (char*)NULL);

    for (i=0; i < ensData->numOptions; i++) {
        if (ItclEnsOptionIsError(ensData->options[i])) {
            isOpenEnded = 1;
        } else {
            Tcl_AppendResult(interp, "\n  ",
                ItclEnsOptionUsage(ensData->options[i]),
                (char*)NULL);
        }
    }

    if (isOpenEnded) {
        Tcl_AppendResult(interp,
            "\n...and others described on the man page",
            (char*)NULL);
    }
    return TCL_ERROR;
}


/*
 * ------------------------------------------------------------------------
 *  ItclDelEnsemble()
 *
 *  Invoked when the data associated with an ensemble is destroyed.
 *  This happens automatically, for example, when the ensemble access
 *  command is removed from a Tcl interpreter.  Destroys all options
 *  included in the ensemble, including all sub-ensembles.
 * ------------------------------------------------------------------------
 */
static void
ItclDelEnsemble(cdata)
    ClientData cdata;    /* client data for ensemble access command */
{
    Ensemble* ensData = (Ensemble*)cdata;
    int i;

    for (i=0; i < ensData->numOptions; i++) {
        ItclDelEnsembleOption(ensData->options[i]);
    }
    ckfree((char*)ensData->options);
    ckfree((char*)ensData);
}

/*
 * ------------------------------------------------------------------------
 *  ItclDelEnsembleCommon()
 *
 *  Invoked when all references to the common data shared by ensemble
 *  creation/parsing commands are destroyed.  This common data allows
 *  all of these ensemble-related commands to communicate.  Frees
 *  all memory associated with this data.
 * ------------------------------------------------------------------------
 */
static void
ItclDelEnsembleCommon(cdata)
    ClientData cdata;    /* client data for ensemble-related commands */
{
    EnsembleCommonInfo* ensInfo = (EnsembleCommonInfo*)cdata;
    ckfree((char*)ensInfo);
}


/*
 * ------------------------------------------------------------------------
 *  ItclFindEnsemble()
 *
 *  Searches for an existing ensemble with the given name.  This name
 *  is parsed as a space separated list of options identifying the
 *  ensemble, so this procedure can be used to find an ensemble within
 *  an ensemble:
 *
 *      name <=> "namespace::namespace::ensemble sub-ensemble ..."
 *
 *  Returns TCL_ERROR (along with an error message in the interp)
 *  if an error is encountered.  Otherwise, returns TCL_OK.
 *  If the name components identify an existing ensemble, ensDataPtr
 *  returns a pointer to its data; otherwise, it returns NULL.
 * ------------------------------------------------------------------------
 */
static int
ItclFindEnsemble(interp,name,ensDataPtr)
    Tcl_Interp *interp;            /* interpreter containing the ensemble */
    char* name;                    /* name of the ensemble */
    Ensemble** ensDataPtr;         /* returns: ensemble data */
{
    int i, ensc;
    char **ensv;
    Tcl_Command cmd;
    Command* cmdPtr;
    Ensemble *ensData;
    EnsembleOption *ensOpt;

    *ensDataPtr = NULL;  /* assume that no data will be found */

    /*
     *  Treat the ensemble name as a space-separated list of ensemble
     *  names.
     */
    if (Tcl_SplitList(interp, name, &ensc, &ensv) != TCL_OK) {
        return TCL_ERROR;
    }

    /*
     *  If the name has more than one part, follow the list of
     *  sub-ensemble names up to the one containing the new ensemble.
     */
    if (ensc > 0) {
        if (Itcl_FindCommand(interp, ensv[0], 0, &cmd) != TCL_OK) {
            goto ensFail;
        }
        cmdPtr = (Command*)cmd;

        if ((cmdPtr == NULL) ||
            (cmdPtr->deleteProc != ItclDelEnsemble)) {
            Tcl_AppendResult(interp, "command \"", ensv[0],
                "\" is not an ensemble",
                (char*)NULL);
            goto ensFail;
        }
        ensData = (Ensemble*)cmdPtr->clientData;

        for (i=1; i < ensc; i++) {
            if (ItclFindEnsOption(interp, ensData, ensv[i],
                    &ensOpt) == TCL_OK && ensOpt) {

                cmdPtr = ensOpt->cmdPtr;
                if (cmdPtr && (cmdPtr->deleteProc == ItclDelEnsemble)) {
                    ensData = (Ensemble*)cmdPtr->clientData;
                } else {
                    ensData = NULL;
                }
            }
            if (ensData == NULL) {
                Tcl_ResetResult(interp);
                Tcl_AppendResult(interp, "option \"", ensv[i],
                    "\" is not an ensemble",
                    (char*)NULL);
                goto ensFail;
            }
        }
        *ensDataPtr = ensData;
    }

    ckfree((char*)ensv);
    return TCL_OK;

ensFail:
    ckfree((char*)ensv);
    return TCL_ERROR;
}


/*
 * ------------------------------------------------------------------------
 *  ItclCreateEnsOption()
 *
 *  Adds an option with a simple (one-word) name to an existing ensemble.
 *  Creates an empty record to represent the option and returns a
 *  pointer to it.  If an option already exists with that name, then
 *  it is deleted and replaced with an empty record.
 *
 *  Options are stored in lexicographical order, so that a binary
 *  search can be used for subsequent lookup.
 *
 *  Returns a pointer to a structure that can be used to represent
 *  the option.
 * ------------------------------------------------------------------------
 */
static EnsembleOption*
ItclCreateEnsOption(ensData,name,flags)
    Ensemble *ensData;        /* ensemble being modified */
    char* name;               /* name of the new option */
    int flags;                /* flags controlling option creation */
{
    int first, last, pos;
    int i, cmp, size;
    EnsembleOption** optList;
    EnsembleOption* opt;

    /*
     *  Search for an existing option with the same name.
     */
    first = 0;
    last  = ensData->numOptions-1;

    while (last >= first) {
        pos = (first+last)/2;
        if (*name == *ensData->options[pos]->name) {
            cmp = strcmp(name, ensData->options[pos]->name);
            if (cmp == 0) {
                break;    /* found it! */
            }
        }
        else if (*name < *ensData->options[pos]->name) {
            cmp = -1;
        }
        else {
            cmp = 1;
        }

        if (cmp > 0)
            first = pos+1;
        else
            last = pos-1;
    }

    /*
     *  If a matching entry was found, then consult the flags to
     *  proceed.  Either return the existing entry or destroy it
     *  and make a new one.
     */
    if (last >= first) {
        if ((flags & ENS_USE_EXISTING) != 0) {
            return ensData->options[pos];
        }
        ItclDelEnsembleOption(ensData->options[pos]);
    }

    /*
     *  Otherwise, make room for a new entry.
     */
    else {
        pos = first;

        if (ensData->numOptions >= ensData->maxOptions) {
            size = ensData->maxOptions*sizeof(EnsembleOption*);
            optList = (EnsembleOption**)ckalloc((unsigned)2*size);
            memcpy((VOID*)optList, (VOID*)ensData->options, (size_t)size);
            ckfree((char*)ensData->options);

            ensData->options = optList;
            ensData->maxOptions *= 2;
        }

        for (i=ensData->numOptions; i > pos; i--) {
            ensData->options[i] = ensData->options[i-1];
        }
        ensData->numOptions++;
    }

    opt = (EnsembleOption*)ckalloc(sizeof(EnsembleOption));
    opt->name = (char*)ckalloc((unsigned)(strlen(name)+1));
    strcpy(opt->name, name);
    opt->cmdPtr  = NULL;
    opt->minArgs = ITCL_VAR_ARGS;
    opt->maxArgs = ITCL_VAR_ARGS;
    opt->usage   = NULL;
    opt->ensemble = ensData;

    ensData->options[pos] = opt;

    /*
     *  Compare the new option against the one on either side of
     *  it.  Determine how many letters are needed in each option
     *  to guarantee that an abbreviated form is unique.  Update
     *  the options on either side as well, since they are influenced
     *  by the new option.
     */
    ComputeMinChars(ensData, pos);
    ComputeMinChars(ensData, pos-1);
    ComputeMinChars(ensData, pos+1);

    return opt;
}


/*
 * ------------------------------------------------------------------------
 *  ItclDelEnsembleOption()
 *
 *  Destroys an ensemble option previously created by ItclCreateEnsOption().
 *  Destroys the command handling the option and frees any associated
 *  memory.  Usually invoked when an ensemble is being destroyed.
 * ------------------------------------------------------------------------
 */
static void
ItclDelEnsembleOption(ensOpt)
    EnsembleOption *ensOpt;     /* ensemble being destroyed */
{
    Command *cmdPtr;
    cmdPtr = ensOpt->cmdPtr;

    if (cmdPtr->deleteData && cmdPtr->deleteProc) {
        (*cmdPtr->deleteProc)(cmdPtr->deleteData);
    }
    if (cmdPtr->cacheInfo) {
        if (cmdPtr->cacheInfo->usage != 0) {
            cmdPtr->cacheInfo->element = NULL;
        } else {
            ckfree((char*)cmdPtr->cacheInfo);
        }
        cmdPtr->cacheInfo = NULL;
    }
    ckfree((char*)cmdPtr);

    if (ensOpt->usage) {
        ckfree(ensOpt->usage);
    }
    ckfree(ensOpt->name);
    ckfree((char*)ensOpt);
}


/*
 * ------------------------------------------------------------------------
 *  ComputeMinChars()
 *
 *  Examines the specified ensemble option to determine the minimum
 *  number of characters that could be used to abbreviate its name.
 *  Updates the option record in place.  If the specified index is
 *  not valid, this routine does nothing.
 * ------------------------------------------------------------------------
 */
static void
ComputeMinChars(ensData,pos)
    Ensemble *ensData;        /* ensemble being modified */
    int pos;                  /* index of option being updated */
{
    int min, max;
    char *p, *q;

    /*
     *  If the position is invalid, do nothing.
     */
    if (pos < 0 || pos >= ensData->numOptions) {
        return;
    }

    /*
     *  Start by assuming that only the first letter is required
     *  to uniquely identify this option.  Then compare the name
     *  against each neighboring option to determine the real minimum.
     */
    ensData->options[pos]->minChars = 1;

    if (pos-1 >= 0) {
        p = ensData->options[pos]->name;
        q = ensData->options[pos-1]->name;
        for (min=1; *p == *q && *p != '\0' && *q != '\0'; min++) {
            p++;
            q++;
        }
        if (min > ensData->options[pos]->minChars) {
            ensData->options[pos]->minChars = min;
        }
    }

    if (pos+1 < ensData->numOptions) {
        p = ensData->options[pos]->name;
        q = ensData->options[pos+1]->name;
        for (min=1; *p == *q && *p != '\0' && *q != '\0'; min++) {
            p++;
            q++;
        }
        if (min > ensData->options[pos]->minChars) {
            ensData->options[pos]->minChars = min;
        }
    }

    max = strlen(ensData->options[pos]->name);
    if (ensData->options[pos]->minChars > max) {
        ensData->options[pos]->minChars = max;
    }
}


/*
 * ------------------------------------------------------------------------
 *  ItclFindEnsOption()
 *
 *  Searches for an option with a simple (one-word) name within an
 *  existing ensemble.  Abbreviated option names are also honored,
 *  as long as the abbreviation is unique.
 *
 *  Returns TCL_ERROR if an error is encountered (i.e., if the name
 *  is malformed or ambiguous) along with an error message in the
 *  interpreter.  Otherwise, returns TCL_OK, and if the option is
 *  found, "rensOpt" returns a pointer to the option.
 * ------------------------------------------------------------------------
 */
static int
ItclFindEnsOption(interp,ensData,name,rensOpt)
    Tcl_Interp *interp;       /* interpreter containing the ensemble */
    Ensemble *ensData;        /* ensemble being searched */
    char* name;               /* name of the desired option */
    EnsembleOption **rensOpt; /* returns:  pointer to the desired option */
{
    int first, last, pos, nlen;
    int i, cmp;

    *rensOpt = NULL;

    /*
     *  Search for an existing option with the same name.
     */
    first = 0;
    last  = ensData->numOptions-1;
    nlen  = strlen(name);

    while (last >= first) {
        pos = (first+last)/2;
        if (*name == *ensData->options[pos]->name) {
            cmp = strncmp(name, ensData->options[pos]->name, nlen);
            if (cmp == 0) {
                break;    /* found it! */
            }
        }
        else if (*name < *ensData->options[pos]->name) {
            cmp = -1;
        }
        else {
            cmp = 1;
        }

        if (cmp > 0)
            first = pos+1;
        else
            last = pos-1;
    }

    /*
     *  If a matching entry could not be found, then quit.
     */
    if (last < first) {
        return TCL_OK;
    }

    /*
     *  If a matching entry was found, there may be some ambiguity
     *  if the user did not specify enough characters.  Find the
     *  top-most match in the list, and see if the option name has
     *  enough characters.  If there are two options like "foo"
     *  and "food", this allows us to match "foo" exactly.
     */
    if (nlen < ensData->options[pos]->minChars) {
        while (pos > 0) {
            pos--;
            if (strncmp(name, ensData->options[pos]->name, nlen) != 0) {
                pos++;
                break;
            }
        }
    }
    if (nlen < ensData->options[pos]->minChars) {
        Tcl_AppendResult(interp, "ambiguous option \"", name,
            "\": should be one of...",
            (char*)NULL);

        for (i=pos; i < ensData->numOptions; i++) {
            if (strncmp(name, ensData->options[i]->name, nlen) != 0) {
                break;
            }
            Tcl_AppendResult(interp, "\n  ",
                ItclEnsOptionUsage(ensData->options[i]),
                (char*)NULL);
        }
        return TCL_ERROR;
    }

    /*
     *  Found a match.  Return the desired option.
     */
    *rensOpt = ensData->options[pos];
    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  ItclEnsOptionUsage()
 *
 *  Returns a string showing how this ensemble option should be invoked.
 *  Each option has a slot for usage information, which is usually set
 *  when the option is created.  If the option represents a sub-ensemble
 *  or a Tcl proc, the usage information can be deduced automatically.
 *  If all else fails, the generic "?arg arg ...?" usage is returned.
 *
 *  Returns a pointer to a string of the form:
 *
 *      ensemble sub-ensemble sub-ensemble option x y ?z? ?arg arg ...?
 *
 *  This string remains valid until the next call to this procedure.
 * ------------------------------------------------------------------------
 */
static char*
ItclEnsOptionUsage(ensOpt)
    EnsembleOption *ensOpt;   /* ensemble option for usage info */
{
    static Tcl_DString *buffer = NULL;
    Itcl_Stack trail;
    EnsembleOption *opt;
    Command *cmdPtr;
    char *name;

    /*
     *  If a buffer has not yet been created, do it now.
     */
    if (buffer == NULL) {
        buffer = (Tcl_DString*)ckalloc(sizeof(Tcl_DString));
        Tcl_DStringInit(buffer);
    }
    Tcl_DStringTrunc(buffer, 0);

    /*
     *  Build the trail of ensemble names leading to this option.
     */
    Itcl_InitStack(&trail);
    for (opt=ensOpt; opt; opt=opt->ensemble->parent) {
        Itcl_PushStack((ClientData)opt, &trail);
        if (opt->ensemble->parent == NULL) {
            break;
        }
    }

    cmdPtr = (Command*)opt->ensemble->cmd;
    name = Tcl_GetHashKey(&cmdPtr->namesp->commands, cmdPtr->hPtr);
    Tcl_DStringAppendElement(buffer, name);

    while (Itcl_GetStackSize(&trail) > 0) {
        opt = (EnsembleOption*)Itcl_PopStack(&trail);
        Tcl_DStringAppendElement(buffer, opt->name);
    }
    Itcl_DeleteStack(&trail);

    /*
     *  If the option has usage info, use it directly.
     */
    if (ensOpt->usage && *ensOpt->usage != '\0') {
        Tcl_DStringAppend(buffer, " ", -1);
        Tcl_DStringAppend(buffer, opt->usage, -1);
    }

    /*
     *  Otherwise, try to deduce the usage information.
     *  If the option is itself an ensemble, summarize its usage.
     */
    else if (ensOpt->cmdPtr &&
             (ensOpt->cmdPtr->deleteProc == ItclDelEnsemble)) {
        Tcl_DStringAppend(buffer, " option ?arg arg ...?", -1);
    }

    /*
     *  If the option is a Tcl proc, get info from the argument list.
     */
    else if (ensOpt->cmdPtr && TclIsProc(ensOpt->cmdPtr)) {
        Proc *procPtr = (Proc*)ensOpt->cmdPtr->clientData;
        Arg *argPtr;

        for (argPtr=procPtr->argPtr; argPtr != NULL; argPtr=argPtr->nextPtr) {
            if (argPtr->nextPtr == NULL && strcmp(argPtr->name, "args") == 0) {
                Tcl_DStringAppend(buffer, " ?arg arg ...?", -1);
            }
            else if (argPtr->defValue) {
                Tcl_DStringAppend(buffer, " ?", -1);
                Tcl_DStringAppend(buffer, argPtr->name, -1);
                Tcl_DStringAppend(buffer, "?", -1);
            }
            else {
                Tcl_DStringAppend(buffer, " ", -1);
                Tcl_DStringAppend(buffer, argPtr->name, -1);
            }
        }
    }
    return Tcl_DStringValue(buffer);
}


/*
 * ------------------------------------------------------------------------
 *  ItclEnsOptionIsError()
 *
 *  Returns non-zero if the specified ensemble option represents an
 *  "@error" error handler.  Such options are ignored when reporting
 *  the allowed options in an ensemble.
 * ------------------------------------------------------------------------
 */
static int
ItclEnsOptionIsError(ensOpt)
    EnsembleOption *ensOpt;    /* ensemble option being checked */
{
    if (*ensOpt->name == '@' && strcmp(ensOpt->name,"@error") == 0) {
        return 1;
    }
    return 0;
}


/*
 * ------------------------------------------------------------------------
 *  ItclHandleEnsemble()
 *
 *  Invoked by Tcl whenever the user issues an ensemble command.
 *  Handles the following syntax:
 *
 *    <ensembleName> <option> <args>...
 *
 *  Looks for <option> among the list of recognized ensemble commands
 *  and passes execution to it.  If the <option> is not recognized,
 *  an error message summarizes available options.
 * ------------------------------------------------------------------------
 */
static int
ItclHandleEnsemble(cdata, interp, argc, argv)
    ClientData cdata;        /* ensemble data */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    Ensemble *ensData = (Ensemble*)cdata;

    int i, status;
    Command *cmdPtr;
    EnsembleOption *ensOpt, *savedEnsOpt;
    Ensemble *savedEnsData;

    if (argc < 2) {
    	int isOpenEnded = 0;

        Tcl_AppendResult(interp, "wrong # args: should be one of...",
            (char*)NULL);

        for (i=0; i < ensData->numOptions; i++) {
            if (ItclEnsOptionIsError(ensData->options[i])) {
                isOpenEnded = 1;
            } else {
                Tcl_AppendResult(interp, "\n  ",
                    ItclEnsOptionUsage(ensData->options[i]),
                    (char*)NULL);
            }
        }

        if (isOpenEnded) {
            Tcl_AppendResult(interp,
                "\n...and others described on the man page",
                (char*)NULL);
        }
        return TCL_ERROR;
    }

    /*
     *  Lookup the desired option.  If an error is found, return
     *  immediately.
     */
    if (ItclFindEnsOption(interp, ensData, argv[1], &ensOpt) != TCL_OK) {
        return TCL_ERROR;
    }

    /*
     *  If the option was not found, then look for an "@error" option
     *  to handle the error.
     */
    if (ensOpt == NULL) {
        if (ItclFindEnsOption(interp, ensData, "@error", &ensOpt) != TCL_OK) {
            return TCL_ERROR;
        }
    }
    if (ensOpt == NULL) {
        return Itcl_EnsembleErrorCmd((ClientData)ensData,
            interp, argc-1, argv+1);
    }

    /*
     *  Do some simple error checking on the number of required args.
     */
    if (!ItclEnsOptionIsError(ensOpt)) {
        if (ensOpt->minArgs >= 0 && argc-2 < ensOpt->minArgs) {
            Tcl_AppendResult(interp, "wrong # args: should be \"",
                ItclEnsOptionUsage(ensOpt), "\"",
                (char*)NULL);
            return TCL_ERROR;
        }

        if (ensOpt->maxArgs >= 0 && argc-2 > ensOpt->maxArgs) {
            Tcl_AppendResult(interp, "wrong # args: should be \"",
                ItclEnsOptionUsage(ensOpt), "\"",
                (char*)NULL);
            return TCL_ERROR;
        }
        --argc;  /* skip over option name */
        ++argv;
    }

    /*
     *  Pass control to the option, and return the result.
     */
    savedEnsData = LastEnsemble;   /* save state for Itcl_EnsembleInvoc() */
    LastEnsemble = ensData;

    savedEnsOpt  = LastOption;
    LastOption   = ensOpt;

    cmdPtr = (Command*)ensOpt->cmdPtr;
    status = (*cmdPtr->proc)(cmdPtr->clientData, interp, argc, argv);

    LastEnsemble = savedEnsData;
    LastOption   = savedEnsOpt;

    return status;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_EnsembleCmd()
 *
 *  Invoked by Tcl whenever the user issues an ensemble command.
 *  Handles the following syntax:
 *
 *      ensemble <name> <ensemble-defn>
 *
 *      EXAMPLE:
 *      ensemble info {
 *          option cmdcount {} { ... }
 *          option args {procName} { ... }
 *          option body {procName} { ... }
 *      }
 *
 *  If ensemble <name> does not exist, it is automatically created.
 *  the <ensemble-defn> body of commands is then evaluated in the
 *  context of the ::tcl::ensemble-parser namespace, to create the
 *  options contained within the ensemble.
 *
 *  Returns TCL_OK if successful, and TCL_ERROR (along with an error
 *  message in the interpreter) if anything goes wrong.
 * ------------------------------------------------------------------------
 */
int
Itcl_EnsembleCmd(cdata, interp, argc, argv)
    ClientData cdata;        /* ensemble data */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    EnsembleCommonInfo *ensInfo = (EnsembleCommonInfo*)cdata;
    Ensemble *ensData = NULL;

    int status;
    Tcl_Command cmd;
    Command* cmdPtr;

    if (argc != 3) {
        Tcl_AppendResult(interp, "wrong # args: should be \"",
            argv[0], " name {commands...}\"",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  If an ensemble does not exist with the desired name,
     *  create one.
     */
    if (Itcl_FindCommand(interp, argv[1], 0, &cmd) != TCL_OK) {
        return TCL_ERROR;
    }
    cmdPtr = (Command*)cmd;
    if (cmdPtr && (cmdPtr->deleteProc == ItclDelEnsemble)) {
        ensData = (Ensemble*)cmdPtr->clientData;
    }

    if (ensData == NULL) {
        if (Itcl_CreateEnsemble(interp, argv[1]) != TCL_OK) {
            return TCL_ERROR;
        }
        if (Itcl_FindCommand(interp, argv[1], 0, &cmd) != TCL_OK) {
            return TCL_ERROR;
        }
        cmdPtr = (Command*)cmd;
        ensData = (Ensemble*)cmdPtr->clientData;
    }

    /*
     *  Setup this ensemble for editing and evaluate the ensemble
     *  definition in the context of the "ensemble-parser" namespace.
     */
    ensInfo->parsing = ensData;
    status = Itcl_NamespEval(interp, ensInfo->parserNs, argv[2]);

    if (status == TCL_BREAK) {
        Tcl_SetResult(interp, "invoked \"break\" outside of a loop",
            TCL_STATIC);
        status = TCL_ERROR;
    }
    else if (status == TCL_CONTINUE) {
        Tcl_SetResult(interp, "invoked \"continue\" outside of a loop",
            TCL_STATIC);
        status = TCL_ERROR;
    }
    else if (status != TCL_OK) {
        char mesg[256];
        sprintf(mesg, "\n    (ensemble \"%100s\" body line %d)",
            argv[1], interp->errorLine);
        Tcl_AddErrorInfo(interp, mesg);
    }

    ensInfo->parsing = NULL;
    return status;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_EnsOptionCmd()
 *
 *  Invoked by Tcl whenever the user issues an "option" command
 *  within the body of an ensemble definition.  Handles the following
 *  syntax:
 *
 *      option <name> <args> <body>
 *
 *      EXAMPLE:
 *      ensemble info {
 *          option cmdcount {} { ... }
 *          option args {procName} { ... }
 *          option body {procName} { ... }
 *      }
 *
 *  If an option called <name> already exists within the ensemble, it
 *  is replace with the current option definition.
 *
 *  Returns TCL_OK if successful, and TCL_ERROR (along with an error
 *  message in the interpreter) if anything goes wrong.
 * ------------------------------------------------------------------------
 */
int
Itcl_EnsOptionCmd(cdata, interp, argc, argv)
    ClientData cdata;        /* ensemble data */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    EnsembleCommonInfo *ensInfo = (EnsembleCommonInfo*)cdata;
    Ensemble *ensData = (Ensemble*)ensInfo->parsing;

    Proc *procPtr;
    Command *cmdPtr;
    EnsembleOption *ensOpt;

    assert(ensData != NULL);

    if (argc != 4) {
        Tcl_AppendResult(interp, "wrong # args: should be \"",
            argv[0], " name args body\"",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  Try to create a Tcl-style proc definition using the
     *  specified args and body.
     */
    cmdPtr = (Command*)ensData->cmd;
    if (TclCreateProc(interp, cmdPtr->namesp, argv[1], argv[2], argv[3],
        &procPtr) != TCL_OK) {
        return TCL_ERROR;
    }

    /*
     *  Install the new option into the option list.
     */
    ensOpt = ItclCreateEnsOption(ensData, argv[1], ENS_REPLACE);

    cmdPtr = (Command*)ckalloc(sizeof(Command));
    cmdPtr->hPtr = NULL;
    cmdPtr->proc = TclInterpProc;
    cmdPtr->clientData = (ClientData)procPtr;
    cmdPtr->deleteProc = TclCleanupProc;
    cmdPtr->deleteData = (ClientData)procPtr;
    cmdPtr->cacheInfo = NULL;

    ensOpt->cmdPtr = cmdPtr;
    return TCL_OK;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_EnsEnsembleCmd()
 *
 *  Invoked by Tcl whenever the user issues an "ensemble" command
 *  within the body of an ensemble definition.  Handles the following
 *  syntax:
 *
 *      ensemble <name> <ensemble-defn>
 *
 *      EXAMPLE:
 *      ensemble tkwait {
 *          option variable {varName} { ... }
 *          ensemble object {
 *              option creation {objName} { ... }
 *              option destruction {objName} { ... }
 *          }
 *      }
 *
 *  If an ensemble called <name> already exists within the ensemble,
 *  new elements are added to it.  If any other option called <name>
 *  exists within the ensemble, it is replaced with an empty
 *  ensemble.
 *
 *  Returns TCL_OK if successful, and TCL_ERROR (along with an error
 *  message in the interpreter) if anything goes wrong.
 * ------------------------------------------------------------------------
 */
int
Itcl_EnsEnsembleCmd(cdata, interp, argc, argv)
    ClientData cdata;        /* ensemble data */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    EnsembleCommonInfo *ensInfo = (EnsembleCommonInfo*)cdata;
    Ensemble *ensData = (Ensemble*)ensInfo->parsing;

    int status;
    Ensemble *subEnsData, *savedEnsData;
    EnsembleOption *ensOpt;
    Command *cmdPtr;

    assert(ensData != NULL);

    if (argc != 3) {
        Tcl_AppendResult(interp, "wrong # args: should be \"",
            argv[0], " name {commands...}\"",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  If an ensemble already exists with this name, then simply
     *  add to it.  Otherwise, create a new ensemble, or replace an
     *  existing option with an ensemble.
     */
    ensOpt = ItclCreateEnsOption(ensData, argv[1], ENS_USE_EXISTING);
    if (ensOpt->cmdPtr && ensOpt->cmdPtr->deleteProc == ItclDelEnsemble) {
        subEnsData = (Ensemble*)ensOpt->cmdPtr->clientData;
    }
    else {
        if (ensOpt->cmdPtr) {
            ensOpt = ItclCreateEnsOption(ensData, argv[1], ENS_REPLACE);
        }
        subEnsData = (Ensemble*)ckalloc(sizeof(Ensemble));
        subEnsData->interp = interp;
        subEnsData->numOptions = 0;
        subEnsData->maxOptions = 10;
        subEnsData->options = (EnsembleOption**)ckalloc(
            (unsigned)ensData->maxOptions*sizeof(EnsembleOption*)
        );
        subEnsData->cmd = ensData->cmd;
        subEnsData->parent = ensOpt;

        cmdPtr = (Command*)ckalloc(sizeof(Command));
        cmdPtr->hPtr = NULL;
        cmdPtr->proc = ItclHandleEnsemble;
        cmdPtr->clientData = (ClientData)subEnsData;
        cmdPtr->deleteProc = ItclDelEnsemble;
        cmdPtr->deleteData = (ClientData)subEnsData;
        cmdPtr->cacheInfo  = NULL;

        ensOpt->cmdPtr = cmdPtr;
    }

    /*
     *  Setup this ensemble for editing and evaluate the ensemble
     *  definition in the context of the "ensemble-parser" namespace.
     */
    savedEnsData = ensInfo->parsing;
    ensInfo->parsing = subEnsData;
    status = Itcl_NamespEval(interp, ensInfo->parserNs, argv[2]);

    if (status == TCL_BREAK) {
        Tcl_SetResult(interp, "invoked \"break\" outside of a loop",
            TCL_STATIC);
        status = TCL_ERROR;
    }
    else if (status == TCL_CONTINUE) {
        Tcl_SetResult(interp, "invoked \"continue\" outside of a loop",
            TCL_STATIC);
        status = TCL_ERROR;
    }
    else if (status != TCL_OK) {
        char mesg[256];
        sprintf(mesg, "\n    (ensemble \"%100s\" body line %d)",
            argv[1], interp->errorLine);
        Tcl_AddErrorInfo(interp, mesg);
    }

    ensInfo->parsing = savedEnsData;
    return status;
}
