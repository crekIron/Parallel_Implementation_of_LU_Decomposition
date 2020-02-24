#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <string.h>

int num_threads;
int mat_size;


void write_P_to_file(int* P_mat[]){

    FILE *file;
    file = fopen("P.txt", "wb");

    for(int i=0;i<mat_size;i++) {
        for(int j=0;j<mat_size;j++) {
            fprintf(file,"%d ",P_mat[i][j]);
        }
        fprintf(file,"\n");
    }
    fclose(file);
}

void write_U_to_file(double* u_mat[]){

    FILE *file;
    file = fopen("U.txt", "wb");

    for(int i=0;i<mat_size;i++) {
        for(int j=0;j<mat_size;j++) {
            fprintf(file,"%lf ",u_mat[i][j]);
        }
        fprintf(file,"\n");
    }
    fclose(file);
}

void write_L_to_file(double* l_mat[]){

    FILE *file;
    file = fopen("L.txt", "wb");

    for(int i=0;i<mat_size;i++) {
        for(int j=0;j<mat_size;j++) {
            fprintf(file,"%lf ",l_mat[i][j]);
        }
        fprintf(file,"\n");
    }
    fclose(file);
}

void verification(int size, double* pi, double* mat_a[], double* mat_l[], double* mat_u[]){
    double* result[size];
    int* P_mat[size];     
    double norm = 0;
    double sum = 0;

    //Generating P matrix
    for(int i =0; i<size; i++){
        P_mat[i] = (int*)malloc(size * sizeof(int));
        result[i] = (double*) malloc(size * sizeof(double));
        for(int j =0; j<size;j++){
            if(j == pi[i])
                P_mat[i][j] = 1;
            else
                P_mat[i][j] = 0;
        }
    }

    //Computing PA-LU in result
    for(int i=0 ;i<size; i++){
        for(int j=0; j<size;j++){
            result[j][i] = 0;
            for(int k=0; k<size;k++){
                result[j][i] += ((P_mat[j][k] * mat_a[k][i]) - (mat_l[j][k] * mat_u[k][i]));
            }
            sum += (result[j][i] * result[j][i]);
        }
        norm += sqrt(sum);
    }
    printf("The L(2,1) Norm of the residual matrix PA-LU is: %lf\n",norm );
    write_P_to_file(P_mat);
    write_U_to_file(mat_u);
    write_L_to_file(mat_l);
}

int main(int argc, char* argv[])
{
    // inputs: a(n,n)
    // outputs: pi(n), l(n,n), and u(n,n)
    
    //Variables for time calculation
    double start, end;
    start = omp_get_wtime();

    int n = atoi(argv[2]);
    mat_size = n;

    //Buffer for random number generator
    struct drand48_data drand_buf;

    //Initialisation of Arrays
    double *a[n];
    double *pi;
    double *l[n];
    double *u[n];
    double *a_copy[n];
    int i ,j, k;
    
    int seed = 0;

    num_threads = atoi(argv[3]);
    //Setting the number of threads
    omp_set_num_threads(num_threads);
    

    //Memory Allocation for pi
    pi = malloc(n*sizeof(double*));

    //Seeding the random number generator
    seed = omp_get_thread_num() * 19 + 23;
    srand48_r(seed, &drand_buf);
    FILE *file;
    file = fopen(argv[1],"r");
    
    //Memory allocation and Initialization of matrices.
    //Parallelization with parallel for. Row-wise data partitioning
    #pragma omp parallel for private(j)
        for(i=0; i<n; i++){
            a[i] = malloc(n*sizeof(double*));
            a_copy[i] = malloc(n*sizeof(double*));
            l[i] = malloc(n*sizeof(double*));
            u[i] = malloc(n*sizeof(double*));
            pi[i] = i;
            for(j=0; j<n;j++){
                fscanf(file, "%lf", &a[i][j]);
                a_copy[i][j] = a[i][j];
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
    printf("Time Taken by the program is: %lf\n", time_spent);
    verification(n,pi,a_copy,l,u);

    //FREEING THE DYNAMICALLY ALLOCATED MEMORY
    for(i=0; i<n; i++){
        free(a[i]);
        free(l[i]);
        free(u[i]);
    }
    free(pi);
    return 0;
}
