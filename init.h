//////////////////////////////////////////////////
// This file will contain the thread functions
// for initialisation of the matrix.
//////////////////////////////////////////////////
#ifndef INIT_H
#define INIT_H

#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>


#include "global.h"

void* Init_pi(void* pi);
void* Init_l(void* l);
void* Init_u(void* u);
void* genrateMat(void* a);



struct Arr_arg{
    long rank;
    double *pi;
    int size;
};

struct Mat_arg{
    long rank;
    double *arr;
    int size;
    long int seedval;
};

void* genrateMat(void* a)
{
    struct Mat_arg *l_args = (struct Mat_arg*) a;
    long int rank = (*l_args).rank;
    long int seedval = (*l_args).seedval;
    int size = (*l_args).size;
    //Generation of the random number
    double x;
    struct drand48_data randBuffer;
    srand48_r(seedval, &randBuffer);
    for (int row=rank, i=0; row < size; row=rank+thread_count*(++i))
    {
        for (int col = 0; col < size; col++)
        {
            //random initialisation
            drand48_r(&randBuffer, &x);
            (*l_args).arr[row*size+col] = x;
        }
    }
    // printf("Thread %ld completed the work\n", rank);
    return NULL;
}

void* Init_pi(void* pi)
{
    struct Arr_arg *pi_args = (struct Arr_arg*) pi;
    long int rank = (*pi_args).rank;
    int size = (*pi_args).size;  

    for (int row=rank, i=0; row < size; row=rank+thread_count*(++i))
    {
        (*pi_args).pi[row] = row;
    }
    // printf("Thread %ld completed the work\n", rank);
    return NULL;
}

void* Init_u(void* l)
{
    struct Mat_arg *l_args = (struct Mat_arg*) l;
    unsigned rank = (*l_args).rank;
    long int seedval = (*l_args).seedval;
    int size = (*l_args).size;

    //Generation of the random number
    double x;
    struct drand48_data randBuffer;
    srand48_r(seedval, &randBuffer);
    //division on the basis of the row
    for (int row=rank, i=0; row < size; row=rank+thread_count*(++i))
    {
        for (int col = 0; col < size; col++)
        {
            if(row<col)
            {
                //random initialisation
                drand48_r(&randBuffer, &x);
                (*l_args).arr[row*size+col] = x;
            }
            else
            {
                (*l_args).arr[row*size+col] = 0;
            }
            
        }
    }
    return NULL;
}
void* Init_l(void* u)
{
    struct Mat_arg *l_args = (struct Mat_arg*) u;
    unsigned int rank = (*l_args).rank;
    long int seedval = (*l_args).seedval;
    int size = (*l_args).size;

    //Generation of the random number
    double x;
    struct drand48_data randBuffer;
    srand48_r(seedval, &randBuffer);
    //division on the basis of the row
    for (int row=rank, i=0; row < size; row=rank+thread_count*(++i))
    {
        for (int col = 0; col < size; col++)
        {
            if(row==col)
            {
                (*l_args).arr[row*size+col] = 1;
            }
            else if(row<col)
            {
                (*l_args).arr[row*size+col] = 0;
            }
            else
            {
                //random initialisation
                drand48_r(&randBuffer, &x);
                (*l_args).arr[row*size+col] = x;
            }
        }
    }
    return NULL;
}

#endif