#pragma once
#include "LinkCutTree.h"
#include <chrono>
#include <random>
#include <fstream>

int main() {
	ios::sync_with_stdio(0);
	cin.tie(0), cout.tie(0);
	
	fstream ifs("tree_tests/tree_1000_1000000");

	double maxTime = 0, avrgTime = 0;
	int t; ifs >> t;
	for (int tt = t; tt > 0; tt--) {
		time_t start = clock();
		int n, q; ifs >> n >> q;
		LinkCutTree LCT;
		for (int i = 0; i < n; i++) {
			LCT.addNode(new NodeVal(i));
		}

		while (q--) {
			int op, u, v; ifs >> op >> u >> v;
			if (op == 1) {
				if (LCT.findRoot(u) == LCT.findRoot(v)) { continue; }
				LCT.link(u, v);
			}
			else if (op == 2) {
				LCT.cut(u, v);
			}
			else {
				LCT.findRoot(u); LCT.findRoot(v);
			}
		}

		time_t finish = clock();
		double elapsed = 1000.0 * (finish - start) / CLOCKS_PER_SEC; //ms
		maxTime = max(maxTime, elapsed);
		avrgTime += elapsed;
		cout << "Finished " << t - tt + 1 << '\n';
	}
	avrgTime /= t;

	cout << avrgTime << " " << maxTime << '\n';
}
