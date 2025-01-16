#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

void merge(int *, int *, int, int, int);
void mergeSort(int *, int *, int, int);

int main(int argc, char** argv) {
	
	// Create and populate the array 
	int n = atoi(argv[1]);
	int *original_array = malloc(n * sizeof(int));
        int c;	
	
	printf("\n");
	
	//Initialize MPI 
	int myid;
	int nump;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Comm_size(MPI_COMM_WORLD, &nump);
	
	if(myid==0){
	   printf("Enter %d elements of the array:\n", n);
           
           for (int i = 0; i < n; i++) {
            scanf("%d", &original_array[i]);
           }
	
	}
	printf("\n");
	// Start timing
         double start_time = MPI_Wtime(); 	
	
	// Divide the array in equal-sized chunks 
	  int size = n / nump;
         	
	//Send each subarray to each process 
	int *sub_array = malloc(size * sizeof(int));	
	MPI_Scatter(original_array, size, MPI_INT, sub_array, size, MPI_INT, 0, MPI_COMM_WORLD);

	//Perform the mergesort on each process 
	int *tmp_array = malloc(size * sizeof(int));
	mergeSort(sub_array, tmp_array, 0, (size - 1));
	
	//Print each process its sorted array
	printf("This is process %d , my sorted array: ",myid);
	for(c=0;c<size;c++){
	   printf(" %d ",sub_array[c]);
	}
	printf("\n");
	
	// Gather the sorted subarrays into one 
	int *sorted = NULL;
	if(myid == 0) {
		sorted = malloc(n * sizeof(int));
	}	
	
	MPI_Gather(sub_array, size, MPI_INT, sorted, size, MPI_INT, 0, MPI_COMM_WORLD);
	
	//Make the final mergeSort call 
	if(myid == 0) {	
		int *other_array = malloc(n * sizeof(int));
		if(nump==2){
			merge(sorted,other_array,0,(n/2)-1,(n-1));
		}
		else mergeSort(sorted, other_array, 0, (n - 1));
		
		//Display the sorted array 
		printf("\nThis is the sorted array from the master process:   ");
		for(c = 0; c < n; c++) {
			printf("%d ", sorted[c]);		
		}
			
		printf("\n");
		printf("\n");
		
               // End timing
                double end_time = MPI_Wtime();
                double time_taken_ms = (end_time - start_time) * 1000000; // Convert seconds to milliseconds
                printf("\nTime taken for the computation: %f microseconds\n", time_taken_ms);
                printf("\n");
                
		//Clean up root 
		free(sorted);
		free(other_array);
			
	  }
	
	//Clean up rest 
	free(original_array);
	free(sub_array);
	free(tmp_array);
	
	// Finalize MPI 
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	
	}

//Merge Function 
void merge(int *arr, int *temp, int low, int mid, int high) {
	
	int i, j,k;
	k= low;
	i = low;
	j = mid + 1;
	
	while((i <= mid) && (j <= high)) {

		if(arr[i] <= arr[j]) {			
			temp[k++] = arr[i++];
	         }
			
		else temp[k++] = arr[j++];
			
		
		
        }
	while(i<=mid){
	
		temp[k++]=arr[i++];
		
	}
	while (j <= high){
	
		temp[k++] = arr[j++];
	}
		
	for(int z=low;z<=high;z++){
	 arr[z]=temp[z];
	}
		
}

// Merge Sort Function 
void mergeSort(int *arr, int *temp, int low, int high) {
	
	int mid;
	
	if(low < high) {
		
		mid = (high+ low)/2;
		
		mergeSort(arr, temp, low,mid);
		mergeSort(arr, temp, (mid + 1), high);
		merge(arr, temp, low, mid, high);
		
	}
		
} 
