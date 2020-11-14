#include<array>
#include<assert.h>
#include<stack>
#include<string>
#include<vector>
#include <algorithm>
using namespace std;

struct NodeVal {
	int subtreeSize;
	NodeVal(): subtreeSize(1) {}

	void update(NodeVal* lChild, NodeVal* rChild) {
		subtreeSize = 1 + (lChild ? lChild->subtreeSize : 0) + (rChild ? rChild->subtreeSize : 0);
	}
};

struct Node {
	array<Node*, 2> child{};
	Node* splayTreeParent = nullptr;
	Node* pathParentPointer = nullptr;
	NodeVal* val;

	Node(NodeVal* _val) : val(_val) {}
	bool getSide() {
		return splayTreeParent ? splayTreeParent->child[1] == this : false;
	}
	Node* splay() {
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
		assert(!child->child[0] && !child->child[0]);

		child->attach(parent, 0);
	}

	void cut(Node* u) {
		access(u);
		assert(u->child[0]);
		u->child[0]->splayTreeParent = nullptr;
		u->child[0] = nullptr;
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

	int pathAggregate(Node* u) {
		access(u);
		return u->val->subtreeSize;
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

	void detachChild(Node* u, bool b) {
		if (!u->child[b]) { return; }

		u->child[b]->pathParentPointer = u;
		u->child[b]->splayTreeParent = nullptr;
		u->child[b] = nullptr;
	}
};