#include "scripting/OMParser.h"
#include "scripting/OMCompiler.h"
#include <iostream>
#include <fstream>
#include <sstream>

void PrintUsage() {
    std::cout << "OmegaScript Compiler (OMC)" << std::endl;
    std::cout << "Usage: omc [options] <input.om>" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -o <output>   Output bytecode file (default: input.omc)" << std::endl;
    std::cout << "  -v            Verbose output" << std::endl;
    std::cout << "  -h            Show this help" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        PrintUsage();
        return 1;
    }

    std::string inputFile;
    std::string outputFile;
    bool verbose = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            PrintUsage();
            return 0;
        } else if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        } else if (arg == "-o" && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (arg[0] != '-') {
            inputFile = arg;
        }
    }

    if (inputFile.empty()) {
        std::cerr << "Error: No input file specified" << std::endl;
        PrintUsage();
        return 1;
    }

    if (outputFile.empty()) {
        outputFile = inputFile + "c"; // input.om -> input.omc
    }

    // Read input file
    std::ifstream file(inputFile);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file '" << inputFile << "'" << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string sourceCode = buffer.str();
    file.close();

    if (verbose) {
        std::cout << "Compiling: " << inputFile << std::endl;
        std::cout << "Output: " << outputFile << std::endl;
    }

    // Parse
    Omega::OMParser parser;
    auto ast = parser.Parse(sourceCode);
    
    if (!ast) {
        std::cerr << "Error: Failed to parse source code" << std::endl;
        return 1;
    }

    // Compile
    Omega::OMCompiler compiler;
    auto bytecode = compiler.Compile(ast.get());
    
    if (!bytecode) {
        std::cerr << "Error: Failed to compile AST" << std::endl;
        return 1;
    }

    // Write bytecode
    std::ofstream outFile(outputFile, std::ios::binary);
    if (!outFile.is_open()) {
        std::cerr << "Error: Cannot create output file '" << outputFile << "'" << std::endl;
        return 1;
    }

    outFile.write(reinterpret_cast<const char*>(bytecode->data()), bytecode->size());
    outFile.close();

    if (verbose) {
        std::cout << "Compilation successful!" << std::endl;
        std::cout << "Bytecode size: " << bytecode->size() << " bytes" << std::endl;
    }

    return 0;
}
