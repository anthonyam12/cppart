#include <iostream> 
#include <string>
#include <fstream>
#include <sstream>
#include <math.h>

#include "structures.h"
#include "data_handler.h"
#include "partition.h"

using namespace std;

/*
*	Global definitions
*/


/*
*	Function Definition
*/
void printUsage();
int parseParameters(char* argv[], params *p);
node buildTree(params *p, int);
void fixTree(node *root, float cpScale, int nodeId, int nodeCount, int iNode);
cpTable buildCpTable(node *root, params *p);
