#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <assert.h>
using namespace std;
typedef long long ll;

//Building the treap in O(n) doesn't make it that much faster than using the insert 
//at the end of the array n times
struct TreapNode {
	int priority, size, from, to;
	ll val, sum;
	TreapNode* l, * r, * p;

	TreapNode(int _from, int _to, ll _v = 0) :val(_v), priority(rand()),
		l(NULL), r(NULL), p(NULL), size(1), sum(val), from(_from), to(_to) {}
	TreapNode() : TreapNode(0, 0, 0) {}

	void updateSize() {
		size = 1 + (l ? l->size : 0) + (r ? r->size : 0);
	}
	//Combining left subarray, cur element, right subarray
	void combine() {
		sum = val + (l ? l->sum : 0) + (r ? r->sum : 0);
	}
};

int sz(TreapNode* t) { return t ? t->size : 0; }

void operation(TreapNode* t) {
	if (!t) { return; }
	t->combine();
}

//Splits the subtree rooted at t into two trees according pos (pos is in the left subtree)
//It will set l as the root of the l subtree and r as the root of the r subtree
void treapSplit(TreapNode* t, TreapNode*& l, TreapNode*& r, int pos, int add = 0) {
	if (!t) { l = r = NULL; return; }
	int curPos = add + sz(t->l);
	if (curPos < pos) {
		treapSplit(t->r, t->r, r, pos, curPos + 1);
		l = t; if (t->r) { t->r->p = t; }
	}
	else {
		treapSplit(t->l, l, t->l, pos, add);
		r = t; if (t->l) { t->l->p = t; }
	}
	if (t) { t->updateSize(); operation(t); }
	if (l) { l->p = NULL; }
	if (r) { r->p = NULL; }
}

//merges two treaps l and r and stores the root in t
//assumes nodes in l have < keys than nodes in r
void treapMerge(TreapNode*& t, TreapNode* l, TreapNode* r) {
	if (!l || !r) { t = l ? l : r; }
	else if (l->priority > r->priority) {
		treapMerge(l->r, l->r, r);
		t = l; if (l->r) { l->r->p = l; }
	}
	else {
		treapMerge(r->l, l, r->l);
		t = r; if (r->l) { r->l->p = r; }
	}
	if (t) { t->updateSize(); operation(t); }
}

//Inserts x at the end of the array represented by root
void treapInsert(TreapNode*& root, TreapNode* x) {
	if (!root) { root = x; return; }
	treapMerge(root, root, x);
}
//Inserts x at position i (1-indexed) in the array represented by root
void treapInsert(TreapNode*& root, TreapNode* x, int i) {
	if (!root) { root = x; return; }
	TreapNode* lt, * rt;
	treapSplit(root, lt, rt, i - 1);
	treapMerge(root, lt, x);
	treapMerge(root, root, rt);
}
//Removes A[i] (1-indexed) from the array represented by root
void treapErase(TreapNode*& root, int i) {
	TreapNode* lt, * rt, * lt2, * rt2;
	treapSplit(root, lt, rt, i - 1);
	treapSplit(rt, lt2, rt2, 1);
	treapMerge(root, lt, rt2);
}

//Updates A[i] (1-indexed) in the array represented by root
void treapPointUpdate(TreapNode*& root, int i, ll newVal) {
	TreapNode* lt, * rt, * lt2, * rt2;
	treapSplit(root, lt, rt, i - 1); treapSplit(rt, lt2, rt2, 1);
	lt2->val = newVal;
	treapMerge(root, lt, lt2); treapMerge(root, root, rt2);
}

//Returns sum[l,r] (1-indexed) in the array represented by t
ll treapRangeQuery(TreapNode*& t, int l, int r) {
	TreapNode* L, * mid, * R;
	treapSplit(t, L, mid, l - 1); treapSplit(mid, t, R, r - l + 1);
	ll ans = t->sum;
	treapMerge(mid, L, t); treapMerge(t, mid, R);
	return ans;
}

void treapFindIdx(TreapNode*& cur, int& idx) {
	idx = sz(cur->l) + 1; while (cur->p != nullptr) {
		if (cur->p->l == cur) { cur = cur->p; continue; }
		idx += 1 + sz(cur->p->l); cur = cur->p;
	}
}

TreapNode* treapFind(TreapNode* cur, int idx) {
	int s = sz(cur->l); if (s == idx) { return cur; }
	return s < idx ? treapFind(cur->r, idx - s - 1) : treapFind(cur->l, idx);
}

