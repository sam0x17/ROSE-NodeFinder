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

#include <NodeFinder.h>
#include <AstMatching.h>
#include <boost/algorithm/string/predicate.hpp>
#include <time.h>

int main(int argc, char** argv)
{
   std::string source_file;
   bool found_source_file = false;
   for(int i = 0; i < argc; i++)
   {
      source_file = argv[i];
      if(boost::ends_with(source_file, ".C") || boost::ends_with(source_file, ".cpp"))
      {
         std::cout << "Loading AST from: " << source_file << std::endl;
         found_source_file = true;
         break;
      }
   }
   if(!found_source_file)
   {
      std::cout << "Error: no source file was specified!" << std::endl;
      return 1;
   }

   SgProject *project = frontend(argc, argv);
   SgNode *root_node = (SgNode*)project;

   std::cout << "AST loaded successfully." << std::endl;

   NodeFinder finder(root_node);
   AstMatching matcher;

   double minutes = 1.0/6;
   clock_t begin;
   clock_t dest_clock = (clock_t)(minutes * 60 * CLOCKS_PER_SEC);

   std::cout << std::endl << "Benchmarks will run for " << (int)(minutes * 60) << " CPU seconds (" << dest_clock << " CPU cycles) each" << std::endl;
   std::cout << "Results will be printed in terms of the # of iterations each algorithm was able to run within the given time frame." << std::endl;

   std::cout << std::endl << "Comparing traversal speed..." << std::endl << std::endl;

   std::cout << "AST Matching\tNodeFinder" << std::endl;

   long iterations;

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
      finder.rebuildIndex();
      if(clock() - begin >= dest_clock) break;
   }
   std::cout << iterations << std::flush << std::endl;


   std::cout << std::endl << std::endl << "Comparing search speed..." << std::endl << std::endl;
   std::cout << "AST Matching\tNodeFinder" << std::flush << std::endl;

   begin = clock();
   for(iterations = 0;; iterations++)
   {
      matcher.performMatching("$v=SgVarRefExp", root_node);
      if(clock() - begin >= dest_clock) break;
   }
   std::cout << iterations << "\t\t" << std::flush;


   begin = clock();
   for(iterations = 0;; iterations++)
   {
      finder.find(root_node, V_SgVarRefExp);
      if(clock() - begin >= dest_clock) break;
   }
   std::cout << iterations << std::flush << std::endl;


   std::cout << std::endl << std::endl << "Comparing search iteration speed..." << std::endl << std::endl;
   std::cout << "AST Matching\tNodeFinder" << std::flush << std::endl;

   begin = clock();
   for(iterations = 0;; iterations++)
   {
      MatchResult matches = matcher.performMatching("$v=SgVarRefExp", root_node);
      BOOST_FOREACH(SingleMatchVarBindings match, matches)
      {
         SgVarRefExp *var = (SgVarRefExp *)match["$v"];
      }
      if(clock() - begin >= dest_clock) break;
   }
   std::cout << iterations << "\t\t" << std::flush;


   begin = clock();
   finder.rebuildIndex();
   for(iterations = 0;; iterations++)
   {
      NodeFinderResult res = finder.find(root_node, V_SgVarRefExp);
      BOOST_FOREACH(SgNode *node, res)
      {
         SgVarRefExp *var = (SgVarRefExp *)node;
      }
      if(clock() - begin >= dest_clock) break;
   }
   std::cout << iterations << std::flush << std::endl;


   std::cout << std::endl << "NodeFinder vs AST Matching benchmarks: [DONE]" << std::endl;
}
