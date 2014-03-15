/*
 * NodeFinderTest.C
 *
 * Contains tests for the NodeFinder library. Tests are run by running
 * make test or by passing test_sample.C to the NodeFinderTest binary.
 * Tests will fail without the proper sample file.
 *
 *  Created on: Nov 6, 2013
 *      Author: Sam Kelly <kellys@dickinson.edu>
 */
#include <NodeFinder.h>
#include <boost/algorithm/string/predicate.hpp>

std::vector<NodeFinderResult>* find_tests(NodeFinder finder, SgNode *root_node)
{
	// store results for comparison with different indexing algorithms
	std::vector<NodeFinderResult> *arr = new std::vector<NodeFinderResult>();

   std::cout << "Nest Level 1: ";
   NodeFinderResult var_dec_res_1 = finder.find(root_node, V_SgVariableDeclaration);
   ROSE_ASSERT(var_dec_res_1.size() == 16);
	arr->push_back(var_dec_res_1);
   NodeFinderResult if_res_1 = finder.find(root_node, V_SgIfStmt);
   ROSE_ASSERT(if_res_1.size() == 16);
	arr->push_back(if_res_1);
   NodeFinderResult for_res_1 = finder.find(root_node, V_SgForStatement);
   ROSE_ASSERT(for_res_1.size() == 4);
	arr->push_back(for_res_1);
   std::cout << "[PASS]" << std::endl;

   std::cout << "Nest Level 2: ";
   root_node = if_res_1[0];
   NodeFinderResult var_dec_res_2 = finder.find(root_node, V_SgVariableDeclaration);
   ROSE_ASSERT(var_dec_res_2.size() == 15);
	arr->push_back(var_dec_res_2);
   NodeFinderResult if_res_2 = finder.find(root_node, V_SgIfStmt);
   ROSE_ASSERT(if_res_2.size() == 15);
	arr->push_back(if_res_2);
   NodeFinderResult for_res_2 = finder.find(root_node, V_SgForStatement);
   ROSE_ASSERT(for_res_2.size() == 4);
	arr->push_back(for_res_2);
   std::cout << "[PASS]" << std::endl;

   std::cout << "Nest Level 3: ";
   root_node = if_res_2[0];
   NodeFinderResult var_dec_res_3 = finder.find(root_node, V_SgVariableDeclaration);
   ROSE_ASSERT(var_dec_res_3.size() == 14);
	arr->push_back(var_dec_res_3);
   NodeFinderResult if_res_3 = finder.find(root_node, V_SgVariableDeclaration);
   ROSE_ASSERT(if_res_3.size() == 14);
	arr->push_back(if_res_3);
   NodeFinderResult for_res_3 = finder.find(root_node, V_SgForStatement);
   ROSE_ASSERT(for_res_3.size() == 4);
	arr->push_back(for_res_3);
   std::cout << "[PASS]" << std::endl;

   std::cout << "Nest Level 4: ";
   root_node = if_res_1[4];
   NodeFinderResult if_res_4 = finder.find(root_node, V_SgIfStmt);
   ROSE_ASSERT(if_res_4.size() == 11);
	arr->push_back(if_res_4);
   NodeFinderResult var_dec_res_4 = finder.find(root_node, V_SgVariableDeclaration);
   ROSE_ASSERT(var_dec_res_4.size() == 11);
	arr->push_back(var_dec_res_4);
   NodeFinderResult for_res_4 = finder.find(root_node, V_SgForStatement);
   ROSE_ASSERT(for_res_4.size() == 4);
	arr->push_back(for_res_4);
   std::cout << "[PASS]" << std::endl;

   std::cout << "Nest Level 5-9: ";
   root_node = if_res_4[0];
   NodeFinderResult if_res_5 = finder.find(root_node, V_SgIfStmt);
   ROSE_ASSERT(if_res_5.size() == 10);
	arr->push_back(if_res_5);
   NodeFinderResult var_dec_res_5 = finder.find(root_node, V_SgVariableDeclaration);
   ROSE_ASSERT(var_dec_res_5.size() == 9);
	arr->push_back(var_dec_res_5);

   root_node = if_res_5[0];
   NodeFinderResult if_res_6 = finder.find(root_node, V_SgIfStmt);
   ROSE_ASSERT(if_res_6.size() == 8);
	arr->push_back(if_res_6);
   NodeFinderResult var_dec_res_6 = finder.find(root_node, V_SgVariableDeclaration);
   ROSE_ASSERT(var_dec_res_6.size() == 9);
	arr->push_back(var_dec_res_6);
   NodeFinderResult for_res_5 = finder.find(root_node, V_SgForStatement);
   ROSE_ASSERT(for_res_5.size() == 4);
	arr->push_back(for_res_5);

   root_node = for_res_5[0];
   NodeFinderResult if_res_7 = finder.find(root_node, V_SgIfStmt);
   ROSE_ASSERT(if_res_7.size() == 7);
	arr->push_back(if_res_7);
   NodeFinderResult var_dec_res_7 = finder.find(root_node, V_SgVariableDeclaration);
   ROSE_ASSERT(var_dec_res_7.size() == 9);
	arr->push_back(var_dec_res_7);
   NodeFinderResult for_res_6 = finder.find(root_node, V_SgForStatement);
   ROSE_ASSERT(for_res_6.size() == 3);
	arr->push_back(for_res_6);

   root_node = for_res_6[0];
   NodeFinderResult for_res_7 = finder.find(root_node, V_SgForStatement);
   ROSE_ASSERT(for_res_7.size() == 2);
	arr->push_back(for_res_7);
   NodeFinderResult var_dec_res_8 = finder.find(root_node, V_SgVariableDeclaration);
   ROSE_ASSERT(var_dec_res_8.size() == 8);
	arr->push_back(var_dec_res_8);
   NodeFinderResult if_res_8 = finder.find(root_node, V_SgIfStmt);
   ROSE_ASSERT(if_res_8.size() == 6);
	arr->push_back(if_res_8);


   root_node = for_res_7[1];
   NodeFinderResult for_res_8 = finder.find(root_node, V_SgForStatement);
   ROSE_ASSERT(for_res_8.size() == 0);
	arr->push_back(for_res_8);
   NodeFinderResult var_dec_res_9 = finder.find(root_node, V_SgVariableDeclaration);
   ROSE_ASSERT(var_dec_res_9.size() == 4);
	arr->push_back(var_dec_res_9);
   NodeFinderResult if_res_9 = finder.find(root_node, V_SgIfStmt);
   ROSE_ASSERT(if_res_9.size() == 4);
	arr->push_back(if_res_9);
   std::cout << "[PASS]" << std::endl;


   // test empty results
   std::cout << "Empty Result Test: ";
   NodeFinderResult var_dec_res_10 = finder.find(var_dec_res_9[0], V_SgVariableDeclaration);
   ROSE_ASSERT(var_dec_res_10.size() == 0);
	arr->push_back(var_dec_res_10);
   NodeFinderResult namespace_res = finder.find(if_res_1[0], V_SgNamespaceDeclarationStatement);
   ROSE_ASSERT(namespace_res.size() == 0);
	arr->push_back(namespace_res);
   std::cout << "[PASS]" << std::endl;

	return arr;
}

