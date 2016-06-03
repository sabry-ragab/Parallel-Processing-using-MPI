/*
 ==============================================================================
 Author      : Sabry Ragab Darwish
 Approach    : MPI_Send, MPI_Recv		   
 Description : This program gets the prime numbers between two numbers [lower_bound, upper_bound]. 
		Initialize lower and upper bound of range then calculate and send 
		start and end of each process/slave, slaves check prime numbers in the range 
		then put prime numbers in the 'sub_prime_numbers' and send it to the master.
		finally master receive prime numbers from each slave then check remaining 
		numbers and print all prime numbers of the actual range.
 ==============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"

int is_prime( int n ){
	if( n < 2)
		return 0; //not prime
	int i;
	for( i = 2 ; i * i <= n; i++){
		if( n % i == 0)
			return 0; //not prime
	}
	return 1;//is prime
}

int main(int argc , char * argv[])
{
	const int MAX_SIZE = 1000000;
	int sub_prime_numbers[MAX_SIZE]; // List of prime numbers sent by slaves
	int all_prime_numbers[MAX_SIZE]; // List of all prime numbers
	int lower_bound, upper_bound, block, sub_prime_count, tag = 0;
	int start, end; //range of each process
	MPI_Status status;
	
	MPI_Init(&argc, &argv); // Start up MPI
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Find out process rank
	int nProcess;
	MPI_Comm_size(MPI_COMM_WORLD, &nProcess); // Find out number of processes
	
	if(rank == 0){ //Master process
		printf("Enter lower bound:");
		scanf("%d", &lower_bound);
		printf("Enter upper bound:");
		scanf("%d", &upper_bound);
		block =  ( upper_bound - lower_bound + 1 ) / (nProcess-1);
		
		printf("%d\n", block);
		
		//Calculate and send start and end of range of each slave
		int dest;
		for( dest = 1; dest < nProcess; dest++ ){
			start = lower_bound + ( (dest-1) * block );
			end = lower_bound + ( dest * block );
			MPI_Send(&start, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
			MPI_Send(&end, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);			
		}
		
		int prime_index = 0; //index of prime number in all_prime_numbers array
		
		//receive prime numbers from slaves
		int source;
		for( source = 1; source < nProcess; source++ ){
			
			MPI_Recv(&sub_prime_count, 1, MPI_INT, source, tag, 
				MPI_COMM_WORLD, &status); //get count first
			MPI_Recv(sub_prime_numbers, sub_prime_count, MPI_INT,
				source, tag, MPI_COMM_WORLD, &status);//then get prime numbers
				
			//add 'sub prime number' array to the 'all prime numbers array'
			int i;
			for( i = 0; i < sub_prime_count; i++){
				all_prime_numbers[prime_index] = sub_prime_numbers[i];
				prime_index++;
			}
		}
		
		//handle remaining numbers of the range 
		int current_number = lower_bound + ( (nProcess-1) * block );
		while( current_number <= upper_bound){
			if( is_prime( current_number ) ){
				all_prime_numbers[prime_index] = current_number;
				prime_index++;
			}
			current_number++;
		}
		
		//print all prime numbers
		printf("Number of prime numbers = %d\n", prime_index);
		printf("Prime numbers: ");
		int i;
		for( i = 0; i < prime_index; i++){
			printf("%d  ", all_prime_numbers[i]);
		}
		printf("\n");
		
	}else{ //Slaves part
		
		//receive start and end of range
		int source = 0;
		MPI_Recv(&start, 1, MPI_INT, source, tag, 
				MPI_COMM_WORLD, &status); //get count first
		MPI_Recv(&end, 1, MPI_INT, source, tag, 
				MPI_COMM_WORLD, &status); //get count first
				
		//Count prime numbers between start and end
		sub_prime_count = 0;
		int i;
		for( i = start; i < end; i++){
			if( is_prime( i ) ){
				sub_prime_numbers[sub_prime_count] = i;//add prime of the array
				sub_prime_count++;
			}
		}
		
		//send count and prime numbers array to master
		int dest = 0;
		MPI_Send(&sub_prime_count, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
		MPI_Send(sub_prime_numbers, sub_prime_count, MPI_INT, dest, tag, MPI_COMM_WORLD);	
		
	}
	
	MPI_Finalize(); // Shut down MPI 
    return 0;
}
