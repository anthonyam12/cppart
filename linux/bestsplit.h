#pragma once
#include "globals.h"
#include "structures.h"
#include "data_handler.h"
#include "methods.h"
#include <cfloat>
#include <pthread.h>
#include <thread>
#include <string>
#include <unistd.h>
#include <vector>

struct thread_params {
    int varIdx2, numLeft, numRight, respCol, colCount;
    float **L1, **L2;
    params *p;
    void (*split_func) (float*, float*, params*, int&, int&, float&, float&, int);
};

void bestsplit(node *n, params *p, string response, int &numLeft, int &numRight);
void *delayedThread(void *t_params);
void mergeSort(float *x, float *y, int low, int high, int varIdx, int colCount, int respCol);
void merge(float *x, float *y, int low, int high, int mid, int varIdx, int colCount, int respCol);

void getSplitCounts(float **data, int splitVar, float splitPoint, int direction, int numObs, int &leftCount, int &rightCount);
void splitData(int direction, float splitPoint, int splitVar, int colCount, int numObs, float **left, float **right, float **data);

double getSplitCriteria(methods m, int total, int n, float y[]);
