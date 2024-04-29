#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <mpi.h>

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

void oddEvenSort(vector<int> &localArr, int rank, int size)
{
    vector<int> partnerData(localArr.size());
    MPI_Status status;

    for (int phase = 0; phase < size; ++phase)
    {
        sort(localArr.begin(), localArr.end());

        int partner;
        if (phase % 2 == 0)
        {
            if (rank % 2 == 0)
            {
                partner = rank + 1;
            }
            else
            {
                partner = rank - 1;
            }
        }
        else
        {
            if (rank % 2 == 0)
            {
                partner = rank - 1;
            }
            else
            {
                partner = rank + 1;
            }
        }

        if (partner >= 0 && partner < size)
        {
            MPI_Sendrecv(localArr.data(), localArr.size(), MPI_INT, partner, 0,
                         partnerData.data(), localArr.size(), MPI_INT, partner, 0,
                         MPI_COMM_WORLD, &status);

            if (rank < partner)
            {
                for (size_t i = 0; i < localArr.size(); ++i)
                {
                    localArr[i] = min(localArr[i], partnerData[i]);
                }
            }
            else
            {
                for (size_t i = 0; i < localArr.size(); ++i)
                {
                    localArr[i] = max(localArr[i], partnerData[i]);
                }
            }
        }
    }
}
int main(int argc, char **argv)
{
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

    int N = array.size();
    int localN = N / size;
    int remainder = N % size; // Залишок елементів, які не розподілені рівномірно

    // Кількість елементів, які потрібно додати до локального масиву для кожного процесу
    int extraElements = (rank < remainder) ? 1 : 0;

    // Розмір локального масиву для кожного процесу з додатковими елементами, якщо потрібно
    localN += extraElements;

    vector<int> localArr(localN);
    MPI_Scatter(array.data(), localN, MPI_INT, localArr.data(), localN, MPI_INT, 0, MPI_COMM_WORLD);

    double startTimePar = MPI_Wtime();
    oddEvenSort(localArr, rank, size);
    double endTimePar = MPI_Wtime();

    vector<int> sortedArray(N);
    MPI_Allgather(localArr.data(), localN, MPI_INT, sortedArray.data(), localN, MPI_INT, MPI_COMM_WORLD);

    if (rank == 0)
    {
        cout << "Parallel sort time: " << endTimePar - startTimePar << " seconds." << endl;
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
