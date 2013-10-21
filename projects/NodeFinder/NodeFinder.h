/*
 * NodeFinder.h
 *
 *  Created on: Oct 8, 2013
 *      Author: Sam Kelly <kellys@dickinson.edu>
 */
#ifndef ROSE_Project_NodeFinder_H
#define ROSE_Project_NodeFinder_H

#include <stdio.h>
#include <rose.h>
#include <algorithm>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <boost/foreach.hpp>
#include <NodeFinderResult.h>

class NodeFinder
{
   public:
      /**
       * index_root: a pointer to the root node of the portion of the AST that this NodeFinder will index
       */
      NodeFinder(SgNode *index_root);
      void rebuildIndex();
      void rebuildIndex(SgNode *index_root);
      NodeFinderResult find(SgNode *search_root, VariantT search_type);
      struct region_info
      {
         int begin_index; // inclusive
         int end_index; // exclusive
         bool operator!=(const region_info& RHS)
         {
            return begin_index != RHS.begin_index || end_index != RHS.end_index;
         }
         bool operator==(const region_info& RHS)
         {
            return begin_index == RHS.begin_index && end_index == RHS.end_index;
         }
      };
   private:
      void rebuildIndex_helper(SgNode *node);
      SgNode *index_root;
      boost::unordered_map<SgNode*, boost::unordered_map<VariantT, region_info>*> node_region_map;
      boost::unordered_map<SgNode*, boost::unordered_set<VariantT>*> node_contained_types;
      boost::unordered_map<VariantT, std::vector<SgNode*>*> node_map;
};

#endif
