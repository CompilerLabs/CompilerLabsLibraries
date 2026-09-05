#pragma once

// includes
#include "Basic.hpp"
#include "Logs.hpp"

namespace CompilerLabs::FileSystem {
    // file loader
    class FileLoader {
    public:
        // load file into std::string
        static std::string LoadTextFile(CompilerLabs::Logs::Log* log, std::string _filePath) {
            // open file stream
            std::ifstream file(_filePath);

            // check for error
            if (!file) {
                // setup error
                log->LogError("Text file could not be loaded / found. File path: '" + _filePath + "'");

                // return empty
                return "";
            }

            // read file into string
            std::ostringstream stringstream;
            stringstream << file.rdbuf();
            return stringstream.str();
        }
    };
}
