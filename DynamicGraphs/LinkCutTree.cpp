#include "SplayTree.h"


template <class T>
struct TreeNode {
	TreeNode* parent;
	SplayTree* splayTree;
	T value;
};

template <class T>
struct LinkCutTree {
	void addNode(TreeNode u) {
		u.splayTree = new SplayTree();
		u.splayTree->insert();
	}

	void link(TreeNode u, TreeNode v) {
		// assert v is root and u, v in different represented trees
	}

	void cut(TreeNode u) {

	}
	
	TreeNode findRoot(TreeNode u) {

	}

	int pathAggregate(TreeNode u) {

	}

private:
	void access(TreeNode u) {

	}
};