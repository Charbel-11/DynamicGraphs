#include "GraphGenerator.h"

int main() {

	for (int n = 1000000; n <= 1000000; n *= 10) {
		for (int q = 10; q <= 100000; q *= 10) {
			cout << "doing n = " << n << ", q = " << q << "\n";
			GraphGenerator::GenerateTree(10, n, q, "tree_" + to_string(n) + "_" + to_string(q));
		}
	}
}
