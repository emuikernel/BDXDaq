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
//      Transition Graph Implementation
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: transitionGraph.cc,v $
//   Revision 1.3  1999/12/10 21:22:16  rwm
//   int i at function scope.
//
//   Revision 1.2  1999/11/22 16:46:50  rwm
//   Fix scoping warning
//
//   Revision 1.1.1.1  1996/10/11 13:39:18  chen
//   run control source
//
//
#include <daqSystem.h>
#include "transitionGraph.h"

//=====================================================================
//             GraphNode Implementation
//=====================================================================
graphNode::graphNode (int st)
:state_ (st), color_ (TRG_WHITE), linkToParent_ (0), parent_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("Create graphNode Class Object\n");
#endif
  // empty
}

graphNode::~graphNode (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Delete graphNode Class Object\n");
#endif
  // empty
}

const int
graphNode::state (void) const
{
  return state_;
}

int
graphNode::color (void) const
{
  return color_;
}

void
graphNode::color (int col)
{
  color_ = col;
}

void*
graphNode::linkToParent (void) const
{
  return linkToParent_;
}

void
graphNode::linkToParent (void* link)
{
  linkToParent_ = link;
}

graphNode*
graphNode::parent (void) const
{
  return parent_;
}

void
graphNode::parent (graphNode* pa)
{
  parent_ = pa;
}

//========================================================================
//           Implementation of adjNode
//========================================================================
adjNode::adjNode (void* tr, graphNode& node)
:traner_ (tr), node_ (node)
{
#ifdef _TRACE_OBJECTS
  printf ("Create adjNode Class Object\n");
#endif
}

adjNode::~adjNode (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Delete adjNode Class Object\n");
#endif
}

void*
adjNode::traner (void) const
{
  return traner_;
}

graphNode&
adjNode::node (void) const
{
  return node_;
}

//=========================================================================
//       Implementation of transition graph
//=========================================================================
transitionGraph::transitionGraph (daqSystem* system)
:size_ (NUM_GRAPH_NODES), system_ (system)
{
#ifdef _TRACE_OBJECTS
  printf ("Create transitionGraph Class Object\n");
#endif
  // conbstruct graph
  constructGraph ();
}

transitionGraph::~transitionGraph (void)
{
  int i;
#ifdef _TRACE_OBJECTS
  printf ("Delete transitionGraph Class Object\n");
#endif

  for (i = 0; i < size_; i++) {
    codaSlistIterator ite (adjLists_[i]);
    adjNode *adjnode = 0;

    for (ite.init(); !ite; ++ite) {
      adjnode = (adjNode *)ite();
      delete adjnode;
    }
  }
  
  for (i = 0; i < size_; i++)
    delete nodes_[i];
}

void
transitionGraph::constructGraph (void)
{
  int i = 0;

  graphNode* dormant = new graphNode (CODA_DORMANT);
  graphNode* booted = new graphNode (CODA_BOOTED);  
  graphNode* configured = new graphNode (CODA_CONFIGURED);
  graphNode* downloaded = new graphNode (CODA_DOWNLOADED);
  graphNode* paused = new graphNode (CODA_PAUSED);
  graphNode* active = new graphNode (CODA_ACTIVE);

  i = 0;
  nodes_[i++] = dormant;
  nodes_[i++] = booted;  
  nodes_[i++] = configured;
  nodes_[i++] = downloaded;
  nodes_[i++] = paused;
  nodes_[i++] = active;

  i = 0;
  adjNode* node1 = 0;
  adjNode* node = 0;
  // adjcency list for dormant node
  adjNode* adjdormant = new adjNode (0, *dormant);
  node = new adjNode ((void *)system_->booter_, *booted);
  // add always add to header
  adjLists_[i].add ((void *)node);
  adjLists_[i++].add ((void *)adjdormant);		    
  
  // adjcency list for booted node
  adjNode* adjbooted = new adjNode (0, *booted);
  node = new adjNode ((void *)system_->configurer_, *configured);
  adjLists_[i].add ((void *)node);
  adjLists_[i++].add ((void *)adjbooted);  

  // adjcency list for configured node
  adjNode* adjconfigured = new adjNode (0, *configured);
  node = new adjNode ((void *)system_->downloader_, *downloaded);
  adjLists_[i].add ((void *)node);
  adjLists_[i++].add ((void *)adjconfigured);  

  // adjcency list for downloaded node
  adjNode* adjdownloaded = new adjNode (0, *downloaded);
  node = new adjNode ((void *)system_->prestarter_, *paused);
  node1 = new adjNode ((void *)system_->configurer_, *configured);
  adjLists_[i].add ((void *)node);
  adjLists_[i].add ((void *)node1);
  adjLists_[i++].add ((void *)adjdownloaded);    

  // adjcency list for paused node
  adjNode* adjpaused = new adjNode (0, *paused);
  node = new adjNode ((void *)system_->activater_, *active);
  node1 = new adjNode ((void *)system_->ender_, *downloaded);
  adjLists_[i].add ((void *)node);
  adjLists_[i].add ((void *)node1);
  adjLists_[i++].add ((void *)adjpaused);    

  // adjcency list for active node
  adjNode* adjactive = new adjNode (0, *active);
  node = new adjNode ((void *)system_->ender_, *downloaded);
  node1 = new adjNode ((void *)system_->pauser_, *paused);
  // Active -> Download Directly
  adjLists_[i].add ((void *)node1);
  adjLists_[i].add ((void *)node);
  adjLists_[i++].add ((void *)adjactive);    
}

