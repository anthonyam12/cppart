/*
 * The heart and soul of the greedy algorithm. The only affected piece 
 * of logic is the selection of the best split. Creates partitions
 * of the dataset based on ANOVA splitting criterion for greedy and
 * non-greedy trees.
 *
 * Author: Anthony Morast
 */


#include "bestsplit.h"

void bestsplit(node *n, params *p, string response, int & numleft, int & numright)
{
    void (*split_func) (float*, float*, params*, int&, int&, float&, float&, int);
    void (*ss_func) (float*, int, double&, double&);
    if(p->method == ANOVA) {
        split_func = anovaSplit;
        ss_func = anovaSS;
    } else {
        split_func = giniSplit;
        ss_func = giniDev;
    }

    float **data = n->data;
    n->rightNode = new node;
    n->leftNode = new node;

    int colCount = getColumnCount(p->headers);
    int respCol = getResponseColumnNumber(p->response, p->headers);
    int numObs = n->numObs;
    double bestSS = DBL_MAX;
    double baseSS = bestSS;
    double yBar, deviance;
    double bestImprove = 0;

    float* x;
    float* y = getResponseData(p->response, p->headers, data, numObs);
    (*ss_func)(y, numObs, yBar, deviance);
    free1DData(y);
    for (int varIdx = 0; varIdx < colCount; varIdx++) {
        if (varIdx == respCol) {
            continue;
        }

        x = getExplanatoryDataCol(p->response, p->headers, data, numObs, varIdx);
        y = getResponseData(p->response, p->headers, data, numObs);
        mergeSort(x, y, 0, numObs-1, varIdx, colCount, respCol); // -1 since we go until <= high

        // call anova split point
        int where, direction;
        float splitPoint, improve;
        double leftSS, rightSS, mean, thisSS, thisSSLeft, thisSSRight;

        (*split_func)(x, y, p, where, direction, splitPoint, improve, numObs);
        free1DData(y);
        free1DData(x);

        // split data (set left, right, numLeft, numRight)
        int numLeft = 0;
        int numRight = 0;
        getSplitCounts(data, varIdx, splitPoint, direction, numObs, numLeft, numRight);

        float **L1 = new float*[numLeft];
        float **L2 = new float*[numRight];
        for (int i = 0; i < numLeft; i++) {
            L1[i] = new float[colCount];
        }for (int i = 0; i < numRight; i++) {
            L2[i] = new float[colCount];
        }

        // split the data
        splitData(direction, splitPoint, varIdx, colCount, numObs, L1, L2, data);
        if(p->verbose > 1) {
            cout << "\t\tVar 1: " << p->varNames[varIdx] << endl;
        }
        if (p->delayed && improve > 0) {
            double bestLeftSS = DBL_MAX, bestRightSS = DBL_MAX;

            for (int varIdx2 = 0; varIdx2 < colCount; varIdx2++) {
                if(varIdx2 == respCol) {
                    continue;
                }
                x = getExplanatoryDataCol(p->response, p->headers, L1, numLeft, varIdx2);
                y = getResponseData(p->response, p->headers, L1, numLeft);
                mergeSort(x, y, 0, numLeft-1, varIdx2, colCount, respCol);

                int whereL, directionL, whereR, directionR;
                float splitPointL = 0, improveL, splitPointR = 0, improveR;

                // left data
                (*split_func)(x, y, p, whereL, directionL, splitPointL, improveL, numLeft);
                free1DData(y);
                free1DData(x);

                int L3Size = 0, L4Size = 0;
                getSplitCounts(L1, varIdx2, splitPointL, directionL, numLeft, L3Size, L4Size);
                float **L3 = new float*[L3Size];
                float **L4 = new float*[L4Size];
                for (int i = 0; i < L3Size; i++) {
                    L3[i] = new float[colCount];
                }
                for (int i = 0; i < L4Size; i++) {
                    L4[i] = new float[colCount];
                }

                splitData(directionL, splitPointL, varIdx2, colCount, numLeft, L3, L4, L1);
                double l3SS, l4SS;
                y = getResponseData(p->response, p->headers, L3, L3Size);
                (*ss_func)(y, L3Size, mean, l3SS);
                free1DData(y);
                
                y = getResponseData(p->response, p->headers, L4, L4Size);
                (*ss_func)(y, L4Size, mean, l4SS);
                free1DData(y);

                thisSSLeft = l3SS + l4SS;

                if (thisSSLeft < bestLeftSS && improveL > 0) {
                    bestLeftSS = thisSSLeft;
                }

                // right data
                x = getExplanatoryDataCol(p->response, p->headers, L2, numRight, varIdx2);
                y = getResponseData(p->response, p->headers, L2, numRight);
                mergeSort(x, y, 0, numRight-1, varIdx2, colCount, respCol);

                (*split_func)(x, y, p, whereR, directionR, splitPointR, improveR, numRight);
                free1DData(y);
                free1DData(x);

                int L5Size = 0, L6Size = 0;
                getSplitCounts(L2, varIdx2, splitPointR, directionR, numRight, L5Size, L6Size);
                float **L5 = new float*[L5Size];
                float **L6 = new float*[L6Size];
                for (int i = 0; i < L5Size; i++) {
                    L5[i] = new float[colCount];
                }
                for (int i = 0; i < L6Size; i++) {
                    L6[i] = new float[colCount];
                }
                splitData(directionR, splitPointR, varIdx2, colCount, numRight, L5, L6, L2);
                double l5SS, l6SS;

                y = getResponseData(p->response, p->headers, L5, L5Size);
                (*ss_func)(y, L5Size, mean, l5SS);
                free1DData(y);
                y = getResponseData(p->response, p->headers, L6, L6Size);
                (*ss_func)(y, L6Size, mean, l6SS);
                free1DData(y);
                thisSSRight = l5SS + l6SS;

/*               if(numObs == 43) {
                  cout << p->varNames[varIdx2] << "\n\t" << splitPointR << "\t" << directionR << "\t" << improveR << "\n\t" << splitPointL << "\t" << directionL << "\t" << improveL << endl;
                  }*/

                if (thisSSRight < bestRightSS && improveR > 0) {
                    bestRightSS = thisSSRight;
                }

                // Free memory
                free2DData(L3, L3Size);
                free2DData(L4, L4Size);
                free2DData(L5, L5Size);
                free2DData(L6, L6Size);
            }

            if (bestLeftSS != baseSS && bestRightSS != baseSS) {
                thisSS = bestLeftSS + bestRightSS;
            }
            else {
                thisSS = baseSS;
            }

        } else {
            y = getResponseData(p->response, p->headers, L1, numLeft);
            (*ss_func)(y, numLeft, mean, leftSS);
            free1DData(y);
            y = getResponseData(p->response, p->headers, L2, numRight);
            (*ss_func)(y, numRight, mean, rightSS);
            free1DData(y);
            thisSS = leftSS + rightSS;
            if(numObs == 842)
                cout << leftSS << " " << rightSS << endl;
        }

        if(numObs == 842 || numObs == 1599) {
            cout << p->varNames[varIdx] << "\n\t" << splitPoint << "\t" << direction << "\t" << improve << "\t" << thisSS << "\t" << bestSS << endl;
        }
        if (thisSS == baseSS) {
            y = getResponseData(p->response, p->headers, L1, numLeft);
            (*ss_func)(y, numLeft, mean, leftSS);
            free1DData(y);
            y = getResponseData(p->response, p->headers, L2, numRight);
            (*ss_func)(y, numRight, mean, rightSS);
            free1DData(y);
            thisSS = leftSS + rightSS;
        }

		// compare only 6 digits of doubles since roundoff error causes discrepencies between pypart/rpart and cppart
        if (improve > 0 && improve > bestImprove) {
                //((p->method == ANOVA && trunc(1000000.*thisSS) < trunc(1000000.*bestSS)) ||
                // (p->method == GINI && improve > bestImprove))) {
            bestSS = thisSS;
            bestImprove = improve;
            n->splitPoint = splitPoint;
            n->direction = direction;
            n->index = where;
            n->varIndex = varIdx;
            n->varName = p->varNames[varIdx];
            n->yval = yBar;
            n->dev = deviance;
            n->improvement = improve;

            // free old data before reassigning pointer to data
            free2DData(n->rightNode->data, numright);
            free2DData(n->leftNode->data, numleft);
            
            n->rightNode->data = deepCopyData(L2, numRight, colCount);
            n->leftNode->data = deepCopyData(L1, numLeft, colCount);

            numleft = numLeft;
            numright = numRight;
        }
        free2DData(L1, numLeft);
        free2DData(L2, numRight);
    }
}

