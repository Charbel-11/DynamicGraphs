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

struct Node {
	array<Node*, 2> child{};
	Node* splayTreeParent = nullptr;
	Node* pathParentPointer = nullptr;
	bool reverse = false;

	Node() {}
	bool getSide() {
		return splayTreeParent ? splayTreeParent->child[1] == this : false;
	}
	void tryReverse() {
		if (!reverse) { return; }
		reverse = false;
		swap(child[0], child[1]);
		if (child[0] != nullptr) { child[0]->reverse = !child[0]->reverse; }
		if (child[1] != nullptr) { child[1]->reverse = !child[1]->reverse; }
	}
	Node* splay() {
		vector<Node*> ancestors;
		Node* cur = this;
		while (cur != nullptr) { ancestors.push_back(cur); cur = cur->splayTreeParent; }
		for (int i = ancestors.size() - 1; i >= 0; i--) {
			ancestors[i]->tryReverse();
		}

		while (splayTreeParent != nullptr) {
			if (splayTreeParent->splayTreeParent != nullptr) {
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

		if (ancestor != nullptr) ancestor->attach(this, parentSide);
		else splayTreeParent = nullptr;
		return this;
	}
	Node* attach(Node* node, int side) {
		if (node != nullptr) node->splayTreeParent = this;
		child[side] = node;
		return this;
	}
};

struct LinkCutTreeGeneral {
	void link(Node* parent, Node* child) {
		if (!child || !parent) { return; }
		if (findRoot(child) == findRoot(parent)) { return; }
		//		while(findRoot(child) == findRoot(parent));
		access(child); access(parent);

		Node* lChild = child->child[0];
		if (lChild != nullptr) {
			lChild->reverse = !lChild->reverse;
			detachChild(child, 0);
		}

		child->attach(parent, 0);
	}

	void cut(Node* u) {
		if (!u) { return; }
		access(u);
		if (!(u->child[0])) { return; }
		u->child[0]->splayTreeParent = nullptr;
		u->child[0] = nullptr;
	}

	void cut(Node* u, Node* v) {
		if (!u || !v) { return; }
		access(v); access(u);
		if (u->child[0] != nullptr && findMax(u->child[0]) == v) { cut(u); return; }
		access(v);
		if (v->child[0] != nullptr && findMax(v->child[0]) == u) { cut(v); }
	}

	//Finds the root of u in the represented tree
	Node* findRoot(Node* u) {
		if (!u) { return nullptr; }
		access(u);
		while (u->child[0] != nullptr) { u = u->child[0]; }
		access(u);
		return u;
	}

private:
	//Returns the last path Parent Pointer
	Node* access(Node* u) {
		if (!u) { return nullptr; }
		u->splay();
		detachChild(u, 1);

		Node* curPP = u;
		while (u->pathParentPointer != nullptr) {
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
		if (!u) { return nullptr; }
		while (u->child[1] != nullptr) { u = u->child[1]; }
		u->splay();
		return u;
	}

	void detachChild(Node* u, bool b) {
		if (!u) { return; }
		if (!u->child[b]) { return; }

		u->child[b]->pathParentPointer = u;
		u->child[b]->splayTreeParent = nullptr;
		u->child[b] = nullptr;
	}
};

struct NodeWithId : Node {
	int id;
	NodeWithId(int id) : Node(), id(id) {}
};

struct DynamicTreeLCT {
	LinkCutTreeGeneral lct;
	vector<NodeWithId*> nodes;
	int n = 0;
	DynamicTreeLCT() {}

	// returns Id of newly added node
	int addNode() {
		nodes.push_back(new NodeWithId(n));
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
		//while (u < 0 || v < 0 || u >= n || v >= n);
		lct.cut(nodes[u], nodes[v]);
	}

	int findRoot(int u) {
		auto res = (NodeWithId*)lct.findRoot(nodes[u]);
		return res->id;
	}

	bool connected(int u, int v) {
		return findRoot(u) == findRoot(v);
	}

};
int main() {
	ios::sync_with_stdio(0);
	cin.tie(0), cout.tie(0);

	DynamicTreeLCT dynamicTree;

	int n, m; cin >> n >> m;
	for (int i = 0; i < n; i++) { dynamicTree.addNode(); }
	while (m--) {
		string type; cin >> type;
		int u, v; cin >> u >> v; u--; v--;
		if (type == "conn") {
			if (dynamicTree.connected(u, v)) { cout << "YES\n"; }
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