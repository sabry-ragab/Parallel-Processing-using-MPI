/*
 ============================================================================
 Author      : Sabry Ragab Darwish
 Approach    : MPI_Reduce, MPI_Bcast		   
 Description : This program will examine collective communication. The master
		process first initialize the scalar and then broadcast scalar.
		Every process will add scalar plus its rank and then reduce it 
		to master process(root).
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"
int main(int argc , char * argv[])
{

	MPI_Init(&argc, &argv); // Start up MPI
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Find out process rank
	int nProcess;
	MPI_Comm_size(MPI_COMM_WORLD, &nProcess); // Find out number of processes
	
	int scalar, result;
	if(rank == 0){
		printf("Enter scalar:");
		scanf("%d", &scalar);
	}
	
	int root = 0;
	MPI_Bcast(&scalar, 1, MPI_INT, root, MPI_COMM_WORLD);
	
	scalar += rank;
	
	MPI_Reduce(&scalar, &result, 1, MPI_INT, MPI_SUM, root, MPI_COMM_WORLD);
	
	if(rank == 0){
		printf("Result = %d\n", result);
	}
	
	MPI_Finalize(); // Shut down MPI 
    return 0;
}
