//https://codeforces.com/contest/1109/problem/F
#include <iostream>
#include<array>
#include <set>
#include<assert.h>
#include<stack>
#include<string>
#include<vector>
#include <algorithm>
using namespace std;

struct StVal {
	int minV = 0, numMin = 0; StVal() {}
	StVal(const int _v) : minV(_v), numMin(1) { }
	StVal(const StVal& v1, const StVal& v2) {
		if (v1.minV < v2.minV) { minV = v1.minV; numMin = v1.numMin; }
		else if (v1.minV > v2.minV) { minV = v2.minV; numMin = v2.numMin; }
		else { minV = v1.minV;	numMin = v1.numMin + v2.numMin; }
	}
};

struct StUpdate {
	int v = 0; StUpdate() {}
	StUpdate(const int v) : v(v) {}
	StUpdate(const StUpdate& u1, const StUpdate& u2) { v = u1.v + u2.v; }
	void apply(StVal& sv, const int lo, const int hi) {
		sv.minV += v;
	}
};

struct SegTree {
	int n;
	vector<StVal> st;
	vector<StUpdate> lazy;
	vector<int> leaves;

	SegTree(const int n) : n(n) {
		leaves.resize(n);
		init(1, 0, n - 1);
		lazy.resize(st.size());
	}

	//lo, hi indices of the actual array; si is the index of the segment tree array
	void init(const int si, const int lo, const int hi) {
		if (lo == hi) {
			if (si >= (int)st.size()) { st.resize(si + 1); }
			st[si] = StVal(0);			//Set the actual array here
			leaves[lo] = si;
			return;
		}
		int lC = si << 1, rC = (si << 1) | 1, mid = (lo + hi) >> 1;
		init(lC, lo, mid); init(rC, mid + 1, hi);
		st[si] = StVal(st[lC], st[rC]);
	}

	void updateLazy(const int si, const int lo, const int hi) {
		lazy[si].apply(st[si], lo, hi);
		if (lo != hi) {
			lazy[si << 1] = StUpdate(lazy[si << 1], lazy[si]);
			lazy[si << 1 | 1] = StUpdate(lazy[si << 1 | 1], lazy[si]);
		}
		lazy[si] = StUpdate();
	}

	StVal query(const int l, const int r) { return (l <= r && l < n&& r >= 0) ? query(l, r, 1, 0, n - 1) : StVal(); }
	StVal query(const int l, const int r, const int si, const int lo, const int hi) {
		updateLazy(si, lo, hi);
		if (l <= lo && hi <= r) return st[si];

		const int mid = (lo + hi) >> 1;
		if (r <= mid) return query(l, r, si << 1, lo, mid);
		if (mid < l) return query(l, r, si << 1 | 1, mid + 1, hi);
		return StVal(query(l, r, si << 1, lo, mid), query(l, r, si << 1 | 1, mid + 1, hi));
	}

	void update(const int l, const int r, const StUpdate u) { if (l <= r) update(l, r, u, 1, 0, n - 1); }
	void update(const int l, const int r, const StUpdate& u, const int si, const int lo, const int hi) {
		if (l <= lo && hi <= r) {
			lazy[si] = StUpdate(lazy[si], u);
			updateLazy(si, lo, hi);
		}
		else {
			updateLazy(si, lo, hi);
			if (hi < l || r < lo) return;
			const int mid = (lo + hi) >> 1;
			update(l, r, u, si << 1, lo, mid);
			update(l, r, u, si << 1 | 1, mid + 1, hi);
			st[si] = StVal(st[si << 1], st[si << 1 | 1]);
		}
	}

	void updateAllLazy() { updateAllLazy(1, 0, n - 1); }
	void updateAllLazy(const int si, const int lo, const int hi) {
		updateLazy(si, lo, hi);
		if (lo != hi) {
			const int mid = (lo + hi) >> 1;
			updateAllLazy(si << 1, lo, mid);
			updateAllLazy(si << 1 | 1, mid + 1, hi);
		}
	}

};

