/*
 ==============================================================================
 Author      : Sabry Ragab Darwish
 Approach    : MPI_Reduce, MPI_Bcast		   
 Description : This program gets the Count of prime numbers between
				[ upper_bound and lower_bound ]. Calculate the sub range size
				block = (upper_bound - lower_bound + 1) / n_process) 
				Broadcast 'lower_bound' and 'block' to each process using MPI_Bcast 
				Receive 'sub count' from each process using MPI_Reduce.
				handle remaining numbers using only master process.
 ==============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"

int is_prime( int n ){
	if( n < 2)
		return 0; //not prime
	int i = 2;
	for( ; i * i <= n; i++){
		if( n % i == 0)
			return 0; //not prime
	}
	return 1;//is prime
}

int main(int argc , char * argv[])
{
	int lower_bound, upper_bound, total_count = 0, sub_count = 0, block;
	
	MPI_Init(&argc, &argv); // Start up MPI
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Find out process rank
	int nProcess;
	MPI_Comm_size(MPI_COMM_WORLD, &nProcess); // Find out number of processes
	
	if(rank == 0){
		printf("Enter lower bound:");
		scanf("%d", &lower_bound);
		printf("Enter upper bound:");
		scanf("%d", &upper_bound);
		block =  ( upper_bound - lower_bound + 1 ) / nProcess;
		
	}
	
	//Broad cast lower bound and block size
	int root = 0;
	MPI_Bcast(&lower_bound, 1, MPI_INT, root, MPI_COMM_WORLD);
	MPI_Bcast(&block, 1, MPI_INT, root, MPI_COMM_WORLD);
	
	//Calculate start and end of each process
	int start = lower_bound + rank * block;
	int end = lower_bound + (rank + 1) * block;	
	
	//Count prime numbers between start and end
	sub_count = 0;
	int i;
	for( i = start; i < end; i++){
		sub_count += is_prime( i );
	}
	
	//add sub count to total count
	MPI_Reduce(&sub_count, &total_count, 1, MPI_INT, MPI_SUM, root, MPI_COMM_WORLD);
	
	if(rank == 0){
		//handle remaining numbers 
		int current_number = lower_bound + (nProcess * block);
		while( current_number <= upper_bound){
			total_count += is_prime( current_number );
			current_number++;
		}
		
		printf("There is %d prime number between %d and %d.\n", total_count, lower_bound, upper_bound);
	}
	
	MPI_Finalize(); // Shut down MPI 
    return 0;
}
