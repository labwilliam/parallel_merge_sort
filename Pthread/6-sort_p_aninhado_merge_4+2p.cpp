#include <iostream>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <algorithm>
#include <sys/time.h>
#include <stdlib.h>

#define size 2*1000*1000

//real	0m0.684s
//user	0m2.503s
//sys	0m0.016s

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

void *PMB(void *param){
    struct dataPM *parametersPM = (struct dataPM *)param;
    
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
    
    if(n1 == 0) {
        return 0;
    }
    else {
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
    
    pthread_exit(NULL);
}

void PM(struct dataPM *parametersPM){    
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
    
    if(n1 == 0) {
        return;
    }
    else {
        int q1 = floor((parametersPM->p1 + parametersPM->r1)/2);
        int q2 = BS(parametersPM->T[q1], parametersPM->T, parametersPM->p2, parametersPM->r2);
        int q3 = parametersPM->p3 + (q1 - parametersPM->p1) + (q2 - parametersPM->p2);
        
        parametersPM->A[q3] = parametersPM->T[q1];

        pthread_t threads[2];
                
        //SPAWN A
        struct dataPM partA;
        partA.T = parametersPM->T;
        partA.p1 = parametersPM->p1;
        partA.r1 = q1 - 1;
        partA.p2 = parametersPM->p2;
        partA.r2 = q2 - 1;
        partA.A = parametersPM->A;
        partA.p3 = parametersPM->p3;
        pthread_create(&threads[0], NULL, PMB, (void *)&partA);
        
        //SPAWN B
        struct dataPM partB;
        partB.T = parametersPM->T;
        partB.p1 = q1 + 1;
        partB.r1 = parametersPM->r1;
        partB.p2 = q2;
        partB.r2 = parametersPM->r2;
        partB.A = parametersPM->A;
        partB.p3 = q3 + 1;
        pthread_create(&threads[1], NULL, PMB, (void *)&partB);
        
        pthread_join(threads[0], NULL);
        pthread_join(threads[1], NULL);
    }
}

void PMSA(struct dataPMS *parametersPMS){
    int n = parametersPMS->r - parametersPMS->p + 1;
    
    if(n == 1){
        parametersPMS->B[parametersPMS->s] = parametersPMS->A[parametersPMS->p];
    }
    else{
        int T[n+1];
                
        int q = floor((parametersPMS->p + parametersPMS->r) / 2);
        int qL = q - parametersPMS->p + 1;
        
        //SPAWN A
        struct dataPMS partA;
        partA.A = parametersPMS->A;
        partA.p = parametersPMS->p;
        partA.r = q;
        partA.B = T;
        partA.s = 1;
        PMSA(&partA); 
        
        //SPAWN B
        struct dataPMS partB;
        partB.A = parametersPMS->A;
        partB.p = q + 1;
        partB.r = parametersPMS->r;
        partB.B = T;
        partB.s = qL + 1;
        PMSA(&partB);
        
        struct dataPM parametersPM;
        parametersPM.T = T;
        parametersPM.p1 = 1;
        parametersPM.r1 = qL;
        parametersPM.p2 = qL + 1;
        parametersPM.r2 = n;
        parametersPM.A = parametersPMS->B;
        parametersPM.p3 = parametersPMS->s;
        PMA(&parametersPM);
    }
}

void *PMS(void *param){
    struct dataPMS *parametersPMS = (struct dataPMS *)param;
    
    int n = parametersPMS->r - parametersPMS->p + 1;
    
    if(n == 1){
        parametersPMS->B[parametersPMS->s] = parametersPMS->A[parametersPMS->p];
    }
    else{
        int T[n+1];
                
        int q = floor((parametersPMS->p + parametersPMS->r) / 2);
        int qL = q - parametersPMS->p + 1;
        
        pthread_t threads[2];
        
        //SPAWN A
        struct dataPMS partA;
        partA.A = parametersPMS->A;
        partA.p = parametersPMS->p;
        partA.r = q;
        partA.B = T;
        partA.s = 1;
        PMSA(&partA); 
        
        //SPAWN B
        struct dataPMS partB;
        partB.A = parametersPMS->A;
        partB.p = q + 1;
        partB.r = parametersPMS->r;
        partB.B = T;
        partB.s = qL + 1;
        PMSA(&partB);
        
        pthread_join(threads[0], NULL);
        pthread_join(threads[1], NULL);
        
        struct dataPM parametersPM;
        parametersPM.T = T;
        parametersPM.p1 = 1;
        parametersPM.r1 = qL;
        parametersPM.p2 = qL + 1;
        parametersPM.r2 = n;
        parametersPM.A = parametersPMS->B;
        parametersPM.p3 = parametersPMS->s;
        PMA(&parametersPM);
    }

    pthread_exit(NULL);
}

void *MS(void *param){
    struct dataPMS *parametersPMS = (struct dataPMS *)param;
    
    int n = parametersPMS->r - parametersPMS->p + 1;
    
    int T[n+1];
                
    int q = floor((parametersPMS->p + parametersPMS->r) / 2);
    int qL = q - parametersPMS->p + 1;
    
    pthread_t threads[2];
    
    //SPAWN A
    struct dataPMS partA;
    
    partA.A = parametersPMS->A;
    partA.p = parametersPMS->p;
    partA.r = q;
    partA.B = T;
    partA.s = 1;

    pthread_create(&threads[0], NULL, PMS, (void *)&partA);
       
    //SPAWN B
    struct dataPMS partB;
    
    partB.A = parametersPMS->A;
    partB.p = q + 1;
    partB.r = parametersPMS->r;
    partB.B = T;
    partB.s = qL + 1;
    
    pthread_create(&threads[1], NULL, PMS, (void *)&partB);
       
    pthread_join(threads[0], NULL); 
    pthread_join(threads[1], NULL);
    
    struct dataPM parametersPM;
    
    parametersPM.T = T;
    parametersPM.p1 = 1;
    parametersPM.r1 = qL;
    parametersPM.p2 = qL + 1;
    parametersPM.r2 = n;
    parametersPM.A = parametersPMS->B;
    parametersPM.p3 = parametersPMS->s;
    
    PM(&parametersPM); 
        
    pthread_exit(NULL);
}

int main()
{
    struct dataPMS parametersPMS;
    //int *Assert = (int *) malloc((size +1) * sizeof (int));
    
    parametersPMS.p = 1;
    parametersPMS.r = size;
    parametersPMS.s = 1;
    parametersPMS.A = (int *) malloc((size +1) * sizeof (int));
    parametersPMS.B = (int *) malloc((size +1) * sizeof (int));
    
    srand(1);
    for(int i = 1; i <= size; i++){
        parametersPMS.A[i] = rand() % size * size + i + 1;
        //Assert[i] = parametersPMS.A[i];
        //parametersPMS.B[i] = 0;
    }
        
    //struct timeval t1;
    //gettimeofday(&t1, 0);
    
    int n = parametersPMS.r - parametersPMS.p + 1;
    
    int T[n+1];
                
    int q = floor((parametersPMS.p + parametersPMS.r) / 2);
    int qL = q - parametersPMS.p + 1;
    
    pthread_t threads[2];
    
    //SPAWN A
    struct dataPMS partA;
    partA.A = parametersPMS.A;
    partA.p = parametersPMS.p;
    partA.r = q;
    partA.B = T;
    partA.s = 1;
    pthread_create(&threads[0], NULL, MS, (void *)&partA);
       
    //SPAWN B
    struct dataPMS partB;
    partB.A = parametersPMS.A;
    partB.p = q + 1;
    partB.r = parametersPMS.r;
    partB.B = T;
    partB.s = qL + 1;
    pthread_create(&threads[1], NULL, MS, (void *)&partB);
       
    pthread_join(threads[0], NULL); 
    pthread_join(threads[1], NULL);
    
    struct dataPM parametersPM;
    parametersPM.T = T;
    parametersPM.p1 = 1;
    parametersPM.r1 = qL;
    parametersPM.p2 = qL + 1;
    parametersPM.r2 = n;
    parametersPM.A = parametersPMS.B;
    parametersPM.p3 = parametersPMS.s;
    PM(&parametersPM); 
    
    struct timeval t2;
    gettimeofday(&t2, 0); 
    
    //assert(Assert, parametersPMS.B);
    
    //cout<<"TIME: "<<(t2.tv_sec*1000. + t2.tv_usec/1000.)-(t1.tv_sec*1000. + t1.tv_usec/1000.)<<endl;
    
    cout << "FIM" << endl;
    
}
