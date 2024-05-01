#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <mpi.h>
#include <iomanip>
#include <chrono>

using namespace std;

void sequentialOddEvenSort(vector<int> &arr)
{
    bool isSorted = false;
    int n = arr.size();
    while (!isSorted)
    {
        isSorted = true;
        for (int i = 1; i < n; i += 2)
        {
            if (arr[i - 1] > arr[i])
            {
                swap(arr[i - 1], arr[i]);
                isSorted = false;
            }
        }
        for (int i = 1; i < n - 1; i += 2)
        {
            if (arr[i] > arr[i + 1])
            {
                swap(arr[i], arr[i + 1]);
                isSorted = false;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    ifstream inFile("randoms.txt");
    vector<int> array;
    int value;
    while (inFile >> value)
    {
        array.push_back(value);
    }
    inFile.close();

    double totalParTime = 0;

    if (rank == 0)
    {
        double startTimeSeq = MPI_Wtime();
        sequentialOddEvenSort(array);
        double endTimeSeq = MPI_Wtime();
        cout << "Sequential sort time: " << endTimeSeq - startTimeSeq << " seconds." << endl;
        ofstream outFile("SequentialSortedArray.txt");
        for (int num : array)
        {
            outFile << num << "\n";
        }
        outFile.close();
    }

    const int ROOT = 0;
    int N = array.size();
    MPI_Bcast(&N, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

    int localN = N / size;
    int remainder = N % size;

    vector<int> localBuffer(localN);
    if (rank == size - 1) {
        localBuffer.resize(localN + remainder);
    }

    MPI_Scatter(array.data(), localBuffer.size(), MPI_INT, localBuffer.data(), localBuffer.size(), MPI_INT, ROOT, MPI_COMM_WORLD);

    double startTimePar = MPI_Wtime();
    sequentialOddEvenSort(localBuffer);
    double endTimePar = MPI_Wtime();
    double execTime = endTimePar - startTimePar;

    cout << "Thread " << rank << ": " << fixed << execTime << " seconds" << endl;

    MPI_Reduce(&execTime, &totalParTime, 1, MPI_DOUBLE, MPI_SUM, ROOT, MPI_COMM_WORLD);

    vector<int> sortedArray(N);
    MPI_Gather(localBuffer.data(), localN, MPI_INT, sortedArray.data(), localN, MPI_INT, ROOT, MPI_COMM_WORLD);

    if (rank == ROOT)
    {
        
        auto startTime = high_resolution_clock::now();
        sequentialOddEvenSort(sortedArray); 
        auto endTime = high_resolution_clock::now();

        duration<double> execTime = (endTime - startTime);

        totalParTime += execTime.count();
        cout << "Total parallel sort time: " << fixed << totalParTime << " seconds." << endl;
        ofstream outFile("ParallelSortedArray.txt");
        for (int num : sortedArray)
        {
            outFile << num << "\n";
        }
        outFile.close();
    }

    MPI_Finalize();

    return 0;
}
