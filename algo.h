//////////////////////////////////////////////////
// This file will contain the helper functions
// used for main for loop.
//////////////////////////////////////////////////
#ifndef ALGO_H
#define ALGO_H
#include "global.h"
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

void *findMax(void *a);
void *swapA(void *a);
void *firstFor(void *forone);
void *SecondFor(void *fortwo);

struct findMaxArgs{
    int k;
    int rank;
    int sizeForAlgo;
    double *arr;
};

struct swapAArgs{
    int k;
    int rank;
    int sizeForAlgo;
    double *arr;
    double tmp;
};

struct swapLArgs{
    int k;
    int rank;
    int sizeForAlgo;
    double *arr;
    double tmp;
};

struct ForArgs{
    int k;
    int start;//k+1
    int rank;
    int sizeForAlgo;//n
    double *l;
    double *a;
    double *u;
};

double Abs(double a)
{
    if (a<0) return -1*a;
    return a;
}

void *findMax(void *a)
{
    struct findMaxArgs *l_args = (struct findMaxArgs*) a;
    int rank = (*l_args).rank;
    int k = (*l_args).k;
    int sizeForAlgo = (*l_args).sizeForAlgo;

    //divison on basis of the row
    for (int row=k+rank, i=0; row < sizeForAlgo; row=k+rank+thread_count*(++i))
    {
        double tmp = Abs((*l_args).arr[row*sizeForAlgo+k]);
        //critical section
        pthread_mutex_lock(&lock);
        if (max < tmp)
        {
            max = tmp;
            kdash = row;
        }
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

void *swap(void *amat)
{
    struct swapAArgs *l_args = (struct swapAArgs*) amat;
    int rank = (*l_args).rank;
    int k = (*l_args).k;
    int sizeForAlgo = (*l_args).sizeForAlgo;
    
    // printf("The threads has rank:%d kdash:%d k:%d sizeForAlgo:%d\n", rank, kdash, k, (*l_args).sizeForAlgo);

    for (int col = rank, i=0; col < sizeForAlgo; col = rank + thread_count*(++i))
    {
        (*l_args).tmp = (*l_args).arr[k*n+col];
        (*l_args).arr[k*n+col] = (*l_args).arr[kdash*n+col];
        (*l_args).arr[kdash*n+col] = (*l_args).tmp;
    }
    return NULL;
}

void *firstFor(void *forone)
{
    struct ForArgs *l_args = (struct ForArgs*) forone;
    int rank = (*l_args).rank;
    int k = (*l_args).k;
    int sizeForAlgo = (*l_args).sizeForAlgo;
    int start = (*l_args).start;

    // printf("The threads has rank:%d kdash:%d k:%d sizeForAlgo:%d\n", rank, kdash, start, (*l_args).sizeForAlgo);

    for (int rowcol = start+rank, i=0; rowcol < sizeForAlgo; rowcol = start + rank + thread_count*(++i))
    {
        (*l_args).l[rowcol*sizeForAlgo+k] = ((*l_args).a[rowcol*sizeForAlgo+k])/((*l_args).u[k*sizeForAlgo+k]);
        (*l_args).u[k*sizeForAlgo+rowcol] = (*l_args).a[k*sizeForAlgo+rowcol];
    }
    return NULL;
}

void *SecondFor(void *fortwo)
{
    struct ForArgs *l_args = (struct ForArgs*) fortwo;
    int rank = (*l_args).rank;
    int k = (*l_args).k;
    int sizeForAlgo = (*l_args).sizeForAlgo;
    int start = (*l_args).start;

    // printf("The threads has rank:%d kdash:%d k:%d sizeForAlgo:%d\n", rank, kdash, start, (*l_args).sizeForAlgo);

    for (int row=start+rank, i=0; row < sizeForAlgo; row=start+rank+thread_count*(++i))
    {
        for (int col = start; col < sizeForAlgo; col++)
        {
            (*l_args).a[row*sizeForAlgo+col] -= ((*l_args).l[row*sizeForAlgo+k])*((*l_args).u[k*sizeForAlgo+col]);
        }
    }
    return NULL;
}

#endif