//https://www.spoj.com/problems/DYNACON1/
//Tests connectivity with links and cuts

#include <iostream>
#include<array>
#include<assert.h>
#include<stack>
#include<string>
#include<vector>
#include <algorithm>
using namespace std;

struct NodeVal {
	int subtreeSize;
	NodeVal() : subtreeSize(1) {}
	void update(NodeVal* lChild, NodeVal* rChild) {
		subtreeSize = 1 + (lChild ? lChild->subtreeSize : 0) + (rChild ? rChild->subtreeSize : 0);
	}
};

struct Node {
	int id;
	array<Node*, 2> child{};
	Node* splayTreeParent = nullptr;
	Node* pathParentPointer = nullptr;
	NodeVal* val;
	bool reverse = false;

	Node(int id, NodeVal* _val) : id(id), val(_val) {}
	bool getSide() {
		return splayTreeParent ? splayTreeParent->child[1] == this : false;
	}
	void tryReverse() {
		if (!reverse) { return; }
		reverse = false;
		swap(child[0], child[1]);
		if (child[0]) { child[0]->reverse = !child[0]->reverse; }
		if (child[1]) { child[1]->reverse = !child[1]->reverse; }
	}
	Node* splay() {
		vector<Node*> ancestors;
		Node* cur = this;
		while (cur) { ancestors.push_back(cur); cur = cur->splayTreeParent; }
		for (int i = ancestors.size() - 1; i >= 0; i--) {
			if (!ancestors[i]) { continue; }
			ancestors[i]->tryReverse();
		}

		while (splayTreeParent) {
			if (splayTreeParent->splayTreeParent) {
				(getSide() == splayTreeParent->getSide() ? splayTreeParent : this)->rotate();
			}
			rotate();
		}
		return this;
	}
	Node* rotate() {
		bool side = getSide(), parentSide = splayTreeParent->getSide();
		auto ancestor = splayTreeParent->splayTreeParent;

		pathParentPointer = splayTreeParent->pathParentPointer;
		splayTreeParent->pathParentPointer = nullptr;
		splayTreeParent->attach(child[!side], side);
		attach(splayTreeParent, !side);

		if (ancestor) ancestor->attach(this, parentSide);
		else splayTreeParent = nullptr;
		return this;
	}
	Node* attach(Node* node, int side) {
		if (node) node->splayTreeParent = this;
		child[side] = node;
		val->update((child[0] ? child[0]->val : nullptr), (child[1] ? child[1]->val : nullptr));
		return this;
	}
};

struct LinkCutTree {
	int n = 0;
	vector<Node*> nodes;

	// returns Id of newly added node
	int addNode(NodeVal* nodeVal) {
		nodes.push_back(new Node(n, nodeVal));
		return n++;
	}

	//Assumes child is a root in the represented tree
	void link(int parentId, int childId) {
		Node* parent = nodes[parentId], * child = nodes[childId];
		assert(findRoot(child->id) != findRoot(parent->id));
		access(child); access(parent);

		Node* lChild = child->child[0];
		if (lChild) {
			lChild->reverse = !lChild->reverse;
			detachChild(child, 0);
		}

		child->attach(parent, 0);
	}

	void cut(int id) {
		Node* u = nodes[id];
		access(u);
		assert(u->child[0]);
		u->child[0]->splayTreeParent = nullptr;
		u->child[0] = nullptr;
	}

	void cut(int id1, int id2) {
		Node* u = nodes[id1], * v = nodes[id2];
		access(v); access(u);
		if (u->child[0] && findMax(u->child[0]) == v) { cut(u->id); return; }
		access(v);
		if (v->child[0] && findMax(v->child[0]) == u) { cut(v->id); }
	}

	//Finds the root of u in the represented tree
	int findRoot(int id) {
		Node* u = nodes[id];
		access(u);
		while (u->child[0]) { u = u->child[0]; }
		access(u);
		return u->id;
	}

	int LCA(int id1, int id2) {
		Node* u = nodes[id1], * v = nodes[id2];
		if (findRoot(u->id) != findRoot(v->id)) { return -1; }
		access(u);
		return access(v)->id;
	}

	int pathAggregate(int id) {
		Node* u = nodes[id];
		access(u);
		return u->id;
	}

	Node* getNode(int id) {
		return nodes[id];
	}

private:
	//Returns the last path Parent Pointer
	Node* access(Node* u) {
		u->splay();
		detachChild(u, 1);

		Node* curPP = u;
		while (u->pathParentPointer) {
			curPP = u->pathParentPointer;
			curPP->splay();
			detachChild(curPP, 1);
			curPP->attach(u, 1);
			u->pathParentPointer = nullptr;
			u->splay();
		}

		return curPP;
	}

	Node* findMax(Node* u) {
		while (u && u->child[1]) { u = u->child[1]; }
		u->splay();
		return u;
	}

	void detachChild(Node* u, bool b) {
		if (!u->child[b]) { return; }

		u->child[b]->pathParentPointer = u;
		u->child[b]->splayTreeParent = nullptr;
		u->child[b] = nullptr;
	}
};

int main() {
	ios::sync_with_stdio(0);
	cin.tie(0), cout.tie(0);

	LinkCutTree dynamicTree;

	int n, m; cin >> n >> m;
	for (int i = 0; i < n; i++) { dynamicTree.addNode(new NodeVal()); }
	while (m--) {
		string type; cin >> type;
		int u, v; cin >> u >> v; u--; v--;
		if (type == "conn") {
			if (dynamicTree.findRoot(u) == dynamicTree.findRoot(v)) { cout << "YES\n"; }
			else { cout << "NO\n"; }
		}
		else if (type == "add") {
			dynamicTree.link(u, v);
		}
		else {
			dynamicTree.cut(u, v);
		}
	}
}