struct NodeVal {
	int subtreeSize;
	NodeVal() : subtreeSize(1) {}
	// update should be symmetric with respect to lchild and rchild if we wish to have the ability to link any 2 nodes
	// i.e. update(x, y) == update(y, x) 
	void update(NodeVal* lChild, NodeVal* rChild) {
		subtreeSize = 1 + (lChild ? lChild->subtreeSize : 0) + (rChild ? rChild->subtreeSize : 0);
	}
};

struct Node {
	int id;
	array<Node*, 2> child{};
	Node* splayTreeParent = nullptr;
	Node* pathParentPointer = nullptr;
	NodeVal* val;
	bool reverse = false;

	Node(int id, NodeVal* _val) : id(id), val(_val) {}
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
		val->update((child[0] ? child[0]->val : nullptr), (child[1] ? child[1]->val : nullptr));
		return this;
	}

	void detachChild(bool b) {
		if (!child[b]) { return; }
		child[b]->pathParentPointer = this;
		child[b]->splayTreeParent = nullptr;
		child[b] = nullptr;
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

	// special cut where cut does not do anything if there is no edge between id1 and id2
	void cut(int id1, int id2) {
		Node* u = nodes[id1], * v = nodes[id2];
		access(u);
		if (u->child[0] && u->child[0]->findMax() == v) { cut(u->id); return; }
		access(v);
		if (v->child[0] && v->child[0]->findMax() == u) { cut(v->id); return; }
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

	int pathAggregate(int id) {
		Node* u = nodes[id];
		access(u);
		return u->id;
	}

	Node* getNode(int id) {
		return nodes[id];
	}

private:
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

int dx[] = { 0,0,1,-1 };
int dy[] = { 1,-1,0,0 };
vector<int> numToID;
int n, m;

void cutNeighbors(LinkCutTree& lct, vector<vector<int>>& grid, int l, int r) {
	int i = numToID[l] / m, j = numToID[l] % m;
	int idx = numToID[l];

	for (int k = 0; k < 4; k++) {
		int ni = i + dx[k], nj = j + dy[k];
		if (ni < 0 || nj < 0 || ni == n || nj == m) { continue; }
		if (grid[ni][nj] < l || grid[ni][nj] > r) { continue; }

		int nIdx = numToID[grid[ni][nj]];
		lct.cut(idx, nIdx);
	}
}

int main() {
	ios::sync_with_stdio(0);
	cin.tie(0), cout.tie(0);

	LinkCutTree lct;

	cin >> n >> m;
	vector<vector<int>> grid(n, vector<int>(m));
	numToID.resize(n * m + 1);
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			cin >> grid[i][j];
			numToID[grid[i][j]] = lct.addNode(new NodeVal());
		}
	}

	SegTree st(n * m + 1);
	long long ans = 0;
	int l = 1;
	for (int r = 1; r <= m * n; r++) {
		st.update(l, r, StUpdate(1));

		int i = numToID[r] / m, j = numToID[r] % m;
		int idx = numToID[r];

		for (int k = 0; k < 4; k++) {
			int ni = i + dx[k], nj = j + dy[k];
			if (ni < 0 || nj < 0 || ni == n || nj == m) { continue; }
			if (grid[ni][nj] < l || grid[ni][nj] > r) { continue; }

			int nIdx = numToID[grid[ni][nj]];
			while (lct.findRoot(idx) == lct.findRoot(nIdx)) {
				cutNeighbors(lct, grid, l, r);
				l++;
			}

			if (l > grid[ni][nj]) { continue; }
			if (l == r) { break; }

			lct.link(idx, nIdx);
			st.update(l, grid[ni][nj], StUpdate(-1));
		}

		ans += st.query(l, r).numMin;
	}

	cout << ans << '\n';
}