#ifndef ROSE_Project_NodeFinder_H
#define ROSE_Project_NodeFinder_H

#include <stdio.h>
#include <rose.h>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <boost/foreach.hpp>

class NodeFinder
{
   public:
      /**
       * index_root: a pointer to the root node of the portion of the AST that this NodeFinder will index
       */
      NodeFinder(SgNode *index_root);
      ~NodeFinder();
      void rebuildIndex();
      void rebuildIndex(SgNode *index_root);
      std::vector<SgNode*> find(SgNode *search_root, VariantT return_type);

   private:
      struct region_info
      {
         int begin_index;
         int end_index;
      };
      SgNode *index_root;
      boost::unordered_map<SgNode*, boost::unordered_map<VariantT, region_info>> node_region_map;
      boost::unordered_map<VariantT, std::vector<SgNode*>> node_map;
      void rebuildIndex_helper(SgNode *node, int dfs_index);

};

#endif
