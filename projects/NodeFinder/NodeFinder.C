/*
 * NodeFinder.C
 *
 *  Created on: Oct 8, 2013
 *      Author: Sam Kelly <kellys@dickinson.edu>
 */
#include <NodeFinder.h>
#include <AstMatching.h>

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
   //std::cout << "begin index: " << begin_index << std::endl;
   //std::cout << "end index: " << end_index << std::endl;
   return NodeFinderResult(node_map[search_type], begin_index, end_index);
}

void NodeFinder::rebuildIndex()
{
   rebuildIndex(index_root);
}

void NodeFinder::rebuildIndex(SgNode *index_root)
{
   //std::cout << "Traversing AST and generating data structures..." << std::endl;
   this->index_root = index_root;
   node_region_map.clear();
   node_map.clear();
   rebuildIndex_helper(index_root);
   node_contained_types.clear(); // don't need node_contained_types to perform searches

}

void NodeFinder::rebuildIndex_helper(SgNode *node)
{
   ROSE_ASSERT(node != NULL);

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

   // traverse children
   for(int i = 0; i < node->get_numberOfTraversalSuccessors(); i++)
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

   NodeFinder finder = NodeFinder(root_node);


   // test normal nested complex results

   NodeFinderResult var_dec_res_1 = finder.find(root_node, V_SgVariableDeclaration);
   ROSE_ASSERT(var_dec_res_1.size() == 16);

   NodeFinderResult if_res_1 = finder.find(root_node, V_SgIfStmt);
   ROSE_ASSERT(if_res_1.size() == 16);

   NodeFinderResult for_res_1 = finder.find(root_node, V_SgForStatement);
   ROSE_ASSERT(for_res_1.size() == 4);


   root_node = if_res_1[0];

   NodeFinderResult var_dec_res_2 = finder.find(root_node, V_SgVariableDeclaration);
   ROSE_ASSERT(var_dec_res_2.size() == 15);

   NodeFinderResult if_res_2 = finder.find(root_node, V_SgIfStmt);
   ROSE_ASSERT(if_res_2.size() == 15);

   NodeFinderResult for_res_2 = finder.find(root_node, V_SgForStatement);
   ROSE_ASSERT(for_res_2.size() == 4);


   root_node = if_res_2[0];

   NodeFinderResult var_dec_res_3 = finder.find(root_node, V_SgVariableDeclaration);
   ROSE_ASSERT(var_dec_res_3.size() == 14);

   NodeFinderResult if_res_3 = finder.find(root_node, V_SgVariableDeclaration);
   ROSE_ASSERT(if_res_3.size() == 14);

   NodeFinderResult for_res_3 = finder.find(root_node, V_SgForStatement);
   ROSE_ASSERT(for_res_3.size() == 4);


   root_node = if_res_1[4];

   NodeFinderResult if_res_4 = finder.find(root_node, V_SgIfStmt);
   ROSE_ASSERT(if_res_4.size() == 11);

   NodeFinderResult var_dec_res_4 = finder.find(root_node, V_SgVariableDeclaration);
   ROSE_ASSERT(var_dec_res_4.size() == 11);

   NodeFinderResult for_res_4 = finder.find(root_node, V_SgForStatement);
   ROSE_ASSERT(for_res_4.size() == 4);


   root_node = if_res_4[0];

   NodeFinderResult if_res_5 = finder.find(root_node, V_SgIfStmt);
   ROSE_ASSERT(if_res_5.size() == 10);

   NodeFinderResult var_dec_res_5 = finder.find(root_node, V_SgVariableDeclaration);
   ROSE_ASSERT(var_dec_res_5.size() == 9);


   root_node = if_res_5[0];

   NodeFinderResult if_res_6 = finder.find(root_node, V_SgIfStmt);
   ROSE_ASSERT(if_res_6.size() == 8);

   NodeFinderResult var_dec_res_6 = finder.find(root_node, V_SgVariableDeclaration);
   ROSE_ASSERT(var_dec_res_6.size() == 9);

   NodeFinderResult for_res_5 = finder.find(root_node, V_SgForStatement);
   ROSE_ASSERT(for_res_5.size() == 4);


   root_node = for_res_5[0];

   NodeFinderResult if_res_7 = finder.find(root_node, V_SgIfStmt);
   ROSE_ASSERT(if_res_7.size() == 7);

   NodeFinderResult var_dec_res_7 = finder.find(root_node, V_SgVariableDeclaration);
   ROSE_ASSERT(var_dec_res_7.size() == 9);

   NodeFinderResult for_res_6 = finder.find(root_node, V_SgForStatement);
   ROSE_ASSERT(for_res_6.size() == 3);


   root_node = for_res_6[0];

   NodeFinderResult for_res_7 = finder.find(root_node, V_SgForStatement);
   ROSE_ASSERT(for_res_7.size() == 2);

   NodeFinderResult var_dec_res_8 = finder.find(root_node, V_SgVariableDeclaration);
   ROSE_ASSERT(var_dec_res_8.size() == 8);

   NodeFinderResult if_res_8 = finder.find(root_node, V_SgIfStmt);
   ROSE_ASSERT(if_res_8.size() == 6);


   root_node = for_res_7[1];

   NodeFinderResult for_res_8 = finder.find(root_node, V_SgForStatement);
   ROSE_ASSERT(for_res_8.size() == 0);

   NodeFinderResult var_dec_res_9 = finder.find(root_node, V_SgVariableDeclaration);
   ROSE_ASSERT(var_dec_res_9.size() == 4);

   NodeFinderResult if_res_9 = finder.find(root_node, V_SgIfStmt);
   ROSE_ASSERT(if_res_9.size() == 4);


   // test empty results

   NodeFinderResult var_dec_res_10 = finder.find(var_dec_res_9[0], V_SgVariableDeclaration);
   ROSE_ASSERT(var_dec_res_10.size() == 0);

   NodeFinderResult namespace_res = finder.find(if_res_1[0], V_SgNamespaceDeclarationStatement);
   ROSE_ASSERT(namespace_res.size() == 0);

   std::cout << "All NodeFinder tests have passed!" << std::endl;

   root_node = (SgNode*)project;
   AstMatching matcher;
   MatchResult matching_res = matcher.performMatching("$f=SgVariableDeclaration", root_node);

}
