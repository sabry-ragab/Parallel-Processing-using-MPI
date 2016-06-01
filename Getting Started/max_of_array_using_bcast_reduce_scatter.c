/*
 ==================================================================================
 Author      : Sabry Ragab Darwish
 Approach    : MPI_Scatter, MPI_Bcast, MPI_Reduce		   
 Description : In this example we will get maximum number in a given array,
		broadcast block size, scatter this array to all processes, then get maximum
		in each block, then reduce all sub maximum using MPI_Reduce with MPI_MAX operator.
 =================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"
int main(int argc , char * argv[])
{
	const int MAX_SIZE = 100000;
	int send_data[MAX_SIZE], recv_data[MAX_SIZE];
	int arr_size, block_size;
	int sub_max, final_max;
	
	MPI_Init(&argc, &argv); // Start up MPI
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Find out process rank
	int nProcess;
	MPI_Comm_size(MPI_COMM_WORLD, &nProcess); // Find out number of processes
	
	if(rank == 0){ //Root process
		printf("Enter array size :");
		scanf("%d", &arr_size);
		block_size =  arr_size / nProcess; //Calculate block size
		printf("Enter array numbers:\n");
		int i; 
		for( i = 0; i < arr_size; i++)
			scanf("%d", &send_data[i]);
	}
	
	//Broad cast block size to reserve this block in received array (recv_data)
	int root = 0;
	MPI_Bcast(&block_size, 1, MPI_INT, root, MPI_COMM_WORLD);
	
	//Scatter initial array 
	MPI_Scatter(send_data, block_size, MPI_INT, recv_data, block_size, MPI_INT, root, MPI_COMM_WORLD);
	
	// Check maximum number (sub_max) of the portion of array that is received by each process
	sub_max = recv_data[0]; // first number
	int i;
	for( i = 1; i < block_size; i++){
		if(sub_max < recv_data[i])
			sub_max = recv_data[i];
	}

	//Reduce all sub maximum by using MPI_MAX operator 
	MPI_Reduce(&sub_max, &final_max, 1, MPI_INT, MPI_MAX, root, MPI_COMM_WORLD);
	
	if(rank == 0){
		//Handle remaining numbers 
		int index = (nProcess * block_size);                         
		while( index < arr_size){
			if(final_max < send_data[index])
				final_max = send_data[index];
			index++;
		}
		
		//display maximum number of the whole array 
		printf("Maximum number = %d\n", final_max);		
	}
	
	MPI_Finalize(); // Shut down MPI 
    return 0;
}
