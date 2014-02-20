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
      /* index_root: a pointer to the root node of the portion of the AST that this NodeFinder
       * will index */
      NodeFinder(SgNode *index_root);


		/* same as default constructor but includes option of whether to enable the alternate
       * indexing method (which only takes O(n)) at the cost of O(log(m)) find times where
		 * m is the number of nodes of the type being searched for that are decendents of the
		 * node currently being searched */
		NodeFinder(SgNode *index_root, bool use_alt_method);

      /* Rebuilds the node index (should be called if a change to the AST has occurred.
       * Automatically called by the NodeFinder constructor. Runs in O(n) time where n
       * is the number of nodes that are descendants of the index_root node. */
      void rebuildIndex();

      /* Rebuilds the node index using the specified node as the new root node for the
       * index. Automatically called by the NodeFinder constructor. Runs in O(n) time
       * where n is the number of nodes that are descendants of index_root. */
      void rebuildIndex(SgNode *index_root);

		void rebuildIndex(SgNode *index_root, bool use_alt_method);

      /* Returns a NodeFinderResult containing the list of nodes of type search_type
       * that are descendants of of the node search_root. This function runs in O(1)
       * time because the returned NodeFinderResult merely indexes into an already
       * existing internal data array created when rebuildIndex() and/or the
       * constructor is called. Can be called repeatedly without side effects.
       * If no results are found, the size() method for the NodeFinderResult
       * will return 0.
       *
       * Preconditions: search_root must be a descendant of the index_root
       * node that was specified the last time the index was built; no
       * changes have occurred to the structure of the AST since the last
       * time the index was built. */
      NodeFinderResult find(SgNode *search_root, VariantT search_type);

      /* Internal data structure used by NodeFinder classes to represent an
       * index into the node_map vector for a given node type */
      struct region_info
      {
         int begin_index; // inclusive
         int end_index; // exclusive
      };

   private:
		int current_df_index;
		bool use_alt_method;
      void rebuildIndex_helper(SgNode *node);
		inline NodeFinderResult find_alt(SgNode *search_root, VariantT search_type);
		inline void rebuildIndex_alt(SgNode *index_root);
		void rebuildIndex_helper_alt(SgNode *node);
      SgNode *index_root;

		// index data structures
      boost::unordered_map<SgNode*, boost::unordered_map<VariantT, region_info>*> node_region_map;
      boost::unordered_map<SgNode*, boost::unordered_set<VariantT>*> node_contained_types;
      boost::unordered_map<VariantT, std::vector<SgNode*>*> node_map;

		// index data structure memory allocation tracking
      std::vector<boost::unordered_map<VariantT, region_info>*> node_region_map_allocations;
      std::vector<boost::unordered_set<VariantT>*> node_contained_types_allocations;
      std::vector<std::vector<SgNode*>*> node_map_allocations;
};



#endif
