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
		long long value, subtreeSize;

		Node(long long value) : value(value), subtreeSize(1) {}
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
			bool side = getSide(), parentSide = parent->getSide();
			auto ancestor = parent->parent;
			parent->attach(child[!side], side);
			attach(parent, !side);
			if (ancestor) ancestor->attach(this, parentSide);
			else parent = nullptr;
			return this;
		}
		Node* attach(Node* node, long long side) {
			if (node) node->parent = this;
			child[side] = node;
			update();
			return this;
		}
		void update() {
			subtreeSize = 1 + (child[0] ? child[0]->subtreeSize : 0) + (child[1] ? child[1]->subtreeSize : 0);
		}
	};

	Node* root = nullptr;
	SplayTree() {}

	Node* insert(long long x) {
		Node* node = new Node(x);
		assert(node);
		if (!root) return root = node;
		Node* cur = root;
		while (true) {
			if (cur->equals(node)) {
				return splay(cur);
			}
			auto& child = cur->next(node);
			if (!child) {
				child = node;
				node->parent = cur;
				return splay(node);
			}
			cur = child;
		}
	}

	Node* find(long long x) {
		Node* node = new Node(x), * cur = root;
		while (cur) {
			if (cur->equals(node)) {
				cur = splay(cur);
				break;
			}
			else if (!cur->next(node)) {
				splay(cur);
				cur = nullptr;
			}
			else {
				cur = cur->next(node);
			}
		}
		delete node;
		return cur;
	}

	Node* lowerBound(long long x) {
		Node* node = new Node(x), * cur = root, *ans = nullptr;
		while (cur) {
			if (cur->equals(node)) {
				ans = cur;
				break;
			}
			else if (cur->value > x) {
				ans = cur;
				if (cur->next(node)) cur = cur->next(node);
				else break;
			}
			else {
				cur = cur->next(node);
			}
		}
		return splay(ans);
	}

	Node* kth(int k) {
		Node* cur = root, * ans = nullptr;
		while (cur) {
			int leftSize = (cur->child[0] ? cur->child[0]->subtreeSize : 0);
			if (leftSize == k - 1) {
				ans = cur;
				break;
			}
			else if (leftSize >= k) cur = cur->child[0];
			else {
				k -= (leftSize + 1);
				cur = cur->child[1];
			}
		}
		return ans;
	}

	Node* splay(Node* node) {
		return node ? root = node->splay() : nullptr;
	}

	//Erase x from the splay tree
	void erase(Node* x) {
		if (!x) { return; }
		x->splay();
		Node* lChild = x->child[0], * rChild = x->child[1];
		if (lChild) lChild->parent = nullptr; 
		if(rChild) rChild->parent = nullptr;
		root = join(lChild, rChild);
		delete x;
	}

	int findOrder(long long x) {
		Node* v = find(x);
		if (!v) {
			return -1;
		}
		else if (v->child[0]) {
			return v->child[0]->subtreeSize + 1;
		}
		else {
			return 1;
		}
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
	std::ios::sync_with_stdio(0);
	std::cin.tie(0);
	std::cout.tie(0);
	int Q; cin >> Q;
	SplayTree splayTree;
	while (Q--) {
		char tp; cin >> tp;
		long long x; cin >> x;
		if (tp == 'I') {
			splayTree.insert(x);
		}
		else if (tp == 'D') {
			auto X = splayTree.find(x);
			if(X) splayTree.erase(X);
		}
		else if (tp == 'K') {
			auto kth = splayTree.kth(x);
			if (!kth) {
				cout << "invalid\n";
			}
			else {
				cout << kth->value  << "\n";
			}
		}
		else {
			auto X = splayTree.lowerBound(x);
			if (!X) {
				cout << splayTree.root->subtreeSize<<"\n";
			}
			else {
				cout << (X->child[0] ? X->child[0]->subtreeSize : 0)<<"\n";
			}
		}
	}
}