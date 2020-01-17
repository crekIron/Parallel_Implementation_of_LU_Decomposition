/* gcc -g -Wall -o main main.c -lpthread */
#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>

#include "init.h"
#include "global.h"

void* Init_pi(void* pi);
void* Init_l(void* l);
void* Init_p(void* p);


int main(int argc, char const *argv[])
{
    // inputs: a(n,n)
    // outputs: pi(n), l(n,n), and u(n,n)
    
    //Implementation of array depends
    //Using single array; access element by row*n+col
    double *a;
    //Using array of pointers; access element by a[i][j] or *(*(a+row)+col)
    //Required: No of Rows = n
    double *a[n];

    double *pi;
    double *l[n];
    double *u[n];
    ///////Intialisation///////
    //initialize n
    
    //initialize pi
    pi = malloc(n*sizeof(double*));
    //initialize l

    //initialize p


    return 0;
}
