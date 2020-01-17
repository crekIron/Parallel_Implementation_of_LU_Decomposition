#include<stdio.h>
#include<stdlib.h>


int main(int argc, char const *argv[])
{
    //TEST:1//
    double x;
    struct drand48_data randBuffer;

    srand48_r(time(NULL), &randBuffer);
    
    drand48_r(&randBuffer, &x);
    printf("Random number: %f\n", x);

    drand48_r(&randBuffer, &x);
    printf("Random number: %f\n", x);

    drand48_r(&randBuffer, &x);
    printf("Random number: %f\n", x);
    return 0;
}
