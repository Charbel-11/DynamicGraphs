#include<vector>
#include"Link Cut Tree.h"
using namespace std;


struct NodeWithId : Node {
	int id;
	NodeWithId(NodeVal* nodeVal, int id) : Node(nodeVal), id(id){}
};

struct DynamicTreeLCT {
	LinkCutTree lct;
	vector<NodeWithId*> nodes;
	int n = 0;
	DynamicTreeLCT(){}

	// returns Id of newly added node
	int addNode(NodeVal *nodeVal) {
		nodes.push_back(new NodeWithId(nodeVal, n));
		return n++;
	}

	void link(int parentId, int childId) {
		lct.link(nodes[parentId], nodes[childId]);
	}

	void cut(int id) {
		lct.cut(nodes[id]);
	}

	int lca(int u, int v) {
		auto res = (NodeWithId *)lct.LCA(nodes[u], nodes[v]);
		return res->id;
	}

	int findRoot(int u) {
		auto res = (NodeWithId*)lct.findRoot(nodes[u]);
		return res->id;
	}
};