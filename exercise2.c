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
#include "exercise2.h"

int main(int argc, char *argv[]){
	const int tag = 42;	        /* Message tag */
	int id, ntasks, source_id, i, j, k, N;
	/* N is the size of the Matrix */
	double square, sN;	/*square root of the number of processes, sN is the submatrix size*/
	MPI_Status status;
	char msg[80], fileName[20], buff[50];	/* Message array, file name array and buff array*/
	double **A, **B; /* Matrices A and B, input matrices */
	FILE *p;
	struct subMatrix sMTab[ntasks]; /* Creation of a table of struct that contains a matrix*/

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
		if (A==NULL){
			printf("Allowance error.\n");
			MPI_Finalize();
			exit(0);
		}
		for (i=0; i<N; i++){
			A[i] = (double *) malloc(N*sizeof(double));
			if (A[i]==NULL){//Flo il m'a dit qu'il faut désalouer toutes 
			//les alloc jusqu'à  présent avant de terminer ce programme
				printf("Allowance error.\n");
				MPI_Finalize();
				exit(0);
			}
		}
		printf("Matrix A:\n");
		for (i=0; i<N; i++){
			for(j=0;j<N;j++){
				A[i][j] = 0.0;
				printf("[%.2f]", A[i][j]);
			}
			printf("\n");
		}
		/* We do not need to use B as we have to multiply a matrix by itself */
		B = (double **) malloc(N*sizeof(double *));
		if (B==NULL){
			printf("Allowance error.\n");
			MPI_Finalize();
			exit(0);
		}
		for (i=0; i<N; i++){
			B[i] = (double *) malloc(N*sizeof(double));
			if (B[i]==NULL){
				printf("Allowance error.\n");
				MPI_Finalize();
				exit(0);
			}
		}
		printf("Matrix B:\n");
		for (i=0; i<N; i++){
			for(j=0;j<N;j++){
				B[i][j] = 0.0;
				printf("[%.2f]", B[i][j]);
			}
			printf("\n");
		}
		/* alocate memory for local sub-matrices */
		for(i=0; i<ntasks; i++){
			//alocate place for matrix rows at place i of the table
			sMTab[i].sM = (double **) malloc(sN*sizeof(double *));
			if (sMTab[i].sM==NULL){
				printf("Allowance error.\n");
				MPI_Finalize();
				exit(0);
			}
			for (j=0; j<sN; j++){
				//at place i, alocate place for matrix cols
				sMTab[i].sM[j] = (double *) malloc(sN*sizeof(double));
				if (sMTab[i].sM[j]==NULL){
					printf("Allowance error.\n");
					MPI_Finalize();
					exit(0);
				}
			}
			printf("Matrix sM%i:\n", i);
			for (j=0; j<sN; j++){
				for(k=0;k<sN;k++){
					sMTab[i].sM[j][k] = 0.0;
					printf("[%.2f]", sMTab[i].sM[j][k]);
				}
				printf("\n");
			}
		}
		
		/* Open the binary file, read the input matrices */
		/*p=fopen(fileName,"rb");	
		if (!p)
		{
			printf("Unable to open file!");
			MPI_Finalize();
			exit(0);
		}
		fread(buff,sizeof(buff),1,p);
		for(i=0;i<N;i++){
			printf("%g ",buff[i]);
		}*/
		
		for (i=0; i<N; i++){
			free(A[i]);
		}
		free(A);
		for (i=0; i<N; i++){
			free(B[i]);
		}
		free(B);
		for(i=0; i<ntasks; i++){
			for (j=0; j<sN; j++){
				free(sMTab[i].sM[j]);
			}
			free(sMTab[i].sM);
		}
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