codaSlist&
transitionGraph::listRef (graphNode* node)
{
  adjNode* fnode = 0;
  graphNode *gnode = 0;
  for (int i = 0; i < size_; i++) {
    fnode = (adjNode *)adjLists_[i].firstElement ();
    gnode = &(fnode->node ());
    if (node->state () == gnode->state ()) 
      return  adjLists_[i];
  }
}

codaSlist&
transitionGraph::listRef (int state)
{
  adjNode*   fnode = 0;
  graphNode* gnode = 0;
  for (int i = 0; i < size_; i++) {
    fnode = (adjNode *)adjLists_[i].firstElement ();
    gnode = &(fnode->node());
    if (state == gnode->state ())
      return  adjLists_[i];
  }
}

void
transitionGraph::cleanup (void)
{
  for (int i = 0; i < size_; i++) {
    nodes_[i]->color (TRG_WHITE);
    nodes_[i]->linkToParent (0);
    nodes_[i]->parent (0);
  }
}

int
transitionGraph::DFS (int istate, int fstate, 
		      graphNode* &src, graphNode* &dest)
{
  int found = 0;
  int i;

  for (i = 0; i < size_; i++) {
    if (istate == nodes_[i]->state()){
      found = 1;
      break;
    }
  }
  if (!found) {
    src = 0;
    dest = 0;
    return CODA_ERROR;
  }
  found = 0;
  graphNode* source = nodes_[i];
      
  for (i = 0; i < size_; i++) {
    if (fstate == nodes_[i]->state()){
      found = 1;
      break;
    }
  }
  if (!found) {
    src = 0;
    dest = 0;
    return CODA_ERROR;
  }
  found = 0;
  graphNode* des = nodes_[i];

  // cleanup all the nodes
  cleanup ();

  // do recursive visit for all the nodes
  int status = CODA_ERROR;
  DFS_VISIT (source, des, status);
  src = source;
  dest = des;
  return status;
}

void
transitionGraph::DFS_VISIT (graphNode* u, graphNode* des, int& status)
{
  u->color (TRG_GREY);
  codaSlist& list = listRef (u);
  codaSlistIterator ite (list);
  adjNode* v = 0;

  for (ite.init(); !ite; ++ite) {
    v = (adjNode *)ite ();
    graphNode& vref = v->node();
    if (vref.color () == TRG_WHITE) {
      vref.linkToParent (v->traner ());
      vref.parent (u);
      if (des->state () == vref.state ()) {
	status = CODA_SUCCESS;
	break;
      }
      DFS_VISIT (&vref, des, status);
    }
    u->color (TRG_BLACK);
  }
}
  
void
transitionGraph::printPath (graphNode *source, graphNode *dest)
{
  graphNode *node;

  printf ("destination is %d\n",dest->state());
  node = dest;
  while (node != 0) {
    printf ("Link to %d\n", node->state ());
    node = node->parent ();
  }
}
