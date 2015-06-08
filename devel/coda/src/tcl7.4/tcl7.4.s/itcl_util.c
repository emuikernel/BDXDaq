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
 *  This segment provides common utility functions used throughout
 *  the other [incr Tcl] source files.
 *
 * ========================================================================
 *  AUTHOR:  Michael J. McLennan       Phone: (610)712-2842
 *           AT&T Bell Laboratories   E-mail: michael.mclennan@att.com
 *     RCS:  $Id: itcl_util.c,v 1.1.1.1 1996/08/21 19:30:11 heyes Exp $
 * ========================================================================
 *             Copyright (c) 1993-1995  AT&T Bell Laboratories
 * ------------------------------------------------------------------------
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
#include <assert.h>
#include "tclInt.h"

static char rcsid[] = "$Id: itcl_util.c,v 1.1.1.1 1996/08/21 19:30:11 heyes Exp $";

/*
 *  PRESERVED DATA INFO
 */
typedef struct Itcl_PreservedData {
    ClientData data;        /* reference to data */
    int usage;              /* number of active uses */
    Itcl_FreeProc *fproc;   /* procedure used to free data */
} Itcl_PreservedData;

static Tcl_HashTable *Itcl_PreservedList = NULL;

/*
 *  POOL OF LIST ELEMENTS FOR LINKED LIST
 */
static Itcl_ListElem *listPool = NULL;
static int listPoolLen = 0;

#define ITCL_VALID_LIST 0x01face10  /* magic bit pattern for validation */
#define ITCL_LIST_POOL_SIZE 200     /* max number of elements in listPool */

/*
 *  Interpreter state info
 */
typedef struct InterpState {
    int validate;            /* validation stamp */
    int status;              /* return code status */
    char *result;            /* result string */
    int resultType;          /* source of result string (see below) */
    char *errorInfo;         /* contents of errorInfo variable */
    char *errorCode;         /* contents of errorCode variable */
} InterpState;

#define ITCL_VALID_STATE 0x01233210  /* magic bit pattern for validation */

#define ITCL_RESULT_INTERP   1       /* result came from interp buffer */
#define ITCL_RESULT_STATIC   2       /* result is static string */
#define ITCL_RESULT_DYNAMIC  3       /* just copy result string */



/*
 * ------------------------------------------------------------------------
 *  Itcl_InitStack()
 *
 *  Initializes a stack structure, allocating a certain amount of memory
 *  for the stack and setting the stack length to zero.
 * ------------------------------------------------------------------------
 */
