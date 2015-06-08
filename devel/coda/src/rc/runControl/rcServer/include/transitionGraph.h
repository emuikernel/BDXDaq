//-----------------------------------------------------------------------------
// Copyright (c) 1994,1995 Southeastern Universities Research Association,
//                         Continuous Electron Beam Accelerator Facility
//
// This software was developed under a United States Government license
// described in the NOTICE file included as part of this distribution.
//
// CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606
//       coda@cebaf.gov  Tel: (804) 249-7030     Fax: (804) 249-5800
//-----------------------------------------------------------------------------
//
// Description:
//      A graph consisting nodes of the transition states and edges of
//      transitioners. Use DFS algorithm to find out a path from
//      one state to another.
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: transitionGraph.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:18  chen
//   run control source
//
//
#ifndef _CODA_TRANSITION_GRAPH_H
#define _CODA_TRANSITION_GRAPH_H

#include <daqConst.h>
#include <codaSlist.h>

#define TRG_WHITE 0
#define TRG_GREY  1
#define TRG_BLACK 2

#define NUM_GRAPH_NODES 6

class daqSystem;
class transitionGraph;

class graphNode
{
public:
  // constructor and destructor
  graphNode  (int state);
  ~graphNode (void);

  const int   state         (void) const;
  int         color         (void) const;
  void        color         (int col);
  void*       linkToParent  (void) const;
  void        linkToParent  (void* link);
  graphNode*  parent        (void) const;
  void        parent        (graphNode* pa);

private:
  // node denoted by state value
  const int       state_;
  // color denotes whther this node has been visited
  int             color_;
  // transitioner from parent
  void*           linkToParent_;
  // parent node
  graphNode*      parent_;
};

// Adjcency List Node
class adjNode
{
public:
  adjNode  (void* tr, graphNode& node);
  ~adjNode (void);

  void*       traner    (void) const;
  graphNode&  node       (void) const;

private:
  graphNode&      node_;
  void*           traner_;
};

class transitionGraph
{
public:
  // constructor and destructor
  transitionGraph  (daqSystem* system);
  ~transitionGraph (void);

  // operations
  // construct a graph associated with transition diagram
  void constructGraph (void);

  // get list head from graphNode or from state
  codaSlist& listRef  (graphNode *node);
  codaSlist& listRef  (int       state);

  // clean out last DFS information
  void      cleanup (void);

  // do DFS search
  int       DFS (int istate, int fstate, 
		 graphNode* &source, graphNode* &dest);
  // status == CODA_SUCCESS, Find a path
  void      DFS_VISIT (graphNode* src, graphNode* dest, int& status);
  // must be called after DFS
  void      printPath (graphNode* source, graphNode* dest);

private:
  // array of lists of adjNodes
  codaSlist          adjLists_[NUM_GRAPH_NODES];
  // array of pointer of graphNodes
  graphNode*          nodes_[NUM_GRAPH_NODES];
  const unsigned int   size_;
  // daqSystem pointer
  daqSystem*           system_;
};
#endif
