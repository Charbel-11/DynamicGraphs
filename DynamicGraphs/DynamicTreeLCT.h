#include<vector>
#include"LinkCutTree.h"
#include <iostream>
using namespace std;


struct MyNodeVal: NodeVal {
	int subtreeSize;
	MyNodeVal() : subtreeSize(1) {}
	void update(NodeVal* lChild, NodeVal* rChild) {
		auto lChildCasted = ((MyNodeVal*)lChild);
		auto rChildCasted = ((MyNodeVal*)rChild);
		subtreeSize = 1 + (lChildCasted ? lChildCasted->subtreeSize : 0) + (rChildCasted ? rChildCasted->subtreeSize : 0);
	}
};

struct NodeWithId : Node {
	int id;
	NodeWithId(MyNodeVal* nodeVal, int id) : Node(nodeVal), id(id){}
};

struct DynamicTreeLCT {
	LinkCutTree lct;
	vector<NodeWithId*> nodes;
	int n = 0;
	DynamicTreeLCT(){}

	// returns Id of newly added node
	int addNode(MyNodeVal* nodeVal) {
		nodes.push_back(new NodeWithId(nodeVal, n));
		return n++;
	}

	// assuming childId is a root
	void link(int parentId, int childId) {
		lct.link(nodes[parentId], nodes[childId]);
	}

	void cut(int id) {
		lct.cut(nodes[id]);
	}
	
	void cut(int u, int v) {
		lct.cut(nodes[u], nodes[v]);
	}

	int lca(int u, int v) {
		auto res = (NodeWithId *)lct.LCA(nodes[u], nodes[v]);
		return res->id;
	}

	int findRoot(int u) {
		auto res = (NodeWithId*)lct.findRoot(nodes[u]);
		return res->id;
	}

	bool connected(int u, int v) {
		return findRoot(u) == findRoot(v);
	}

	// number of nodes on the path from root to u including u
	int height(int u) {
		auto res = (NodeWithId*)lct.pathAggregate(nodes[u]);
		return 1 + (res->child[0] ? ((MyNodeVal*)res->child[0]->val)->subtreeSize : 0);
	}
};