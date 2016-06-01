/*
 ============================================================================
 Author      : Sabry Ragab Darwish
 Approach    : MPI_Send() and MPI_Recv()		   
 Description : This program demonstrates simple data decomposition. The master
		process first initialize the array and then distributes and sends 
		an equal portion of the array to the other processes (Slaves).
		After the slaves receive and accumulate their portion of 
		the array , they send the portion sum to the master.
		The master process handle remaining cells of the array and 
		receive the sub sums from slaves and accumulate them and 
		display sum of the whole array.
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"
int main(int argc , char * argv[])
{
	const int MAX = 100000;
	float arr[MAX], sub_sum; 
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
		
		//Divide array into blocks to be summed in the slaves
		block_size = arr_size / (nProcess - 1); // -1 as master not included
		int dest, index = 0;
		for(dest = 1; dest < nProcess; dest++){
			MPI_Send(&block_size, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
			 // (arr + index) is address of block
			MPI_Send(arr + index, block_size, MPI_FLOAT, dest, tag, MPI_COMM_WORLD);
			index += block_size;// Go to next block address
		}
		
		//Handle remainder -> accumulated remaining numbers in the array
		float total_sum = 0;
		while(index < arr_size){
			total_sum += arr[index++];
		}
		
		//receive sub sums from slaves
		MPI_Status status; // Status to be used in receive function
		int source;
		for(source = 1; source < nProcess; source++){
			MPI_Recv(&sub_sum, 1, MPI_FLOAT, source, tag, MPI_COMM_WORLD, &status);
			total_sum += sub_sum;
		}
		
		printf("Total Sum = %f\n", total_sum);
		
	}else{ // Slaves processes
		
		MPI_Status status; // Status to be used in receive function
		int source = 0; // Master rank
		MPI_Recv(&block_size, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &status); 
		MPI_Recv(arr, block_size, MPI_FLOAT, source, tag, MPI_COMM_WORLD, &status);
		
		//Accumulate block numbers
		sub_sum = 0;
		int i;
		for(i = 0; i < block_size; i++){
			sub_sum += arr[i];
		}
		
		//Send sub sum to master
		int dest = 0;
		MPI_Send(&sub_sum, 1, MPI_FLOAT, dest, tag, MPI_COMM_WORLD);
	}
	
	MPI_Finalize(); // Shut down MPI 
    return 0;
}
