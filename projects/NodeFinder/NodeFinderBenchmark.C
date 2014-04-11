/*
 * NodeFinderBenchmark.C
 *
 * Runs a series of benchmarks comparing the
 * performance of NodeFinder to that of ROSE's
 * built-in AST Matching functionality.
 *
 *  Created on: Nov 6, 2013
 *      Author: Sam Kelly <kellys@dickinson.edu>
 */

// 10, 8, 900

#define NDEBUG // disable debugging to increase performance
#include <NodeFinder.h>
#include <AstMatching.h>
#include <boost/algorithm/string/predicate.hpp>
#include <time.h>
#include <iostream>
#include <string>
#include <stdlib.h> 

enum BenchmarkType
{
   WARMUP,
   INDEX_BUILDING,
   ROOT_LEVEL_QUERY,
   ROOT_LEVEL_QUERY_ITERATE,
   NESTED_QUERY,
   TRIPLE_NESTED_QUERY
};

enum BenchmarkAlgorithm
{
   AST_MATCHING,
   ALGORITHM_A,
   ALGORITHM_B
};

SgProject *project;
SgNode *root_node;
SgNode *old_root;
clock_t dest_elapsed;
NodeFinder finder;
AstMatching matcher;

// returns a vector containing node and all descendants of node
void getNodes(SgNode *node, std::vector<SgNode *> *nodes)
{
   nodes->push_back(node);
   for(uint i = 0; i < node->get_numberOfTraversalSuccessors(); i++)
   {
      SgNode *child = node->get_traversalSuccessorByIndex(i);
      if(child != NULL)
         getNodes(child, nodes);
   }
}

// performs binary search on an array of ints and returns the
// insertion index (finds index of closest element to key)
int binarySearchClosest(std::vector<int> data, int key, int imin, int imax)
{
   while (imin < imax)
   {
      int imid = (imax + imin) / 2;
      if (data[imid] < key)
         imin = imid + 1;
      else
         imax = imid;
   }
   if(imin > 0 && abs(key - data[imin - 1]) < abs(key - data[imin]))
      return imin - 1;
   return imin;
}

long benchmark_portion(BenchmarkType type, BenchmarkAlgorithm algorithm)
{
   long iterations;
   SgVarRefExp *var;
   if(type == NESTED_QUERY || type == TRIPLE_NESTED_QUERY)
      finder.rebuildIndex(old_root, algorithm == ALGORITHM_B);
   else finder.rebuildIndex(root_node, algorithm == ALGORITHM_B);
   clock_t begin = clock();
   for(iterations = 1;; iterations++)
   {
      switch(type)
      {
         case WARMUP:
            matcher.performMatching("$v=SgVarRefExp", root_node);
		      finder.find(root_node, V_SgVarRefExp);
            break;
         case INDEX_BUILDING:
            switch(algorithm)
            {
               case AST_MATCHING:
                  matcher.performMatching("$v=SgVarRefExp", root_node);
                  break;
               case ALGORITHM_A:
                  finder.rebuildIndex(root_node, false);
                  break;
               case ALGORITHM_B:
                  finder.rebuildIndex(root_node, true);
                  break;
            }
            break;
         case ROOT_LEVEL_QUERY:
            switch(algorithm)
            {
               case AST_MATCHING:
                  matcher.performMatching("$v=SgVarRefExp", root_node);
                  break;
               case ALGORITHM_A:
               case ALGORITHM_B:
                  finder.find(root_node, V_SgVarRefExp);
                  break;
            }
            break;
         case ROOT_LEVEL_QUERY_ITERATE:
            switch(algorithm)
            {
               case AST_MATCHING:
               {
                  MatchResult matches = matcher.performMatching("$v=SgVarRefExp", root_node);
                  BOOST_FOREACH(SingleMatchVarBindings match, matches)
                  {
                     var = (SgVarRefExp *)match["$v"];
                  }
                  break;
               }
               case ALGORITHM_A:
               case ALGORITHM_B:
               {
                  NodeFinderResult res = finder.find(root_node, V_SgVarRefExp);
                  BOOST_FOREACH(SgNode *node, res)
                  {
                     var = (SgVarRefExp *)node;
                  }
                  break;
               }
            }
            break;
         case NESTED_QUERY:
            switch(algorithm)
            {
               case AST_MATCHING:
               {
		            // for each if statement, iterate over all variable references
                  MatchResult matches = matcher.performMatching("$b=SgBasicBlock", root_node);
                  BOOST_FOREACH(SingleMatchVarBindings match, matches)
                  {
			            SgBasicBlock *basic_block = (SgBasicBlock *)match["$b"];
			            AstMatching matcher2;
			            MatchResult matches2 = matcher2.performMatching("$v=SgVarRefExp", basic_block);
			            BOOST_FOREACH(SingleMatchVarBindings match2, matches2)
			            {
			            	var = (SgVarRefExp *)match2["$v"];
			            }
                  }
                  break;
               }
               case ALGORITHM_A:
               case ALGORITHM_B:
               {

		            // for each if statement, iterate over all variable references
		            NodeFinderResult res1 = finder.find(root_node, V_SgBasicBlock);
                  BOOST_FOREACH(SgNode *basic_block, res1)
                  {
		            	NodeFinderResult res2 = finder.find(basic_block, V_SgVarRefExp);
		            	BOOST_FOREACH(SgNode *var_ref, res2)
		            	{
				            var = (SgVarRefExp *)var_ref;
			            }
                  }
                  break;
               }
            }
            break;
         case TRIPLE_NESTED_QUERY:
            switch(algorithm)
            {
               case AST_MATCHING:
               {
		            // for each function definition, iterate over all if statements
                  // then for each if statement, iterate over all variable references
                  MatchResult matches = matcher.performMatching("$b=SgBasicBlock", root_node);
                  BOOST_FOREACH(SingleMatchVarBindings match, matches)
                  {
			            SgFunctionDefinition *basic_block = (SgFunctionDefinition *)match["$b"];
			            AstMatching matcher2;
			            MatchResult matches2 = matcher2.performMatching("$i=SgIfStmt", basic_block);
			            BOOST_FOREACH(SingleMatchVarBindings match2, matches2)
			            {
				            SgIfStmt *if_stmt = (SgIfStmt *)match2["$i"];
				            AstMatching matcher3;
				            MatchResult matches3 = matcher3.performMatching("$v=SgVarRefExp", if_stmt);
				            BOOST_FOREACH(SingleMatchVarBindings match3, matches3)
				            {
					            var = (SgVarRefExp *)match3["$v"];
				            }
			            }
                  }
                  break;
               }
               case ALGORITHM_A:
               case ALGORITHM_B:
               {
		            // for each function definition, iterate over all if statements
                  // then for each if statement, iterate over all variable references
		            NodeFinderResult res1 = finder.find(root_node, V_SgBasicBlock);
                  BOOST_FOREACH(SgNode *basic_block, res1)
                  {
			            NodeFinderResult res2 = finder.find(basic_block, V_SgIfStmt);
			            BOOST_FOREACH(SgNode *node2, res2)
			            {
				            SgIfStmt *if_stmt = (SgIfStmt *)node2;
				            NodeFinderResult res3 = finder.find(if_stmt, V_SgVarRefExp);
				            BOOST_FOREACH(SgNode *var_ref, res3)
				            {
				            	var = (SgVarRefExp *)var_ref;
				            }
			            }
                  }
                  break;
               }
            }
            break;
      }
      if(clock() - begin >= dest_elapsed) break;
   }
   return iterations;
}

