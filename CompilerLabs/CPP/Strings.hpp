#pragma once

#include "Basic.hpp"

namespace CompilerLabs::Strings {
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

    // tabs
    class Tabs {
    public:
        // style of tabs
        std::string tabStyle;
        CompilerLabs::TabDepth tabDepth;

        // constructors
        Tabs() {
            tabStyle = "\t";
            tabDepth = 0;
        }
        Tabs(std::string _tabStyle) {
            tabStyle = _tabStyle;
            tabDepth = 0;
        }
        Tabs(CompilerLabs::TabDepth _tabDepth) {
            tabStyle = "\t";
            tabDepth = _tabDepth;
        }
        Tabs(std::string _tabStyle, CompilerLabs::TabDepth _tabDepth) {
            tabStyle = _tabStyle;
            tabDepth = _tabDepth;
        }

        // generate tabs
        std::string GenerateTabs(CompilerLabs::TabDepth tabCount) {
            std::string output = "";

            // append tabs
            for (CompilerLabs::TabDepth i = 0; i < tabCount; i++) {
                output += tabStyle;
            }
            
            return output;
        }
        std::string GenerateTabs() {
            return GenerateTabs(tabDepth);
        }
    };
}
