#include <iostream>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <algorithm>
#include <sys/time.h>
#include <stdlib.h>
#include <limits.h>

#define size 2*1000*1000

//real	0m0.494s
//user	0m0.974s
//sys	0m0.175s

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

int BS(int &x, int *T, int &p, int &r){
    int low = p;
    int mid;
    int high = max(p, r + 1);
    
    while(low < high){
        mid = floor((low + high)/2);
        if(x <= T[mid]){
            high = mid;
        }
        else{
            low = mid + 1;
        }
    }
    
    return high;
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

void PMA(struct dataPM *parametersPM){    
    int n1 = parametersPM->r1 - parametersPM->p1 + 1;
    int n2 = parametersPM->r2 - parametersPM->p2 + 1;
    
    if(n1 < n2){
        int aux = parametersPM->p1;
        parametersPM->p1 = parametersPM->p2;
        parametersPM->p2 = aux;
        
        aux = parametersPM->r1;
        parametersPM->r1 = parametersPM->r2;
        parametersPM->r2 = aux;
        
        aux = n1;
        n1 = n2;
        n2 = aux;
    }
    
    if(n1 == 0){
        return;
    }
    else{
        int q1 = floor((parametersPM->p1 + parametersPM->r1)/2);
        int q2 = BS(parametersPM->T[q1], parametersPM->T, parametersPM->p2, parametersPM->r2);
        int q3 = parametersPM->p3 + (q1 - parametersPM->p1) + (q2 - parametersPM->p2);
        
        parametersPM->A[q3] = parametersPM->T[q1];
        
        //SPAWN A
        struct dataPM partA;
        partA.T = parametersPM->T;
        partA.p1 = parametersPM->p1;
        partA.r1 = q1 - 1;
        partA.p2 = parametersPM->p2;
        partA.r2 = q2 - 1;
        partA.A = parametersPM->A;
        partA.p3 = parametersPM->p3;
        PMA(&partA);
        
        //SPAWN B
        struct dataPM partB;
        partB.T = parametersPM->T;
        partB.p1 = q1 + 1;
        partB.r1 = parametersPM->r1;
        partB.p2 = q2;
        partB.r2 = parametersPM->r2;
        partB.A = parametersPM->A;
        partB.p3 = q3 + 1;
        PMA(&partB);
    }
}

void *MS(void *param){
    struct dataPMS *parametersPMS = (struct dataPMS *)param;
    
    MergeSort(parametersPMS->A, parametersPMS->p, parametersPMS->r);
            
    pthread_exit(NULL);
}

int main()
{
    struct dataPMS parametersPMS;
    //int *Assert = (int *) malloc((size +1) * sizeof (int));
    parametersPMS.A = (int *) malloc((size +1) * sizeof (int));
      
    srand(1);
    for(int i = 1; i <= size; i++){
        parametersPMS.A[i] = rand() % size * size + i + 1;
        //Assert[i] = parametersPMS.A[i];
    }

    int p = 1;
    int r = size;
    int s = 1;
    int n = r - p + 1;   
    int q = floor((p + r) / 2);
    int qL = q - p + 1;
    int T[size + 1];
    
    pthread_t threads[4];
    
    parametersPMS.p = 1;
    parametersPMS.r = size / 2;
    parametersPMS.s = 1;
    int nA = parametersPMS.r - parametersPMS.p + 1;     
    int qA = floor((parametersPMS.p + parametersPMS.r) / 2);
    int qLA = qA - parametersPMS.p + 1;
    
    //SPAWN A
    struct dataPMS partAPMS;
    partAPMS.A = parametersPMS.A;
    partAPMS.p = parametersPMS.p;
    partAPMS.r = qA;
    partAPMS.B = T;
    partAPMS.s = 1;
    pthread_create(&threads[0], NULL, MS, (void *)&partAPMS);
       
    //SPAWN B
    struct dataPMS partBPMS;
    partBPMS.A = parametersPMS.A;
    partBPMS.p = qA + 1;
    partBPMS.r = parametersPMS.r;
    partBPMS.B = T;
    partBPMS.s = qLA + 1;
    pthread_create(&threads[1], NULL, MS, (void *)&partBPMS);
    
    parametersPMS.p = (size / 2) + 1;
    parametersPMS.r = size;
    parametersPMS.s = (size / 2) + 1;
    int nB = parametersPMS.r - parametersPMS.p + 1;     
    int qB = floor((parametersPMS.p + parametersPMS.r) / 2);
    int qLB = qB - parametersPMS.p + 1;
    
    //SPAWN C
    struct dataPMS partCPMS;
    partCPMS.A = parametersPMS.A;
    partCPMS.p = parametersPMS.p;
    partCPMS.r = qB;
    partCPMS.B = T;
    partCPMS.s = 1;
    pthread_create(&threads[2], NULL, MS, (void *)&partCPMS);
       
    //SPAWN D
    struct dataPMS partDPMS;
    partDPMS.A = parametersPMS.A;
    partDPMS.p = qB + 1;
    partDPMS.r = parametersPMS.r;
    partDPMS.B = T;
    partDPMS.s = qLB + 1;
    pthread_create(&threads[3], NULL, MS, (void *)&partDPMS);
    
    pthread_join(threads[0], NULL); 
    pthread_join(threads[1], NULL);
    pthread_join(threads[2], NULL);
    pthread_join(threads[3], NULL);
    
    struct dataPM parametersPMA;
    parametersPMA.T = parametersPMS.A;
    parametersPMA.p1 = 1;
    parametersPMA.r1 = qLA;
    parametersPMA.p2 = qLA + 1;
    parametersPMA.r2 = nA;
    parametersPMA.A = T;
    parametersPMA.p3 = s;
    PMA(&parametersPMA);
    
    struct dataPM parametersPMB;
    parametersPMB.T = parametersPMS.A;
    parametersPMB.p1 = parametersPMS.p;
    parametersPMB.r1 = qLB + parametersPMA.r2;
    parametersPMB.p2 = qLB + 1 + parametersPMA.r2;
    parametersPMB.r2 = nB + parametersPMA.r2;
    parametersPMB.A = T;
    parametersPMB.p3 = parametersPMS.s;
    PMA(&parametersPMB);
  
    struct dataPM parametersPMC;
    parametersPMC.T = T;
    parametersPMC.p1 = 1;
    parametersPMC.r1 = qL;
    parametersPMC.p2 = qL + 1;
    parametersPMC.r2 = n;
    parametersPMC.A = parametersPMS.A;
    parametersPMC.p3 = 1;
    PMA(&parametersPMC);
    
    //assert(Assert, parametersPMS.A);
    
    cout << "FIM" << endl;
    
}
