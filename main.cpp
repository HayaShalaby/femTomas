#include<iostream>
#include <vector>
#include <string>
#include <fstream>
#include <limits>
#include <iomanip>
#include <cmath>
#include <cstdint>
using namespace std;

int PC; // Program Counter
int initialPC;
int reg[32]; // Array of 32 registers
vector<pair<int, int>> memory; // Data memory representation
vector<string> instruction; // Vector to store instructions

void driversprogram() {
    cout << "Please enter the starting address of the program: ";
    cin >> PC; // Set the starting address
    initialPC = PC;
    for (int i = 0; i < 8; i++) {
        reg[i] = 0; // Initialize registers to 0
    }
}

void read_input_from_file(const string& filename) {
    ifstream inputFile(filename);
    if (!inputFile) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }

    string line;
    int i = 0;
    int increment = 0;
    while (getline(inputFile, line)) {
        if (line[0] == '#')
            continue;

        instruction.push_back(line); // Store the instruction
        pair<int, int> x(PC + increment, 0);
        memory.push_back(x);
        increment += 4;
        i++;
    }
    for (int i = 0; i < memory.size(); i++)
    {
        cout << memory[i].first << " " << memory[i].second << endl;
    }
    inputFile.close(); // Close the file
}

// Writing the 'second' value to memory at address 'first'
void memWrite(int first, int second, bool isHalfword = false) {
    int index = (first - initialPC) / 4;
    cout << first << " " << initialPC << endl;
    // Ensure 'memory' has enough space up to 'index'
    for (int i = memory.size(); i <= index; i++) {
        int address = initialPC + (i * 4);  // Calculate each address incrementally
        memory.push_back({ address, 0 });     // Initialize with default {address, 0}
    }

    // Handle halfword storage if specified
    if (isHalfword) {
        // Extract the lower 16 bits of 'second' for halfword write
        int halfwordValue = second & 0xFFFF;

        // Combine the halfword with the existing word in memory (preserving upper 16 bits)
        memory[index].second = (memory[index].second & 0xFFFF0000) | halfwordValue;
    }
    else {
        // For a full word write, replace the entire word
        cout << index << endl;
        memory[index] = { first, second };
    }
}

int main() {
    // Start of the driver program
    driversprogram();
    // End of the driver program

    // Read the RISC-V code from a file
    string filename;
//    filename = "C:/Users/Haya/Desktop/Assembly Project 1 Final/testProgram5.txt";
    cout << "Please enter the filename to read instructions from: ";
    cin >> filename; // Get filename from user
    read_input_from_file(filename);

    // Input program data from user
    int programDataAm;
    cout << "Please enter the number of values you want to initially load in memory: " << endl;
    cin >> programDataAm;

    int first, second;
    for (int i = 0; i < programDataAm; i++) {
        cout << "Please enter address " << i << ":" << endl;
        cin >> first;

        cout << "Please enter value " << i << ":" << endl;
        cin >> second;

        memWrite(first, second);
    }

    // Start computing the RISC-V code
//    simulator(PC);
    // End of computing the RISC-V code
    return 0;
}
