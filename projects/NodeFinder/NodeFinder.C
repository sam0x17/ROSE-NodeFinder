/*
 * NodeFinder.C
 *
 *  Created on: Oct 8, 2013
 *      Author: Sam Kelly <kellys@dickinson.edu>
 */
#include <NodeFinder.h>
class DepthFirstIndexAttribute : public AstAttribute
{
	public:
		int df_index;
		int num_descendants;
};

NodeFinder::NodeFinder()
{
   this->index_root = NULL;
   this->use_alt_method = false;
}

NodeFinder::NodeFinder(SgNode *index_root)
{
   this->index_root = index_root;
	this->use_alt_method = false;
   rebuildIndex(index_root);
}

void NodeFinder::dispose()
{
   node_region_map.clear();
   node_map.clear();
   for(uint i = 0; i < node_region_map_allocations.size(); i++)
      delete node_region_map_allocations[i];
   for(uint i = 0; i < node_map_allocations.size(); i++)
      delete node_map_allocations[i];
   node_region_map_allocations.clear();
   node_map_allocations.clear();
   node_contained_types.clear();
   for(uint i = 0; i < node_contained_types_allocations.size(); i++)
      delete node_contained_types_allocations[i];
   node_contained_types_allocations.clear();
}

int NodeFinder::getDepthFirstIndex(SgNode *node)
{
	DepthFirstIndexAttribute *att = (DepthFirstIndexAttribute*)(node->getAttribute("depth-first-index"));
	return att->df_index;
}

int NodeFinder::getNumDescendants(SgNode *node)
{
	DepthFirstIndexAttribute *att = (DepthFirstIndexAttribute*)(node->getAttribute("depth-first-index"));
	return att->num_descendants;
}

int NodeFinder::getTotalNodes()
{
	if(current_df_index == 0) return 0;
	return getNumDescendants(index_root) + 1;
}

NodeFinder::NodeFinder(SgNode *index_root, bool use_alt_method)
{
	this->index_root = index_root;
	this->use_alt_method = use_alt_method;
	rebuildIndex(index_root);
}

NodeFinderResult NodeFinder::find(SgNode *search_root, VariantT search_type)
{
   ROSE_ASSERT(search_root != NULL);
	if(use_alt_method)
		return find_alt(search_root, search_type);
   boost::unordered_map<VariantT, region_info> *relevant_info = node_region_map[search_root];
	region_info *info = &relevant_info->operator[](search_type);
   int begin_index;
   int end_index;
   if(relevant_info->find(search_type) == relevant_info->end())
   {
      begin_index = 0;
      end_index = 0;
   } else {
      begin_index = info->begin_index;
      if(search_root->variantT() == search_type) begin_index++;
      end_index = info->end_index - 1;
      if(end_index < 0) end_index = 0;
   }
   return NodeFinderResult(node_map[search_type], begin_index, end_index);
}

inline NodeFinder::region_info NodeFinder::binarySearchRange(int start_target, int end_target, std::vector<SgNode*> *nodes)
{
	ROSE_ASSERT(end_target > start_target);
	ROSE_ASSERT(start_target >= 0);
	ROSE_ASSERT(end_target >= 0);
	region_info ret;
	ret.begin_index = binarySearchRangeHelper(start_target, 0, nodes->size(), true, nodes);
	ret.end_index = binarySearchRangeHelper(end_target, ret.begin_index, nodes->size(), false, nodes);
	ROSE_ASSERT(ret.begin_index <= ret.end_index);
	ROSE_ASSERT(ret.begin_index >= 0);
	ROSE_ASSERT(ret.end_index <= (int)nodes->size());
	return ret;
}

inline int NodeFinder::binarySearchRangeHelper(int target, int min, int max, bool is_start, std::vector<SgNode*> *nodes)
{
	int mid = -1;
	int mid_index = -1;
	while(min < max)
	{

		mid = (max + min) / 2;
		mid_index = getDepthFirstIndex(nodes->operator[](mid));
		if(mid_index < target) min = mid + 1;
		else max = mid;
	}
	// we just want the insertion index -- we don't
	// actually care about equality
	if(is_start)
	{
		if(max >=0 && max <= (int)nodes->size() && getDepthFirstIndex(nodes->operator[](max)) == target)
			return max + 1;
		return max;
	} else return min;
}

NodeFinderResult NodeFinder::find_alt(SgNode *search_root, VariantT search_type)
{
	if(node_map.find(search_type) == node_map.end())
	{
		return NodeFinderResult(NULL, 0, 0);
	}
	std::vector<SgNode*> *nodes = node_map[search_type];
	region_info found_range = binarySearchRange(getDepthFirstIndex(search_root),
		getDepthFirstIndex(search_root) + getNumDescendants(search_root) + 1, nodes);
	return NodeFinderResult(nodes, found_range.begin_index, found_range.end_index);
}

void NodeFinder::rebuildIndex()
{
   rebuildIndex(index_root);
}

void NodeFinder::rebuildIndex(SgNode *index_root, bool use_alt_method)
{
	this->use_alt_method = use_alt_method;
	rebuildIndex(index_root);
}

void NodeFinder::rebuildIndex(SgNode *index_root)
{
   this->index_root = index_root;
   node_region_map.clear();
   node_map.clear();
   for(uint i = 0; i < node_region_map_allocations.size(); i++)
      delete node_region_map_allocations[i];
   for(uint i = 0; i < node_map_allocations.size(); i++)
      delete node_map_allocations[i];
   node_region_map_allocations.clear();
   node_map_allocations.clear();
	current_df_index = 0;
	if(use_alt_method)
	{
		rebuildIndex_alt(index_root);
	} else {
   	rebuildIndex_helper(index_root);
	}
   node_contained_types.clear(); // don't need node_contained_types to perform searches
   for(uint i = 0; i < node_contained_types_allocations.size(); i++)
      delete node_contained_types_allocations[i];
   node_contained_types_allocations.clear();
}

