#include<iostream>
#include"DynamicTreeLCT.h"
using namespace std;

void basicConnectivityTest() {
	DynamicTreeLCT dynamicTree;
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

int main() {
	basicConnectivityTest();
	cout << "success!\n";
}