int main(int argc, char** argv)
{
   // load specified source file(s) into ROSE and get the root SgNode
   std::cout << "Loading AST: ";
   SgProject *project = frontend(argc, argv);
   SgNode *root_node = (SgNode*)project;

	// generate the initial index and get a count of nodes
   ROSE_ASSERT(boost::starts_with(root_node->sage_class_name(), "SgProject"));
   std::cout << "[DONE]" << std::endl;
	std::cout << "Generating Index: ";
   NodeFinder finder = NodeFinder(root_node);
	NodeFinder finder2 = NodeFinder(root_node);
	std::cout << "[DONE]" << std::endl;
	std::cout << "Total Nodes: " << finder.getTotalNodes() << std::endl;

	// perform main battery of tests on indexing method A
	std::vector<NodeFinderResult> *resultsA = find_tests(finder, root_node);

	std::cout << "Indexing Method A Tests: [PASS]" << std::endl;

	// begin indexing method B tests
	std::cout << "Generating Index: ";
	finder2.rebuildIndex(root_node, true);
	std::cout << "[DONE]" << std::endl;

	// perform main battery of tests on indexing method B
	std::vector<NodeFinderResult> *resultsB = find_tests(finder2, root_node);

	std::cout << "Indexing Method B Tests: [PASS]" << std::endl;

	// check for consistency between memory addresses of results in methods A and B
	ROSE_ASSERT(resultsA->size() == resultsB->size());
	for(int i = 0; i < (int)resultsA->size(); i++)
	{
		NodeFinderResult resultA = resultsA->operator[](i);
		NodeFinderResult resultB = resultsB->operator[](i);
		// ensure that result sizes are the same between A and B
		ROSE_ASSERT(resultA.size() == resultB.size());
		for(int j = 0; j < (int)resultA.size(); j++)
		{
			// ensure that the memory addresses of items in results are the same between A and B
			SgNode *nodeA = resultA[j];
			SgNode *nodeB = resultB[j];
			//std::cout << nodeA << " " << nodeB << std::endl;
			ROSE_ASSERT(nodeA == nodeB);
		}
	}
	std::cout << "Result consistency test between A and B: [PASS]" << std::endl;
	delete resultsA;
	delete resultsB;

	finder.dispose();
	finder2.dispose();

   std::cout << "All NodeFinder tests have passed!" << std::endl;
	
}



