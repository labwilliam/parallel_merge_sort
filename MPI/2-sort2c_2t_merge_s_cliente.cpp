#include <iostream>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <algorithm>
#include <sys/time.h>
#include <stdlib.h>
#include <limits.h>
#include <mpi.h>

#define size 2*1000*1000

//init
//time= 0m1.025s

//real	0m1.515s
//user	0m1.490s
//sys	0m0.153s


using namespace std;

struct dataMS{
    int *A;
    int p;
    int r;
};

struct dataPM{
    int *T;
    int p1;
    int r1;
    int p2;
    int r2;
    int *A;
    int p3;
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

void MergeSortSerial(int *A, int p, int r){
	if(p < r){
		int q = floor((p + r)/2);
		MergeSortSerial(A, p, q);
		MergeSortSerial(A, q + 1, r);
		Merge(A, p, q, r);
	}
}

void *MergeSort(void *param){
    struct dataMS *parametersMS = (struct dataMS *)param;
    
	if(parametersMS->p < parametersMS->r){
		int q = floor((parametersMS->p + parametersMS->r)/2);
		MergeSortSerial(parametersMS->A, parametersMS->p, q);
		MergeSortSerial(parametersMS->A, q + 1, parametersMS->r);
		Merge(parametersMS->A, parametersMS->p, q, parametersMS->r);
	}
	
	pthread_exit(NULL);
}

void PMergeSerial(struct dataPM *parametersPM){    
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
        PMergeSerial(&partA);
        
        //SPAWN B
        struct dataPM partB;
        partB.T = parametersPM->T;
        partB.p1 = q1 + 1;
        partB.r1 = parametersPM->r1;
        partB.p2 = q2;
        partB.r2 = parametersPM->r2;
        partB.A = parametersPM->A;
        partB.p3 = q3 + 1;
        PMergeSerial(&partB);
    }
}

void *PMergeParallel(void *param){    
    
    struct dataPM *parametersPM = (struct dataPM *) param;
    
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
        return 0;
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
        PMergeSerial(&partA);
        
        //SPAWN B
        struct dataPM partB;
        partB.T = parametersPM->T;
        partB.p1 = q1 + 1;
        partB.r1 = parametersPM->r1;
        partB.p2 = q2;
        partB.r2 = parametersPM->r2;
        partB.A = parametersPM->A;
        partB.p3 = q3 + 1;
        PMergeSerial(&partB);
    }
    pthread_exit(NULL);
}

void MergeSortParallelClient(int *A, int p, int r, int *T){
	
    int s = 1;
    int n = r - p + 1;
    int q = floor((p + r) / 2);
    int qL = q - p + 1;

    pthread_t threads[2];
	
    //SPAWN A
    struct dataMS partAMS;
    partAMS.A = A;
    partAMS.p = p;
    partAMS.r = q;
    pthread_create(&threads[0], NULL, MergeSort, (void *)&partAMS);
       
    //SPAWN B
    struct dataMS partBMS;
    partBMS.A = A;
    partBMS.p = q + 1;
    partBMS.r = r;
    pthread_create(&threads[1], NULL, MergeSort, (void *)&partBMS);
       
    pthread_join(threads[0], NULL); 
    pthread_join(threads[1], NULL);
	
    struct dataPM parametersPM;
    parametersPM.T = A;
    parametersPM.p1 = 1;
    parametersPM.r1 = qL;
    parametersPM.p2 = qL + 1;
    parametersPM.r2 = n;
    parametersPM.A = T;
    parametersPM.p3 = s;
    
    int q1 = floor((parametersPM.p1 + parametersPM.r1)/2);
    int q2 = BS(parametersPM.T[q1], parametersPM.T, parametersPM.p2, parametersPM.r2);
    int q3 = parametersPM.p3 + (q1 - parametersPM.p1) + (q2 - parametersPM.p2);
    
    parametersPM.A[q3] = parametersPM.T[q1];
    
    //SPAWN A
    struct dataPM partAPM;
    partAPM.T = parametersPM.T;
    partAPM.p1 = parametersPM.p1;
    partAPM.r1 = q1 - 1;
    partAPM.p2 = parametersPM.p2;
    partAPM.r2 = q2 - 1;
    partAPM.A = parametersPM.A;
    partAPM.p3 = parametersPM.p3;
    PMergeSerial(&partAPM);
        
    //SPAWN B
    struct dataPM partBPM;
    partBPM.T = parametersPM.T;
    partBPM.p1 = q1 + 1;
    partBPM.r1 = parametersPM.r1;
    partBPM.p2 = q2;
    partBPM.r2 = parametersPM.r2;
    partBPM.A = parametersPM.A;
    partBPM.p3 = q3 + 1;
    PMergeSerial(&partBPM);
       
    //pthread_join(threads[0], NULL); 
    //pthread_join(threads[1], NULL);
}

