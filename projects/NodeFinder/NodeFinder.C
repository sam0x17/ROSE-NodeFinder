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
      current_list = &node_map[node->variantT()];
   } else {
      current_list = new std::vector<SgNode*>();
      node_map[node->variantT()] = *current_list; // ? memory leak ?
   }
   current_list->push_back(node);

   // create region map for this node
   boost::unordered_map<VariantT, region_info> *current_region_map = new boost::unordered_map<VariantT, region_info>();
   node_region_map[node] = *current_region_map; // ? memory leak ?

   // store dfs index
   //dfs_index_map[node] = dfs_index;

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

   // store the value of dfs_index for the current node
   //int node_dfs_index = dfs_index;

   // create contained types set for this node
   boost::unordered_set<VariantT> *current_contained_types = new boost::unordered_set<VariantT>();
   node_contained_types[node] = *current_contained_types; // <----- ? is this a memory leak ?

   // if internal node
   for(int i = 0; i < node->get_numberOfTraversalSuccessors(); i++)
   {
      // traverse children
      SgNode *child = node->get_traversalSuccessorByIndex(i);

      //rebuildIndex_helper(child, ++dfs_index); // RECURSIVE CALL
      rebuildIndex_helper(child);

      boost::unordered_map<VariantT, region_info> *current_child_region_map;
      current_child_region_map = &node_region_map[child];

      // bubble up contained types and node info
      current_contained_types->insert(child->variantT());
      BOOST_FOREACH(VariantT type, node_contained_types[child])
      {
         region_info child_info = (*current_child_region_map)[type];
         region_info current_info;
         if(current_contained_types->find(type) == current_contained_types->end())
         {
            current_info.begin_index = child_info.begin_index;
            current_info.end_index = child_info.end_index;
            current_contained_types->insert(type);
         } else {
            // merge child region info
            current_info = node_region_map[node][type];
            if(child_info.begin_index < current_info.begin_index)
               current_info.begin_index = child_info.begin_index;
            if(child_info.end_index > current_info.end_index)
               current_info.end_index = child_info.end_index;
         }
         (*current_region_map)[type] = current_info;
      }
   }
}

int main()
{
   NodeFinder nf(NULL);
   std::cout << "yo" << std::endl;
}
