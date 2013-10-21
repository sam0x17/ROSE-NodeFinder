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
   //std::cout << "RESULT_CREATED: " << begin_index << ", " << end_index << std::endl;
   this->nodes = nodes;
   this->begin_index = begin_index;
   this->end_index = end_index;
   //std::cout << "begin_index: " << begin_index << std::endl;
   //std::cout << "end_index: " << end_index << std::endl;
}

int NodeFinderResult::size()
{
   return end_index - begin_index;
}

SgNode* NodeFinderResult::operator [](int index)
{
   //std::cout << "RESULT_ACCESS: " << index << "(" << begin_index << ", " << end_index << ")" <<std::endl;
   ROSE_ASSERT(index >= 0);
   ROSE_ASSERT(index < size());
   return (*nodes)[begin_index + index];
}



