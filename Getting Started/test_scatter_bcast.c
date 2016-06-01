/*
 ==================================================================================
 Author      : Sabry Ragab Darwish
 Approach    : MPI_Scatter, MPI_Bcast		   
 Description : In this example we fill an array with 1 2 3 . . . N and then scatter this array to all
		processes, then test in each process what did it receive.
 =================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"
int main(int argc , char * argv[])
{
	const int MAX = 100000;
	int send_data[MAX], recv_data[MAX];
	int size, block;
	
	MPI_Init(&argc, &argv); // Start up MPI
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Find out process rank
	int nProcess;
	MPI_Comm_size(MPI_COMM_WORLD, &nProcess); // Find out number of processes
	
	if(rank == 0){ //root process
		printf("Enter array size :");
		scanf("%d", &size);
		block =  size / nProcess;
		int i; 
		for( i = 0; i < size; i++)
			send_data[i] = i + 1;
	}
	
	//Broad cast block size to reserve this block in received array (recv_data)
	int root = 0;
	MPI_Bcast(&block, 1, MPI_INT, root, MPI_COMM_WORLD);
	
	//Scatter send array 
	MPI_Scatter(send_data, block, MPI_INT, recv_data, block, MPI_INT, root, MPI_COMM_WORLD);
	
	// display portion of array that is received by each process
	printf("Process %d is received :\n", rank);
	int i;
	for( i = 0; i < block; i++){
		printf("%d  ", recv_data[i]);
	}
	printf("\n");	
	
	if(rank == 0){
		//handle remaining numbers 
		printf("Remaining numbers are :\n");
		int index = (nProcess * block);                         
		while( index < size){
			printf("%d  ", send_data[index]);
			index++;
		}
		printf("\n");	
	}
	
	MPI_Finalize(); // Shut down MPI 
    return 0;
}
