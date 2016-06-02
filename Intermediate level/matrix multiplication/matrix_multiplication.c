/*
 ============================================================================
 Author      : Sabry Ragab Darweesh
 ID          : 20120214
 Description : CS471: Parallel Processing - Assignment-4
 Approach    : MPI_Bcast , MPI_Scatter and MPI_Gather
 ============================================================================
 */
#include <stdio.h>
#include "mpi.h"
#include <string.h>

int getIndex(int r, int c, int nCol){
	return r * nCol + c;
}

int main(int argc, char** argv) {

	int i, j, k, rank, nprocess, source, dest, tag = 0;
	MPI_Status status;
	int reading_isOk;
	const int MAX = 100000;
	int row1, row2, col1, col2, num_rows_per_process, num_cells;
	int matrix1[MAX], matrix2[MAX], sub[MAX], ans[MAX], sub_ans[MAX];
	
	MPI_Init( &argc , &argv );
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocess);
		
	/* Reading input in master process */
	if(rank == 0){
		reading_isOk = 1;
		printf("Reading input process, press 1 (File) or any number for (console) :");
		int option;
		scanf("%d", &option);
		if(option == 1){
			/* read from file */
			char path[50];
			printf("Enter file path : ");
			scanf("%s", path);
			FILE* fr;
			fr = fopen("input.txt", "r");
			if (fr == NULL) {
				printf("Can't open input file!\n");
				reading_isOk = 0;
			}else{
				fscanf(fr, "%d%d%d%d", &row1, &col1, &row2, &col2);
				if(row2 != col1){
					printf("The number of columns of the 1st matrix must equal the number of rows of the 2nd matrix.\n");
					reading_isOk = 0;
				}else{
					for(i = 0; i < row1 * col1; i++){
						fscanf(fr, "%d", &matrix1[i]);
					}
					for(i = 0; i < row2 * col2; i++){
						fscanf(fr, "%d", &matrix2[i]);
					}
				}
			}
		}else{
			/* read from console */
			printf("Please enter the first matrix dimensions elements:\n");
			scanf("%d%d", &row1, &col1);
			printf("Please enter the second matrix dimensions elements:\n");
			scanf("%d%d", &row2, &col2);
			if(row2 != col1){
				printf("The number of columns of the 1st matrix must equal the number of rows of the 2nd matrix.\n");
				reading_isOk = 0;
			}else{
				printf("Please enter the first matrix elements:\n");
				for(i = 0; i < row1 * col1; i++){
					scanf( "%d", &matrix1[i]);
				}
				printf("Please enter the second matrix elements:\n");
				for(i = 0; i < row2 * col2; i++){
					scanf("%d", &matrix2[i]);
				}
			}
		}
			
	}
	
	source = 0;
	MPI_Bcast(&reading_isOk, 1, MPI_INT, source, MPI_COMM_WORLD);
	if(reading_isOk){
		
		/* part 1 -> broadcast matrix 2 to all processes */
		MPI_Bcast(&row2,1 ,MPI_INT, source, MPI_COMM_WORLD);
		MPI_Bcast(&col2,1 ,MPI_INT, source, MPI_COMM_WORLD);
		MPI_Bcast(matrix2, row2 * col2 ,MPI_INT, source, MPI_COMM_WORLD);
		
		/* part 2 -> scatter matrix1 */
		MPI_Bcast(&row1, 1, MPI_INT, source, MPI_COMM_WORLD);
		col1 = row2; 
		num_rows_per_process = row1 / nprocess;
		num_cells = num_rows_per_process * col1;
		MPI_Scatter(matrix1, num_cells, MPI_INT, sub, num_cells, MPI_INT, source, MPI_COMM_WORLD); //Scatter
		
		/* part 3 ->  matrix multiplication*/
		for(i = 0; i < num_rows_per_process; i++){
			for(j = 0; j < col2; j++){
				int index = getIndex(i, j, col2);
				sub_ans[index] = 0;
				for(k = 0; k < row2; k++){
					sub_ans[index] += sub[getIndex(i, k, col1)] * matrix2[getIndex(k, j, col2)];
				}
			}
		}
		
		/* part 4 -> gather multiplication */
		num_cells = num_rows_per_process * col2;
		MPI_Gather(sub_ans, num_cells, MPI_INT, ans, num_cells, MPI_INT, source, MPI_COMM_WORLD); //Gather
		
		/* part 5 -> Handle Reminder*/
		if(rank == 0){
			int rem_rows = row1 % nprocess;
			int start = nprocess * col1 * num_rows_per_process;
			int end = row1 * col1;
			for(i = 0, j = start; j < end; j++, i++){
				sub[i] = matrix1[j];
			}
			//multiply remaining rows
			for(i = 0; i < rem_rows; i++){
				for(j = 0; j < col2; j++){
					int index = getIndex(i, j, col2);
					sub_ans[index] = 0;
					for(k = 0; k < row2; k++){
						sub_ans[index] += sub[getIndex(i, k, col1)] * matrix2[getIndex(k, j, col2)];
					}
				}
			}
			//add it to final answer
			start = nprocess * col2 * num_rows_per_process;
			end = row1 * col2;
			for(i = 0, j = start; j < end; j++, i++){
				ans[j] = sub_ans[i];
			}
		}
		
		
		/* part 6 -> print Result */
		if(rank == 0){
			printf("Answer :\n");
			int pos = 0;
			for(i = 0; i < row1; i++){
				for(j = 0; j < col2; j++){
					printf("%d ", ans[pos++]);
				}
				printf("\n");
			}
		}
	
	}
	
	MPI_Finalize();
	return 0;
}
