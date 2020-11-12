#include<iostream>
#include<array>
#include<assert.h>
#include<stack>
#include<string>
#include<vector>
#include <algorithm>
using namespace std;

struct NodeVal {
	int key, subtreeSize;
	NodeVal(int _k) : key(_k), subtreeSize(1) {}

	void update(NodeVal* lChild, NodeVal* rChild) {
		subtreeSize = 1 + (lChild ? lChild->subtreeSize : 0) + (rChild ? rChild->subtreeSize : 0);
	}
};

struct SplayNode {
	array<SplayNode*, 2> child{};
	SplayNode* parent = nullptr;
	SplayNode* pathParentPointer = nullptr;
	NodeVal* val;

	SplayNode(NodeVal* _val) : val(_val) {}
	SplayNode*& next(SplayNode* other) {
		return child[val->key < other->val->key];
	}
	bool equals(SplayNode* other) {
		return other ? other->val->key == val->key : false;
	}
	bool getSide() {
		return parent ? parent->child[1] == this : false;
	}
	SplayNode* splay() {
		while (parent) {
			if (parent->parent) {
				(getSide() == parent->getSide() ? parent : this)->rotate();
			}
			rotate();
		}
		return this;
	}
	SplayNode* rotate() {
		bool side = getSide(), parentSide = parent->getSide();
		auto ancestor = parent->parent;
		parent->attach(child[!side], side);
		attach(parent, !side);
		if (ancestor) ancestor->attach(this, parentSide);
		else parent = nullptr;
		return this;
	}
	SplayNode* attach(SplayNode* node, int  side) {
		if (node) node->parent = this;
		child[side] = node;
		val->update((child[0] ? child[0]->val : nullptr), (child[1] ? child[1]->val : nullptr));
		return this;
	}
};

struct TreeNode {
	SplayNode* splayNode;
	NodeVal* val;
};

struct LinkCutTree {
	void addNode(TreeNode &u) {
		u.splayNode = new SplayNode(u.val);
	}

	//Assumes child is a root
	void link(TreeNode &child, TreeNode &parent) {
		assert(findRoot(child) != findRoot(parent));
		access(child); access(parent);
		assert(!child.splayNode->child[0] && !child.splayNode->child[0]);

		child.splayNode->attach(parent.splayNode, 0);
	}

	void cut(TreeNode &u) {
		access(u);
		assert(u.splayNode->child[0]);
		u.splayNode->child[0]->parent = nullptr;
		u.splayNode->child[0] = nullptr;
	}
	
	TreeNode* findRoot(TreeNode &u) {		
		access(u);
		SplayNode* cur = u.splayNode;
		while (cur->child[0]) { cur = cur->child[0]; }
		access(cur);
		//return cur;
	}

	int pathAggregate(TreeNode &u) {

	}

private:
	void access(TreeNode& u) { access(u.splayNode); }
	void access(SplayNode* cur) {
		cur->splay();
		detachChild(cur, 1);

		SplayNode* curPP = cur->pathParentPointer;
		while (curPP) {
			curPP->splay();
			detachChild(curPP, 1);
			curPP->attach(cur, 1);
			cur->pathParentPointer = nullptr;
			cur->splay();

			cur = curPP;
			curPP = cur->pathParentPointer;
		}
	}

	void detachChild(SplayNode* u, bool b) {
		if (!u->child[b]) { return; }

		u->child[b]->pathParentPointer = u;
		u->child[b]->parent = nullptr;
		u->child[b] = nullptr;
	}
};