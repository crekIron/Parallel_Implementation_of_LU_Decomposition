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

    int *arr;
    arr = malloc(4*sizeof(int*));
    arr[0]=0;arr[1]=1;arr[2]=2;arr[3]=3;

    &arr[3] = &arr[1];

    for (int i = 0; i < 4; i++)
    {
        printf("%d", arr[i]);
    }
    

    return 0;
}
