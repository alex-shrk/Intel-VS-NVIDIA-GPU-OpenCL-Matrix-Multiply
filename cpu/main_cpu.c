#include <stdio.h>
#include <stdlib.h>
#include <time.h>
// Size of the matrices - K, M, N (squared)
#define SIZE 1024
#define COUNT_RUNS 3


double calculate(){
  //int i,j,k;
  int K = SIZE;
  int M = SIZE;
  int N = SIZE;
  // Create the matrices and initialize them with random values
  float **A = (float**)malloc(M*K*sizeof(float*));
  float **B = (float**)malloc(K*N*sizeof(float*));
  float **C = (float**)malloc(M*N*sizeof(float*));

  for (int i=0;i<SIZE*SIZE;i++){
    A[i] = (float *)malloc(sizeof(float));
    B[i] = (float *)malloc(sizeof(float));
    C[i] = (float *)malloc(sizeof(float));
  }

  for (int i=0; i<M*K; i++) { *A[i] = 3.6*i + i*i + 3.1; }
  for (int i=0; i<K*N; i++) { *B[i] = 1.2*i + 0.01*i*i + 13.9; }
  for (int i=0; i<M*N; i++) { *C[i] = 0.0; }
  float acc = 0.0f;
  clock_t begin = clock();
  
  for (int i = 0; i < M; i++) {
    for (int j = 0; j < N; j++) {
      for (int k = 0; k < K; k++) {
        acc+= A[i][k] * B[k][j];
      }
      C[i][j] = acc;
    }
  }

  clock_t end = clock();
  //Display the result to the screen
  //for (int i = 0; i < M; i++) {
  //  for (int j = 0; j < N; j++) {
  //    printf("%f\t", C[i][j]);
  //  }
  //  printf("\n");
  //}
  
  free(A);
  free(B);
  free(C);
  double runtime = (double)(end - begin) / CLOCKS_PER_SEC;
  return runtime;
}
int main() {
  double time_execution=0.0;
  double average_time=0.0;
 
  //cold start
  time_execution=calculate();

  for (int j =0;j<COUNT_RUNS;j++){
      time_execution=calculate();
      average_time+=time_execution;
	  printf("Execution time is: %0.5f milliseconds \n", time_execution);
  }
  average_time=average_time / COUNT_RUNS;
  printf("Measure execution time is: %0.5f milliseconds \n", average_time);
}
