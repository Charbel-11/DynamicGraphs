#pragma once
#include "ImplicitTreap.h"
#include <vector>
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
	vector<TreapNode*> first, last; int n;

	EulerTourTree(Forest F) : n(F.n), first(F.n, nullptr), last(F.n, nullptr) {
		for (int i = 0; i < n; i++) if (first[i] == nullptr) {
			TreapNode* root = NULL;	eulerTour(F, root, i);
		}
	}

	void eulerTour(Forest& F, TreapNode*& root, int cur, int par = -1) {
		TreapNode* curN = new TreapNode(); 
		treapInsert(root, curN); 
		first[cur] = curN;

		for (int& e : F.adj[cur]) if(e != par) {
			eulerTour(F, root, e, cur);
			TreapNode* nextN = new TreapNode();
			treapInsert(root, nextN); 
			last[cur] = nextN;
		}

		if (last[cur] == nullptr) { last[cur] = curN; }
	}

	bool connected(int u, int v) {
		return findRoot(u) == findRoot(v);
	}

	void link(int par, int child) {
		TreapNode* lastPar = last[par];
		TreapNode* firstChild = first[child];
		TreapNode* cur = lastPar; int idx = 0; 
		treapFindIdx(cur, idx);

		TreapNode* left = nullptr, * right = nullptr, * root = nullptr;
		treapSplit(cur, left, right, idx);
		treapMerge(root, left, firstChild); //Assumes child is a root of some tree in the forest (To be investigated later...)
		treapInsert(root, last[par] = new TreapNode());
		treapMerge(root, root, right);
	}

	void cut(int par, int child) {
		TreapNode* firstChild = first[child];
		TreapNode* lastChild = last[child];

		TreapNode* firstCur = firstChild; int firstIdx = 0;
		treapFindIdx(firstCur, firstIdx);
		TreapNode* firstLeft = nullptr, * firstRight = nullptr;
		treapSplit(firstCur, firstLeft, firstRight, firstIdx - 1);

		TreapNode* lastCur = lastChild; int lastIdx = 0;
		treapFindIdx(lastCur, lastIdx);
		TreapNode* childTree = nullptr, * lastRight = nullptr;
		treapSplit(lastCur, childTree, lastRight, lastIdx);

		if (first[par] == treapFind(firstLeft, firstIdx))
			first[par] = treapFind(lastRight, 0);
		treapErase(firstLeft, firstIdx - 1);
		
		TreapNode* parentTree = nullptr;
		treapMerge(parentTree, firstLeft, lastRight);
	}

	ll subtreeAggregate(int cur) {
		TreapNode* firstCur = first[cur]; int l = 0;
		TreapNode* lastCur = last[cur]; int r = 0;
		treapFindIdx(firstCur, l); treapFindIdx(lastCur, r);
		return treapRangeQuery(firstCur, l, r);
	}

private:	

	TreapNode* findRoot(int cur) {
		TreapNode* curN = first[cur];
		while (curN->p != nullptr) curN = curN->p;
		while (curN->l != nullptr) curN = curN->l;
		return curN;
	}

	void reroot(int cur) {
		TreapNode* firstCur = first[cur]; int idx = 0;
		treapFindIdx(firstCur, idx);
		treapErase(firstCur, 0); idx--;
		TreapNode* left = nullptr, * right = nullptr, * root = nullptr;
		treapSplit(firstCur, left, right, idx - 1);
		treapMerge(root, right, left);
	}
};