int main(int argc, char** argv)
{
   std::string source_file;
   bool found_source_file = false;
   for(int i = 0; i < argc; i++)
   {
      source_file = argv[i];
      if(boost::ends_with(source_file, ".C") ||
			boost::ends_with(source_file, ".cpp") ||
			boost::ends_with(source_file, ".c" ) ||
			boost::ends_with(source_file, ".h"))
      {
         std::cout << "Loading AST from: " << source_file << std::endl;
         found_source_file = true;
         break;
      }
   }
   if(!found_source_file)
   {
      std::cout << "Error: no valid source file was specified (must end in .h, .c, .C, or .cpp)!" << std::endl;
      return 1;
   }

   project = frontend(argc, argv);
   root_node = (SgNode*)project;
   std::cout << "building preliminary index... ";
   finder.rebuildIndex(root_node);
   std::cout << "[DONE]" << std::endl;
   int total_nodes = finder.getNumDescendants(root_node) + 1;
   std::cout << std::endl << "AST loaded successfully (" << total_nodes << " total nodes indexed)" << std::endl << std::endl;

   std::cout << "Enter the number of CPU seconds each algorithm should run for during each trial, followed by the number of data points we should generate for each algorithm. The AST will be broken up into progressively larger sections that will be benchmarked seperately based on the number of data points you specify." << std::endl << std::endl;
   std::cout << "Num CPU seconds: ";
   double seconds;
   std::cin >> seconds;
   dest_elapsed = (clock_t)(seconds * CLOCKS_PER_SEC);

   std::cout << "Num data points: ";
   int num_datapoints;
   std::cin >> num_datapoints;

   std::cout << "Total nodes: ";
   int artificial_total;
   std::cin >> artificial_total;

   std::cout << std::endl << "Finding appropriate AST subtrees..." << std::endl;
   std::vector<int> subtree_sizes;
   std::vector<SgNode *> subtrees;
   boost::unordered_map<int, SgNode*> size_map;
   getNodes(root_node, &subtrees);
   for(uint i = 0; i < subtrees.size(); i++)
   {
      SgNode *node = subtrees[i];
      int descendants = finder.getNumDescendants(node);
      if(size_map.find(descendants) == size_map.end())
      {
         size_map[descendants] = node;
         subtree_sizes.push_back(descendants);
      }
   }
   std::sort(subtree_sizes.begin(), subtree_sizes.end());
   int increment = artificial_total / num_datapoints;
   std::vector<SgNode *> final_nodes;
   std::vector<int> final_sizes;
   boost::unordered_set<int> duplicate_prevention;
   for(int i = 1; i <= num_datapoints; i++)
   {
      int desired = i * increment;
      int closest_index = binarySearchClosest(subtree_sizes, desired, 0, subtree_sizes.size() - 1);
      int closest_size = subtree_sizes[closest_index];
      if(duplicate_prevention.find(closest_size) == duplicate_prevention.end())
      {
         final_nodes.push_back(size_map[closest_size]);
         final_sizes.push_back(closest_size);
         duplicate_prevention.insert(closest_size);
         std::cout << desired << "\t=>\t" << closest_size << "\t(off by " << abs(closest_size - desired) << ")" << std::endl;
      } else {
         std::cout << desired << "\t=>\t" << closest_size << "\t(skipped duplicate)" << std::endl;
      }
   }
   std::cout << std::endl << "Benchmarks will run for " << seconds << " CPU seconds (" << dest_elapsed << " CPU cycles) each" << std::endl;
   std::cout << "Results will be printed in terms of the # of iterations each algorithm was able to run within the given time frame. (higher means faster)" << std::endl;

   old_root = root_node;
   
   std::cout << std::endl << "Warming up... " << std::flush;
   benchmark_portion(WARMUP, ALGORITHM_A);
   std::cout << "[OK]" << std::endl << std::flush;

   std::cout << std::endl << "Running index building benchmark..." << std::endl;
   std::cout << "Nodes\tAST-M\tALG-A\tALG-B" << std::endl;
   for(uint i = 0; i < final_nodes.size(); i++)
   {
      root_node = final_nodes[i];
      std::cout << final_sizes[i] << "\t" << std::flush;
      std::cout << benchmark_portion(INDEX_BUILDING, AST_MATCHING) << "\t" << std::flush;
      std::cout << benchmark_portion(INDEX_BUILDING, ALGORITHM_A) << "\t" << std::flush;
      std::cout << benchmark_portion(INDEX_BUILDING, ALGORITHM_B) << std::endl << std::flush;
   }

   std::cout << std::endl << "Running root level query benchmark..." << std::endl;
   std::cout << "Nodes\tAST-M\tALG-A\tALG-B" << std::endl;
   for(uint i = 0; i < final_nodes.size(); i++)
   {
      root_node = final_nodes[i];
      std::cout << final_sizes[i] << "\t" << std::flush;
      std::cout << benchmark_portion(ROOT_LEVEL_QUERY, AST_MATCHING) << "\t" << std::flush;
      std::cout << benchmark_portion(ROOT_LEVEL_QUERY, ALGORITHM_A) << "\t" << std::flush;
      std::cout << benchmark_portion(ROOT_LEVEL_QUERY, ALGORITHM_B) << std::endl << std::flush;
   }

   std::cout << std::endl << "Running root level query w/iteration over results benchmark..." << std::endl;
   std::cout << "Nodes\tAST-M\tALG-A\tALG-B" << std::endl;
   for(uint i = 0; i < final_nodes.size(); i++)
   {
      root_node = final_nodes[i];
      std::cout << final_sizes[i] << "\t" << std::flush;
      std::cout << benchmark_portion(ROOT_LEVEL_QUERY_ITERATE, AST_MATCHING) << "\t" << std::flush;
      std::cout << benchmark_portion(ROOT_LEVEL_QUERY_ITERATE, ALGORITHM_A) << "\t" << std::flush;
      std::cout << benchmark_portion(ROOT_LEVEL_QUERY_ITERATE, ALGORITHM_B) << std::endl << std::flush;
   }

   std::cout << std::endl << "Running nested query benchmark..." << std::endl;
   std::cout << "Nodes\tAST-M\tALG-A\tALG-B" << std::endl;
   for(uint i = 0; i < final_nodes.size(); i++)
   {
      root_node = final_nodes[i];
      std::cout << final_sizes[i] << "\t" << std::flush;
      std::cout << benchmark_portion(NESTED_QUERY, AST_MATCHING) << "\t" << std::flush;
      std::cout << benchmark_portion(NESTED_QUERY, ALGORITHM_A) << "\t" << std::flush;
      std::cout << benchmark_portion(NESTED_QUERY, ALGORITHM_B) << std::endl << std::flush;
   }

   std::cout << std::endl << "Running triple-nested query benchmark..." << std::endl;
   std::cout << "Nodes\tAST-M\tALG-A\tALG-B" << std::endl;
   for(uint i = 0; i < final_nodes.size(); i++)
   {
      root_node = final_nodes[i];
      std::cout << final_sizes[i] << "\t" << std::flush;
      std::cout << benchmark_portion(TRIPLE_NESTED_QUERY, AST_MATCHING) << "\t" << std::flush;
      std::cout << benchmark_portion(TRIPLE_NESTED_QUERY, ALGORITHM_A) << "\t" << std::flush;
      std::cout << benchmark_portion(TRIPLE_NESTED_QUERY, ALGORITHM_B) << std::endl << std::flush;
   }

   finder.dispose();
}
