#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
 
// void verification(){
//     double x1[8000][8000] ;
//     double sum1 =0.0;
//     double sum =0.0;
//     for(int i=0; i<n; i++){
//         for(int j=0; j<n; j++){
//             sum=0;
//             for(int k=0; k<n; k++){
//                sum = sum + (l[i][k]*u[k][j]);
//             }
//             x1[i][j] = sum;
//      // printf("%lf ",sum);
//         }
//     // printf("\n");
//     }
//     // printf("PA\n");
//     for(int i=0; i<n; i++){
//         for(int j=0; j<n; j++){
//              sum=0;
//              for(int k=0; k<n; k++){
//        // sum = sum + (p[i][k]*a[k][j]);
//                sum = sum + (p[i][k]*xap[k][j]);
//              }
//             x1[i][j] = sum - x1[i][j];
//      // printf("%lf ",sum);
//         }
//     // printf("\n");
//     }
//     // printf("\n");
//     sum1 = 0;
//     for(int i=0; i<n; i++){
//         sum  = 0;
//         for(int j=0; j<n; j++){
//          sum =  sum + (x1[j][i]*x1[j][i]);
//         }
//         sum1 = sum1 + sqrt(sum);
//     }
//     printf("%lf\n",sum1 );
// }

int main(int argc, char const *argv[])
{
    // inputs: a(n,n)
    // outputs: pi(n), l(n,n), and u(n,n)
    
    //Variables for time calculation
    double start, end;
    int n = 8000;
    printf("Size of the Matrix:");
    scanf("%d", &n);
    int thread=4;
    printf("No of threads:");
    scanf("%d", &thread);
    start = omp_get_wtime();
    //Buffer for random number generator
    struct drand48_data drand_buf;

    //Initialisation of Arrays
    double *a[n];
    double *pi;
    double *l[n];
    double *u[n];
    int i ,j, k;
    
    int seed = 0;

    //Setting the number of threads
    omp_set_num_threads(thread);
    

    //Memory Allocation for pi
    pi = malloc(n*sizeof(double*));

    //Seeding the random number generator
    seed = omp_get_thread_num() * 19 + 23;
    srand48_r(seed, &drand_buf);
    
    
    //Memory allocation and Initialization of matrices.
    //Parallelization with parallel for. Row-wise data partitioning
    #pragma omp parallel for private(j)
        for(i=0; i<n; i++){
            a[i] = malloc(n*sizeof(double*));
            l[i] = malloc(n*sizeof(double*));
            u[i] = malloc(n*sizeof(double*));
            pi[i] = i;
            for(j=0; j<n;j++){
                drand48_r(&drand_buf,&a[i][j]);
                a[i][j] = (a[i][j] + 0.5 )/2;
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
    
    int k_dash = 0;

    //MAIN FUNCTION BEGINS
    for(k=0; k<n;k++){

        
        double max = 0;
        double temp = 0;
        #pragma omp parallel private(temp)
        {
                //FINDING MAX AND ERROR CHECK ALONG WITH SWAP OF PI
                #pragma omp for
                for(i = k; i<n;i++){
                    temp = a[i][k];
                    if(temp < 0)
                        temp = temp*(-1);
                    #pragma omp critical
                    {
                        if(max < temp){
                            max = temp;
                            k_dash = i;
                        }
                    }
                }
            
                #pragma omp barrier
                #pragma omp single
                {
                    if(max == 0){    
                        printf("%d Error: Singular Matrix\n", k_dash);
                        exit(0);
                    }
                    temp = pi[k];
                    pi[k] = pi[k_dash];
                    pi[k_dash] = temp;
                }
                //MAX LOOP FINISH

                //SWAPPING LOOP BEGINS FOR MATRICES A AND L. UPDATES TO U MATRIX ALSO HAPPEN. DATA PARTITIONING WITHIN A COLUMN
                #pragma omp for
                for(int i=0; i<n;i++){
                    temp = a[k][i];
                    a[k][i] = a[k_dash][i];
                    a[k_dash][i] = temp;

                    if(i<k){
                        temp = l[k][i];
                        l[k][i] = l[k_dash][i];
                        l[k_dash][i] = temp;
                    }
                    else{
                        u[k][i] = a[k][i];
                    }
                }
                
            #pragma omp barrier
            //SWAPPING FINISH AND BARRIER PLANTED SO THAT ALL THREADS SYNCHRONISE HERE FIRST BEFORE PROCEEDING AHEAD

            //UPDATES TO L MATRIX
            #pragma omp for nowait
            for(j = k+1; j<n;j++){
                l[j][k] = a[j][k]/u[k][k];
            }
            
            #pragma omp barrier
            //END OF UPDATES TO L. AGAIN A SYNCHRONISATION WAIT SO THAT LOOP AHEAD GETS THE RIGHT VALUE

            //NESTED FOR LOOP TO UPDATE A MATRIX.
            #pragma omp for private(i,j) collapse(2)
            for(i = k+1; i<n;i++){
                for(j = k+1; j<n;j++){
                    a[i][j] = a[i][j] - (l[i][k] * u[k][j]);
                }
            }
            //LOOP FINISH
        }
        //HERE PARALLEL PRAGMA DECLARED AT TOP ENDS AND ALL THE THREADS CREATED SYNCHRONISE AND DIE 
        //SO THAT ONLY MASTER THREAD CONTINUES THE EXTERNAL LOOP AHEAD BEFORE THE CREATION OF NEW THREADS AGAIN
    }
    //NOTING THE TIME TAKEN IN THE PROCESS
    end = omp_get_wtime();
    double time_spent = (double)(end-start);
    printf("\n %f\n", time_spent);

    //FREEING THE DYNAMICALLY ALLOCATED MEMORY
    for(i=0; i<n; i++){
        free(a[i]);
        free(l[i]);
        free(u[i]);
    }
    free(pi);
    return 0;
}
