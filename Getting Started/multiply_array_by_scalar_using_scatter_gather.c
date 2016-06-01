/*
 ==================================================================================
 Author      : Sabry Ragab Darwish
 Approach    : MPI_Scatter, MPI_Bcast, MPI_Reduce		   
 Description : In this example we fill an array with 1 2 3 . . . N and 
		then scatter this array to all processes, and broadcast block size and scalar
		then multiply in each process received block cells by scalar , then gather 
		sub_arr after multiplication.
 =================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"
int main(int argc , char * argv[])
{
	const int MAX = 100000;
	int initial_data[MAX], sub_data[MAX], final_data[MAX];
	int size, block, scalar;
	
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
			initial_data[i] = i + 1;
		printf("Enter scalar:");
		scanf("%d", &scalar);
	}
	
	//Broad cast block size to reserve this block in received array (recv_data)
	int root = 0;
	MPI_Bcast(&block, 1, MPI_INT, root, MPI_COMM_WORLD);
	MPI_Bcast(&scalar, 1, MPI_INT, root, MPI_COMM_WORLD);
	
	//Scatter initial array 
	MPI_Scatter(initial_data, block, MPI_INT, sub_data, block, MPI_INT, root, MPI_COMM_WORLD);
	
	// multiply scalar by  portion of array that is received by each process
	int i;
	for( i = 0; i < block; i++){
		sub_data[i] *= scalar;
	}

	//gather sub data in final array
	MPI_Gather(sub_data, block, MPI_INT, final_data, block, MPI_INT, root, MPI_COMM_WORLD);
	
	if(rank == 0){
		//handle remaining numbers 
		int index = (nProcess * block);                         
		while( index < size){
			final_data[index] = initial_data[index] * scalar;
			index++;
		}
		
		//display final array 
		printf("Result Array :\n");
		int i;
		for( i = 0; i < size; i++)
			printf("%d  ", final_data[i]);
		printf("\n");
	}
	
	MPI_Finalize(); // Shut down MPI 
    return 0;
}