/*
 ============================================================================
 Author      : Sabry Ragab Darwish
 Approach    : MPI_Send() and MPI_Recv()
 Description : Simple program that Send message from master process then
			   slaves receive and print it.
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"
int main(int argc , char * argv[])
{
	char message[10000]; // Storage of message
	int message_length;
	int tag = 0; // Tag of message

	MPI_Init(&argc, &argv); // Start up MPI
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Find out process rank
	int nProcess;
	MPI_Comm_size(MPI_COMM_WORLD, &nProcess); // Find out number of processes
	
	if(rank == 0){ //master process
		sprintf(message, "Greeting from process %d\n", rank);
	    message_length = strlen(message)+1; 
		int dest;
		for(dest = 1; dest < nProcess; dest++){
			MPI_Send(&message_length, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
			MPI_Send(&message, message_length, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
		}
	}else{ // slaves processes
		MPI_Status status; // Status to be used in receive function
		int source = 0; // master rank
		MPI_Recv(&message_length, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &status); 
		MPI_Recv(&message, message_length, MPI_CHAR, source, tag, MPI_COMM_WORLD, &status);
		printf("process %d has received : %s", rank, message);
	}
	MPI_Finalize(); // Shut down MPI 
    return 0;
}
