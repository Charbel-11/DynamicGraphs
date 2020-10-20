#include<iostream>
#include<array>
#include<assert.h>
#include<stack>
#include<string>
#include<vector>
#include <algorithm>
using namespace std;

struct SplayTree {
	struct Node {
		array<Node*, 2> child{};
		Node* parent = nullptr;
		int value;

		Node(int value) : value(value){}
		Node*& next(Node* other) {
			return child[value < other->value];
		}
		bool equals(Node* other) {
			return other ? other->value == value : false;
		}
		bool getSide() {
			return parent ? parent->child[1] == this : false;
		}
		Node* splay() {
			while (parent) {
				if (parent->parent) {
					(getSide() == parent->getSide() ? parent : this)->rotate();
				}
				rotate();
			}
			return this;
		}
		Node* rotate() {
			bool side = getSide(),  parentSide = parent->getSide();
			auto ancestor = parent->parent;
			parent->attach(child[!side], side);
			attach(parent, !side);
			if (ancestor) ancestor->attach(this, parentSide);
			else parent = nullptr;
			return this;
		}
		Node* attach(Node* node, int side) {
			if (node) node->parent = this;
			child[side] = node;
			return this;
		}
	};

	Node* root = nullptr;
	SplayTree() {}

	Node* insert(int x) {
		Node* node = new Node(x);
		assert(node);
		if (!root) return root = node;
		Node* cur = root;
		while (true) {
			auto &child = cur->next(node);
			if (!child) {
				child = node;
				node->parent = cur;
				return splay(node);
			}
			cur = child;
		}
	}

	Node* search(int x) {
		Node* node = new Node(x), * cur = root;
		if (!root) return nullptr;
		while (cur) {
			if (cur->equals(node)) return splay(cur);
			else if (!cur->next(node)) {
				splay(cur);
				return nullptr;
			}
			else {
				cur = cur->next(node);
			}
		}
	}

	Node* splay(Node* node) {
		return root = node->splay();
	}

	//Erase x from the splay tree
	void erase(Node* x) {
		if (!x) { return; }
		x->splay();
		Node* lChild = x->child[0], *rChild = x->child[1];
		lChild->parent = rChild->parent = nullptr;
		root = join(lChild, rChild);
		delete x;
	}

	vector<int> inorder() {
		vector<int> ans;
		stack<Node*> st;
		Node* cur = root;
		while (cur) st.push(cur), cur = cur->child[0];
		while (st.size()) {
			cur = st.top(); st.pop();
			ans.push_back(cur->value);
			cur = cur->child[1];
			while (cur) st.push(cur), cur = cur->child[0];
		}
		return ans;
	}

private:
	//Finds the node with max value in the subtree of x
	Node* findMax(Node* x) {
		if (!x) { return nullptr; }
		while (x->child[1]) { x = x->child[1]; }
		return x->splay();
	}
	//Finds the node with min value in the subtree of x
	Node* findMin(Node* x) {
		if (!x) { return nullptr; }
		while (x->child[0]) { x = x->child[0]; }
		return x->splay();
	}
	//Joins 2 splay trees rooted at a and b, assumes that all values in a are smaller than all values in b
	Node* join(Node* a, Node* b) {
		if (!a && !b) { return nullptr; }
		if (!a) { b->parent = nullptr; return b; }
		if (!b) { a->parent = nullptr; return a; }
		Node* newRoot = findMax(a);
		newRoot->attach(b, 1);
		return newRoot;
	}
};

int main() {
	SplayTree splayTree;
	int n; cin >> n;
	vector<int> v;
	for (int i = 0; i < n; i++) {
		v.push_back(rand() % 100000);
		splayTree.insert(v.back());
	}
	cout << "done inserting\n";
	for (auto& it : v) assert(splayTree.search(it));
	sort(v.begin(), v.end());
	vector<int> inorder = splayTree.inorder();
	assert(inorder == v);
}