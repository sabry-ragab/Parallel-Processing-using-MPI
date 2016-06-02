/*
 ============================================================================
 Author      : Sabry Ragab Darwish
 Approach    : MPI_Bcast() and MPI_Reduce()		   
 Description : This program calculate factorial of a number.
		first initialize number 'n' and calculate block size in the root process,
		send 'block_size' to each process to calculate its interval by MPI_Bcast,
		each process should calculate sub factorial, multiply/reduce all sub factorial
		using MPI_Reduce, in root handle remainder and print result.
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"
int main(int argc , char * argv[])
{
	int block_size, n;
	long long sub_factorial = 1LL, factorial = 1LL;

	MPI_Init(&argc, &argv); // Start up MPI
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Find out process rank
	int nProcess;
	MPI_Comm_size(MPI_COMM_WORLD, &nProcess); // Find out number of processes
	
	if(rank == 0){ //Root process
		
		printf("Enter number: ");
		scanf("%d", &n);// n should be non negative 
		block_size = n / nProcess ; //master included
	}
	
	//Send block size to each process via broadcast method
	int root = 0;
	MPI_Bcast(&block_size, 1, MPI_INT, root, MPI_COMM_WORLD);
	
	//Calculate start and end range of each process 
	int start = (block_size * rank) + 1;
	int end = block_size * (rank + 1);
	
	//Calculate sub factorial of each process 
	sub_factorial = 1LL;
	int i;
	for( i = start; i <= end; i++){
		sub_factorial *= i;
	}
	
	//Reduce all sub_factorials to actual factorial result
	MPI_Reduce(&sub_factorial, &factorial, 1, MPI_LONG_LONG, MPI_PROD,
		root, MPI_COMM_WORLD);
		
	if(rank == 0){
		//Handle remainder
		int current_number = (nProcess * block_size) + 1;
		while( current_number <= n)
			factorial *= current_number++; //It seems to be easy :D 
		
		printf("!%d = %lld\n", n, factorial);
	}
	
	MPI_Finalize(); // Shut down MPI 
    return 0;
}
