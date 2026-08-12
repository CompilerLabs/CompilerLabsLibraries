#pragma once

#include "Basic.hpp"

namespace CompilerLabs {
    // string length
    typedef CompilerLabs::Length StringLength;

    // text location
    class TextLocation {
    public:
        CompilerLabs::FileIndex fileIndex;
        CompilerLabs::LineNumber lineNumber;
        CompilerLabs::Buffer range;

        // constructors
        TextLocation(CompilerLabs::FileIndex _fileIndex, CompilerLabs::LineNumber _lineNumber, CompilerLabs::Buffer _range) {
            fileIndex = _fileIndex;
            lineNumber = _lineNumber;
            range = _range;
        }
        TextLocation() {
            fileIndex = 0;
            lineNumber = 0;
            range = CompilerLabs::Buffer();
        }
    };

    // Compiler Labs version of a string
    class Text {
    public:
        CompilerLabs::List data;
    };
}
