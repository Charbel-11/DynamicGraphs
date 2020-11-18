// https://codeforces.com/edu/course/2/lesson/7/3/practice/contest/289392/problem/C
#include<iostream>
#include<vector>
#include<stack>
#include<map>
using namespace std;


struct PersistentDSU {
	int n;
	vector<int> p, size, saves;
	vector<pair<int, int>> pHist, sizeHist;
	int numOfComponents;
	PersistentDSU(int n) : n(n), p(n), size(n, 1){
		for (int i = 0; i < n; i++) p[i] = i;
		numOfComponents = n;
	}

	int get(int a) {
		while (a != p[a]) a = p[a];
		return a;
	}

	void uni(int a, int b) {
		a = get(a), b = get(b);
		if (a != b) {
			if (size[a] > size[b]) swap(a, b);
			pHist.push_back({ a, p[a] });
			sizeHist.push_back({ b, size[b] });
			p[a] = b; size[b] += size[a]; numOfComponents--;
		}
	}

	void save() {
		saves.push_back(pHist.size());
	}

	void rollback() {
		while (pHist.size() > saves.back()) {
			int a = pHist.back().first, b = sizeHist.back().first;
			p[a] = pHist.back().second, size[b] = sizeHist.back().second;
			numOfComponents++;
			pHist.pop_back(), sizeHist.pop_back();
		}
		saves.pop_back();
	}
};

enum QueryType {
	addEdge, numOfComponents, areConnected
};

struct Query {
	QueryType tp;
	int t1, t2, u, v;
	Query(QueryType tp, int t1, int t2, int u, int v) : tp(tp), t1(t1), t2(t2), u(u), v(v){}
};

struct DynamicGraphDSU {
	int n;
	vector<int> ans;
	DynamicGraphDSU(int n) : n(n), ans(n, -1), dsu(n) {}
	
	void link(int u, int v) {
		if (u > v) swap(u, v);
		if (start.count({ u, v }) == 0) {
			start[{u, v}] = t++;
		}
	}

	void cut(int u, int v) {
		if (u > v) swap(u, v);
		queries.push_back(Query(addEdge, start[{u, v}], t++, u, v));
		start.erase({ u, v });
	}

	void getNumOfComponents() {
		queries.push_back(Query(numOfComponents, t, t, -1, -1));
		t++;
	}

	void processAll() {
		ans = vector<int>(t, -1);
		for (auto st : start) {
			queries.push_back(Query(addEdge, st.second, t, st.first.first, st.first.second));
		}
		fillAnswers(0, t, queries);
	}

	void fillAnswers(int t1, int t2, vector<Query> queriesInRange) {
		if (t1 == t2) {
			dsu.save();
			for (auto query : queriesInRange) {
				if (query.tp == addEdge && query.t1 <= t1 && query.t2 >= t2) {
					dsu.uni(query.u, query.v);
				}
			}
			for (auto query : queriesInRange) {
				if (query.t1 >= t1 && query.t2 <= t2) {
					if (query.tp == numOfComponents) {
						ans[query.t1] = dsu.numOfComponents;
					}
					else if (query.tp == areConnected) {
						ans[query.t1] = (dsu.get(query.u) == dsu.get(query.v));
					}
				}
			}
			dsu.rollback();
			return;
		}
		dsu.save();
		vector<Query> nxtQueriesInRange;
		for (auto query : queriesInRange) {
			if (query.tp == addEdge && query.t1 <= t1 && query.t2 >= t2) {
				dsu.uni(query.u, query.v);
			}
			else if ((query.tp == addEdge && !(t2 < query.t1 || t1 > query.t2)) || (query.tp != addEdge && query.t1 >= t1 && query.t2 <= t2)) {
				nxtQueriesInRange.push_back(query);
			}
		}
		int mid = (t1 + t2) / 2;
		fillAnswers(t1, mid, nxtQueriesInRange);
		fillAnswers(mid + 1, t2, nxtQueriesInRange);
		dsu.rollback();
	}
private:
	int t = 0;
	map<pair<int, int>, int> start;
	PersistentDSU dsu;
	vector<Query> queries;
};

int main() {
	std::ios::sync_with_stdio(0);
	std::cin.tie(0);
	std::cout.tie(0);
	
	int n, m; cin >> n >> m;
	DynamicGraphDSU dynamicGraphDSU(n);
	for (int i = 0; i < m; i++) {
		char c; cin >> c;
		if (c == '+') {
			int u, v; cin >> u >> v; u--, v--;
			dynamicGraphDSU.link(u, v);
		}
		else if (c == '-') {
			int u, v; cin >> u >> v; u--, v--;
			dynamicGraphDSU.cut(u, v);
		}
		else {
			dynamicGraphDSU.getNumOfComponents();
		}
	}
	dynamicGraphDSU.processAll();
	for (auto it : dynamicGraphDSU.ans) {
		if (it >= 0) cout << it << "\n";
	}
}