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
		return this;
	}
};

struct LinkCutTreeGeneral {
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
		access(u);
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
		if (!u) { return nullptr; }
		while (u->child[1]) { u = u->child[1]; }
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

int dx[] = { 0,0,1,-1 };
int dy[] = { 1,-1,0,0 };
vector<int> numToID;
int n, m;

void cutNeighbors(DynamicTreeLCT& lct, vector<vector<int>>& grid, int l, int r) {
	int i = numToID[l] / m, j = numToID[l] % m;
	int idx = numToID[l];

	for (int k = 0; k < 4; k++) {
		int ni = i + dx[k], nj = j + dy[k];
		if (ni < 0 || nj < 0 || ni == n || nj == m) { continue; }
		if (grid[ni][nj] < l || grid[ni][nj] > r) { continue; }

		int nIdx = numToID[grid[ni][nj]];
		if (lct.connected(idx, nIdx)) { lct.cut(idx, nIdx); }
	}
}

int main() {
	ios::sync_with_stdio(0);
	cin.tie(0), cout.tie(0);

	DynamicTreeLCT lct;

	cin >> n >> m;
	vector<vector<int>> grid(n, vector<int>(m));
	numToID.resize(n * m + 1);
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			cin >> grid[i][j];
			numToID[grid[i][j]] = lct.addNode();
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
			while (lct.connected(idx, nIdx)) {
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