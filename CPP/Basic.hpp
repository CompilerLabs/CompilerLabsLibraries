#pragma once

#include <cstring>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdint.h>

/* Basic Types */
namespace CompilerLabs {
	// integer types
	typedef uint8_t U8;
	typedef uint16_t U16;
	typedef uint32_t U32;
	typedef uint64_t U64;
	typedef int8_t S8;
	typedef int16_t S16;
	typedef int32_t S32;
	typedef int64_t S64;

	// pointer types
	typedef void* Address;

	// boolean type
	typedef bool Boolean;

	// floating points
	typedef float F32;
	typedef double F64;

    // character defines
    typedef CompilerLabs::U8 Character;
    typedef CompilerLabs::U64 TabDepth;

    // generic defines
    typedef CompilerLabs::U8 Byte;
    typedef CompilerLabs::U64 Length;
    typedef CompilerLabs::U64 Index;
    typedef CompilerLabs::U64 Increase;

    // text location defines
    typedef CompilerLabs::Index FileIndex;
    typedef CompilerLabs::U64 LineNumber;
    typedef CompilerLabs::Index CharacterIndex;

    // default list increase
    const CompilerLabs::Increase DefaultListIncrease = 1024;

    // buffer
    class Buffer {
    public:
        CompilerLabs::Address start;
        CompilerLabs::Address end;

        // constructors
        Buffer() {
            start = 0;
            end = 0;
        }
        Buffer(CompilerLabs::Address _start, CompilerLabs::Address _end) {
            start = _start;
            end = _end;
        }
        Buffer(char* cString) {
            start = cString;
            end = (CompilerLabs::Address)(strlen(cString) - 2);
        }

        // length of buffer
        CompilerLabs::Length Length() {
            return (CompilerLabs::Length)((CompilerLabs::U64)end - (CompilerLabs::U64)start + 1);
        }

        // open
        void Allocate(CompilerLabs::Length _allocationSize) {
            start = malloc(_allocationSize);
            if (start != 0) {
                end = (CompilerLabs::Address)((CompilerLabs::U64)start + _allocationSize - 1);
            } else {
                end = 0;
            }
        }

        // close
        void Deallocate() {
            if (start != 0) {
                // clear buffer
                free(start);

                // zero out values
                start = 0;
                end = 0;
            }
        }

        // check if buffer is empty
        CompilerLabs::Boolean IsEmpty() {
            return start == 0 && end == 0;
        }
    };

    class Current {
    public:
        CompilerLabs::Buffer range;
        CompilerLabs::Address progress;

        Current() {
            range = CompilerLabs::Buffer();
            progress = 0;
        }
        Current(CompilerLabs::Buffer _range, CompilerLabs::Address _progress) {
            range = _range;
            progress = _progress;
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
