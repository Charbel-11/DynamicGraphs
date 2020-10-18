#include<iostream>
#include<array>
#include<assert.h>
#include<stack>
#include<string>
#include<vector>
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
			int side = getSide(),  parentSide = parent->getSide();
			auto ancestor = parent->parent;
			parent->attach(child[1 - side], side);
			attach(parent, 1 - side);
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