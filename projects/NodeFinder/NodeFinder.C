#include <NodeFinder.h>



NodeFinder::NodeFinder(SgNode *index_root)
{
   this->index_root = index_root;
   //node_map = new boost::unordered_map<VariantT, std::vector<SgNode*>>();
   //node_region_map = new boost::unordered_map<SgNode*, boost::unordered_map<VariantT, region_info>>();
   rebuildIndex(index_root);
}

NodeFinder::~NodeFinder(){}

void NodeFinder::rebuildIndex()
{
   rebuildIndex(index_root);
}

void NodeFinder::rebuildIndex_helper(SgNode *node, int dfs_index)
{
   // add node to the corresponding vector
   std::vector<SgNode*> *current_list = &node_map[node->variantT()];
   if(current_list == NULL)
   {
      current_list = &std::vector<SgNode*>();
      node_map[node->variantT()] = *current_list; // ? do we want to dereference this ?
   }
   current_list->push_back(node);

   // get reference to proper region map
   boost::unordered_map<VariantT, region_info> current_region_map;
   node_region_map[node] = current_region_map; // ? do we want to dereference this ?


   // if leaf
   if(node->get_numberOfTraversalSuccessors() == 0)
   {
      // create region info
      region_info info;
      info.begin_index = current_list->size() - 1;
      info.end_index = info.begin_index + 1;
      current_region_map[node->variantT()] = info;
      return;
   }

   // if internal node
   for(int i = 0; i < node->get_numberOfTraversalSuccessors(); i++)
   {
      // traverse children
      SgNode *child = node->get_traversalSuccessorByIndex(i);
      rebuildIndex_helper(child, ++dfs_index);
   }
}

/*
stack.push(root)
while !stack.isEmpty() do
    node = stack.pop()
    for each node.childNodes do
        stack.push(stack)
    endfor
    // …
endwhile
 */
void NodeFinder::rebuildIndex(SgNode *index_root)
{
   /*
   this->index_root = index_root;

   std::stack<SgNode*> node_stack;
   boost::unordered_map<VariantT, std::stack<int>> region_start_stacks;

   // perform depth first search of AST starting at index_root
   node_stack.push(index_root);

   while(!node_stack.empty())
   {
      SgNode *node = node_stack.pop();
      std::vector<SgNode*> *current_list = node_map[node->variantT()];
      if(current_list == NULL)
      {
         current_list = new std::vector<SgNode*>();
         node_map[node->variantT()] = current_list;
      }
      current_list->push_back(node);

      for(int i = 0; i < node->get_numberOfTraversalSuccessors(); i++)
      {
         SgNode *child = node->get_traversalSuccessorByIndex(i);
         node_stack.push(child);

      }

   }
   */
}

int main()
{
   NodeFinder nf(NULL);
   std::cout << "yo" << std::endl;
}
