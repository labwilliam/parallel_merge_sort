#include <iostream>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <algorithm>
#include <sys/time.h>
#include <stdlib.h>
#include <limits.h>

#define size 2*1000*1000

//real	0m0.823s
//user	0m0.730s
//sys	0m0.092s


using namespace std;

struct dataPM{
    int *T;
    int p1;
    int r1;
    int p2;
    int r2;
    int *A;
    int p3;
}; 

struct dataPMS{
    int *A;
    int p;
    int r;
    int *B;
    int s;
};

void assert(int *Assert, int *B){
    sort(Assert+1,Assert+size+1);
	for(int i = 1; i <= size; i++){
        if(Assert[i] != B[i]){
            cout << "\tFAIL" << endl;
            break;
        }
    }
    cout << endl;
}

void printArray(int *A){
    for(int i = 1; i <= size; i++)
        cout << A[i] << " ";
        
    cout << endl;
}

void Merge(int *A, int p, int q, int r){
	int n1 = q - p + 1;
	int n2 = r - q;
	
	int *L = (int *) malloc((n1 + 2) * sizeof (int));
	int *R = (int *) malloc((n2 + 2) * sizeof (int));
	
	for(int i = 1; i <= n1; i++){
		L[i] = A[p + i - 1];
	}
	
	for(int i = 1; i <= n2; i++){
		R[i] = A[q + i];
	}
	
	L[n1 + 1] = INT_MAX;
	R[n2 + 1] = INT_MAX;
	
	int i = 1;
	int j = 1;
	
	for(int k = p; k <= r; k++){
		if(L[i] <= R[j]){
			A[k] = L[i];
			i++;
		}
		else{
			A[k] = R[j];
			j++;
		}
	}
}

void MergeSort(int *A, int p, int r){
	if(p < r){
		int q = floor((p + r)/2);
		MergeSort(A, p, q);
		MergeSort(A, q + 1, r);
		Merge(A, p, q, r);
	}
}

int main()
{
    struct dataPMS parametersPMS;
    //int *Assert = (int *) malloc((size +1) * sizeof (int));
    
    int *A = (int *) malloc((size +1) * sizeof (int));
        
    srand(1);
    for(int i = 1; i <= size; i++){
        A[i] = rand() % size * size + i + 1;
        //Assert[i] = A[i];
    }
          
    MergeSort(A, 1, size);    
            
    //assert(Assert, A);
    
    cout << "FIM" << endl;
    
}
