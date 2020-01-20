#include<stdio.h>
#include<stdlib.h>
#include <time.h>

void swap(int n,int k, int k_dash, double *pi, double *a[], double *l[]){
    #pragma omp parallel
    {
        double temp = 0;

        //Swap of pi
        temp = pi[k];
        pi[k] = pi[k_dash];
        pi[k_dash] = temp;

        #pragma omp for
        //Swap of a and l
        for(int i=0; i<n;i++){
            temp = a[k][i];
            a[k][i] = a[k_dash][i];
            a[k_dash][i] = temp;

            if(i<k){
                temp = l[k][i];
                l[k][i] = l[k_dash][i];
                l[k_dash][i] = temp;
            }
        }
    }
} 

double get_abs(double num){
    if(num >= 0)
        return num;
    else
        return (-1 * num);
}
int main(int argc, char const *argv[])
{
    // inputs: a(n,n)
    // outputs: pi(n), l(n,n), and u(n,n)
    
    //Initialisation of Arrays
    clock_t begin = clock();
    int n = 1000;
    struct drand48_data drand_buf;
    double *a[n];
    double *pi;
    double *l[n];
    double *u[n];
    int i ,j, k;
    
    int seed = 0;
    double rand_num;
    omp_set_num_threads(16);

    printf("%d \n", omp_get_num_procs());
    ///////Intialisation///////
    //initialize n
    
    //initialize pi
    pi = malloc(n*sizeof(double*));
    seed = omp_get_thread_num() * 19 + 23;
    srand48_r(seed, &drand_buf);
    
    
    //initialize a
    #pragma omp parallel for private(j)
        for(i=0; i<n; i++){
            a[i] = malloc(n*sizeof(double*));
            l[i] = malloc(n*sizeof(double*));
            u[i] = malloc(n*sizeof(double*));
            pi[i] = i;
            // #pragma omp for
            for(j=0; j<n;j++){
                drand48_r(&drand_buf,&a[i][j]);
                if(i == j){
                    l[i][j] = 1;
                    drand48_r(&drand_buf,&u[i][j]);
                }
                else{
                    if(i > j){
                        drand48_r(&drand_buf,&l[i][j]);
                        u[i][j] = 0;
                    }
                    else{
                        drand48_r(&drand_buf,&u[i][j]);
                        l[i][j] = 0;   
                    }
                }
            }
        }

    printf("Initialisation done!");
    // #pragma omp parallel for
    int k_dash = 0;

    for(k=0; k<n;k++){
        double max = 0;
        double temp = 0;
        #pragma omp parallel for
        for(int i = k; i<n;i++){
            temp = get_abs(a[i][k]);
            #pragma omp critical
            {
                if(max < temp){
                    max = temp;
                    k_dash = i;
                }
            }
        }
        if(max == 0){
            printf("Error: Singular Matrix");
            exit(0);
        }
        swap(n,k, k_dash, pi, a, l);
        u[k][k] = a[k][k];

        for(i = k+1; i<n;i++){
            for(j = k+1; j<n;j++){
                l[j][k] = a[j][k]/u[k][k];
                u[k][j] = a[k][j];
                a[i][j] = a[i][j] - (l[i][k] * u[k][j]);
            }
        }
    }
    clock_t end = clock();
    double time_spent = (double)((end-begin))/CLOCKS_PER_SEC;
    printf("\n %f\n", time_spent);

    return 0;
}
