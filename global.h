//////////////////////////////////////////////////
// This file will contain global variables.
//
// CAUTION and COOL BOTH: These variables are also
// accessible by threads.
//////////////////////////////////////////////////
#ifndef GLOBAL_H
#define GLOBAL_H 
#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>

/*  Global variable: accessible to all threads */
int thread_count;
int n;
long int seedval=0;
double max=0;
int kdash=0;
int remain;

pthread_mutex_t lock;

#endif