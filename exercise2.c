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
	int   id, ntasks, source_id, i, N;//N is the size of the Matrix
	double square;
	MPI_Status status;
	char msg[80], fileName[20];	      	       /* Message array */
	
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
		printf("Enter file name:\n");
		fflush(stdout);
		scanf("%s", fileName);
		int nameSize = 0;
		while(fileName[nameSize]!='\0'){
			nameSize++;
		}
		if(fileName[0]!='M'){
			MPI_Finalize();
			printf("Error in file name, it has to start with char M!\n");
			exit(0);
		}
		if(fileName[nameSize-4]!='.'){
			MPI_Finalize();
			printf("Error in file name, no extension!\n");
			exit(0);
		}
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
		char tabN[nameSize-4];
		int j=0;
		for(i=0;i<nameSize;i++){
			if(fileName[i]=='M'){				
			}
			else if(fileName[i]=='.'){
					break;
			}
			else{
				tabN[j]=fileName[i];
				j++;
			}
		}
		sscanf(tabN,"%d",&N);
		printf("The matrix size is: %dx%d.\n", N, N);
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