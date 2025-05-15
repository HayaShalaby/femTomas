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
int reg[8]; // Array of 8 registers
vector<pair<int, int>> memory; // Data memory representation
vector<string> instruction; // Vector to store instructions

void driversprogram() {
    cout << "Please enter the starting address of the program: ";
    cin >> PC; // Set the starting address
    initialPC = PC;
    for (int i = 0; i < 8; i++) { // changed registers from 32 to 8
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

// OUTPUT FUNCTION, COMMENTED OUT BC NEEDS TO BE MODIFIED ACCORDING TO NEW CODE
//void simDisplay() {
// Formatted simulation output
//    // Output PC
//    cout << "Program Counter (decimal): " << PC << "\n\n";
//    cout << "Program Counter (binary): " << decimal_to_binary(PC) << "\n\n";
//    cout << "Program Counter (binary): " << decimal_to_hexadecimal(PC) << "\n\n";
//
//    if (reg[0] != 0) {
//        cout << "WARNING: Attempt to manipulate zero register -- rejected." << endl;
//        reg[0] = 0;
//    }
//
//    // Output RF
//    std::cout << "Register File:" << std::endl;
//    for (int i = 0; i < 32; ++i) {
//        std::cout << "  Register [" << i << "]: " << std::setw(10) << reg[i] << std::setw(10) << " " << decimal_to_binary(reg[i]) << std::setw(10) << " " << decimal_to_hexadecimal(reg[i]) << std::endl;
//    }
//    std::cout << std::endl;
//
//    // Output Memory
//    std::cout << "Memory:" << std::endl;
//    for (size_t i = 0; i < memory.size(); ++i) {
//        std::cout << "  Location [" << i << "]: "
//                  << std::setw(4) << memory[i].first
//                  << " -> " << std::setw(10) << memory[i].second << std::setw(10) << " " << decimal_to_binary(memory[i].second) << std::setw(10) << " " << decimal_to_hexadecimal(memory[i].second) << std::endl;
//    }
//}

void simulator(int& programcount) {
    cout << "----------------------------------------------------------------------" << endl;

    string action;
    int i = 0;
    //every iteration in the loop one must change i and PC and size according to the
    // instruction that is currently executed
    while (true) {
        action = "";
        string rd = "";
        string imm = "";
        string rs1 = "";
        string rs2 = "";

        // Extract the action (instruction) part
        for (int j = 0; j < instruction[i].size(); j++) {
            if (instruction[i][j] == ' ') {
                break;
            }
            action += instruction[i][j];
        }

        cout << action << endl;
        if (action == "ECALL" || action == "EBREAK" || action == "PAUSE" || action == "FENCE" || action == "FENCE.TSO")
        {
            return;
        }


        if (action == "BEQ")
        {
            int rs1Index, rs2Index;
            int immediateValue;

            //Parse `rs1`, `rs2`, and `imm` from the instruction
            size_t pos = instruction[i].find(" ");
            if (pos != string::npos) {
                string rs1, rs2, imm;

                //Extract `rs1` (between the first space and first comma)
                rs1 = instruction[i].substr(pos + 1, instruction[i].find(",", pos) - pos - 1);
                rs1Index = stoi(rs1.substr(1));  // Convert to index

                //Extract `rs2` (between first and second commas)
                size_t pos2 = instruction[i].find(",", pos + 1);
                rs2 = instruction[i].substr(pos2 + 1, instruction[i].find(",", pos2 + 1) - pos2 - 1);
                rs2Index = stoi(rs2.substr(1));  // Convert to index

                //Extract `imm` (after the last comma)
                imm = instruction[i].substr(instruction[i].rfind(",") + 1);
                immediateValue = stoi(imm);  // Convert to integer
            }

            cout << "BEQ: Comparing registers x" << rs1Index << " and x" << rs2Index << endl;
            cout << "Values: " << reg[rs1Index] << " and " << reg[rs2Index] << endl;

            //Check if registers are equal
            if (reg[rs1Index] == reg[rs2Index]) {
                //If equal, update the PC to the branch target address
                PC = PC + immediateValue;
                cout << "Branch taken. New PC: " << PC << endl;

                //Update loop index based on new PC value
                i = (PC - initialPC) / 4;
            }
            else
            {
                cout << "Branch not taken. Continuing to next instruction." << endl;
                PC = PC + 4;
                i++;
            }
        }

        else if (action == "STORE") {
            int rdIndex, rs1Index;
            int offset;

            size_t pos = instruction[i].find(" ");
            if (pos != string::npos) {
                // Parsing rs1 by locating the first space character in the string
                rs1 = instruction[i].substr(pos + 1, instruction[i].find(",", pos) - pos - 1);

                // Parsing offset
                size_t pos2 = instruction[i].find(",");
                imm = instruction[i].substr(pos2 + 1, instruction[i].find("(", pos2) - pos2 - 1);

                // Parsing rd by locating the first open bracket in the string and extracting the register
                size_t pos3 = instruction[i].find("(");
                string rd = instruction[i].substr(pos3 + 1, instruction[i].find(")") - pos3 - 1).substr(1);
                if (rd.length() == 3)
                {
                    rd = rd.erase(0, 1);
                }
                rdIndex = stoi(rd);
            }

            // Convert rs1 to register index (assuming rs1 is in the form "x0", "x1", etc.)
            rs1Index = stoi(rs1.substr(1)); // Convert "x1" to 1, "x2" to 2, etc.

            // Convert offset (string) to an integer directly as decimal
            offset = stoi(imm); // Treat the input as a decimal value

            // Storing the value into memory address at rd
            memWrite(reg[rdIndex] + offset, reg[rs1Index]);
            cout << "Value " << reg[rs1Index] << " stored at memory address " << reg[rdIndex + offset] << endl;

            // Increment program counter & i to point to next instruction
            i++;
            PC += 4;
        }

        else if (action == "LOAD")
        {
            int rdIndex, rs1Index, offset;
            // Parse `rd`, `rs1`, and `offset` from the instruction
            size_t pos = instruction[i].find(" ");
            if (pos != string::npos) {
                string rd, rs1, offsetStr;

                // Extract `rd` (between the first space and first comma)
                rd = instruction[i].substr(pos + 1, instruction[i].find(",", pos) - pos - 1);
                rdIndex = stoi(rd.substr(1));  // Convert to index

                // Extract `offset` (between the first and second commas)
                size_t pos2 = instruction[i].find(",", pos + 1);
                offsetStr = instruction[i].substr(pos2 + 1, instruction[i].find("(", pos2 + 1) - pos2 - 1);
                offset = stoi(offsetStr);  // Convert to integer

                // Extract `rs1` (between parentheses after `offset`)
                size_t pos3 = instruction[i].find("(", pos2 + 1);
                rs1 = instruction[i].substr(pos3 + 1, instruction[i].find(")", pos3) - pos3 - 1);
                rs1Index = stoi(rs1.substr(1));  // Convert to index
            }

            // Calculate the memory address (rs1 + offset)
            int address = reg[rs1Index] + offset;

            // Fetch the byte from memory at the calculated address
            int32_t byte = memory[(address - initialPC) / 4].second; // Assuming memory is a byte-addressable array/vector

            reg[rdIndex] = byte;

            cout << "LB: Loaded byte from address " << address << " into register x" << rdIndex << endl;
            cout << "Value in x" << rdIndex << ": " << reg[rdIndex] << endl;

            // Increment the program counter to the next instruction
            PC += 4;
            i++;
        }

        else if (action == "SUB") {
            int rdIndex, rs1Index, rs2Index;
            string rd, rs1, rs2;
            // Parsing of rd, rs1, rs2 from instruction
            size_t pos = instruction[i].find(" "); // First space after the instruction name
            size_t pos2 = instruction[i].find(",", pos + 1); // First comma
            size_t pos3 = instruction[i].find(",", pos2 + 1); // Second comma


            if (pos != string::npos) {
                // Extracting rd
                rd = instruction[i].substr(pos + 1, pos2 - pos - 1);
                rdIndex = stoi(rd.substr(1)); // converts register number into its index (e.g. x1 -> 1)

                // Extracting rs1
                rs1 = instruction[i].substr(pos2 + 1, pos3 - pos2 - 1);
                rs1Index = stoi(rs1.substr(1));

                // Extracting rs2
                rs2 = instruction[i].substr(pos3 + 1);
                rs2Index = stoi(rs2.substr(1));
            }
            reg[rdIndex] = reg[rs1Index] - reg[rs2Index]; // The subtraction process itself

            cout << "SUB: Subtracting x" << rs2Index << " from x" << rs1Index << " and storing result in x" << rdIndex << endl;
            cout << "Register x" << rdIndex << " now contains: " << reg[rdIndex] << " (Decimal)" << endl;

            PC += 4; // updating program counter
            i++; // moving to the next instruction
        }

        // NEED THIS TO BE NOR !!!
        else if (action == "OR") {
            int rdIndex, rs1Index, rs2Index;
            string rd, rs1, rs2;

            // Parse rd, rs1, and rs2 from the instruction
            size_t pos = instruction[i].find(" ");
            size_t pos2 = instruction[i].find(",", pos + 1);
            size_t pos3 = instruction[i].find(",", pos2 + 1);

            if (pos != string::npos) {
                rd = instruction[i].substr(pos + 1, pos2 - pos - 1); // Extract rd
                rs1 = instruction[i].substr(pos2 + 1, pos3 - pos2 - 1); // Extract rs1
                rs2 = instruction[i].substr(pos3 + 1); // Extract rs2
            }

            // Convert register numbers to indices
            rdIndex = stoi(rd.substr(1));
            rs1Index = stoi(rs1.substr(1));
            rs2Index = stoi(rs2.substr(1));

            // ORing rs1 and rs2 (bitwise OR)
            reg[rdIndex] = reg[rs1Index] | reg[rs2Index];


            cout << "OR: Performed bitwise OR between registers " << rs1 << " and " << rs2;
            cout << ". Stored result in register " << rd << endl;
            cout << "Register " << rd << " now contains: " << reg[rdIndex] << " (Decimal)" << endl;

            PC += 4;
            i++;
        }

        else if (action == "ADD") {
            int rdIndex, rs1Index, rs2Index;
            string rd, rs1, rs2;

            // Parse rd, rs1, and rs2 from the instruction
            size_t pos = instruction[i].find(" ");
            size_t pos2 = instruction[i].find(",", pos + 1);
            size_t pos3 = instruction[i].find(",", pos2 + 1);

            if (pos != string::npos) {
                rd = instruction[i].substr(pos + 1, pos2 - pos - 1); // Extract rd
                rs1 = instruction[i].substr(pos2 + 1, pos3 - pos2 - 1);  // Extract rs1
                rs2 = instruction[i].substr(pos3 + 1); // Extract rs2
            }

            // Convert register names to indices
            rdIndex = stoi(rd.substr(1));
            rs1Index = stoi(rs1.substr(1));
            rs2Index = stoi(rs2.substr(1));

            // Perform the addition
            reg[rdIndex] = reg[rs1Index] + reg[rs2Index];

            cout << "ADD: Added registers " << rs1 << " and " << rs2;
            cout << ". Stored result in register " << rd << endl;
            cout << "Register " << rd << " now contains: " << reg[rdIndex] << " (Decimal)" << endl;

            PC += 4; // update program counter
            i++;
        }

        // Add additional instructions here as needed
        else if(action == "MUL"){

        }

        else if(action == "CALL"){

        }

        else if(action == "RET"){

        }

        else {
            cout << "Unknown instruction: " << action << endl;
            exit(0);
        }

//        simDisplay();
        cout << "-----------------------------------------------------------------------" << endl;
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
