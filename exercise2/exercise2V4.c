/*
* Parallel programming
* program using MPI - exercise 2
* Author: Candice PERSIL
* Goal:
*
* Compile the program with 'mpicc -O3 exercise2V4.c -o exercise2V4'
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <mpi.h>

/* Read the matrix size in the file name and convert int in an integer*/
int sizeOfMatrix(char *fileName){
	int i, N;
	int nameSize = 0;
	/* Count the file name array size */
	while(fileName[nameSize]!='\0'){
		nameSize++;
	}
	/* If the file name does not start with M, all processes have to be stoped */
	if(fileName[0]!='M'){
		MPI_Finalize();
		printf("Error in file name, it has to start with char M!\n");
		exit(1);
	}
	/* If the file name has no extension, stop */
	if(fileName[nameSize-4]!='.'){
		MPI_Finalize();
		printf("Error in file name, no extension!\n");
		exit(1);
	}
	/* If the file name has not the right extension, stop */
	for(i=0;i<nameSize;i++){
		if(fileName[i]=='.'){
			if((fileName[i+1]!='d')||(fileName[i+2]!='a')||(fileName[i+3]!='t')){
				MPI_Finalize();
				printf("Error in file name, wrong file extension!\n");
				exit(1);
			}
		}
	}
	printf("File %s has a name of %d char.\n", fileName, nameSize);
	/* Creation of a new table of char that contains only the matrix size */
	char tabN[nameSize-4];
	int j=0;
	for(i=0;i<nameSize;i++){
		if((fileName[i]!='M')&&(fileName[i]!='K')&&(fileName[i]!='.')&&(fileName[i]!='d')&&(fileName[i]!='a')&&(fileName[i]!='t')){
			tabN[j]=fileName[i];
			j++;
		}
		/* Take into account when it is written K */
		else if(fileName[i]=='K'){
			tabN[j]=tabN[j+1]=tabN[j+2]='0';
			j=j+2;
		}
	}
	/* Convert this array in an integer N */
	sscanf(tabN,"%d",&N);
	return N;
}

int sizeOfSubMatrix(int N, float square){
	/* check that the number of elements in the matrices is evenly divisible by the square 
	root of the number of processes */
	float sN = N/square;
	if(sN != floor(sN)){
		printf("The submatrix size is not an integer.\n");
		/* Quit if there is not a valid number of processes */
		MPI_Finalize();
		exit(1);
	}
	int sNint = (int)sN;
	return sNint;
}

/* Reads a matrix M of size N*N from the file fn in binary format
   Returns zero if the file couldn't be opened, otherwise 1  */
int fread_matrix(float *matrix, int mSize, char *fileName) {
  FILE *p;
  int i;
  /* Open the file */
  if ((p=fopen(fileName, "r")) == NULL) {
    printf("Couldn't open file %s\n", fileName);
    return(0);
  }
  /* Read the matrix from the file fn in binary format */
  fread(matrix, sizeof(float), mSize*mSize, p);
  fclose (p);
  return(1);
}  


/* Writes a matrix M of size N*N to the file fn in binary format
   Returns zero if the file couldn't be opened, otherwise 1       */
int fwrite_matrix(float *matrix, int mSize, char *fileName) {
  FILE *p;
  int i;
  /* Open the file */
  if ((p=fopen(fileName, "w")) == NULL) {
    printf("Couldn't open file %s\n", fileName);
    return 0;
  }
  /* Write the matrix to the file fn in binary format */
  fwrite(matrix, sizeof(float), mSize*mSize, p);
  fclose(p);  /* Close the file */
  return(1);
}

/* Prints a matrix of size N*N  */
void print_matrix(float *matrix, int mSize) {
  int i, j, tmp;
  for (i=0; i<mSize; i++) {
    tmp = i*mSize;
    for (j=0; j<mSize; j++) {
      printf("%5.1f ", matrix[tmp+j]);
    }
    printf("\n");
  }
  printf("\n");
}

