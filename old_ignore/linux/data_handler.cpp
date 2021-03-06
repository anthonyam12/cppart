/*
 * Handles data input and manipulation. Gets desired columns, finds
 * response column, etc.
 *
 * Author: Anthony Morast
 */


#include "data_handler.h"

/*
 *	Function Implementation
 */
/*
 *	Gets the x data, returns the number of observations in the dataset.
 */
void getData(string filename, string response, string headers,  float** x) {
    ifstream fin;
    fin.open(filename);
    if (!fin.is_open()) {
        cout << "Error opening file " << filename << endl;
        cout << "Does the file exist?" << endl;
        exit(0);
    }

    string line;
    int row = 0;
    getline(fin, line);
    while (getline(fin, line)) {
        istringstream ss(line);
        string value;
        int curCol = 0;
        while (getline(ss, value, ',')) {
            x[row][curCol] = stof(value);
            curCol++;
        }
        row++;
    }

    fin.close();
}

/*
 *	Gets the y data, returns the number of observations in the dataset.
 */
float* getResponseData(string response, string headers, float** data, int numObs) {
    int responseColumn = getResponseColumnNumber(response, headers);
    float *y = new float[numObs];

    for (int i = 0; i < numObs; i++) {
        y[i] = data[i][responseColumn];
    }

    return y;
}

/*
 *	Get everything except the response variable column.
 */
float *getExplanatoryDataCol(string response, string headers, float **data, int numObs, int col) {
    int responseColumn = getResponseColumnNumber(response, headers);
    int colCount = getColumnCount(headers);

    float *x = new float[numObs];
    for (int i = 0; i < numObs; i++) {
        x[i] = data[i][col];
    }

    return x;
}


float getMean(float ** data, string response, string headers, int numObs){
    float *y = getResponseData(response, headers, data, numObs);
    float mean = 0;
    for (int i = 0; i < numObs; i++) {
        mean += y[i];
    }

    mean /= numObs;
    free1DData(y);
    return mean;
}


int getResponseColumnNumber(string response, string headers) {
    istringstream ss(headers);
    string value;

    int index = 0;
    while (getline(ss, value, ',')) {
        if (!strcmp(value.c_str(), response.c_str())) {
            break;
        }
        index++;
    }

    return index;
}

int getLineCount(string filename) {
    int count = 0;

    ifstream fin;
    fin.open(filename);

    string line;
    while (getline(fin, line)) {
        count++;
    }

    return count;
}

int getColumnCount(string headers) {
    int count = 0;

    istringstream ss(headers);
    string value;
    while (getline(ss, value, ',')) {
        count++;
    }

    return count;
}

void free1DData(float* y) {
    delete[] y;
    y = NULL;
}

void free2DData(float** data, int numObs) {
    for(int i = 0; i < numObs; i++) {
        delete[] data[i];
    }
    delete[] data;
    data = NULL;
}

void freeTreeData(node *root) {
    free2DData(root->data, root->numObs);

    if(root->leftNode != NULL) {
        freeTreeData(root->leftNode);
    }
    if(root->rightNode != NULL) {
        freeTreeData(root->rightNode);
    }
}


float** deepCopyData(float** old_data, int numObs, int colCount) {
    float** new_data = new float*[numObs];
    for(int i = 0; i < numObs; i++) {
        new_data[i] = new float[colCount];
    }
    for(int i = 0; i < numObs; i++) {
        for (int j = 0; j < colCount; j++) {
            new_data[i][j] = old_data[i][j];
        }
    }
    return new_data;
}