void mergeSort(float *x, float *y, int low, int high, int varIdx, int colCount, int respCol) {
    int mid;
    if (low < high) {
        mid = (low + high) / 2;
        mergeSort(x, y, low, mid, varIdx, colCount, respCol);
        mergeSort(x, y, mid + 1, high, varIdx, colCount, respCol);

        merge(x, y, low, high, mid, varIdx, colCount, respCol);
    }
}

void merge(float *x, float *y, int low, int high, int mid, int varIdx, int colCount, int respCol) {
    int i = low, j = mid + 1, k = 0;
    const int size = high - low + 1;

    float *tempx = new float[high - low + 1];
	float *tempy = new float[high - low + 1];
    //for (int idx = 0; idx < (high - low + 1); idx++) {
    //    temp[idx] = new float[2];
    //}

    while (i <= mid && j <= high) {
        if (x[i] < x[j]) {
            //for (int col = 0; col < colCount; col++) {
                tempx[k] = x[i];
				tempy[k] = y[i];
            //}
            k++;
            i++;
        }
        else if (x[i] >= x[j]) {
            //for (int col = 0; col < colCount; col++) {
                tempx[k] = x[j];
				tempy[k] = y[j];
            //}
            k++;
            j++;
        }
        /*else { // if equal sort on response column (is this what excel does?)[stopped doing this 6/4/18]
            if (x[i][respCol] < x[j][respCol]) {
                //for (int col = 0; col < colCount; col++) {
                    temp[k] = x[i];
                //}
                k++;
                i++;
            }
            else {
                //for (int col = 0; col < colCount; col++) {
                    temp[k] = x[j];
                //}
                k++;
                j++;
            }
        }*/ // did away with this response column stuff...
    }

    while (i <= mid) {
        //for (int col = 0; col < colCount; col++) {
            tempx[k] = x[i];
			tempy[k] = y[i];
        //}
        k++;
        i++;
    }

    while (j <= high) {
        //for (int col = 0; col < colCount; col++) {
            tempx[k] = x[j];
			tempy[k] = y[j];
        //}
        k++;
        j++;
    }
    for (k = 0, i = low; i <= high; ++i, ++k) {
        //for (int col = 0; col < colCount; col++) {
            x[i] = tempx[k];
			y[i] = tempy[k];
        //}
    }

    //free2DData(temp, high - low + 1); 
	free1DData(tempx);
	free1DData(tempy);
}

