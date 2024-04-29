#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <climits>
#include <mpi.h>

using namespace std;

void sequentialOddEvenSort(vector<int>& arr) {
    bool isSorted = false;
    int n = arr.size();
    while (!isSorted) {
        isSorted = true;
        for (int i = 1; i < n; i += 2) {
            if (arr[i - 1] > arr[i]) {
                swap(arr[i - 1], arr[i]);
                isSorted = false;
            }
        }
        for (int i = 1; i < n - 1; i += 2) {
            if (arr[i] > arr[i + 1]) {
                swap(arr[i], arr[i + 1]);
                isSorted = false;
            }
        }
    }
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    ifstream inFile("randoms.txt");
    vector<int> array;
    int value;
    while (inFile >> value) {
        array.push_back(value);
    }
    inFile.close();

    int N = array.size();
    int remainder = N % size;
    if (remainder != 0) {
        int fillCount = size - remainder;
        for (int i = 0; i < fillCount; ++i) {
            array.push_back(INT_MAX);
        }
        N += fillCount;
    }

    vector<int> localArr(N / size);
    MPI_Scatter(array.data(), N / size, MPI_INT, localArr.data(), N / size, MPI_INT, 0, MPI_COMM_WORLD);

    double startTimePar = MPI_Wtime();
    sort(localArr.begin(), localArr.end());
    double endTimePar = MPI_Wtime();

    vector<int> sortedArray(N);
    MPI_Gather(localArr.data(), N / size, MPI_INT, sortedArray.data(), N / size, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        sortedArray.erase(remove(sortedArray.begin(), sortedArray.end(), INT_MAX), sortedArray.end());
        ofstream outFilePar("ParallelSortedArray.txt");
        for (int num : sortedArray) {
            outFilePar << num << "\n";
        }
        outFilePar.close();

        vector<int> sequentialArray = array;
        sequentialArray.resize(N - (N % size)); // Видалення доданих INT_MAX
        double startTimeSeq = MPI_Wtime();
        sequentialOddEvenSort(sequentialArray);
        double endTimeSeq = MPI_Wtime();
        cout << "Sequential sort time: " << endTimeSeq - startTimeSeq << " seconds." << endl;

        ofstream outFileSeq("SequentialSortedArray.txt");
        for (int num : sequentialArray) {
            outFileSeq << num << "\n";
        }
        outFileSeq.close();
    }

    MPI_Finalize();
    return 0;
}
