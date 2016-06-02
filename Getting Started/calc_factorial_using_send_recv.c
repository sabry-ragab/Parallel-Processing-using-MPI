/*
 ============================================================================
 Author      : Sabry Ragab Darwish
 Approach    : MPI_Send() and MPI_Recv()		   
 Description : This program calculate factorial of a number.
		first initialize number 'n' and calculate block size in the master process,
		send 'block_size' to each process to calculate its interval,
		each process should calculate sub factorial and send it to the master, 
		master receive and multiply all sub_factorial, handle remainder and print result.
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
	int tag = 0; // Tag of message
	MPI_Status status; // To be used in MPI_Recv
	
	MPI_Init(&argc, &argv); // Start up MPI
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Find out process rank
	int nProcess;
	MPI_Comm_size(MPI_COMM_WORLD, &nProcess); // Find out number of processes
	
	if(rank == 0){ //Master process
		
		printf("Enter number: ");
		scanf("%d", &n);// n should be non negative 
		block_size = n / (nProcess - 1); //Master process is excluded
		
		//Send block to slaves
		int dest;
		for(dest = 1; dest < nProcess; dest++){
			MPI_Send(&block_size, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);			
		}
		
		//Receive sub factorial and multiply all of them to get factorial
		factorial = 1LL;
		int source;
		for( source = 1; source < nProcess; source++){
			MPI_Recv(&sub_factorial, 1, MPI_LONG_LONG, source, tag, 
			MPI_COMM_WORLD, &status);
			factorial *= sub_factorial;
		}
		
		//Handle remainder
		int current_number = ( (nProcess-1) * block_size ) + 1;
		while( current_number <= n)
			factorial *= current_number++; //It seems to be easy :D 
		
		printf("!%d = %lld\n", n, factorial);
	}else{ //Slaves part
		
		//Receive 'block_size' in each process to calculate its interval
		int source = 0;
		MPI_Recv(&block_size, 1, MPI_INT, source, tag, 
			MPI_COMM_WORLD, &status);
			
		//Calculate start and end range of each process 
		int start = (block_size * (rank-1)) + 1;
		int end = block_size * rank;
		
		//Calculate sub factorial of each process 
		sub_factorial = 1LL;
		int i;
		for( i = start; i <= end; i++){
			sub_factorial *= i;
		}
		
		//Send sub factorial to master
		int dest = 0;
		MPI_Send(&sub_factorial, 1, MPI_LONG_LONG, dest, tag, MPI_COMM_WORLD);	
	}
	
	MPI_Finalize(); // Shut down MPI 
    return 0;
}
