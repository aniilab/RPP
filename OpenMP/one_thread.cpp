#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <ctime>

int main()
{
    std::ifstream inputFile("randoms10.txt");
    std::ofstream outputFile("output.txt");

    if (!inputFile)
    {
        std::cerr << "Error: Could not open input file." << std::endl;
        return 1;
    }

    if (!outputFile)
    {
        std::cerr << "Error: Could not create output file." << std::endl;
        return 1;
    }

    std::vector<int> numbers;
    int number;
    while (inputFile >> number)
    {
        numbers.push_back(number);
    }

    clock_t startTime = clock();

    int n = numbers.size();
    bool isSorted = false;

    while (!isSorted)
    {
        isSorted = true;
        for (int i = 1; i < n - 1; i += 2)
        {
            if (numbers[i] > numbers[i + 1])
            {
                std::swap(numbers[i], numbers[i + 1]);
                isSorted = false;
            }
        }

        for (int i = 0; i < n - 1; i += 2)
        {
            if (numbers[i] > numbers[i + 1])
            {
                std::swap(numbers[i], numbers[i + 1]);
                isSorted = false;
            }
        }
    }

    clock_t endTime = clock();
    double executionTime = static_cast<double>(endTime - startTime) / CLOCKS_PER_SEC;

    for (size_t i = 0; i < numbers.size(); ++i)
    {
        outputFile << numbers[i] << std::endl;
    }

    std::cout << "Numbers sorted and written to output.txt." << std::endl;
    std::cout << "Execution time: " << executionTime << " seconds." << std::endl;
    std::cin >> executionTime;

    return 0;
}
