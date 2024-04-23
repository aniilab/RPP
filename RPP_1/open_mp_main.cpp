#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <omp.h>

void oddEvenSort(std::vector<int>& numbers, int numThreads) {
    int n = numbers.size();
    bool globallySorted = false;

    while (!globallySorted) {
        bool locallySorted = true;
        
        #pragma omp parallel for num_threads(numThreads) shared(numbers) reduction(&&:locallySorted)
        for (int i = 1; i < n - 1; i += 2) {
            if (numbers[i] > numbers[i + 1]) {
                std::swap(numbers[i], numbers[i + 1]);
                locallySorted = false;
            }
        }
        
        #pragma omp parallel for num_threads(numThreads) shared(numbers) reduction(&&:locallySorted)
        for (int i = 0; i < n - 1; i += 2) {
            if (numbers[i] > numbers[i + 1]) {
                std::swap(numbers[i], numbers[i + 1]);
                locallySorted = false;
            }
        }

        globallySorted = locallySorted;
    }
}

int main() {
    std::ifstream inputFile("randoms10.txt");
    std::ofstream outputFile("output.txt");

    if (!inputFile) {
        std::cerr << "Error: Could not open input file." << std::endl;
        return 1;
    }

    std::vector<int> numbers;
    int number;
    while (inputFile >> number) {
        numbers.push_back(number);
    }
    inputFile.close();

    int startThreads, endThreads;
    std::cout << "Enter the start and end number of threads: ";
    std::cin >> startThreads >> endThreads;

    for (int numThreads = startThreads; numThreads <= endThreads; ++numThreads) {
        std::vector<int> tempNumbers = numbers; 
        double startTime = omp_get_wtime();
        oddEvenSort(tempNumbers, numThreads);
        double endTime = omp_get_wtime();

        std::cout << "Execution time with " << numThreads << " threads: "
                  << (endTime - startTime) << " seconds." << std::endl;
    }

    for (int num : numbers) {
        outputFile << num << std::endl;
    }
    outputFile.close();
    std::cin >> endThreads;
    return 0;
}