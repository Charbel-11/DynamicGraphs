#include<array>
#include<assert.h>
#include<stack>
#include<string>
#include<vector>
#include <algorithm>
using namespace std;

struct NodeVal {
	virtual void update(NodeVal* lChild, NodeVal* rChild) = 0;
};

struct Node {
	array<Node*, 2> child{};
	Node* splayTreeParent = nullptr;
	Node* pathParentPointer = nullptr;
	NodeVal* val;
	bool reverse = false;

	Node(NodeVal* _val): val(_val) {}
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
	//Assumes child is a root in the represented tree
	void link(Node* parent, Node* child) {
		assert(findRoot(child) != findRoot(parent));
		access(child); access(parent);

		Node* lChild = child->child[0];
		if (lChild) {
			lChild->reverse = !lChild->reverse;
			detachChild(child, 0);
		}

		child->attach(parent, 0);
	}

	void cut(Node* u) {
		access(u);
		assert(u->child[0]);
		u->child[0]->splayTreeParent = nullptr;
		u->child[0] = nullptr;
	}

	void cut(Node* u, Node* v) {
		access(v); access(u);
		if (u->child[0] && findMax(u->child[0]) == v) { cut(u); return; }
		access(v);
		if (v->child[0] && findMax(v->child[0]) == u) { cut(v); }
	}

	//Finds the root of u in the represented tree
	Node* findRoot(Node* u) {
		access(u);
		while (u->child[0]) { u = u->child[0]; }
		access(u);
		return u;
	}

	Node* LCA(Node* u, Node* v) {
		if (findRoot(u) != findRoot(v)) { return nullptr; }
		access(u);
		return access(v);
	}

	Node* pathAggregate(Node* u) {
		access(u);
		return u;
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