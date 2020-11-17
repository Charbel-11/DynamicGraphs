#pragma once
#include <iostream>
#include "LinkCutTree.h"
using namespace std;

void basicConnectivityTest() {
	LinkCutTree dynamicTree;
	for (int i = 0; i < 5; i++) dynamicTree.addNode(new NodeVal());
	for (int i = 0; i < 4; i++) dynamicTree.link(i, i + 1);
	for (int i = 0; i < 5; i++) {
		for (int j = i; j < 5; j++) {
			assert(dynamicTree.findRoot(i) == dynamicTree.findRoot(j));
		}
	}
	dynamicTree.cut(2);
	for (int i = 0; i < 2; i++) {
		for (int j = i; j < 2; j++) {
			assert(dynamicTree.findRoot(i) == dynamicTree.findRoot(j));
		}
	}
	for (int i = 2; i < 5; i++) {
		for (int j = 2; j < 5; j++) {
			assert(dynamicTree.findRoot(i) == dynamicTree.findRoot(j));
		}
	}
	for (int i = 0; i < 2; i++) {
		for (int j = 2; j < 5; j++) {
			assert(dynamicTree.findRoot(i) != dynamicTree.findRoot(j));
		}
	}
	dynamicTree.link(0, 2);
	for (int i = 0; i < 5; i++) {
		for (int j = i; j < 5; j++) {
			assert(dynamicTree.findRoot(i) == dynamicTree.findRoot(j));
		}
	}
}


void basicHeightTest() {
	LinkCutTree dynamicTree;
	for (int i = 0; i < 5; i++) dynamicTree.addNode(new NodeVal());
	for (int i = 0; i < 4; i++) dynamicTree.link(i, i + 1);
	auto height = [&](int id) {
		auto res = dynamicTree.getNode(dynamicTree.pathAggregate(id));
		return 1 + (res->child[0] ? (res->child[0]->val)->subtreeSize : 0);
	};
	for (int i = 0; i < 5; i++) {
		assert(height(i) == i + 1);
	}
	dynamicTree.cut(2);
	for (int i = 0; i < 2; i++) {
		assert(height(i) == i + 1);
	}
	for (int i = 2; i < 5; i++) {
		assert(height(i) == i - 1);
	}
	dynamicTree.link(0, 2);
	assert(height(0) == 1);
	assert(height(1) == 2);
	assert(height(2) == 2);
	assert(height(3) == 3);
	assert(height(4) == 4);
}

int main() {
	basicConnectivityTest();
	basicHeightTest();
	cout << "success!\n";
}