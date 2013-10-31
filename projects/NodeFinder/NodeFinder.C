/*
 * NodeFinder.C
 *
 *  Created on: Oct 8, 2013
 *      Author: Sam Kelly <kellys@dickinson.edu>
 */
#include <NodeFinder.h>

NodeFinder::NodeFinder(SgNode *index_root)
{
   this->index_root = index_root;
   rebuildIndex(index_root);
}

NodeFinderResult NodeFinder::find(SgNode *search_root, VariantT search_type)
{
   ROSE_ASSERT(search_root != NULL);
   boost::unordered_map<VariantT, region_info> *relevant_info = node_region_map[search_root];
   region_info *info = &((*relevant_info)[search_type]);
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
   std::cout << "begin index: " << begin_index << std::endl;
   std::cout << "end index: " << end_index << std::endl;
   return NodeFinderResult(node_map[search_type], begin_index, end_index);
}

void NodeFinder::rebuildIndex()
{
   rebuildIndex(index_root);
}

void NodeFinder::rebuildIndex(SgNode *index_root)
{
   std::cout << "Traversing AST and generating data structures..." << std::endl;
   this->index_root = index_root;
   node_region_map.clear();
   node_map.clear();
   rebuildIndex_helper(index_root);
   node_contained_types.clear(); // don't need node_contained_types to perform searches

}

void NodeFinder::rebuildIndex_helper(SgNode *node)
{
   ROSE_ASSERT(node != NULL);
   //for(int i = 0; i < depth; i++) std::cout << "\t";
   //std::cout << node->sage_class_name() << std::endl;

   // add node to the corresponding vector
   std::vector<SgNode*> *current_list;
   if(node_map.count(node->variantT()) > 0)
   {
      current_list = node_map[node->variantT()];
   } else {
      current_list = new std::vector<SgNode*>();
      node_map[node->variantT()] = current_list;
   }
   current_list->push_back(node);

   // setup region map for this node
   boost::unordered_map<VariantT, region_info> *current_region_map;
   //all_nodes.push_back(node);
   current_region_map = new boost::unordered_map<VariantT, region_info>();
   node_region_map[node] = current_region_map;

   // create contained types set for this node
   boost::unordered_set<VariantT> *current_contained_types = new boost::unordered_set<VariantT>();
   node_contained_types[node] = current_contained_types;

   region_info info;
   info.begin_index = current_list->size() - 1;
   info.end_index = info.begin_index + 1;
   (*current_region_map)[node->variantT()] = info;
   current_contained_types->insert(node->variantT());
   region_info *current_info = &(*current_region_map)[node->variantT()];
   current_info->end_index++;

   // if internal node
   for(int i = 0; i < node->get_numberOfTraversalSuccessors(); i++)
   {
      // traverse children
      SgNode *child = node->get_traversalSuccessorByIndex(i);
      if(child == NULL)
      {
         continue;
      }

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
               //std::cout << "it happened" << std::endl;
               current_info = *child_info;
            } else {
               if(child_info->begin_index < current_info.begin_index)
                  current_info.begin_index = child_info->begin_index;
               if(child_info->end_index > current_info.end_index)
                  current_info.end_index = child_info->end_index;
               //std::cout << "merging: (" << current_info.begin_index << ", " << current_info.end_index << ") + (" << child_info->begin_index << ", " << child_info->end_index << ") = (" << current_info.begin_index << ", " << current_info.end_index << ")" << std::endl;
            }
         }
         (*current_region_map)[type] = current_info;
      }
   }

}

int main(int argc, char** argv)
{
   std::cout << "Loading AST into ROSE..." << std::endl;
   // load specified source file(s) into ROSE and get the root SgNode
   SgProject *project = frontend(argc, argv);
   SgNode *root_node = (SgNode*)project;

   std::cout << "Initializing NodeFinder..." << std::endl;
   NodeFinder finder(root_node);
   std::cout << "Done initializing." << std::endl;

   VariantT node_type = V_SgVarRefExp;

   NodeFinderResult res1 = finder.find(root_node, V_SgReturnStmt);
   NodeFinderResult res2 = finder.find(res1[1], node_type);

   std::cout << "first search size: " << res1.size() << std::endl;
   for(int i = 0; i < res1.size(); i++)
   {
      std::cout << "result #" << i + 1 << ":" << res1[i] << " " << res1[i]->sage_class_name() << std::endl;
   }
   std::cout << "second search size: " << res2.size() << std::endl;
   for(int i = 0; i < res2.size(); i++)
   {
      std::cout << "result #" << i + 1 << ":" << res2[i] << " " << res2[i]->sage_class_name() << std::endl;
   }
   BOOST_FOREACH(SgNode *node, res2)
   {
      std::cout << node->sage_class_name() << std::endl;
   }

   /*// test blank results -- using pointer to res2 because we need something non null
   NodeFinderResult blank = NodeFinderResult((std::vector<SgNode*> *)(&res1), 0, 0);
   BOOST_FOREACH(SgNode *node, blank)
   {
      std::cout << "shouldn't see this" << std::endl;
   }
   std::cout << "NodeFinder tests done." << std::endl;*/
}
