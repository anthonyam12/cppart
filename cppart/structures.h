#pragma once
#include <string>

using namespace std;

/*
*	Struct and class definitions that are not used in any particular functionality.
*/
struct params {
	unsigned int maxNodes;
	unsigned int minObs;
	string response;
	string headers;
	unsigned int minNode;	// minimum number of obs for terminal node
	unsigned int maxDepth;
	bool delayed;
	unsigned int numXval;
	float iscale;
	int* where;
	float** data;	// 1 row in the dataframe
};

struct node {
	struct node *rightNode;
	struct node *leftNode;

	unsigned int nodeId = 0;
	float splitPoint;
	int index;
	int direction;
	int varIndex;

	float cp;
	int numObs;
	float dev;
	float yval;
	float improvement;

	float** data;
	string response;
};

struct cpTable {
	float cp;
	float risk;
	float xrisk;
	float xstd;
	int nsplit;

	struct cpTable *forward;
	struct cpTable *back;
};