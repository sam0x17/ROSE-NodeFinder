/*
 * NodeFinderResult.C
 *
 *  Created on: Oct 13, 2013
 *      Author: sam
 */
#include <NodeFinderResult.h>

NodeFinderResult::NodeFinderResult(std::vector<SgNode*> *nodes, int begin_index, int end_index)
{
   ROSE_ASSERT(begin_index >= 0);
   ROSE_ASSERT(end_index >= 0);
   ROSE_ASSERT(end_index >= begin_index);
   ROSE_ASSERT(nodes != NULL);
   this->nodes = nodes;
   this->begin_index = begin_index;
   this->end_index = end_index;
}

int NodeFinderResult::size()
{
   return end_index - begin_index;
}

SgNode* NodeFinderResult::operator [](int index)
{
   ROSE_ASSERT(index >= 0);
   ROSE_ASSERT(index < size());
   return (*nodes)[begin_index + index];
}



