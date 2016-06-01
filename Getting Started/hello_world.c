/*
 ============================================================================
 Author      : Sabry Ragab Darwish
 Description : Hello world program
 ============================================================================
 */
#include<stdio.h>
#include <stdlib.h>
#include "mpi.h"
int main(int argc , char * argv[])
{
	MPI_Init( &argc , &argv ); //  Start up MPI
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Find out process rank
	printf("Hello world from process %d\n", rank);
	MPI_Finalize(); // Shut down MPI 
    return 0;
}
