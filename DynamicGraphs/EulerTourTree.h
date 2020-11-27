#pragma once
#include "ImplicitTreap.h"
#include <vector>
#include <unordered_map>
#include <assert.h>
using namespace std;

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
	unordered_map<pair<int, int>, pair<TreapNode*, TreapNode*>> edges;

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

		for (int& e : F.adj[cur]) if(e != par) {
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