/* gcc -g -Wall -o main main.c -lpthread */
#include "init.h"
#include "global.h"
#include "algo.h"
#include <time.h>
#include <sys/time.h> 
void* Init_pi(void* pi);
void* Init_l(void* l);
void* Init_u(void* u);
void* genrateMat(void* a);
void *swap(void *a);
void *firstFor(void *forone);
void *SecondFor(void *fortwo);

typedef struct {
    int     secs;
    int     usecs;
} TIME_DIFF;

// this is see from outside. time.h technically doesn't give the right answer
// so taken this to get correct answer according to gettimeofday
TIME_DIFF * my_difftime (struct timeval * start, struct timeval * end)
{
    TIME_DIFF * diff = (TIME_DIFF *) malloc ( sizeof (TIME_DIFF) );
 
    if (start->tv_sec == end->tv_sec) {
        diff->secs = 0;
        diff->usecs = end->tv_usec - start->tv_usec;
    }
    else {
        diff->usecs = 1000000 - start->tv_usec;
        diff->secs = end->tv_sec - (start->tv_sec + 1);
        diff->usecs += end->tv_usec;
        if (diff->usecs >= 1000000) {
            diff->usecs -= 1000000;
            diff->secs += 1;
        }
    }
     
    return diff;
}

int main(int argc, char const *argv[])
{
    //start the clock
    struct timeval myTVstart, myTVend;

    //Required: No of Rows = n
    n = 8000;//May be also taken as a file 
    printf("Size of the Matrix:");
    scanf("%d", &n);

    //Num threads
    thread_count=8;
    printf("No of threads:");
    scanf("%d", &thread_count);

    gettimeofday (&myTVstart, NULL);
    pthread_mutex_init(&lock, NULL);
    // inputs: a(n,n)
    // outputs: pi(n), l(n,n), and u(n,n)
    
    //Implementation of array depends
    //Using single array; access element by row*n+col
    // double *a;
    //Using array of pointers; access element by a[i][j] or *(*(a+row)+col)

    double *a;

    double *pi;
    double *l;
    double *u;

    //To start the seed from anywhere
    srand(time(0));
    long int seedval=rand();
    long thread;
    pthread_t* thread_handles;

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
    for (thread = 0; thread < thread_count; thread++)
    {
        pthread_join(thread_handles[thread], NULL);
        // printf("thread %ld JOINED...\n", thread);
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
    for (thread = 0; thread < thread_count; thread++)
    {
        pthread_join(thread_handles[thread], NULL);
        // printf("thread %ld JOINED...\n", thread);
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
    for (thread = 0; thread < thread_count; thread++)
    {
        pthread_join(thread_handles[thread], NULL);
        // printf("thread %ld JOINED...\n", thread);
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

    for (thread = 0; thread < thread_count; thread++)
    {
        pthread_join(thread_handles[thread], NULL);
        // printf("thread %ld JOINED...\n", thread);
    }
    free(thread_handles);

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
    /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////
    // main for loop
    for (int k = 0; k < n; k++)
    {   
        pthread_t* threadMain;
        threadMain = malloc(thread_count*sizeof(pthread_t));
        ///////find Max///////
        max=0;//required to do
        remain = thread_count;//required to synchronise

        struct findMaxArgs mstr[thread_count];
        for (thread = 0; thread < thread_count; thread++)
        {
            mstr[thread].rank=thread;
            mstr[thread].sizeForAlgo=n;
            mstr[thread].arr=a;//a is passed
            mstr[thread].k = k;
            // printf("thread %ld goes on....\n", thread);
            pthread_create(&threadMain[thread], NULL, findMax, (void*)&mstr[thread]);
        }
        for (thread = 0; thread < thread_count; thread++)
        {
            pthread_join(threadMain[thread], NULL);
            // printf("thread %ld JOINED...\n", thread);
        }
        ///////check Max///////
        // printf("max here is %f\n", max);
        if (max==0)
        {
            printf("Matrix is Singular\n");
            break;
        }

        // printf("max: %f in k: %d\n", max, kdash);
        ///////Swapping///////
        //swap π[k] and π[k']
        double tmp;tmp = pi[k];
        pi[k]=pi[kdash];
        pi[kdash]=tmp;

        //swap a
        struct swapAArgs swapastr[thread_count];
        for (thread = 0; thread < thread_count; thread++)
        {
            swapastr[thread].rank=thread;
            swapastr[thread].sizeForAlgo=n;
            swapastr[thread].arr=a;//a is passed
            swapastr[thread].k = k;
            pthread_create(&threadMain[thread], NULL, swap, (void*)&swapastr[thread]);
        }
        for (thread = 0; thread < thread_count; thread++)
        {
            pthread_join(threadMain[thread], NULL);
            // printf("thread %ld JOINED...\n", thread);
        }
        //swap l
        struct swapAArgs swaplstr[thread_count];
        for (thread = 0; thread < thread_count; thread++)
        {
            swaplstr[thread].rank=thread;
            swaplstr[thread].sizeForAlgo=k;
            swaplstr[thread].arr=l;//l is passed
            swaplstr[thread].k = k;
            pthread_create(&threadMain[thread], NULL, swap, (void*)&swaplstr[thread]);
        }
        //u(k,k) = a(k,k)
        u[k*n+k]=a[k*n+k];
        for (thread = 0; thread < thread_count; thread++)
        {
            pthread_join(threadMain[thread], NULL);
            // printf("thread %ld JOINED...\n", thread);
        }
        ///////First For loop///////
        struct ForArgs firstforstr[thread_count];
        for (thread = 0; thread < thread_count; thread++)
        {
            firstforstr[thread].k=k;
            firstforstr[thread].rank=thread;
            firstforstr[thread].sizeForAlgo=n;
            firstforstr[thread].a=a;//a is passed
            firstforstr[thread].l=l;//l is passed
            firstforstr[thread].u=u;//u is passed
            firstforstr[thread].start = k+1;
            pthread_create(&threadMain[thread], NULL, firstFor, (void*)&firstforstr[thread]);
        }
        for (thread = 0; thread < thread_count; thread++)
        {
            pthread_join(threadMain[thread], NULL);
            // printf("thread %ld JOINED...\n", thread);
        }
        ///////Second For loop///////
        struct ForArgs secforstr[thread_count];
        for (thread = 0; thread < thread_count; thread++)
        {
            secforstr[thread].k=k;
            secforstr[thread].rank=thread;
            secforstr[thread].sizeForAlgo=n;
            secforstr[thread].a=a;//a is passed
            secforstr[thread].l=l;//l is passed
            secforstr[thread].u=u;//u is passed
            secforstr[thread].start = k+1;
            pthread_create(&threadMain[thread], NULL, SecondFor, (void*)&secforstr[thread]);
        }
        for (thread = 0; thread < thread_count; thread++)
        {
            pthread_join(threadMain[thread], NULL);
            // printf("thread %ld JOINED...\n", thread);
        }
        free(threadMain);
        // printf("iteration: %d done...........\n", k);
    }

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
    free(a);
    free(pi);
    free(l);
    free(u);

    TIME_DIFF * difference;
    gettimeofday (&myTVend, NULL);
    difference = my_difftime (&myTVstart, &myTVend);
    printf ("Time: %3d.%6d secs.\n", difference->secs, difference->usecs);
    free (difference);

    return 0;
}