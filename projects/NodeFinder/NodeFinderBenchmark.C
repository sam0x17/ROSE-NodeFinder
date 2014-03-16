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

#define NDEBUG // disable debugging to increase performance
#include <NodeFinder.h>
#include <AstMatching.h>
#include <boost/algorithm/string/predicate.hpp>
#include <time.h>

int main(int argc, char** argv)
{
	ROSE_ASSERT(false);
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
      std::cout << "Error: no source file was specified (must end in .h, .c, .C, or .cpp)!" << std::endl;
      return 1;
   }

   SgProject *project = frontend(argc, argv);
   SgNode *root_node = (SgNode*)project;

   std::cout << "AST loaded successfully." << std::endl;

   NodeFinder finder(root_node);
	std::cout << "Total Nodes: " << finder.getTotalNodes() << std::endl;
   AstMatching matcher;

   double minutes = 10.0/60.0;
   clock_t begin;
   clock_t dest_clock = (clock_t)(minutes * 60 * CLOCKS_PER_SEC);

   std::cout << std::endl << "Benchmarks will run for " << (int)(minutes * 60) << " CPU seconds (" << dest_clock << " CPU cycles) each" << std::endl;
   std::cout << "Results will be printed in terms of the # of iterations each algorithm was able to run within the given time frame. (higher means faster)" << std::endl;

   long iterations;
	SgVarRefExp *var;

	std::cout << std::endl << "warming up... " << std::flush;
	// ensures that AST is properly cached and process has consistent
	// process priority by the time we begin timing
   begin = clock();
   for(iterations = 0;; iterations++)
   {
      matcher.performMatching("$v=SgVarRefExp", root_node);
		finder.find(root_node, V_SgVarRefExp);
      if(clock() - begin >= dest_clock) break;
   }
	std::cout << "[DONE]" << std::endl;

   std::cout << std::endl << "Comparing index building (traversal) speed..." << std::endl << std::endl;

   std::cout << "AST Matching\tNodeFinder A\tNodeFinder B" << std::endl;

   begin = clock();
   for(iterations = 0;; iterations++)
   {
      matcher.performMatching("$v=SgVarRefExp", root_node);
      if(clock() - begin >= dest_clock) break;
   }

   std::cout << iterations << std::flush << "\t\t";

   begin = clock();
   for(iterations = 0;; iterations++)
   {
      finder.rebuildIndex(root_node, false);
      if(clock() - begin >= dest_clock) break;
   }

   std::cout << iterations << std::flush << "\t\t";

   begin = clock();
   for(iterations = 0;; iterations++)
   {
      finder.rebuildIndex(root_node, true);
      if(clock() - begin >= dest_clock) break;
   }
   std::cout << iterations << std::flush << std::endl;

   std::cout << std::endl << std::endl << "Comparing root level query speed..." << std::endl << std::endl;
   std::cout << "AST Matching\tNodeFinder A\tNodeFinder B" << std::flush << std::endl;

   begin = clock();
   for(iterations = 0;; iterations++)
   {
      matcher.performMatching("$v=SgVarRefExp", root_node);
      if(clock() - begin >= dest_clock) break;
   }
   std::cout << iterations << "\t\t" << std::flush;

	finder.rebuildIndex(root_node, false);
   begin = clock();
   for(iterations = 0;; iterations++)
   {
      finder.find(root_node, V_SgVarRefExp);
      if(clock() - begin >= dest_clock) break;
   }
   std::cout << iterations << "\t" << std::flush;

	finder.rebuildIndex(root_node, true);
   begin = clock();
   for(iterations = 0;; iterations++)
   {
      finder.find(root_node, V_SgVarRefExp);
      if(clock() - begin >= dest_clock) break;
   }
   std::cout << iterations << std::flush << std::endl;


   std::cout << std::endl << std::endl << "Comparing iterating over a root level query speed..." << std::endl << std::endl;
   std::cout << "AST Matching\tNodeFinder A\tNodeFinder B" << std::flush << std::endl;

   begin = clock();
   for(iterations = 0;; iterations++)
   {
      MatchResult matches = matcher.performMatching("$v=SgVarRefExp", root_node);
      BOOST_FOREACH(SingleMatchVarBindings match, matches)
      {
         var = (SgVarRefExp *)match["$v"];
      }
      if(clock() - begin >= dest_clock) break;
   }
   std::cout << iterations << "\t\t" << std::flush;

   begin = clock();
   finder.rebuildIndex(root_node, false);
   for(iterations = 0;; iterations++)
   {
      NodeFinderResult res = finder.find(root_node, V_SgVarRefExp);
      BOOST_FOREACH(SgNode *node, res)
      {
         var = (SgVarRefExp *)node;
      }
      if(clock() - begin >= dest_clock) break;
   }
   std::cout << iterations << "\t" << std::flush;

   begin = clock();
   finder.rebuildIndex(root_node, true);
   for(iterations = 0;; iterations++)
   {
      NodeFinderResult res = finder.find(root_node, V_SgVarRefExp);
      BOOST_FOREACH(SgNode *node, res)
      {
         var = (SgVarRefExp *)node;
      }
      if(clock() - begin >= dest_clock) break;
   }
   std::cout << iterations << std::flush << std::endl;


   std::cout << std::endl << std::endl << "Comparing iterating over a nested query speed..." << std::endl << std::endl;
   std::cout << "AST Matching\tNodeFinder A\tNodeFinder B" << std::flush << std::endl;

   begin = clock();
   for(iterations = 0;; iterations++)
   {
		// for each function definition, iterate over all variable references
      MatchResult matches = matcher.performMatching("$f=SgFunctionDefinition", root_node);
      BOOST_FOREACH(SingleMatchVarBindings match, matches)
      {
			SgFunctionDefinition *func_def = (SgFunctionDefinition *)match["$f"];
			AstMatching matcher2;
			MatchResult matches2 = matcher2.performMatching("$v=SgVarRefExp", func_def);
			BOOST_FOREACH(SingleMatchVarBindings match2, matches2)
			{
				var = (SgVarRefExp *)match2["$v"];
			}
      }
      if(clock() - begin >= dest_clock) break;
   }
   std::cout << iterations << "\t\t" << std::flush;

   begin = clock();
   finder.rebuildIndex(root_node, false);
   for(iterations = 0;; iterations++)
   {
		// for each function definition, iterate over all variable references
		NodeFinderResult res1 = finder.find(root_node, V_SgFunctionDefinition);
      BOOST_FOREACH(SgNode *func_def, res1)
      {
			NodeFinderResult res2 = finder.find(func_def, V_SgVarRefExp);
			BOOST_FOREACH(SgNode *var_ref, res2)
			{
				var = (SgVarRefExp *)var_ref;
			}
      }
      if(clock() - begin >= dest_clock) break;
   }
   std::cout << iterations << "\t\t" << std::flush;

   begin = clock();
   finder.rebuildIndex(root_node, true);
   for(iterations = 0;; iterations++)
   {
		// for each function definition, iterate over all variable references
		NodeFinderResult res1 = finder.find(root_node, V_SgFunctionDefinition);
      BOOST_FOREACH(SgNode *func_def, res1)
      {
			NodeFinderResult res2 = finder.find(func_def, V_SgVarRefExp);
			BOOST_FOREACH(SgNode *var_ref, res2)
			{
				var = (SgVarRefExp *)var_ref;
			}
      }
      if(clock() - begin >= dest_clock) break;
   }
   std::cout << iterations << std::flush << std::endl;

   std::cout << std::endl << std::endl << "Comparing iterating over a triple nested query speed..." << std::endl << std::endl;
   std::cout << "AST Matching\tNodeFinder A\tNodeFinder B" << std::flush << std::endl;

   begin = clock();
   for(iterations = 0;; iterations++)
   {
		// for each function definition, iterate over all if statements
      MatchResult matches = matcher.performMatching("$f=SgFunctionDefinition", root_node);
      BOOST_FOREACH(SingleMatchVarBindings match, matches)
      {
			SgFunctionDefinition *func_def = (SgFunctionDefinition *)match["$f"];
			AstMatching matcher2;
			MatchResult matches2 = matcher2.performMatching("$i=SgIfStmt", func_def);
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
      if(clock() - begin >= dest_clock) break;
   }
   std::cout << iterations << "\t\t" << std::flush;

   begin = clock();
   finder.rebuildIndex(root_node, false);
   for(iterations = 0;; iterations++)
   {
		// for each function definition, iterate over all variable references
		NodeFinderResult res1 = finder.find(root_node, V_SgFunctionDefinition);
      BOOST_FOREACH(SgNode *func_def, res1)
      {
			NodeFinderResult res2 = finder.find(func_def, V_SgIfStmt);
			BOOST_FOREACH(SgNode *if_stmt, res2)
			{
				SgIfStmt *the_if_stmt = (SgIfStmt *)if_stmt;
				NodeFinderResult res3 = finder.find(the_if_stmt, V_SgVarRefExp);
				BOOST_FOREACH(SgNode *var_ref, res3)
				{
					var = (SgVarRefExp *)var_ref;
				}
			}
      }
      if(clock() - begin >= dest_clock) break;
   }
   std::cout << iterations << "\t\t" << std::flush;

   begin = clock();
   finder.rebuildIndex(root_node, true);
   for(iterations = 0;; iterations++)
   {
		// for each function definition, iterate over all variable references
		NodeFinderResult res1 = finder.find(root_node, V_SgFunctionDefinition);
      BOOST_FOREACH(SgNode *func_def, res1)
      {
			NodeFinderResult res2 = finder.find(func_def, V_SgIfStmt);
			BOOST_FOREACH(SgNode *if_stmt, res2)
			{
				SgIfStmt *the_if_stmt = (SgIfStmt *)if_stmt;
				NodeFinderResult res3 = finder.find(the_if_stmt, V_SgVarRefExp);
				BOOST_FOREACH(SgNode *var_ref, res3)
				{
					var = (SgVarRefExp *)var_ref;
				}
			}
      }
      if(clock() - begin >= dest_clock) break;
   }
   std::cout << iterations << std::flush << std::endl;
   std::cout << std::endl << "NodeFinder Benchmarks: [DONE]" << std::endl;
	finder.dispose();
}
