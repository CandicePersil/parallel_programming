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
	int   id, ntasks, source_id, i;
	double square;
	MPI_Status status;
	char msg[80];	      	       /* Message array */
	
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
	
	/* All processes do this */
	if ( MPI_Finalize() != MPI_SUCCESS) {
		printf("Error in MPI_Finalize!\n");
		exit(1);
	}
	
	if (id==0){
		printf("Ready\n");
	}
	exit(0);
}