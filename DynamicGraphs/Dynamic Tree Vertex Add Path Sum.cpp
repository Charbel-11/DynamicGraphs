#pragma once
#include <iostream>
#include <array>
#include <assert.h>
#include <stack>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

struct NodeVal {
	long long val, splaySubtreeVal;
	NodeVal(long long val = 1) : val(val), splaySubtreeVal(val) {}
	// update should be symmetric with respect to lchild and rchild if we wish to have the ability to link any 2 nodes
	// i.e. update(x, y) == update(y, x) 
	void update(NodeVal* lChild, NodeVal* rChild) {
		splaySubtreeVal = val + (lChild ? lChild->splaySubtreeVal : 0) + (rChild ? rChild->splaySubtreeVal : 0);
	}
};

struct Node {
	int id;
	array<Node*, 2> child{};
	Node* splayTreeParent = nullptr;
	Node* pathParentPointer = nullptr;
	NodeVal* nodeVal;
	bool reverse = false;

	Node(int id, NodeVal* nodeVal) : id(id), nodeVal(nodeVal) {}
	bool getSide() {
		return splayTreeParent ? splayTreeParent->child[1] == this : false;
	}
	void applyReverseLazy() {
		if (!reverse) { return; }
		reverse = false;
		swap(child[0], child[1]);
		if (child[0]) { child[0]->reverse = !child[0]->reverse; }
		if (child[1]) { child[1]->reverse = !child[1]->reverse; }
	}
	Node* splay() {
		stack<Node*> ancestors;
		Node* cur = this;
		while (cur) { ancestors.push(cur); cur = cur->splayTreeParent; }
		while (ancestors.size()) {
			ancestors.top()->applyReverseLazy(); ancestors.pop();
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
		update();
		return this;
	}

	void update() {
		nodeVal->update((child[0] ? child[0]->nodeVal : nullptr), (child[1] ? child[1]->nodeVal : nullptr));
	}

	void detachChild(bool b) {
		if (!child[b]) { return; }
		child[b]->pathParentPointer = this;
		child[b]->splayTreeParent = nullptr;
		child[b] = nullptr;
		update();
	}

	Node* findMax() {
		return getDeepest(1);
	}

	Node* findMin() {
		return getDeepest(0);
	}
private:
	Node* getDeepest(int dir) {
		Node* u = this; applyReverseLazy();
		while (u && u->child[dir]) { u = u->child[dir]; u->applyReverseLazy(); }
		return u->splay();
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

	void link(int parentId, int childId) {
		Node* parent = nodes[parentId], * child = nodes[childId];
		assert(findRoot(child->id) != findRoot(parent->id));
		access(child); access(parent);

		Node* lChild = child->child[0];
		if (lChild) {
			lChild->reverse = !lChild->reverse;
			child->detachChild(0);
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
		access(u);
		if (u->child[0] && u->child[0]->findMax() == v) { cut(u->id); }
		else { cut(v->id); }
	}

	//Finds the root of u in the represented tree
	int findRoot(int id) {
		Node* u = nodes[id];
		access(u);
		Node* res = u->findMin();
		access(res);
		return res->id;
	}

	int LCA(int id1, int id2) {
		Node* u = nodes[id1], * v = nodes[id2];
		if (findRoot(u->id) != findRoot(v->id)) { return -1; }
		access(u);
		return access(v)->id;
	}

	NodeVal* pathAggregate(int id) {
		Node* u = nodes[id];
		access(u);
		return u->nodeVal;
	}

	Node* getNode(int id) {
		return nodes[id];
	}

	Node* access(int id) {
		return access(nodes[id]);
	}

	//Returns the last path Parent Pointer
	Node* access(Node* u) {
		u->splay();
		u->detachChild(1);

		Node* curPP = u;
		while (u->pathParentPointer) {
			curPP = u->pathParentPointer;
			curPP->splay();
			curPP->detachChild(1);
			curPP->attach(u, 1);
			u->pathParentPointer = nullptr;
			u->splay();
		}

		return curPP;
	}
};

int main() {
	std::ios::sync_with_stdio(0);
	std::cin.tie(0);
	std::cout.tie(0);

	int n, q; 
	cin >> n >> q;
	LinkCutTree lct;
	for (int i = 0; i < n; i++) {
		long long a; cin >> a;
		lct.addNode(new NodeVal(a));
	}
	for (int i = 0; i < n - 1; i++) {
		int u, v; cin >> u >> v;
		lct.link(u, v);
	}
	while (q--) {
		int tp; cin >> tp;
		if (tp == 0) {
			int u, v, w, x; cin >> u >> v >> w >> x;
			lct.cut(u, v);
			lct.link(w, x);
		}
		else if (tp == 1) {
			long long p, x; cin >> p >> x;
			lct.access(p);
			lct.nodes[p]->nodeVal->val += x;
			lct.nodes[p]->update();
		} 
		else {
			int u, v; cin >> u >> v;
			int lca = lct.LCA(u, v);
			long long U = lct.pathAggregate(u)->splaySubtreeVal; 
			long long V = lct.pathAggregate(v)->splaySubtreeVal; 
			long long LCA = lct.pathAggregate(lca)->splaySubtreeVal; 
			long long ans =  U + V - 2 * LCA + lct.nodes[lca]->nodeVal->val;
			cout << ans << "\n";
		}
	}
}