/* Multplies two square matrices X and Y of order N and places the
   result in Z. The matrix Z is assumed to be initialized to zero  */
void matrixmult(float **X, float **Y, float **Z, int N) {
  int i,j,k;
  for (i=0; i<N; i++) {
    for (j=0; j<N; j++) {
      for (k=0; k<N; k++)
	Z[i][j] += X[i][k]*Y[k][j];
    }
  }
}


int main(int argc, char *argv[]) {
  
	int id, ntasks, i, j, N, sNint;                  // Size of matrix
	float square, sN;
	char fileName[10];      // table of char that contains file name
	float **A, **C; /* Input matrix A, output matrix C*/
	float *Ab, *Cb; /* Input and output data */
	//clock_t microsecs;
	
	MPI_Status status;
	
	/* Initialize MPI */
	if ( MPI_Init(&argc, &argv) != MPI_SUCCESS) {
	printf("MPI_init failed!\n");
	exit(1);
	}

	/* Get number of tasks */
	if ( MPI_Comm_size(MPI_COMM_WORLD, &ntasks) != MPI_SUCCESS) {
	printf("MPI_Comm_size failed!\n");
	exit(1);
	}

	/* Get id of this process */
	if ( MPI_Comm_rank(MPI_COMM_WORLD, &id) != MPI_SUCCESS) {
	printf("MPI_Comm_rank failed!\n");
	exit(1);
	}
	
	square = sqrt(ntasks);//float that contains the sqrt of the total number of processes
	/* If the square is not equal to the largest integer value less than or equal to ntasks, stop*/
	if (square != floor(square)) {
		if(id==0){
			printf("You have to use a square number of processes: 4, 9, 16, 25, ...\n");
		}
		MPI_Finalize();/* Quit if there is not a valid number of processes */
		exit(1);
	}
	
	if (id==0){
		printf("Give file name: "); fflush(stdout);
		scanf("%s", fileName);
		N = sizeOfMatrix(fileName);
		printf("The matrix size is: %dx%d.\n", N, N);
	
		sNint = sizeOfSubMatrix(N, square);
		printf("We need %d submatrices with a size of: %dx%d\n", ntasks, sNint, sNint);
	
		// Allocate the input matrix A
		A = (float **) malloc(N*sizeof(float *));  /* Row pointers */
		Ab = (float *) malloc(N*N*sizeof(float));   /* Data block */

		// Allocate the result matrix C
		C = (float **) malloc(N*sizeof(float *));  /* Row pointers */
		Cb = (float *) malloc(N*N*sizeof(float));   /* Data block */

		/* Initialize pointers to rows in the matrices */
		for (i=0; i<N; i++) {
		A[i] = Ab + i*N;
		C[i] = Cb + i*N;
		}
		/* Set matrices to zero  */
		for (i=0; i<N; i++)
		for (j=0; j<N; j++) {
		  A[i][j] = 0.0;
		  C[i][j] = 0.0;
		}

		/*printf("Give name of file: "); fflush(stdout);
		scanf("%s", fileName);*/
		fread_matrix(&A[0][0], N, fileName);    // Read input file

		if (N <= 20) {    // Print it if the matrix is small
			printf("The matrix is:\n");
			print_matrix(&A[0][0], N);
		}
	
		// Multiply M with itself, place the result i R
		//clock();    // Measure time
		//matrixmult(M, M, R, N);
		//microsecs = clock();
		//printf("Time for matrix multiplication %6.2f seconds\n\n", microsecs/1000000.0);
	/*
		if (N <= 20) {    // Print it if the matrix is small
			printf("The squared matrix is:\n");
			print_matrix(&C[0][0], N);
		}
	*/
		/* Write the result to a file */
		//fwrite_matrix(&C[0][0], N, "result.dat");
	}
	/* All processes do this */
	if ( MPI_Finalize() != MPI_SUCCESS) {
		printf("Error in MPI_Finalize!\n");
		exit(1);
	}

	if (id==0){
		printf("End\n");
	}
	exit(0);
}
