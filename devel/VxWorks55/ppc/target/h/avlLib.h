/* avlLib.h - AVL trees library header file */

/* Copyright 1999 Wind River Systems, Inc. */

/*
modification history
--------------------
01f,24jan01,sn  end file with newline(!) to avoid cpp errors
01e,10feb00,abd added avlMinimumGet and avlMaximumGet
01d,10feb00,abd added avlTreeWalk, avlTreePrint, avlTreeErase, avlTreePrintErase
01c,03feb00,abd added avlInsertInform, avlRemoveInsert
01b,24jan00,abd added avlSuccessorGet and avlPredecessorGet
01a,08feb99,wkn created.
*/

#ifndef __INCavlLibh
#define __INCavlLibh

#ifdef __cplusplus
extern "C" {
#endif

/* typedefs */

typedef struct
    {
    void *  left;   /* pointer to the left subtree */
    void *  right;  /* pointer to the right subtree */
    int     height; /* height of the subtree rooted at this node */
    } AVL_NODE;

typedef AVL_NODE * AVL_TREE;    /* points to the root node of the tree */

typedef union
    {
    int     i;
    UINT    u;
    void *  p;
    } GENERIC_ARGUMENT;

/* function declarations */

void avlRebalance (AVL_NODE *** ancestors, int count);

void * avlSearch (AVL_TREE root, GENERIC_ARGUMENT key,
          int compare (void *, GENERIC_ARGUMENT));

void * avlSuccessorGet (AVL_TREE root, GENERIC_ARGUMENT key,
         int compare (void *, GENERIC_ARGUMENT)); 

void * avlPredecessorGet (AVL_TREE root, GENERIC_ARGUMENT key,
         int compare (void *, GENERIC_ARGUMENT)); 

void * avlMinimumGet (AVL_TREE root);

void * avlMaximumGet (AVL_TREE root);

STATUS avlInsert (AVL_TREE * root, void * newNode, GENERIC_ARGUMENT key,
          int compare (void *, GENERIC_ARGUMENT));

STATUS avlInsertInform (AVL_TREE * pRoot, void * pNewNode, GENERIC_ARGUMENT key,
          void ** ppKeyHolder,
          int compare (void *, GENERIC_ARGUMENT));

void * avlRemoveInsert (AVL_TREE * pRoot, void * pNewNode, GENERIC_ARGUMENT key,
          int compare (void *, GENERIC_ARGUMENT));

void * avlDelete (AVL_TREE * root, GENERIC_ARGUMENT key,
          int compare (void *, GENERIC_ARGUMENT));

STATUS avlTreeWalk(AVL_TREE * pRoot, void walkExec(AVL_TREE * nodepp));

STATUS avlTreePrint(AVL_TREE * pRoot, void printNode(void * nodep));

STATUS avlTreeErase(AVL_TREE * pRoot);

STATUS avlTreePrintErase(AVL_TREE * pRoot, void printNode(void * nodep));

/* specialized implementation functions */

void * avlSearchUnsigned (AVL_TREE root, UINT key);

STATUS avlInsertUnsigned (AVL_TREE * root, void * newNode);

void * avlDeleteUnsigned (AVL_TREE * root, UINT key);

#ifdef __cplusplus
}
#endif

#endif /* __INCavlLibh */
