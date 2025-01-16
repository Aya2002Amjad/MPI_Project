#include <iostream>
#include <pthread.h>
#include <vector>
#include <fstream>
#include <iomanip>
#include <chrono>
using namespace std;
using namespace chrono; 

pthread_mutex_t mutex1;  // Used for printing thread status, if needed



struct ThreadArgs {

    int low;
    int high;
    int id;

    vector<int>& myarr;

};



// Merge function that combines two sorted halves

void merge(vector<int>& arr, int low, int mid, int high) {

    vector<int> temp(arr.begin() + low, arr.begin() + high + 1);

    int i = low, j = mid + 1, k = low;


    while (i <= mid && j <= high) {

        if (arr[i] <= arr[j]) {
            temp[k- low] = arr[i++];
        } 
        else  temp[k - low] = arr[j++];

        k++;

     }

    while (i <= mid) {

        temp[k - low] = arr[i++];
        
        k++;

    }

    while (j <= high) {

        temp[k - low] = arr[j++];

        k++;

    }



    for (int z = low; z <= high; z++) {

        arr[z] = temp[z - low];

    }

}



// Merge Sort function to divide the work

void mergeSort(vector<int>& arr, int low, int high) {

    if (low >= high) return;

    int mid = (high + low) / 2;



    mergeSort(arr, low, mid);

    mergeSort(arr, mid + 1, high);

    merge(arr, low, mid, high);

}



// Thread function to execute mergeSort



void* threadMergeSort(void* arg) {

    ThreadArgs* args = (ThreadArgs*)arg;

    pthread_mutex_lock(&mutex1);

    cout << "Starting thread: " << args->id << " : low = " << args->low << ", high = " << args->high << endl;

    pthread_mutex_unlock(&mutex1);


    mergeSort(args->myarr, args->low, args->high);

    pthread_exit(NULL);

}


int main(int argc, char* argv[]) {

    string t1 = argv[1];

    int numThreads = stoi(t1);

    int N;
    cout<<"Enter Number (N) of Array:";
    cin >> N;

    vector<int> myarr(N);
    cout<<"Ensert array not sorted:";
    for (int i = 0; i < N; i++) {

        cin >> myarr[i];
    }

    vector<ThreadArgs> args(numThreads, {0, 0, 0, myarr});
    vector<pthread_t> myThreads(numThreads);

    cout << "\nMain: Starting sorting with N= " << N << ", Number of threads " << numThreads << "\n\n";


// Record the start time

    auto start = high_resolution_clock::now();  // Start time

    int chunkSize = N / numThreads;
    int remainder = N % numThreads;
    int index = 0;

    // Divide the work among the threads

    for (int i = 0; i < numThreads; i++) {

        args[i].low = index;
        args[i].high = index + chunkSize - 1;

        if (remainder > 0) {

            args[i].high++;
            remainder--;

        }

        args[i].id = i;
        index = args[i].high + 1;

        if (pthread_create(&myThreads[i], NULL, threadMergeSort, (void*)&args[i]) != 0) {

            cout << "Error creating thread " << i << endl;
            return 1;

        }

    }



    // Wait for all threads to finish

    for (int i = 0; i < numThreads; i++) {

        if (pthread_join(myThreads[i], NULL) != 0) {

            cout << "Error joining thread " << i << endl;
            return 1;

        }

    }


     if(numThreads ==2) {

	 merge(myarr,0,(N/2)-1,N-1);

     }

     else mergeSort(myarr,0,myarr.size()-1);

		 

    cout << "\nSorted array:\n" ;

    for (int i = 0; i < myarr.size(); i++) {

        cout << myarr[i] << " ";

    }

    cout <<"\n\n";

    // Record the end time

    auto end = high_resolution_clock::now();  // End time

    // Calculate the duration

   auto duration = duration_cast<microseconds>(end - start); // Duration in milliseconds

    
    // Output the duration

    cout << "Time taken to sort the array: " << duration.count()<< " microseconds" << endl;



    return 0;

}

