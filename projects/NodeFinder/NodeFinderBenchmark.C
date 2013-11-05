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

   clock_t begin, end;
   double total_time;

   NodeFinder finder(root_node);
   AstMatching matcher;

   std::cout << std::endl << "Comparing traversal speed..." << std::endl << std::endl;

   std::cout << "AST Matching\tNodeFinder" << std::endl;


   int iterations = 10000;


   begin = clock();
   for(int i = 0; i < iterations; i++)
   {
      matcher.performMatching("$v=SgVarRefExp", root_node);
   }
   end = clock();
   total_time = (double)(end - begin) / CLOCKS_PER_SEC;

   std::cout << total_time << std::flush << "\t\t";

   begin = clock();
   for(int i = 0; i < iterations; i++)
   {
      finder.rebuildIndex();
   }
   end = clock();
   total_time = (double)(end - begin) / CLOCKS_PER_SEC;

   std::cout << total_time << std::flush << std::endl;


   std::cout << std::endl << std::endl << "Comparing search time..." << std::endl << std::endl;
   std::cout << "AST Matching\tNodeFinder" << std::flush << std::endl;

   begin = clock();
   for(int i = 0; i < iterations; i++)
   {
      matcher.performMatching("$v=SgVarRefExp", root_node);
   }
   end = clock();
   total_time = (double)(end - begin) / CLOCKS_PER_SEC;
   std::cout << total_time << "\t\t\t" << std::flush;


   begin = clock();
   finder.rebuildIndex();
   for(int i = 0; i < iterations; i++)
   {
      finder.find(root_node, V_SgVarRefExp);
   }
   end = clock();
   total_time = (double)(end - begin) / CLOCKS_PER_SEC;

   std::cout << total_time << std::endl;


   std::cout << std::endl << "NodeFinder vs AST Matching benchmarks: [DONE]" << std::endl;
}
