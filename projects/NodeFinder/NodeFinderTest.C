/*
 * NodeFinderTest.C
 *
 * Contains tests for the NodeFinder library. Tests are run by running
 * make test or by passing test_sample.C to the NodeFinderTest binary
 *
 *  Created on: Nov 6, 2013
 *      Author: Sam Kelly <kellys@dickinson.edu>
 */
#include <NodeFinder.h>
#include <boost/algorithm/string/predicate.hpp>

int main(int argc, char** argv)
{
   // load specified source file(s) into ROSE and get the root SgNode
   std::cout << "Loading AST: ";
   SgProject *project = frontend(argc, argv);
   SgNode *root_node = (SgNode*)project;
   ROSE_ASSERT(boost::starts_with(root_node->sage_class_name(), "SgProject"));
   std::cout << "[DONE]" << std::endl;
   NodeFinder finder = NodeFinder(root_node);


   // test normal nested complex results
   std::cout << "Nest Level 1: ";
   NodeFinderResult var_dec_res_1 = finder.find(root_node, V_SgVariableDeclaration);
   ROSE_ASSERT(var_dec_res_1.size() == 16);
   NodeFinderResult if_res_1 = finder.find(root_node, V_SgIfStmt);
   ROSE_ASSERT(if_res_1.size() == 16);
   NodeFinderResult for_res_1 = finder.find(root_node, V_SgForStatement);
   ROSE_ASSERT(for_res_1.size() == 4);
   std::cout << "[PASS]" << std::endl;


   std::cout << "Nest Level 2: ";
   root_node = if_res_1[0];
   NodeFinderResult var_dec_res_2 = finder.find(root_node, V_SgVariableDeclaration);
   ROSE_ASSERT(var_dec_res_2.size() == 15);
   NodeFinderResult if_res_2 = finder.find(root_node, V_SgIfStmt);
   ROSE_ASSERT(if_res_2.size() == 15);
   NodeFinderResult for_res_2 = finder.find(root_node, V_SgForStatement);
   ROSE_ASSERT(for_res_2.size() == 4);
   std::cout << "[PASS]" << std::endl;

   std::cout << "Nest Level 3: ";
   root_node = if_res_2[0];
   NodeFinderResult var_dec_res_3 = finder.find(root_node, V_SgVariableDeclaration);
   ROSE_ASSERT(var_dec_res_3.size() == 14);
   NodeFinderResult if_res_3 = finder.find(root_node, V_SgVariableDeclaration);
   ROSE_ASSERT(if_res_3.size() == 14);
   NodeFinderResult for_res_3 = finder.find(root_node, V_SgForStatement);
   ROSE_ASSERT(for_res_3.size() == 4);
   std::cout << "[PASS]" << std::endl;

   std::cout << "Nest Level 4: ";
   root_node = if_res_1[4];
   NodeFinderResult if_res_4 = finder.find(root_node, V_SgIfStmt);
   ROSE_ASSERT(if_res_4.size() == 11);
   NodeFinderResult var_dec_res_4 = finder.find(root_node, V_SgVariableDeclaration);
   ROSE_ASSERT(var_dec_res_4.size() == 11);
   NodeFinderResult for_res_4 = finder.find(root_node, V_SgForStatement);
   ROSE_ASSERT(for_res_4.size() == 4);
   std::cout << "[PASS]" << std::endl;

   std::cout << "Nest Level 5-9: ";
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
   std::cout << "[PASS]" << std::endl;

   // test empty results
   std::cout << "Empty Result Test: ";
   NodeFinderResult var_dec_res_10 = finder.find(var_dec_res_9[0], V_SgVariableDeclaration);
   ROSE_ASSERT(var_dec_res_10.size() == 0);
   NodeFinderResult namespace_res = finder.find(if_res_1[0], V_SgNamespaceDeclarationStatement);
   ROSE_ASSERT(namespace_res.size() == 0);
   std::cout << "[PASS]" << std::endl;

   std::cout << "All NodeFinder tests have passed!" << std::endl;
}



