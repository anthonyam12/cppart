#include "cppart.h"

int main(int argc, char* argv[]) {
	if (argc < 4) {
		printUsage();
		exit(0);
	}

	params p;
	int numObs = parseParameters(argv, &p);
	node root = buildTree(&p, numObs);
	buildCpTable(&root, &p);

	return 0;
}

void printUsage()
{
	cout << "Usage:" << endl;
	cout << "\tcpart.exe <data filename> <response> <delayed>" << endl;
}

// returns the number of observations for the first node.
int parseParameters(char * argv[], params *p)
{
	string filename = argv[1];
	string response = argv[2];

	int delayed = 0;
	try {
		delayed = stoi(argv[3]);
	} catch(exception e) {
		cout << "Could not parse integer value from delayed parameter \" " << argv[3] << "\"." << endl;
		printUsage();
		exit(0);
	}

	ifstream fin;
	fin.open(filename);
	if (!fin.is_open()) {
		cout << "File \"" << filename << "\" does not exist." << endl;
		exit(0);
	}

	string headers;
	bool responseFound = false;
	getline(fin, headers);
	string value;
	istringstream ss(headers);
	while (getline(ss, value, ',') && !responseFound) {
		responseFound = !strcmp(value.c_str(), response.c_str());
	}

	if (!responseFound) {
		cout << "Response column not found in dataset, does your data have headers?" << endl;
		exit(0);
	}
	fin.close();

	int lineCount = getLineCount(filename);
	int colCount = getColumnCount(headers);

	float **data = new float*[lineCount - 1];
	for (int i = 0; i < lineCount; i++) {
		data[i] = new float[colCount];
	}

	getData(filename, response, headers, data);

	p->response = response;
	p->maxDepth = 30;	// only used to set maxNodes
	p->maxNodes = (int)pow(2, (p->maxDepth + 1)) - 1;
	p->minObs = 20;
	p->minNode = 7;
	p->numXval = 10;
	p->iscale = 0;		// this may not be used
	p->delayed = delayed == 1;
	p->data = data;
	p->where = new int[lineCount];
	p->headers = headers;

	for (int i = 0; i < lineCount; i++) {
		p->where[i] = 0;	
	}

	return lineCount - 1;
}

node buildTree(params * p, int numObs)
{
	node tree;
	tree.numObs = numObs;
	tree.data = p->data;

	int splits;
	double risk;
	partition(p, &tree, 1, risk);

	return tree;
}

void fixTree(node * root, float cpScale, int nodeId, int nodeCount, int iNode)
{
}

cpTable buildCpTable(node *root, params *p)
{
	cpTable cpTableHead;

	cpTableHead.cp = 0;

	return cpTableHead;
}
	