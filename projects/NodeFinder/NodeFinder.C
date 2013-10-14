#include <NodeFinder.h>



NodeFinder::NodeFinder(SgNode *index_root)
{
   this->index_root = index_root;
   rebuildIndex(index_root);
}

NodeFinder::~NodeFinder(){}

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
   rebuildIndex_helper(index_root, 0);
   node_contained_types.clear(); // don't need node_contained_types to perform searches
}

void NodeFinder::rebuildIndex_helper(SgNode *node, int depth)
{
   ROSE_ASSERT(node != NULL);

   /*for(int i = 0; i < depth; i++) std::cout << "\t";
   std::cout << node->sage_class_name() << std::endl;*/

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

   // create region map for this node
   boost::unordered_map<VariantT, region_info> *current_region_map = new boost::unordered_map<VariantT, region_info>();
   node_region_map[node] = current_region_map;

   // if leaf
   if(node->get_numberOfTraversalSuccessors() == 0)
   {
      // create region info
      region_info info;
      info.begin_index = current_list->size() - 1;
      info.end_index = info.begin_index + 1;
      (*current_region_map)[node->variantT()] = info;
      return;
   }

   // create contained types set for this node
   boost::unordered_set<VariantT> *current_contained_types = new boost::unordered_set<VariantT>();
   node_contained_types[node] = current_contained_types;

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
      rebuildIndex_helper(child, depth + 1);

      boost::unordered_map<VariantT, region_info> *current_child_region_map;
      current_child_region_map = node_region_map[child];

      // bubble up contained types and node info
      current_contained_types->insert(child->variantT());
      if(node_contained_types.find(child) == node_contained_types.end())
      {
         //std::cout << "ATTEMPTING TO FIX PROBLEMATIC NODE!" << std::endl;
         rebuildIndex_helper(child, depth + 1);
         continue;
      }
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
            if(child_info->begin_index < current_info.begin_index)
               current_info.begin_index = child_info->begin_index;
            if(child_info->end_index > current_info.end_index)
               current_info.end_index = child_info->end_index;
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
   std::cout << "Done." << std::endl;
}