void
Itcl_InitStack(stack)
    Itcl_Stack *stack;     /* stack to be initialized */
{
    stack->values = stack->space;
    stack->max = sizeof(stack->space)/sizeof(ClientData);
    stack->len = 0;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_DeleteStack()
 *
 *  Destroys a stack structure, freeing any memory that may have been
 *  allocated to represent it.
 * ------------------------------------------------------------------------
 */
void
Itcl_DeleteStack(stack)
    Itcl_Stack *stack;     /* stack to be deleted */
{
    /*
     *  If memory was explicitly allocated (instead of using the
     *  built-in buffer) then free it.
     */
    if (stack->values != stack->space) {
        ckfree((char*)stack->values);
    }
    stack->values = NULL;
    stack->len = stack->max = 0;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_PushStack()
 *
 *  Pushes a piece of client data onto the top of the given stack.
 *  If the stack is not large enough, it is automatically resized.
 * ------------------------------------------------------------------------
 */
void
Itcl_PushStack(cdata,stack)
    ClientData cdata;      /* data to be pushed onto stack */
    Itcl_Stack *stack;     /* stack */
{
    ClientData *newStack;

    if (stack->len+1 >= stack->max) {
        stack->max = 2*stack->max;
        newStack = (ClientData*)
            ckalloc((unsigned)(stack->max*sizeof(ClientData)));

        if (stack->values) {
            memcpy((char*)newStack, (char*)stack->values,
                (size_t)(stack->len*sizeof(ClientData)));

            if (stack->values != stack->space)
                ckfree((char*)stack->values);
        }
        stack->values = newStack;
    }
    stack->values[stack->len++] = cdata;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_PopStack()
 *
 *  Pops a bit of client data from the top of the given stack.
 * ------------------------------------------------------------------------
 */
ClientData
Itcl_PopStack(stack)
    Itcl_Stack *stack;  /* stack to be manipulated */
{
    if (stack->values && (stack->len > 0)) {
        stack->len--;
        return stack->values[stack->len];
    }
    return (ClientData)NULL;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_PeekStack()
 *
 *  Gets the current value from the top of the given stack.
 * ------------------------------------------------------------------------
 */
ClientData
Itcl_PeekStack(stack)
    Itcl_Stack *stack;  /* stack to be examined */
{
    if (stack->values && (stack->len > 0)) {
        return stack->values[stack->len-1];
    }
    return (ClientData)NULL;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_GetStackValue()
 *
 *  Gets a value at some index within the stack.  Index "0" is the
 *  first value pushed onto the stack.
 * ------------------------------------------------------------------------
 */
ClientData
Itcl_GetStackValue(stack,index)
    Itcl_Stack *stack;  /* stack to be examined */
    int index;          /* get value at this index */
{
    if (stack->values && (stack->len > 0)) {
        assert(index < stack->len);
        return stack->values[index];
    }
    return (ClientData)NULL;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_InitList()
 *
 *  Initializes a linked list structure, setting the list to the empty
 *  state.
 * ------------------------------------------------------------------------
 */
void
Itcl_InitList(listPtr)
    Itcl_List *listPtr;     /* list to be initialized */
{
    listPtr->validate = ITCL_VALID_LIST;
    listPtr->num      = 0;
    listPtr->head     = NULL;
    listPtr->tail     = NULL;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_DeleteList()
 *
 *  Destroys a linked list structure, deleting all of its elements and
 *  setting it to an empty state.  If the elements have memory associated
 *  with them, this memory must be freed before deleting the list or it
 *  will be lost.
 * ------------------------------------------------------------------------
 */
void
Itcl_DeleteList(listPtr)
    Itcl_List *listPtr;     /* list to be deleted */
{
    Itcl_ListElem *elemPtr;

    assert(listPtr->validate == ITCL_VALID_LIST);

    elemPtr = listPtr->head;
    while (elemPtr) {
        elemPtr = Itcl_DeleteListElem(elemPtr);
    }
    listPtr->validate = 0;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_CreateListElem()
 *
 *  Low-level routined used by procedures like Itcl_InsertList() and
 *  Itcl_AppendList() to create new list elements.  If elements are
 *  available, one is taken from the list element pool.  Otherwise,
 *  a new one is allocated.
 * ------------------------------------------------------------------------
 */
Itcl_ListElem*
Itcl_CreateListElem(listPtr)
    Itcl_List *listPtr;     /* list that will contain this new element */
{
    Itcl_ListElem *elemPtr;

    if (listPoolLen > 0) {
        elemPtr = listPool;
        listPool = elemPtr->next;
        --listPoolLen;
    }
    else {
        elemPtr = (Itcl_ListElem*)ckalloc((unsigned)sizeof(Itcl_ListElem));
    }
    elemPtr->owner = listPtr;
    elemPtr->value = NULL;
    elemPtr->next  = NULL;
    elemPtr->prev  = NULL;

    return elemPtr;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_DeleteListElem()
 *
 *  Destroys a single element in a linked list, returning it to a pool of
 *  elements that can be later reused.  Returns a pointer to the next
 *  element in the list.
 * ------------------------------------------------------------------------
 */
Itcl_ListElem*
Itcl_DeleteListElem(elemPtr)
    Itcl_ListElem *elemPtr;     /* list element to be deleted */
{
    Itcl_List *listPtr;
    Itcl_ListElem *nextPtr;

    nextPtr = elemPtr->next;

    if (elemPtr->prev) {
        elemPtr->prev->next = elemPtr->next;
    }
    if (elemPtr->next) {
        elemPtr->next->prev = elemPtr->prev;
    }

    listPtr = elemPtr->owner;
    if (elemPtr == listPtr->head)
        listPtr->head = elemPtr->next;
    if (elemPtr == listPtr->tail)
        listPtr->tail = elemPtr->prev;
    --listPtr->num;

    if (listPoolLen < ITCL_LIST_POOL_SIZE) {
        elemPtr->next = listPool;
        listPool = elemPtr;
        ++listPoolLen;
    }
    else {
        ckfree((char*)elemPtr);
    }
    return nextPtr;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_InsertList()
 *
 *  Creates a new list element containing the given value and returns
 *  a pointer to it.  The element is inserted at the beginning of the
 *  specified list.
 * ------------------------------------------------------------------------
 */
Itcl_ListElem*
Itcl_InsertList(listPtr,val)
    Itcl_List *listPtr;     /* list being modified */
    ClientData val;         /* value associated with new element */
{
    Itcl_ListElem *elemPtr;
    assert(listPtr->validate == ITCL_VALID_LIST);

    elemPtr = Itcl_CreateListElem(listPtr);

    elemPtr->value = val;
    elemPtr->next  = listPtr->head;
    elemPtr->prev  = NULL;
    if (listPtr->head) {
        listPtr->head->prev = elemPtr;
    }
    listPtr->head  = elemPtr;
    if (listPtr->tail == NULL) {
        listPtr->tail = elemPtr;
    }
    ++listPtr->num;

    return elemPtr;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_InsertListElem()
 *
 *  Creates a new list element containing the given value and returns
 *  a pointer to it.  The element is inserted in the list just before
 *  the specified element.
 * ------------------------------------------------------------------------
 */
Itcl_ListElem*
Itcl_InsertListElem(pos,val)
    Itcl_ListElem *pos;     /* insert just before this element */
    ClientData val;         /* value associated with new element */
{
    Itcl_List *listPtr;
    Itcl_ListElem *elemPtr;

    listPtr = pos->owner;
    assert(listPtr->validate == ITCL_VALID_LIST);
    assert(pos != NULL);

    elemPtr = Itcl_CreateListElem(listPtr);
    elemPtr->value = val;

    elemPtr->prev = pos->prev;
    if (elemPtr->prev) {
        elemPtr->prev->next = elemPtr;
    }
    elemPtr->next = pos;
    pos->prev     = elemPtr;

    if (listPtr->head == pos) {
        listPtr->head = elemPtr;
    }
    if (listPtr->tail == NULL) {
        listPtr->tail = elemPtr;
    }
    ++listPtr->num;

    return elemPtr;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_AppendList()
 *
 *  Creates a new list element containing the given value and returns
 *  a pointer to it.  The element is appended at the end of the
 *  specified list.
 * ------------------------------------------------------------------------
 */
Itcl_ListElem*
Itcl_AppendList(listPtr,val)
    Itcl_List *listPtr;     /* list being modified */
    ClientData val;         /* value associated with new element */
{
    Itcl_ListElem *elemPtr;
    assert(listPtr->validate == ITCL_VALID_LIST);

    elemPtr = Itcl_CreateListElem(listPtr);

    elemPtr->value = val;
    elemPtr->prev  = listPtr->tail;
    elemPtr->next  = NULL;
    if (listPtr->tail) {
        listPtr->tail->next = elemPtr;
    }
    listPtr->tail  = elemPtr;
    if (listPtr->head == NULL) {
        listPtr->head = elemPtr;
    }
    ++listPtr->num;

    return elemPtr;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_AppendListElem()
 *
 *  Creates a new list element containing the given value and returns
 *  a pointer to it.  The element is inserted in the list just after
 *  the specified element.
 * ------------------------------------------------------------------------
 */
Itcl_ListElem*
Itcl_AppendListElem(pos,val)
    Itcl_ListElem *pos;     /* insert just after this element */
    ClientData val;         /* value associated with new element */
{
    Itcl_List *listPtr;
    Itcl_ListElem *elemPtr;

    listPtr = pos->owner;
    assert(listPtr->validate == ITCL_VALID_LIST);
    assert(pos != NULL);

    elemPtr = Itcl_CreateListElem(listPtr);
    elemPtr->value = val;

    elemPtr->next = pos->next;
    if (elemPtr->next) {
        elemPtr->next->prev = elemPtr;
    }
    elemPtr->prev = pos;
    pos->next     = elemPtr;

    if (listPtr->tail == pos) {
        listPtr->tail = elemPtr;
    }
    if (listPtr->head == NULL) {
        listPtr->head = elemPtr;
    }
    ++listPtr->num;

    return elemPtr;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_SetListValue()
 *
 *  Modifies the value associated with a list element.
 * ------------------------------------------------------------------------
 */
void
Itcl_SetListValue(elemPtr,val)
    Itcl_ListElem *elemPtr; /* list element being modified */
    ClientData val;         /* new value associated with element */
{
    Itcl_List *listPtr = elemPtr->owner;
    assert(listPtr->validate == ITCL_VALID_LIST);
    assert(elemPtr != NULL);

    elemPtr->value = val;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_EventuallyFree()
 *
 *  Registers a piece of data so that it will be freed when no longer
 *  in use.  The data is registered with an initial usage count of "0".
 *  Future calls to Itcl_PreserveData() increase this usage count, and
 *  calls to Itcl_ReleaseData() decrease the count until it reaches
 *  zero and the data is freed.
 *
 *  The Tk library has a similar function that could be used instead.
 *  This function is provided simply to avoid a dependency on the
 *  Tk library, to make porting easier for Tcl-only users.
 * ------------------------------------------------------------------------
 */
void
Itcl_EventuallyFree(cdata,fproc)
    ClientData cdata;      /* data to be freed when not in use */
    Itcl_FreeProc *fproc;  /* procedure called to free data */
{
    int newEntry;
    Tcl_HashEntry *entry;
    Itcl_PreservedData *chunk;

    /*
     *  If a list has not yet been created to manage bits of
     *  preserved data, then create it.
     */
    if (!Itcl_PreservedList) {
        Itcl_PreservedList = (Tcl_HashTable*)ckalloc(
            (unsigned)sizeof(Tcl_HashTable)
        );
        Tcl_InitHashTable(Itcl_PreservedList,TCL_ONE_WORD_KEYS);
    }

    /*
     *  If the clientData value is NULL, do nothing.
     */
    if (cdata != NULL) {
        /*
         *  Find or create the data in the global list.
         */
        entry = Tcl_CreateHashEntry(Itcl_PreservedList,(char*)cdata, &newEntry);
        if (newEntry) {
            chunk = (Itcl_PreservedData*)ckalloc(
                (unsigned)sizeof(Itcl_PreservedData)
            );
            chunk->data  = cdata;
            chunk->usage = 0;
            chunk->fproc = fproc;
            Tcl_SetHashValue(entry, (ClientData)chunk);
        }
        else {
            chunk = (Itcl_PreservedData*)Tcl_GetHashValue(entry);
            chunk->fproc = fproc;
        }

        /*
         *  If the usage count is zero, then delete the data now.
         */
        if (chunk->usage == 0) {
            chunk->usage = -1;  /* cannot preserve/release anymore */

            (*chunk->fproc)(chunk->data);
            Tcl_DeleteHashEntry(entry);
            ckfree((char*)chunk);
        }
    }
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_PreserveData()
 *
 *  Increases the usage count for a piece of data that will be freed
 *  later when no longer needed.  Each call to Itcl_PreserveData()
 *  puts one claim on a piece of data, and subsequent calls to
 *  Itcl_ReleaseData() remove those claims.  When Itcl_EventuallyFree()
 *  is called, and when the usage count reaches zero, the data is
 *  freed.
 *
 *  The Tk library has a similar function that could be used instead.
 *  This function is provided simply to avoid a dependency on the
 *  Tk library, so Tcl can be used as a stand-alone package.
 * ------------------------------------------------------------------------
 */
ClientData
Itcl_PreserveData(cdata)
    ClientData cdata;      /* data to be preserved */
{
    Tcl_HashEntry *entry;
    Itcl_PreservedData *chunk;
    int newEntry;

    /*
     *  If a list has not yet been created to manage bits of
     *  preserved data, then create it.
     */
    if (!Itcl_PreservedList) {
        Itcl_PreservedList = (Tcl_HashTable*)ckalloc(
            (unsigned)sizeof(Tcl_HashTable)
        );
        Tcl_InitHashTable(Itcl_PreservedList,TCL_ONE_WORD_KEYS);
    }

    /*
     *  If the clientData value is NULL, do nothing.
     */
    if (cdata != NULL) {
        /*
         *  Find the data in the global list and bump its usage count.
         */
        entry = Tcl_CreateHashEntry(Itcl_PreservedList,(char*)cdata, &newEntry);
        if (newEntry) {
            chunk = (Itcl_PreservedData*)ckalloc(
                (unsigned)sizeof(Itcl_PreservedData)
            );
            chunk->data  = cdata;
            chunk->usage = 0;
            chunk->fproc = NULL;
            Tcl_SetHashValue(entry, (ClientData)chunk);
        }
        else {
            chunk = (Itcl_PreservedData*)Tcl_GetHashValue(entry);
        }

        /*
         *  Only increment the usage if it is non-negative.
         *  Negative numbers mean that the data is in the process
         *  of being destroyed by Itcl_ReleaseData(), and should
         *  not be further preserved.
         */
        if (chunk->usage >= 0) {
            chunk->usage++;
        }
    }
    return cdata;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_ReleaseData()
 *
 *  Decreases the usage count for a piece of data that was registered
 *  previously via Itcl_PreserveData().  After Itcl_EventuallyFree()
 *  is called and the usage count reaches zero, the data is
 *  automatically freed.
 *
 *  The Tk library has a similar function that could be used instead.
 *  This function is provided simply to avoid a dependency on the
 *  Tk library, so Tcl can be used as a stand-alone package.
 * ------------------------------------------------------------------------
 */
void
Itcl_ReleaseData(cdata)
    ClientData cdata;      /* data to be released */
{
    Tcl_HashEntry *entry;
    Itcl_PreservedData *chunk;

    /*
     *  If the clientData value is NULL, do nothing.
     *  Otherwise, find the data in the global list and
     *  decrement its usage count.
     */
    if (cdata != NULL) {
        entry = NULL;
        if (Itcl_PreservedList != NULL) {
            entry = Tcl_FindHashEntry(Itcl_PreservedList,(char*)cdata);
        }
        if (!entry) {
            panic("Itcl_ReleaseData() called for data not registered or already freed!");
        }

        /*
         *  Only decrement the usage if it is non-negative.
         *  When the usage reaches zero, set it to a negative number
         *  to indicate that data is being destroyed, and then
         *  invoke the client delete proc.  When the data is deleted,
         *  remove the entry from the preservation list.
         */
        chunk = (Itcl_PreservedData*)Tcl_GetHashValue(entry);
        if ((chunk->usage > 0) && (--chunk->usage == 0) && chunk->fproc) {

            chunk->usage = -1;  /* cannot preserve/release anymore */

            (*chunk->fproc)(chunk->data);
            Tcl_DeleteHashEntry(entry);
            ckfree((char*)chunk);
        }
    }
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_SaveInterpState()
 *
 *  Used to save the state of the interpreter when some other Tcl code
 *  needs to be evaluated right before returning a result.  Takes a
 *  snapshot of the interpreter result and status variables, and returns
 *  an Itcl_InterpState token representing the current state.  This
 *  token can later be passed to Itcl_RestoreInterpState() to restore
 *  the interpreter to its original state.
 * ------------------------------------------------------------------------
 */
Itcl_InterpState
Itcl_SaveInterpState(interp, status)
    Tcl_Interp* interp;     /* interpreter being modified */
    int status;             /* integer status code from current operation */
{
    Interp *iPtr = (Interp*)interp;

    InterpState *info;
    char *val;

    info = (InterpState*)ckalloc(sizeof(InterpState));
    info->validate = ITCL_VALID_STATE;
    info->status = status;
    info->errorInfo = NULL;
    info->errorCode = NULL;

    /*
     *  BE CAREFUL:  Much of the Tcl library assumes that the
     *    freeProc will be null when a command starts executing.
     *    Instead of using Tcl_SetResult(), many commands simply
     *    assign the interp->result member directly.  If the
     *    interpreter has a null free proc, make note of this
     *    here, and restore it back to that state.
     */

    if (iPtr->freeProc == NULL) {
        if (iPtr->result == iPtr->resultSpace) {
            info->resultType = ITCL_RESULT_INTERP;
            info->result = ckalloc((unsigned)(strlen(interp->result)+1));
            strcpy(info->result, interp->result);
        } else {
            info->resultType = ITCL_RESULT_STATIC;
            info->result = interp->result;
        }
    } else {
        info->resultType = ITCL_RESULT_DYNAMIC;
        info->result = ckalloc((unsigned)(strlen(interp->result)+1));
        strcpy(info->result, interp->result);
    }

    if ((iPtr->flags & ERR_IN_PROGRESS) != 0) {
        val = Tcl_GetVar(interp, "errorInfo", TCL_GLOBAL_ONLY);
        if (val) {
            info->errorInfo = ckalloc((unsigned)(strlen(val)+1));
            strcpy(info->errorInfo, val);
        }

        val = Tcl_GetVar(interp, "errorCode", TCL_GLOBAL_ONLY);
        if (val) {
            info->errorCode = ckalloc((unsigned)(strlen(val)+1));
            strcpy(info->errorCode, val);
        }
    }
    return (Itcl_InterpState)info;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_RestoreInterpState()
 *
 *  Used to restore the interpreter to the state that it was in when
 *  some Itcl_InterpState token was created.  The result string is set
 *  back to its previous value, and the "errorInfo" and "errorCode"
 *  variables are restored.  After this call, the Itcl_InterpState
 *  token is no longer valid.
 *
 *  Returns the status code that was pending at the time the state was
 *  captured.
 * ------------------------------------------------------------------------
 */
int
Itcl_RestoreInterpState(interp, state)
    Tcl_Interp* interp;      /* interpreter being modified */
    Itcl_InterpState state;  /* token representing interpreter state */
{
    Interp *iPtr = (Interp*)interp;
    InterpState *info = (InterpState*)state;

    int i, status;
    int ec;
    char **ev;

    assert(info->validate == ITCL_VALID_STATE);

    Tcl_ResetResult(interp);

    if (info->errorInfo) {
        Tcl_AddErrorInfo(interp, info->errorInfo);
        ckfree(info->errorInfo);
    }

    if (info->errorCode) {
        if (Tcl_SplitList(interp, info->errorCode, &ec, &ev) == TCL_OK) {
            for (i=0; i < ec; i++) {
                Tcl_SetErrorCode(interp, ev[i], (char*)NULL);
            }
            ckfree((char*)ev);
        }
        ckfree(info->errorCode);
    }

    /*
     *  Restore the interpreter result.  If the interpreter was
     *  using its resultSpace buffer, then go back to that.  If
     *  it had a static string, then go back to that.  Otherwise,
     *  just set the result and be done with it.
     */
    switch (info->resultType) {
        case ITCL_RESULT_INTERP:
            iPtr->freeProc = NULL;
            iPtr->result = iPtr->resultSpace;
            strcpy(iPtr->result, info->result);
            ckfree(info->result);
            break;

        case ITCL_RESULT_STATIC:
            iPtr->freeProc = NULL;
            iPtr->result = info->result;
            break;

        case ITCL_RESULT_DYNAMIC:
            Tcl_SetResult(interp, info->result, TCL_DYNAMIC);
            /* NOTE:  don't free info->result -- transfer to interp */
            break;

        default:
            panic("strange interpreter state in Itcl_RestoreInterpState()");
    }

    status = info->status;
    info->validate = 0;
    ckfree((char*)info);

    return status;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_DiscardInterpState()
 *
 *  Discards the state information createdby Itcl_SaveInterpState().
 *  This information is discarded automatically by Itcl_RestoreInterpState()
 *  when the interpreter is restored, but sometimes the interpreter
 *  should not be restored and the information is no longer needed.
 *  In such cases this procedure should be called to free resources
 *  used by the state information.
 * ------------------------------------------------------------------------
 */
void
Itcl_DiscardInterpState(state)
    Itcl_InterpState state;  /* token representing interpreter state */
{
    InterpState *info = (InterpState*)state;
    assert(info->validate == ITCL_VALID_STATE);

    if (info->errorInfo) {
        ckfree(info->errorInfo);
    }
    if (info->errorCode) {
        ckfree(info->errorCode);
    }
    ckfree(info->result);

    info->validate = 0;
    ckfree((char*)info);
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_ProtectionStr()
 *
 *  Converts an integer protection code (ITCL_PUBLIC, ITCL_PROTECTED,
 *  or ITCL_PRIVATE) into a human-readable character string.  Returns
 *  a pointer to this string.
 * ------------------------------------------------------------------------
 */
char*
Itcl_ProtectionStr(pLevel)
    int pLevel;     /* protection level */
{
    switch (pLevel) {
    case ITCL_PUBLIC:
        return "public";
    case ITCL_PROTECTED:
        return "protected";
    case ITCL_PRIVATE:
        return "private";
    }
    return "<bad-protection-code>";
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_ParseNamespPath()
 *
 *  Parses a reference to a namespace element of the form:
 *
 *      namesp::namesp::namesp::element
 *
 *  Returns pointers to the head part ("namesp::namesp::namesp")
 *  and the tail part ("element").  These pointers remain valid until
 *  the next call to this routine.  If the head part is missing,
 *  a NULL pointer is returned and the rest of the string is taken
 *  as the tail.
 * ------------------------------------------------------------------------
 */
void
Itcl_ParseNamespPath(name,head,tail)
    char *name;          /* path name to class member */
    char **head;         /* returns "namesp::namesp::namesp" part */
    char **tail;         /* returns "element" part */
{
    static Tcl_DString* buffer = NULL;

    register char *sep;

    /*
     *  If a buffer has not yet been created, do it now.
     */
    if (buffer == NULL) {
        buffer = (Tcl_DString*)ckalloc(sizeof(Tcl_DString));
        Tcl_DStringInit(buffer);
    }
    Tcl_DStringTrunc(buffer, 0);

    /*
     *  Copy the name into the buffer and parse it.  Look
     *  backward from the end of the string to the first '::'
     *  scope qualifier.
     */
    Tcl_DStringAppend(buffer, name, -1);
    name = Tcl_DStringValue(buffer);

    for (sep=name; *sep != '\0'; sep++)
        ;

    while (--sep > name) {
        if (*sep == ':' && *(sep-1) == ':') {
            break;
        }
    }

    if (sep > name) {
        *(sep-1) = '\0';
        *tail = sep+1;
        *head = name;
    }
    else {
        *tail = name;
        *head = NULL;
    }
    return;
}
