#include "GraphGenerator.h"

int main() {
	int n = 1000, q = 1000 * 1000;
	GraphGenerator::GenerateTree(10, n, q, "tree_" + to_string(n) + "_" + to_string(q));
}
