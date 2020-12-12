#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <set>
#include <random>
#include <chrono>
#include "LinkCutTree.h"

using namespace std;

struct GraphGenerator {
	static void GenerateTree(int numberOfTestCases, int sizeOfTree, int numberOfQueries, string fileName) {
		ofstream fout(fileName);
		fout << numberOfTestCases << "\n";
		for (int i = 0; i < numberOfTestCases; i++) {
			cout << "test case: " << i << " started...\n";
			GenerateTreeTestCase(sizeOfTree, numberOfQueries, fout);
			cout << "test case: " << i << " done\n";
		}
	}

private:
	static string GetTwoNodesNotConnected(int  sizeOfTree, LinkCutTree &lct, vector<pair<int, int>> &edges, mt19937 &rng) {
		while (true) {
			int u = rng() % sizeOfTree;
			int v = rng() % sizeOfTree;
			if (lct.findRoot(u) != lct.findRoot(v)) {
				edges.push_back({ u, v });
				lct.link(u, v);
				return to_string(u) + " " + to_string(v);
			}
		}
	}

	static string GetTwoNodesConnected(int  sizeOfTree, LinkCutTree& lct, vector<pair<int, int>>& edges, mt19937& rng) {
		int ind = rng() % edges.size();
		int u = edges[ind].first, v = edges[ind].second;
		lct.cut(u, v);
		swap(edges[ind], edges.back()); edges.pop_back();
		return to_string(u) + " " + to_string(v);
	}

	static void GenerateTreeTestCase(int  sizeOfTree, int numberOfQueries, ofstream & fout) {
		fout << sizeOfTree << " " << numberOfQueries << "\n";
		LinkCutTree lct;
		for (int i = 0; i < sizeOfTree; i++) lct.addNode(new NodeVal());
		vector<pair<int, int>> edges;
		mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
		for (int q = 0; q < numberOfQueries; q++) {
			int decision = rng() % 5;
			if (decision < 3) {
				// add edge
				if (edges.size() == sizeOfTree - 1) {
					q--;
					continue;
				}
				fout << "1 "<<GetTwoNodesNotConnected(sizeOfTree, lct, edges, rng)<<"\n";
			}
			else if (decision == 3) {
				// remove edges
				if (edges.size() == 0) {
					q--;
					continue;
				}
				fout << "2 " << GetTwoNodesConnected(sizeOfTree, lct, edges, rng) << "\n";
			}
			else {
				// query
				fout << "3 " << rng() % sizeOfTree << " "<< rng() % sizeOfTree << "\n";
			}
		}
	}
};