void getSplitCounts(float ** data, int splitVar, float splitPoint, int direction, int numObs, int & leftCount, int & rightCount)
{
    if (direction < 0) {
        for (int i = 0; i < numObs; i++) {
            if (data[i][splitVar] < splitPoint) {
                leftCount++;
            }
            else {
                rightCount++;
            }
        }
    }
    else {
        for (int i = 0; i < numObs; i++) {
            if (data[i][splitVar] <= splitPoint) {
                rightCount++;
            }
            else {
                leftCount++;
            }
        }
    }
}

void splitData(int direction, float splitPoint, int splitVar, int colCount, int numObs, float ** left, float ** right, float ** data)
{
    int leftCnt = 0, rightCnt = 0;
    for (int i = 0; i < numObs; i++) {
        if (direction < 0) {
            if (data[i][splitVar] < splitPoint) {
                for (int j = 0; j < colCount; j++) {
                    left[leftCnt][j] = data[i][j];
                }
                leftCnt++;
            }
            else {
                for (int j = 0; j < colCount; j++) {
                    right[rightCnt][j] = data[i][j];
                }
                rightCnt++;
            }
        }
        else {
            if (data[i][splitVar] <= splitPoint) {
                for (int j = 0; j < colCount; j++) {
                    right[rightCnt][j] = data[i][j];
                }
                rightCnt++;
            }
            else {
                for (int j = 0; j < colCount; j++) {
                    left[leftCnt][j] = data[i][j];
                }
                leftCnt++;
            }
        }
    }

}
