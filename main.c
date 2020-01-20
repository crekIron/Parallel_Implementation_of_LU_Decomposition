/* gcc -g -Wall -o main main.c -lpthread */
#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>

#include "init.h"
#include "global.h"

void* Init_pi(void* pi);
void* Init_l(void* l);
void* Init_u(void* u);
void* genrateMat(void* a);

int main(int argc, char const *argv[])
{
    // inputs: a(n,n)
    // outputs: pi(n), l(n,n), and u(n,n)
    
    //Implementation of array depends
    //Using single array; access element by row*n+col
    // double *a;
    //Using array of pointers; access element by a[i][j] or *(*(a+row)+col)
    
    //Required: No of Rows = n
    int n = 4;//May be also taken as a file input
    double *a;

    double *pi;
    double *l;
    double *u;

    //To start the seed from anywhere
    srand(time(0));
    long int seedval=rand();

    long thread=0;
    pthread_t* thread_handles;

    thread_count=4;
    thread_handles = malloc(thread_count*sizeof(pthread_t));
    
    //////Intialisation///////
    //initialise a
    //randomly genrate an n*n matrix
    long int matrixSize = n*n;
    a = malloc((matrixSize)*sizeof(double*));

    struct Mat_arg mat[thread_count];
    for (thread = 0; thread < thread_count; thread++)
    {
        mat[thread].rank=thread;
        mat[thread].size=n;
        mat[thread].arr=a;
        mat[thread].seedval=seedval++;
        // printf("thread %ld goes on....\n", thread);
        pthread_create(&thread_handles[thread], NULL, genrateMat, (void*)&mat[thread]);
    }
    //initialize pi
    pi = malloc((n)*sizeof(double*));

    struct Arr_arg pistr[thread_count];
    for (thread = 0; thread < thread_count; thread++)
    {
        pistr[thread].rank=thread;
        pistr[thread].size=n;
        pistr[thread].pi=pi;
        // printf("thread %ld goes on....\n", thread);
        pthread_create(&thread_handles[thread], NULL, Init_pi, (void*)&pistr[thread]);
    }
    //initialize l
    l = malloc((matrixSize)*sizeof(double*));

    struct Mat_arg lstr[thread_count];
    for (thread = 0; thread < thread_count; thread++)
    {
        lstr[thread].rank=thread;
        lstr[thread].size=n;
        lstr[thread].arr=l;//l is passed
        lstr[thread].seedval=seedval++;
        // printf("thread %ld goes on....\n", thread);
        pthread_create(&thread_handles[thread], NULL, Init_l, (void*)&lstr[thread]);
    }

    //initialize p
    u = malloc((matrixSize)*sizeof(double*));

    struct Mat_arg ustr[thread_count];
    for (thread = 0; thread < thread_count; thread++)
    {
        ustr[thread].rank=thread;
        ustr[thread].size=n;
        ustr[thread].arr=u;//u is passed
        ustr[thread].seedval = seedval++;
        // printf("thread %ld goes on....\n", thread);
        pthread_create(&thread_handles[thread], NULL, Init_u, (void*)&ustr[thread]);
    }

    //main for loop
    for (int k = 0; k < n; k++)
    {
        double max=0;

        
    }
    

    for (thread = 0; thread < thread_count; thread++)
    {
        printf("thread %ld JOINED...\n", thread);
        pthread_join(thread_handles[thread], NULL);
    }

    // //print a
    // printf("Printing a.......\n");
    // for (int i = 0; i < n; i++)
    // {
    //     for (int j = 0; j < n; j++)
    //     {
    //         printf("%f ", a[i*n+j]);
    //     }
    //     printf("\n");
    // }
    // printf("\n");
    // //print pi
    // printf("Printing pi.......\n");
    // for (int i = 0; i < n; i++)
    // {
    //     printf("%f ", pi[i]);
    // }
    // printf("\n\n");
    // //print l
    // printf("Printing l........\n");
    // for (int i = 0; i < n; i++)
    // {
    //     for (int j = 0; j < n; j++)
    //     {
    //         printf("%f ", l[i*n+j]);
    //     }
    //     printf("\n");
    // }
    // printf("\n");
    // //print u;
    // printf("Printing u.......\n");
    // for (int i = 0; i < n; i++)
    // {
    //     for (int j = 0; j < n; j++)
    //     {
    //         printf("%f ", u[i*n+j]);
    //     }
    //     printf("\n");
    // }
    // printf("\n");

    //free all malloc
    free(thread_handles);
    free(a);
    free(pi);
    return 0;
}
