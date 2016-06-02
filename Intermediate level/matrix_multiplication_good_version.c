/*
 ============================================================================
 Author      : Sabry Ragab Darwish
 Approach    : MPI_Bcast(), MPI_Reduce() and MPI_Gather() 
 Description : This program calculate matrix multiplication.
		first initialize two matrices in root process and calculate number of 
		rows that will sent to each process, send second matrix and its dimensions 
		to all processes and send the number of rows also using 'MPI_Bcast'. 
		Scatter first matrix among all processes using 'MPI_Scatter'. Each process will 
		multiply the given rows with second matrix. Gather all sub multiplication
		in the result matrix using 'MPI_Gather' and handle remaining rows in root process.
		Display result of multiplication.
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"

int get_index(int r, int c, int num_columns){
	return r * num_columns + c;
} 

int main(int argc , char * argv[])
{
	const int MAX_SIZE = 100000;
	int first_matrix[MAX_SIZE], second_matrix[MAX_SIZE];
	int result_matrix[MAX_SIZE], sub_matrix[MAX_SIZE], temp_matrix[MAX_SIZE];
	int num_rows_first, num_columns_first;
	int num_rows_second, num_columns_second;
	int num_rows_per_process;
	
	MPI_Init(&argc, &argv); // Start up MPI
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Find out process rank
	int nProcess;
	MPI_Comm_size(MPI_COMM_WORLD, &nProcess); // Find out number of processes
	
	if(rank == 0){ //Root process

		while(1){
			printf("Please enter the first matrix dimensions elements:\n");
			scanf("%d%d", &num_rows_first, &num_columns_first);
			printf("Please enter the second matrix dimensions elements:\n");
			scanf("%d%d", &num_rows_second, &num_columns_second);
			if(num_rows_second != num_columns_first){
				printf("The number of columns of the 1st matrix ");
				printf("must equal the number of rows of the 2nd matrix.\n");
				continue;
			}else{
				break;
			}
		}
		
		int i;
		printf("Please enter the first matrix elements:\n");
		for(i = 0; i < num_columns_first * num_rows_first; i++){
			scanf( "%d", &first_matrix[i]);
		}
		printf("Please enter the second matrix elements:\n");
		for(i = 0; i < num_columns_second * num_rows_second; i++){
			scanf("%d", &second_matrix[i]);
		}
		
		num_rows_per_process = num_rows_first / nProcess; // master included
	}
	
	//Send matrix 2 and its dimensions to all processes
	int root = 0;
	MPI_Bcast(&num_columns_second, 1, MPI_INT, root, MPI_COMM_WORLD);
	MPI_Bcast(&num_rows_second, 1, MPI_INT, root, MPI_COMM_WORLD);
	MPI_Bcast(second_matrix, num_columns_second * num_rows_second, MPI_INT,
		root, MPI_COMM_WORLD);
	num_columns_first = num_rows_second; // It is a must in matrix multiplication
	MPI_Bcast(&num_rows_per_process, 1, MPI_INT, root, MPI_COMM_WORLD);
	
	//scatter first matrix to all processes
	int num_cells_per_process = num_rows_per_process * num_columns_first;
	MPI_Scatter(first_matrix, num_cells_per_process, MPI_INT, sub_matrix, 
		num_cells_per_process, MPI_INT, root, MPI_COMM_WORLD);
	
	//Multiply given sub matrix with second matrix for each process
	int i, j, k;
	for(i = 0; i < num_rows_per_process; i++){
		for(j = 0; j < num_columns_second; j++){
			temp_matrix[get_index(i, j, num_columns_second)] = 0;
			for(k = 0; k < num_rows_second; k++){
				int temp_index = get_index(i, j, num_columns_second);
				int sub_index = get_index(i, k, num_columns_first);
				int second_index = get_index(k, j, num_columns_second);
				temp_matrix[temp_index] += sub_matrix[sub_index] * second_matrix[second_index];
			}
		}
	}
	
	//Gather sub results  in the result matrix
	num_cells_per_process = num_rows_per_process * num_columns_second;
	MPI_Gather(temp_matrix, num_cells_per_process, MPI_INT, result_matrix, num_cells_per_process,
		MPI_INT, root, MPI_COMM_WORLD);
	
	if( rank == 0){
		
		//Handle remaining rows of first matrix
		int start_row = num_rows_per_process * nProcess;
		for(i = start_row; i < num_rows_first; i++){
			for(j = 0; j < num_columns_second; j++){
				result_matrix[get_index(i, j, num_columns_second)] = 0;
				for(k = 0; k < num_rows_second; k++){
					int result_index = get_index(i, j, num_columns_second);
					int first_index = get_index(i, k, num_columns_first);
					int second_index = get_index(k, j, num_columns_second);
					result_matrix[result_index] += first_matrix[first_index] * second_matrix[second_index];
				}
			}
		}
		
		//Print result
		printf("Multiplication Result:\n");
		for( i = 0; i < num_columns_second*num_rows_first; i++){
			if(i % num_columns_second == 0)
				printf("\n");
			printf("%d  ", result_matrix[i]);
		} 
		printf("\n");
	}
	MPI_Finalize(); // Shut down MPI 
    return 0;
}
