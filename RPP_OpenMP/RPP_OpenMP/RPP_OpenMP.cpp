#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <omp.h>
#include <chrono>

// Function to check if a string is a palindrome
bool is_palindrome(const std::string& str) {
    int len = str.length();
    for (int i = 0; i < len / 2; ++i) {
        if (str[i] != str[len - 1 - i])
            return false;  // If characters at mirroring positions don't match, it's not a palindrome
    }
    return true;  // All mirroring characters matched, so it is a palindrome
}

// Function to read strings from a file into a vector
std::vector<std::string> read_input(const std::string& filename) {
    std::vector<std::string> strings;
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        strings.push_back(line);  // Append each line from the file to the vector
    }
    file.close();
    return strings;
}

// Function to write palindrome strings to a file
void write_output(const std::vector<std::string>& output, const std::string& filename) {
    std::ofstream file(filename);
    for (const auto& s : output) {
        file << s << '\n';  // Write each palindrome to the file, followed by a newline
    }
    file.close();
}

// Main function that processes the input file to find palindromes
void find_palindromes(const std::string& input_file, const std::string& output_file, int num_threads) {
    auto strings = read_input(input_file);  // Load all strings from file
    std::vector<std::string> palindromes;
    int n = strings.size();

    omp_set_num_threads(num_threads);  // Set the number of threads for OpenMP operations

    auto start = std::chrono::high_resolution_clock::now();  // Start timing the palindrome detection

#pragma omp parallel shared(palindromes)  // Begin a parallel region with shared access to 'palindromes'
    {
        std::vector<std::string> local_palindromes;  // Local storage for palindromes found by each thread

#pragma omp for nowait  // Divide the loop iterations across threads without waiting at the end of the loop
        for (int i = 0; i < n; ++i) {
            if (is_palindrome(strings[i])) {
                local_palindromes.push_back(strings[i]);  // Check if the string is a palindrome and add to local list
            }
        }

#pragma omp critical  // Ensures that one thread at a time executes the block to avoid data races
        {
            palindromes.insert(palindromes.end(), local_palindromes.begin(), local_palindromes.end());  // Append local results to global list
        }
    }

    auto end = std::chrono::high_resolution_clock::now();  // Stop timing
    std::chrono::duration<double> elapsed = end - start;  // Calculate elapsed time

    write_output(palindromes, output_file);  // Write found palindromes to the output file

    // Print the number of threads used and the elapsed time for the operation
    std::cout << "Processes: " << num_threads << ", Elapsed time: " << elapsed.count() << " seconds\n";
}

int main() {
    const std::string input_file = "input.txt";
    const std::string output_file = "output.txt";

    find_palindromes(input_file, output_file, 2);

    return 0;
}
