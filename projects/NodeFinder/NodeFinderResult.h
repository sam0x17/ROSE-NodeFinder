/*
 * NodeFinderResult.h
 *
 *  Created on: Oct 13, 2013
 *      Author: Sam Kelly <kellys@dickinson.edu>
 */

#ifndef ROSE_Project_NodeFinderResult_H
#define ROSE_Project_NodeFinderResult_H
#include <stdio.h>
#include <rose.h>
#include <algorithm>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <boost/foreach.hpp>

class NodeFinderResult
{
   public:
      NodeFinderResult(std::vector<SgNode*> *nodes, int begin_index, int end_index);
      SgNode* operator [](int index);
      SgNode* get(int index);
      int size();
      typedef SgNode **iterator;
      typedef const SgNode **const_iterator;
      iterator begin() { return &(*nodes)[begin_index]; }
      iterator end() { return &(*nodes)[end_index]; }
   private:
      int begin_index;
      int end_index;
      std::vector<SgNode*> *nodes;
};


#endif /* ROSE_Project_NodeFinderResult_H */
