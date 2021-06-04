//https://acm.timus.ru/problem.aspx?space=1&num=1553
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
	long long val, subtreeMin, subtreeMax;
	NodeVal(long long val = 1) : val(val), subtreeMin(val), subtreeMax(val) {}
	// update should be symmetric with respect to lchild and rchild if we wish to have the ability to link any 2 nodes
	// i.e. update(x, y) == update(y, x) 
	void update(NodeVal* lChild, NodeVal* rChild) {
		subtreeMin = min({ val , (lChild ? lChild->subtreeMin : LLONG_MAX) , (rChild ? rChild->subtreeMin : LLONG_MAX) });
		subtreeMax = max({ val , (lChild ? lChild->subtreeMax : LLONG_MIN) , (rChild ? rChild->subtreeMax : LLONG_MIN) });
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

	pair<long long, long long> path(int id1, int id2) {
		pair<long long, long long> ans = { LLONG_MAX, LLONG_MIN };
		int lca = LCA(id1, id2);
		auto updateAns = [&](int id) {
			access(id);
			nodes[lca]->splay();
			ans.first = min({ ans.first, nodes[lca]->nodeVal->val, nodes[lca]->child[1] ? nodes[lca]->child[1]->nodeVal->subtreeMin : LLONG_MAX });
			ans.second = max({ ans.second, nodes[lca]->nodeVal->val, nodes[lca]->child[1] ? nodes[lca]->child[1]->nodeVal->subtreeMax : LLONG_MIN });
			access(lca);
		};
		updateAns(id1);
		updateAns(id2);
		return ans;
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
	int n; cin >> n;
	LinkCutTree lct;
	for (int i = 0; i < n; i++) {
		lct.addNode(new NodeVal(0));
	}
	for (int i = 0; i < n - 1; i++) {
		int u, v; cin >> u >> v; u--; v--;
		lct.link(u, v);
	}
	int q; cin >> q;
	while (q--) {
		char tp; cin >> tp;
		if (tp == 'I') {
			int u, x; cin >> u >> x; u--;
			lct.access(u);
			lct.nodes[u]->nodeVal->val += x;
			lct.nodes[u]->update();
		}
		else {
			int u, v; cin >> u >> v; u--, v--;
			auto mx = lct.path(u, v).second;
			cout << mx << "\n";
		}
	}
}