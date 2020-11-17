#pragma once
#include "EulerTourTree.h"
#include <vector>
using namespace std;

struct Forest {
	vector<vector<int>> adj; int n;
	Forest(int _n) : n(_n), adj(_n) {}

	void addEdge(int u, int v) {
		adj[u].push_back(v);
		adj[v].push_back(u);
	}
};

struct DynamicTreeETT {

};