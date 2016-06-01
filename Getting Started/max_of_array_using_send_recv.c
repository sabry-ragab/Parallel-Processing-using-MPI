/*
 ============================================================================
 Author      : Sabry Ragab Darwish
 Approach    : MPI_Send() and MPI_Recv()		   
 Description : This program use Master-Slave paradigm. The master
		process first initialize the array and then distributes and sends 
		an equal portion of the array to the other processes (Slaves).
		After the slaves receive and find maximum in their portion of 
		the array , they send the maximum of the block to the master.
		The master process handle remaining cells of the array and 
		receive the sub maximum from slaves and check maximum between them and 
		display maximum number of the whole array.
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"
int main(int argc , char * argv[])
{
	const int MAX = 100000;
	float arr[MAX], sub_max; 
	int block_size;
	int tag = 0; // Tag of message

	MPI_Init(&argc, &argv); // Start up MPI
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Find out process rank
	int nProcess;
	MPI_Comm_size(MPI_COMM_WORLD, &nProcess); // Find out number of processes
	
	if(rank == 0){ //master process
		
		printf("Enter array size: ");
		int arr_size;
		scanf("%d", &arr_size);
		printf("Enter array numbers:\n");
		int i; 
		for(i = 0; i < arr_size; i++){
			scanf("%f", &arr[i]);
		}
		
		//Divide array into blocks to be checked in the slaves
		block_size = arr_size / (nProcess - 1); // -1 as master not included
		int dest, index = 0;
		for(dest = 1; dest < nProcess; dest++){
			MPI_Send(&block_size, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
			 // (arr + index) is address of block
			MPI_Send(arr + index, block_size, MPI_FLOAT, dest, tag, MPI_COMM_WORLD);
			index += block_size;// Go to next block address
		}
		
		//Handle remainder -> check remaining numbers in the array
		float final_max = -1000000000;//Small value
		while(index < arr_size){
			if(final_max < arr[index])
				final_max = arr[index];
			index++;
		}
		
		//receive sub sums from slaves
		MPI_Status status; // Status to be used in receive function
		int source;
		for(source = 1; source < nProcess; source++){
			MPI_Recv(&sub_max, 1, MPI_FLOAT, source, tag, MPI_COMM_WORLD, &status);
			if(final_max < sub_max)
				final_max = sub_max;
		}
		
		printf("Max number of array = %f\n", final_max);
		
	}else{ // Slaves processes
		
		MPI_Status status; // Status to be used in receive function
		int source = 0; // Master rank
		MPI_Recv(&block_size, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &status); 
		MPI_Recv(arr, block_size, MPI_FLOAT, source, tag, MPI_COMM_WORLD, &status);
		
		//check maximum in block
		sub_max = arr[0];
		int i;
		for(i = 1; i < block_size; i++){
			if(sub_max < arr[i])
				sub_max = arr[i];
		}
		
		//Send sub sub maximum to master
		int dest = 0;
		MPI_Send(&sub_max, 1, MPI_FLOAT, dest, tag, MPI_COMM_WORLD);
	}
	
	MPI_Finalize(); // Shut down MPI 
    return 0;
}
