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
		int  value, subtreeSize;

		Node(int  value) : value(value), subtreeSize(1) {}
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
		Node* attach(Node* node, int  side) {
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

	Node* insert(int  x) {
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

	Node* find(int  x) {
		Node* node = new Node(x), * cur = root;
		while (cur) {
			if (cur->equals(node)) {
				break;
			}
			else if (!cur->next(node)) {
				splay(cur);
			}
			cur = cur->next(node);
		}
		//sss
		delete node;
		return splay(cur);
	}

	Node* lowerBound(int  x) {
		Node* node = new Node(x), * cur = root, * ans = nullptr;
		while (cur) {
			if (cur->equals(node)) {
				ans = cur;
				splay(ans);
				break;
			}
			else if (x < cur->value) {
				ans = cur;
				if (cur->next(node)) cur = cur->next(node);
				else {
					splay(ans);
					break;
				}
			}
			else {
				if (!cur->next(node)) splay(cur);
				cur = cur->next(node);
			}
		}
		return ans;
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
				if (!cur->child[1]) splay(cur);
				cur = cur->child[1];
			}
		}
		return splay(ans);
	}

	Node* splay(Node* node) {
		return node ? root = node->splay() : nullptr;
	}

	//Erase x from the splay tree
	void erase(Node* x) {
		if (!x) { return; }
		splay(x);
		Node* lChild = x->child[0], * rChild = x->child[1];
		if (lChild) lChild->parent = nullptr;
		if (rChild) rChild->parent = nullptr;
		root = join(lChild, rChild);
		delete x;
	}

	int findOrder(int  x) {
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

#define _CRT_SECURE_NO_WARNINGS

int main() {
	int Q; scanf("%d", &Q);
	SplayTree splayTree;
	while (Q--) {
		char tp; scanf("%c%d", &tp);
		int  x; scanf("%d", &x);
		if (tp == 'I') {
			splayTree.insert(x);
		}
		else if (tp == 'D') {
			auto X = splayTree.find(x);
			if (X) splayTree.erase(X);
		}
		else if (tp == 'K') {
			auto kth = splayTree.kth(x);
			if (!kth) {
				printf("invalid\n");
			}
			else {
				printf("%d\n", kth->value);
			}
		}
		else {
			auto X = splayTree.lowerBound(x);
			if (!X) {
				printf("%d\n", splayTree.root->subtreeSize);
			}
			else {
				printf("%d\n", (X->child[0] ? X->child[0]->subtreeSize : 0));
			}
		}
	}
}