#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <omp.h>

using namespace std;

void oddEvenSort(vector<int>& numbers, int numThreads) {
    int n = numbers.size();
    bool globallySorted = false;

    while (!globallySorted) {
        bool locallySorted = true;
        
        #pragma omp parallel for num_threads(numThreads) shared(numbers) reduction(&&:locallySorted)
        for (int i = 1; i < n - 1; i += 2) {
            if (numbers[i] > numbers[i + 1]) {
                swap(numbers[i], numbers[i + 1]);
                locallySorted = false;
            }
        }
        
        #pragma omp parallel for num_threads(numThreads) shared(numbers) reduction(&&:locallySorted)
        for (int i = 0; i < n - 1; i += 2) {
            if (numbers[i] > numbers[i + 1]) {
                swap(numbers[i], numbers[i + 1]);
                locallySorted = false;
            }
        }

        globallySorted = locallySorted;
    }
}

int main() {
    ifstream inputFile("randoms10.txt");
    ofstream outputFile("output.txt");

    if (!inputFile) {
        cerr << "Error: Could not open input file." << endl;
        return 1;
    }

    vector<int> numbers;
    int number;
    while (inputFile >> number) {
        numbers.push_back(number);
    }
    inputFile.close();

    int startThreads, endThreads;
    cout << "Enter the start and end number of threads: ";
    cin >> startThreads >> endThreads;

    for (int numThreads = startThreads; numThreads <= endThreads; ++numThreads) {
        vector<int> tempNumbers = numbers; 
        double startTime = omp_get_wtime();
        oddEvenSort(tempNumbers, numThreads);
        double endTime = omp_get_wtime();

        cout << "Execution time with " << numThreads << " threads: "
                  << (endTime - startTime) << " seconds." << endl;

        if (numThreads == endThreads) {
            for (int num : tempNumbers) {
                outputFile << num << endl;
            }
        }
    }

    outputFile.close();
    cin >> endThreads;
    return 0;
}
