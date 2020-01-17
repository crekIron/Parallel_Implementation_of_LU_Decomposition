#ifndef INIT_H
#define INIT_H

#include <stdlib.h>
#include <pthread.h>

#include "global.h"

struct Arr_arg{
    long rank;
    double *pi;
    int size;
    int thread_count;
};

struct Mat_arg{
    long rank;
    double *arr[n];
    int size;
    int thread_count;
};

void* Init_pi(void* pi)
{
    struct Arr_arg *pi_args = (struct Arr_arg*) pi;
    long rank = (*pi_args).rank;
    int size = (*pi_args).size;
    int thread_count = (*pi_args).thread_count;   

    for (int n=rank, i=0; n < size; n=rank+thread_count*(++i))
    {
        *((*pi_args).pi+n) = n;
    }
}
void* Init_l(void* l)
{
    struct Mat_arg *l_args = (struct Mat_arg*) l;
    long rank = (*l_args).rank;
    int size = (*l_args).size;
    int thread_count = (*l_args).thread_count;   

    //Generation of the random number
    double x;
    struct drand48_data randBuffer;
    srand48_r(time(rank), &randBuffer);

    for (int n=rank, i=0; n < size; n=rank+thread_count*(++i))
    {
        for (int col = 0; col < size; col++)
        {
            if(n<col)
            {
                //random initialisation
                drand48_r(&randBuffer, &x);
                (*l_args).arr[n][col] = x;
            }
            else
            {
                (*l_args).arr[n][col] = 0;
            }
            
        }
    }
}
void* Init_p(void* p)
{
    struct Mat_arg *l_args = (struct Mat_arg*) p;
    long rank = (*l_args).rank;
    int size = (*l_args).size;
    int thread_count = (*l_args).thread_count;   

    //Generation of the random number
    double x;
    struct drand48_data randBuffer;
    srand48_r(time(rank), &randBuffer);

    for (int n=rank, i=0; n < size; n=rank+thread_count*(++i))
    {
        for (int col = 0; col < size; col++)
        {
            if(n=col)
            {
                (*l_args).arr[n][col] = 0;
            }
            else if(n<col)
            {
                (*l_args).arr[n][col] = 0;
            }
            else
            {
                //random initialisation
                drand48_r(&randBuffer, &x);
                (*l_args).arr[n][col] = x;
            }
        }
    }
}

#endif