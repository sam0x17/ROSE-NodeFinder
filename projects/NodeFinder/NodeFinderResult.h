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
   private:
      std::vector<SgNode*> *nodes;
      int begin_index;
      int end_index;
};


#endif /* ROSE_Project_NodeFinderResult_H */
