/*
* Parallel programming
* program using MPI - exercise 2
* Author: Candice PERSIL
* Goal:
*
* Compile the program with 'mpicc -O3 exercise2.c -o exercise2'
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
	const int tag = 42;	        /* Message tag */
	int id, ntasks, source_id, i, j, N;
	/* N is the size of the Matrix */
	double square, sN;	/*square root of the number of processes, sN is the submatrix size*/
	MPI_Status status;
	char msg[80], fileName[20];	/* Message array  and file name array */
	double **A, **B; /* Matrices A and B, input matrices */
	double **subMatrices[ntasks];

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
	square = sqrt(ntasks);//double that contains the sqrt of the total number of processes
	/* If the square is not equal to the largest integer value less than or equal to ntasks, stop*/
	if (square != floor(square)) {
		if(id==0){
			printf("You have to use a square number of processes: 4, 9, 16, 25, ...\n");
		}
		MPI_Finalize();/* Quit if there is not a valid number of processes */
		exit(0);
	}
	if(id==0){
		printf("Enter file name:\n");/*Ask users to enter file name*/
		fflush(stdout);
		scanf("%s", fileName);
		int nameSize = 0;
		/* Count the file name array size */
		while(fileName[nameSize]!='\0'){
			nameSize++;
		}
		/* If the file name does not start with M, all processes have to be stoped */
		if(fileName[0]!='M'){
			MPI_Finalize();
			printf("Error in file name, it has to start with char M!\n");
			exit(0);
		}
		/* If the file name has no extension, stop */
		if(fileName[nameSize-4]!='.'){
			MPI_Finalize();
			printf("Error in file name, no extension!\n");
			exit(0);
		}
		/* If the file name has not the right extension, stop */
		for(i=0;i<nameSize;i++){
			if(fileName[i]=='.'){
				if((fileName[i+1]!='d')||(fileName[i+2]!='a')||(fileName[i+3]!='t')){
					MPI_Finalize();
					printf("Error in file name, wrong file extension!\n");
					exit(0);
				}
			}
		}
		printf("File %s has a name of %d char.\n", fileName, nameSize);
		char tabN[nameSize-4];/* Creation of a new table of char that contains only the matrix size */
		int j=0;
		for(i=0;i<nameSize;i++){
			if(fileName[i]=='M'){			
			}
			else if(fileName[i]=='.'){
					break;
			}
			else{
				tabN[j]=fileName[i];/* Fill in the array with the matrix size */
				j++;
			}
		}
		sscanf(tabN,"%d",&N);/* Convert this array in an integer N */
		printf("The matrix size is: %dx%d.\n", N, N);
		
		/* check that the number of elements in the matrices is evenly divisible by the square 
		root of the number of processes */
		sN=N/square;
		if(sN != floor(sN)){
			printf("The submatrix size is not an integer.\n");
			MPI_Finalize();/* Quit if there is not a valid number of processes */
			exit(0);
		}
		else{
			printf("Submatrix size: %g\n", sN);
		}
		/* alocate memory for the input matrices and local sub-matrices */
		/* we will alocate one row at a time */
		A = (double **) malloc(N*sizeof(double *));
		for (i=0; i<N; i++){
			A[i] = (double *) malloc(N*sizeof(double));
		}
		for (i=0; i<N; i++){
			for(j=0;j<N;j++){
				A[i][j] = 0.0;
			}
		}
		/* We do not need to use B as we have to multiply a matrix by itself */
		/*B = (double **) malloc(N*sizeof(double *));
		for (i=0; i<N; i++){
			B[i] = (double *) malloc(N*sizeof(double));
		}
		for (i=0; i<N; i++){
			for(j=0;j<N;j++){
				B[i][j] = 0.0;
			}
		}*/
		// alocate memory for local sub-matrices
		/*for(i=0;i<ntasks;i++){
			//alocate place for matrix rows at place i of the table
			//subMatrices[i] = (double **) malloc(N*sizeof(double *));
			for (i=0; i<N; i++){
				//at place i, alocate place for matrix cols
				//A[i] = (double *) malloc(N*sizeof(double));
			}
			for (i=0; i<N; i++){
				for(j=0;j<N;j++){
					//A[i][j] = 0.0;
				}
			}
		}*/

		/* Open the binary file, read the input matrices */
		//unsigned char buff[100];
		//File *p;
		//p=fopen(fileName,"rb");		
		//fread(buff,sizeof(buff),1,p);
	}
	free(A);
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