int main()
{
    int p = 1;
    int r = size;
    int s = 1;
    //int *Assert = (int *) malloc((size + 1) * sizeof (int));

    int n = r - p + 1;
    int T[n + 1];
    int q = floor((p + r) / 2);
    int qL = q - p + 1;
    
    int my_rank, proc;
    MPI_Status status;
    MPI_Init (NULL, NULL);
    MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size (MPI_COMM_WORLD, &proc);
    
    int *A;
    if(my_rank == 0){
        A = (int *) malloc((size + 1) * sizeof (int));

		srand(1);
        for(int i = 1; i <= size; i++){
            A[i] = rand() % size * size + i + 1;
            //Assert[i] = A[i];
        }
    
        //da pra fazer com assincrono Isend Irecv
        int sizeSendBuffer0 = q - p + 1;
        MPI_Send(&sizeSendBuffer0, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Send(A + 1, sizeSendBuffer0, MPI_INT, 1, 0, MPI_COMM_WORLD);

        int sizeSendBuffer1 = r - q;
        MPI_Send(&sizeSendBuffer1, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
        MPI_Send(A + q + 1, sizeSendBuffer1, MPI_INT, 2, 0, MPI_COMM_WORLD);

        MPI_Recv(A + 1, sizeSendBuffer0, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(A + q + 1, sizeSendBuffer1, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);

        struct dataPM parametersPM;
        parametersPM.T = A;
        parametersPM.p1 = 1;
        parametersPM.r1 = qL;
        parametersPM.p2 = qL + 1;
        parametersPM.r2 = n;
        parametersPM.A = T;
        parametersPM.p3 = s;
        
        int q1 = floor((parametersPM.p1 + parametersPM.r1)/2);
        int q2 = BS(parametersPM.T[q1], parametersPM.T, parametersPM.p2, parametersPM.r2);
        int q3 = parametersPM.p3 + (q1 - parametersPM.p1) + (q2 - parametersPM.p2);
        
        parametersPM.A[q3] = parametersPM.T[q1];

        pthread_t threads[2];
        
        //SPAWN A
        struct dataPM partAPM;
        partAPM.T = parametersPM.T;
        partAPM.p1 = parametersPM.p1;
        partAPM.r1 = q1 - 1;
        partAPM.p2 = parametersPM.p2;
        partAPM.r2 = q2 - 1;
        partAPM.A = parametersPM.A;
        partAPM.p3 = parametersPM.p3;
        pthread_create(&threads[0], NULL, PMergeParallel, (void *)&partAPM);
            
        //SPAWN B
        struct dataPM partBPM;
        partBPM.T = parametersPM.T;
        partBPM.p1 = q1 + 1;
        partBPM.r1 = parametersPM.r1;
        partBPM.p2 = q2;
        partBPM.r2 = parametersPM.r2;
        partBPM.A = parametersPM.A;
        partBPM.p3 = q3 + 1;
        pthread_create(&threads[1], NULL, PMergeParallel, (void *)&partBPM);
           
        pthread_join(threads[0], NULL); 
        pthread_join(threads[1], NULL);    
    }
    else{
	int sizeRecvBuffer;
        if(my_rank == 1){
            MPI_Recv(&sizeRecvBuffer, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
            A = (int *) malloc ((sizeRecvBuffer + 1) * sizeof (int));
            MPI_Recv(A + 1, sizeRecvBuffer, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        }

        if(my_rank == 2){
            MPI_Recv(&sizeRecvBuffer, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
            A = (int *) malloc ((sizeRecvBuffer + 1) * sizeof (int));
            MPI_Recv(A + 1, sizeRecvBuffer, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        }

	int *T = (int *) malloc ((sizeRecvBuffer + 1) * sizeof (int));

	MergeSortParallelClient(A, 1, sizeRecvBuffer, T);

        MPI_Send(T + 1, sizeRecvBuffer, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }         
    /*    
    if(my_rank == 0){
        assert(Assert, T);
        cout << "FIM" << endl;   
    }*/

    MPI_Finalize();
}