inline void NodeFinder::rebuildIndex_alt(SgNode *index_root)
{
	ROSE_ASSERT(use_alt_method);
	ROSE_ASSERT(index_root != NULL);
	rebuildIndex_helper_alt(index_root);
}

void NodeFinder::rebuildIndex_helper(SgNode *node)
{
   ROSE_ASSERT(node != NULL);

	// add depth first indexing attribute
	DepthFirstIndexAttribute *att;
	if(node->attributeExists("depth-first-index"))
	{
		att = (DepthFirstIndexAttribute*)(node->getAttribute("depth-first-index"));
	} else {
		att = new DepthFirstIndexAttribute();
		node->addNewAttribute("depth-first-index", att);
	}
	att->df_index = current_df_index++; // intentionally post increment
	att->num_descendants = 0;

   // add node to the corresponding type vector
   std::vector<SgNode*> *current_list;
   if(node_map.count(node->variantT()) > 0)
   {
      current_list = node_map[node->variantT()];
   } else {
      current_list = new std::vector<SgNode*>();
      node_map_allocations.push_back(current_list);
      node_map[node->variantT()] = current_list;
   }
   current_list->push_back(node);

   // setup region map for this node
   boost::unordered_map<VariantT, region_info> *current_region_map;
   current_region_map = new boost::unordered_map<VariantT, region_info>();
   node_region_map_allocations.push_back(current_region_map);
   node_region_map[node] = current_region_map;

   // create contained types set for this node
   boost::unordered_set<VariantT> *current_contained_types = new boost::unordered_set<VariantT>();
   node_contained_types_allocations.push_back(current_contained_types);
   node_contained_types[node] = current_contained_types;

   region_info info;
   info.begin_index = current_list->size() - 1;
   info.end_index = info.begin_index + 1;
   (*current_region_map)[node->variantT()] = info;
   current_contained_types->insert(node->variantT());
   region_info *current_info = &(*current_region_map)[node->variantT()];
   current_info->end_index++;

   // traverse children
   for(uint i = 0; i < node->get_numberOfTraversalSuccessors(); i++)
   {
      SgNode *child = node->get_traversalSuccessorByIndex(i);
      if(child == NULL) continue;

      // recursive call
      rebuildIndex_helper(child);
      boost::unordered_map<VariantT, region_info> *current_child_region_map;
      current_child_region_map = node_region_map[child];

      // bubble up contained types and node info
      current_contained_types->insert(child->variantT());
      BOOST_FOREACH(VariantT type, *(node_contained_types[child]))
      {
         region_info *child_info = &(*current_child_region_map)[type];
         region_info current_info;

         if(current_contained_types->find(type) == current_contained_types->end())
         {
            current_info.begin_index = child_info->begin_index;
            current_info.end_index = child_info->end_index;
            current_contained_types->insert(type);
         } else {
            // merge child region info
            current_info = (*node_region_map[node])[type];

            if(current_info.begin_index == current_info.end_index)
            {
               current_info = *child_info;
            } else {
               if(child_info->begin_index < current_info.begin_index)
                  current_info.begin_index = child_info->begin_index;
               if(child_info->end_index > current_info.end_index)
                  current_info.end_index = child_info->end_index;
            }
         }
         (*current_region_map)[type] = current_info;
      }
   }
	// update num descendants
	if(node->get_parent() != NULL)
	{
		SgNode *parent = node->get_parent();
		DepthFirstIndexAttribute *parent_att = (DepthFirstIndexAttribute*)(parent->getAttribute("depth-first-index"));
		parent_att->num_descendants += 1 + att->num_descendants;
	}
}

void NodeFinder::rebuildIndex_helper_alt(SgNode *node)
{
	ROSE_ASSERT(node != NULL);

	// add depth first indexing attribute
	DepthFirstIndexAttribute *att;
	if(node->attributeExists("depth-first-index"))
	{
		att = (DepthFirstIndexAttribute*)(node->getAttribute("depth-first-index"));
	} else {
		att = new DepthFirstIndexAttribute();
		node->addNewAttribute("depth-first-index", att);
	}
	att->df_index = current_df_index++; // intentionally post increment
	att->num_descendants = 0;

	// add node to the corresponding type vector
	std::vector<SgNode*> *current_list;
   if(node_map.count(node->variantT()) > 0)
   {
      current_list = node_map[node->variantT()];
   } else {
      current_list = new std::vector<SgNode*>();
      node_map_allocations.push_back(current_list);
      node_map[node->variantT()] = current_list;
   }
   current_list->push_back(node);

   // traverse children
   for(uint i = 0; i < node->get_numberOfTraversalSuccessors(); i++)
   {
      SgNode *child = node->get_traversalSuccessorByIndex(i);
      if(child == NULL) continue;

		// recursive call
		rebuildIndex_helper_alt(child);
	}

	// update num descendants
	if(node->get_parent() != NULL)
	{
		SgNode *parent = node->get_parent();
		DepthFirstIndexAttribute *parent_att = (DepthFirstIndexAttribute*)(parent->getAttribute("depth-first-index"));
		parent_att->num_descendants += 1 + att->num_descendants;
	}
}
