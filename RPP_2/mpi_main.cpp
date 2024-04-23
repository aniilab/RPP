#include <string>
#include "mpi.h"
#include <chrono>
#include <cmath>

using namespace std;

void printArray(int* arr, int size) {
    for (int i = 0; i < size; ++i) {
        fprintf(stdout, "%d ", arr[i]);
    }
    fprintf(stdout, "\n");
    fflush(stdout);
}

int* oddEvenSort(int* arr, int N, int rank, int size){
    double startTime;
    if (rank == 0) {
        startTime = MPI_Wtime();
    }
    for (int i = 0; i < N; i++) {
        int sendTo = (i % 2 == 0) ? rank - 1 : rank + 1;
        int recvFrom = (i % 2 == 0) ? rank + 1 : rank - 1;
        if (sendTo < 0 || sendTo >= size) sendTo = MPI_PROC_NULL;
        if (recvFrom < 0 || recvFrom >= size) recvFrom = MPI_PROC_NULL;
        
        MPI_Status status;
        MPI_Sendrecv_replace(arr, N, MPI_INT, sendTo, i, recvFrom, i, MPI_COMM_WORLD, &status);
        
        for (int j = 0; j < N; j++) {
            if ((j + i) % 2 == 0 && j < N - 1) {
                if (arr[j] > arr[j + 1]) {
                    int temp = arr[j + 1];
                    arr[j + 1] = arr[j];
                    arr[j] = temp;
                }
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
    
    if (rank == 0) {
        double endTime = MPI_Wtime();
        double elapsedTime = endTime - startTime;
        fprintf(stdout, "Elapsed parallel time:  %f\n", elapsedTime);
        fflush(stdout);
    }
    return arr;
}

int* oddEvenSortNonParallel(int* arr, int N){
    double startTime;
    startTime = MPI_Wtime();
    for (int i = 0; i < N; i++) {
        for (int pos = 0; pos < N; pos++) {
            if (i % 2 == 0) {
                if (pos % 2 == 0) {
                    if (arr[pos] > arr[pos + 1]) {
                        int temp = arr[pos + 1];
                        arr[pos + 1] = arr[pos];
                        arr[pos] = temp;
                    }
                }
            } else {
                if (pos % 2 == 1) {
                    if (arr[pos] > arr[pos + 1]) {
                        int temp = arr[pos + 1];
                        arr[pos + 1] = arr[pos];
                        arr[pos] = temp;
                    }
                }
            }
        }
    }
    double endTime = MPI_Wtime();
    double elapsedTime = endTime - startTime;
    fprintf(stdout, "Elapsed nonparallel time:  %f\n", elapsedTime);
    fflush(stdout);
    return arr;
}

int main(int argc, char **argv) {
    int N = 50000;
    int rank, size;
    int namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Get_processor_name(processor_name, &namelen);

    srand(time(nullptr) + rank); // Different seed for each process

    int* myArray = new int[N];
    for (int i = 0; i < N; ++i) {
        myArray[i] = rand() % 10000;
    }

    int* sortedArray = oddEvenSort(myArray, N, rank, size);
    
    if (rank == 0) {
        sortedArray = oddEvenSortNonParallel(myArray, N);
    }

    delete[] myArray;
    
    MPI_Finalize();

    return 0;
}