struct Forest {
	vector<vector<int>> adj; int n;
	Forest(int _n) : n(_n), adj(_n) {}

	void addEdge(int u, int v) {
		adj[u].push_back(v);
		adj[v].push_back(u);
	}
};

struct EulerTourTree {
	vector<TreapNode*> self; int n;
	map<pair<int, int>, pair<TreapNode*, TreapNode*>> edges;

	EulerTourTree(Forest F) : n(F.n), self(F.n) {
		vector<bool> vis(n, false);
		for (int i = 0; i < n; i++) if (!vis[i]) {
			TreapNode* root = NULL;	eulerTour(F, root, vis, i);
		}
	}

	void eulerTour(Forest& F, TreapNode*& root, vector<bool>& vis, int cur, int par = -1) {
		TreapNode* curN = new TreapNode(cur, cur);
		treapInsert(root, curN); self[cur] = curN;
		vis[cur] = true;

		for (int& e : F.adj[cur]) if (e != par) {
			TreapNode* inE = new TreapNode(cur, e);
			treapInsert(root, inE);
			eulerTour(F, root, vis, e, cur);
			TreapNode* outE = new TreapNode(e, cur);
			treapInsert(root, outE);
			edges[{cur, e}] = edges[{e, cur}] = { inE, outE };
		}
	}

	bool connected(int u, int v) {
		return findRoot(u) == findRoot(v);
	}

	void link(int par, int child) {
		assert(!edges.count({ par, child }));

		reroot(child); TreapNode* cur = self[par];
		int idx = 0; treapFindIdx(cur, idx);
		TreapNode* left = nullptr, * right = nullptr;
		treapSplit(cur, left, right, idx);

		TreapNode* inE = new TreapNode(par, child);
		TreapNode* outE = new TreapNode(child, par);
		edges[{par, child}] = edges[{child, par}] = { inE, outE };

		treapInsert(left, inE); treapInsert(right, outE, 1);
		TreapNode* root = nullptr, * childRoot = self[child];
		treapFindIdx(childRoot, idx);
		treapMerge(root, left, childRoot);
		treapMerge(root, root, right);
	}

	void cut(int u, int v) {
		assert(edges.count({ u, v }));

		pair<TreapNode*, TreapNode*> p = edges[{u, v}];
		TreapNode* first = p.first, * last = p.second;
		int fID = 0; treapFindIdx(p.first, fID);
		int sID = 0; treapFindIdx(p.second, sID);
		if (fID > sID) { swap(fID, sID); swap(first, last); }

		TreapNode* curLeft = nullptr, * right = nullptr;
		treapSplit(p.first, curLeft, right, sID - 1);
		treapErase(right, 1);

		TreapNode* left = nullptr, * childTree = nullptr;
		treapSplit(curLeft, left, childTree, fID - 1);
		treapErase(childTree, 1);

		TreapNode* root = nullptr;
		treapMerge(root, left, right);

		edges.erase({ u, v }); edges.erase({ v, u });
	}

	ll subtreeAggregate(int cur) {
		return 0;
	}

private:

	TreapNode* findRoot(int cur) {
		TreapNode* curN = self[cur];
		while (curN->p != nullptr) curN = curN->p;
		while (curN->l != nullptr) curN = curN->l;
		return curN;
	}

	void reroot(int cur) {
		TreapNode* firstCur = self[cur];
		int idx = 0; treapFindIdx(firstCur, idx);
		TreapNode* left = nullptr, * right = nullptr;
		treapSplit(firstCur, left, right, idx - 1);
		TreapNode* root = nullptr;
		treapMerge(root, right, left);
	}
};

int main() {
	int n, m; cin >> n >> m; 
	Forest F(n); EulerTourTree ett(F);
	while (m--) {
		string s; int u, v; cin >> s >> u >> v;
		if (s == "add") { ett.link(--u, --v); continue; }
		else if (s == "rem") { ett.cut(--u, --v); continue; }
		else { cout << (ett.connected(--u, --v) ? "YES" : "NO") << '\n'; continue; }
		vector<bool> vis(n, false);
		for (int i = 0; i < n; i++) if (!vis[i]) {
			for(int j = 0; j < n; j++)
				if (ett.connected(i, j)) { vis[j] = true; cout << j << ' '; }
			cout << '\n';
		}
	}
	cin.ignore(2); return 0;
}