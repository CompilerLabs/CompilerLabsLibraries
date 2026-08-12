#pragma once

#include "Basic.hpp"
#include "Text.hpp"
#include "Log.hpp"
#include <string>

namespace CompilerLabs {
    enum class LexlingType {
        Invalid,
        LeftSquareBracket,
        RightSquareBracket,
        LeftCurlyBracket,
        RightCurlyBracket,
        Colon,
        Comma,
        False,
        True,
        Null,
        Number,
        String,
        EndOfFile
    };

    class Lexling {
    public:
        CompilerLabs::LexlingType type;
        std::string stringedValue;
        CompilerLabs::TextLocation location;

        Lexling() {
            type = CompilerLabs::LexlingType::Invalid;
            stringedValue = "";
            location = CompilerLabs::TextLocation();
        }
        Lexling(CompilerLabs::LexlingType _type, std::string _stringedValue, CompilerLabs::TextLocation _location) {
            type = _type;
            stringedValue = _stringedValue;
            location = _location;
        }
    };

    enum class ItemType {
        Invalid,
        Object,
        Array,
        String,
        Number,
        False,
        True,
        Null, // is an actual object type, not a dead end marker
    };

    class Item {
    public:
        // the type of item
        CompilerLabs::ItemType type;

        // the name assigned to the item
        CompilerLabs::Lexling name;

        // possible states (null does not require value as type is sufficient)
        CompilerLabs::Lexling stringValue;
        CompilerLabs::Lexling numberValue;
        CompilerLabs::Boolean booleanValue;
        std::vector<CompilerLabs::Item> objectBody;
        std::vector<CompilerLabs::Item> arrayBody;
        
        // constructors
        Item() {
            type = CompilerLabs::ItemType::Invalid;
            name = CompilerLabs::Lexling();
            numberValue = CompilerLabs::Lexling();
            booleanValue = false;
            objectBody = std::vector<CompilerLabs::Item>();
            arrayBody = std::vector<CompilerLabs::Item>();
        }
    };

    class Root {
    public:
        std::vector<CompilerLabs::Lexling> lexlings;
        CompilerLabs::Item masterObject;
    };

    class JSONCompiler {
    public:
        // compile json into C++ data
        CompilerLabs::Root CompileJSON(CompilerLabs::Log* log, std::string& json) {
            // setup output
            CompilerLabs::Root output;

            // lex
            output.lexlings = Lex(log, json);
            if (log->criticalErrorOccured) {
                return output;
            }

            // parse
            output.masterObject = Parse(log, output.lexlings);

            return output;
        }
    
    private:
        // check characters & character ranges
        CompilerLabs::Boolean LexingCheckCharacterRange(CompilerLabs::Character start, CompilerLabs::Character value, CompilerLabs::Character end) {
            return (value >= start) && (value <= end);
        }
        CompilerLabs::Boolean LexingCheckWhitespaceCharacter(CompilerLabs::Character value) {
            return LexingCheckCharacterRange(0, value, 32);
        }

        // check strings
        CompilerLabs::Boolean LexingCheckForSubStringAtIndex(std::string& masterString, std::string searchingFor, CompilerLabs::CharacterIndex startingIndex) {
            // check if master string has enough room left to even search for
            if (masterString.size() < searchingFor.size() || searchingFor.size() + startingIndex > masterString.size()) {
                return false;
            }

            // search for substring
            for (CompilerLabs::CharacterIndex index = 0; index < searchingFor.size(); index++) {
                // if characters differ
                if (masterString[startingIndex + index] != searchingFor[index]) {
                    // not there
                    return false;
                }
            }

            // equal by process of elimination
            return true;
        }

        // check end of file
        CompilerLabs::Boolean IndexInRangeOfString(std::string& input, CompilerLabs::Index index) {
            return index < input.length();
        }

        // lex file
        std::vector<CompilerLabs::Lexling> Lex(CompilerLabs::Log* log, std::string& input) {
            // setup output
            std::vector<CompilerLabs::Lexling> output = std::vector<CompilerLabs::Lexling>();
            CompilerLabs::FileIndex fileIndex = 0;
            CompilerLabs::LineNumber lineNumber = 1;
            CompilerLabs::CharacterIndex index = 0;

            // lex
            while (IndexInRangeOfString(input, index)) {
                // skip whitespace
                while (IndexInRangeOfString(input, index) && LexingCheckWhitespaceCharacter(input[index])) {
                    // check for new lines
                    if (LexingCheckCharacterRange('\n', input[index], '\n') || LexingCheckCharacterRange('\r', input[index], '\r')) {
                        lineNumber++;
                    }

                    // next character
                    index++;
                }
                if (!IndexInRangeOfString(input, index)) {
                    goto quit;
                }

                // check for lexlings
                if (LexingCheckCharacterRange('[', input[index], '[')) {
                    output.push_back(CompilerLabs::Lexling(CompilerLabs::LexlingType::LeftSquareBracket, "[", CompilerLabs::TextLocation(fileIndex, lineNumber, CompilerLabs::Buffer((CompilerLabs::Address)index, (CompilerLabs::Address)index))));
                    index++;
                } else if (LexingCheckCharacterRange(']', input[index], ']')) {
                    output.push_back(CompilerLabs::Lexling(CompilerLabs::LexlingType::RightSquareBracket, "]", CompilerLabs::TextLocation(fileIndex, lineNumber, CompilerLabs::Buffer((CompilerLabs::Address)index, (CompilerLabs::Address)index))));
                    index++;
                } else if (LexingCheckCharacterRange('{', input[index], '{')) {
                    output.push_back(CompilerLabs::Lexling(CompilerLabs::LexlingType::LeftCurlyBracket, "{", CompilerLabs::TextLocation(fileIndex, lineNumber, CompilerLabs::Buffer((CompilerLabs::Address)index, (CompilerLabs::Address)index))));
                    index++;
                } else if (LexingCheckCharacterRange('}', input[index], '}')) {
                    output.push_back(CompilerLabs::Lexling(CompilerLabs::LexlingType::RightCurlyBracket, "}", CompilerLabs::TextLocation(fileIndex, lineNumber, CompilerLabs::Buffer((CompilerLabs::Address)index, (CompilerLabs::Address)index))));
                    index++;
                } else if (LexingCheckCharacterRange(':', input[index], ':')) {
                    output.push_back(CompilerLabs::Lexling(CompilerLabs::LexlingType::Colon, ":", CompilerLabs::TextLocation(fileIndex, lineNumber, CompilerLabs::Buffer((CompilerLabs::Address)index, (CompilerLabs::Address)index))));
                    index++;
                } else if (LexingCheckCharacterRange(',', input[index], ',')) {
                    output.push_back(CompilerLabs::Lexling(CompilerLabs::LexlingType::Comma, ",", CompilerLabs::TextLocation(fileIndex, lineNumber, CompilerLabs::Buffer((CompilerLabs::Address)index, (CompilerLabs::Address)index))));
                    index++;
                } else if (LexingCheckForSubStringAtIndex(input, "false", index)) {
                    output.push_back(CompilerLabs::Lexling(CompilerLabs::LexlingType::False, "false", CompilerLabs::TextLocation(fileIndex, lineNumber, CompilerLabs::Buffer((CompilerLabs::Address)index, (CompilerLabs::Address)(index + (CompilerLabs::CharacterIndex)std::string("false").length())))));
                    index += std::string("false").length();
                } else if (LexingCheckForSubStringAtIndex(input, "true", index)) {
                    output.push_back(CompilerLabs::Lexling(CompilerLabs::LexlingType::True, "true", CompilerLabs::TextLocation(fileIndex, lineNumber, CompilerLabs::Buffer((CompilerLabs::Address)index, (CompilerLabs::Address)(index + (CompilerLabs::CharacterIndex)std::string("true").length())))));
                    index += std::string("true").length();
                } else if (LexingCheckForSubStringAtIndex(input, "null", index)) {
                    output.push_back(CompilerLabs::Lexling(CompilerLabs::LexlingType::Null, "null", CompilerLabs::TextLocation(fileIndex, lineNumber, CompilerLabs::Buffer((CompilerLabs::Address)index, (CompilerLabs::Address)(index + (CompilerLabs::CharacterIndex)std::string("null").length())))));
                    index += std::string("null").length();
                } else if (LexingCheckCharacterRange('"', input[index], '"')) {
                    // setup string start
                    CompilerLabs::CharacterIndex startingIndex = index;
                    
                    // skip past initial double quote
                    index++;

                    // search for end of string
                    while (true) {
                        // check for eof
                        if (!IndexInRangeOfString(input, index)) {
                            // string ended abruptly at the end of the file
                            log->LogError("JSON Error. A string literal did not finish properly.");

                            return output;
                        }

                        // check for new lines
                        if (LexingCheckCharacterRange('\n', input[index], '\n') || LexingCheckCharacterRange('\r', input[index], '\r')) {
                            lineNumber++;
                        }

                        // check for end of string
                        if (LexingCheckCharacterRange('"', input[index], '"')) {
                            // push lexling
                            output.push_back(CompilerLabs::Lexling(CompilerLabs::LexlingType::String, input.substr(startingIndex, index - startingIndex + 1), CompilerLabs::TextLocation(fileIndex, lineNumber, CompilerLabs::Buffer((CompilerLabs::Address)startingIndex, (CompilerLabs::Address)index))));

                            // skip past ending quote
                            index++;

                            // exit loop
                            break;
                        }

                        // next index
                        index++;
                    }
                } else if (LexingCheckCharacterRange('0', input[index], '9') || LexingCheckCharacterRange('-', input[index], '-')) {
                    CompilerLabs::CharacterIndex startingIndex = index;
                    CompilerLabs::CharacterIndex endingIndex = index;

                    // check for negative sign
                    if (LexingCheckCharacterRange('-', input[index], '-')) {
                        // next character
                        index++;

                        // ensure that there is a digit after the minus sign, but don't advance, just check for error
                        if (!IndexInRangeOfString(input, index) || !LexingCheckCharacterRange('0', input[index], '9')) {
                            // number did not contain digits
                            log->LogError("JSON Error. A negative sign was found without digits afterwards.");

                            return output;
                        } else {
                            endingIndex++;
                        }
                    }

                    // check for numeral characters
                    while (IndexInRangeOfString(input, index) && LexingCheckCharacterRange('0', input[index], '9')) {
                        index++;
                        endingIndex++;
                    }

                    // append lexling
                    output.push_back(CompilerLabs::Lexling(CompilerLabs::LexlingType::Number, input.substr(startingIndex, endingIndex - startingIndex), CompilerLabs::TextLocation(fileIndex, lineNumber, CompilerLabs::Buffer((CompilerLabs::Address)startingIndex, (CompilerLabs::Address)endingIndex))));

                    // next character
                    index++;
                } else {
                    // error, no lexling was matched
                    log->LogError("JSON Error. An unrecognized character was found in the JSON string. Line Number: '" + std::to_string(lineNumber) + "'");

                    return output;
                }
            }

            // append EndOfFile lexling
            quit:
            output.push_back(CompilerLabs::Lexling(CompilerLabs::LexlingType::EndOfFile, "[EndOfFile]", CompilerLabs::TextLocation(fileIndex, lineNumber, CompilerLabs::Buffer((CompilerLabs::Address)input.length(), (CompilerLabs::Address)input.length()))));

            return output;
        }

        // parse lexlings
        CompilerLabs::Item Parse(CompilerLabs::Log* log, std::vector<CompilerLabs::Lexling>& lexlings) {
            CompilerLabs::Item output;
            CompilerLabs::Index currentIndex = 0;

            // check for empty file
            if (lexlings[currentIndex].type == CompilerLabs::LexlingType::EndOfFile) {
                // error, nothing in file
                log->LogError("JSON Error. File is empty.");

                return output;
            }

            // parse root object
            output.type = CompilerLabs::ItemType::Object;
            output.objectBody = ParseObjectBody(log, lexlings, currentIndex);

            return output;
        }

        // parse object body
        std::vector<CompilerLabs::Item> ParseObjectBody(CompilerLabs::Log* log, std::vector<CompilerLabs::Lexling>& lexlings, CompilerLabs::Index& currentIndex) {
            std::vector<CompilerLabs::Item> output;

            // create comma tracker
            CompilerLabs::Index commaIndex = 0;

            // if object body is missing container
            if (lexlings[currentIndex].type != CompilerLabs::LexlingType::LeftCurlyBracket) {
                log->LogError("JSON Error. Object body is missing opening left curly bracket. Line Numberr: " + std::to_string(lexlings[currentIndex].location.lineNumber));

                return output;
            // otherwise, skip past bracket
            } else {
                currentIndex++;
            }

            // loop over body
            while (lexlings[currentIndex].type != CompilerLabs::LexlingType::RightCurlyBracket) {
                // setup temp item
                CompilerLabs::Item tempItem;
                tempItem.type = CompilerLabs::ItemType::Invalid;

                // if requires comma
                if (commaIndex > 0 && lexlings[currentIndex].type == CompilerLabs::LexlingType::Comma) {
                    // next lexling index
                    currentIndex++;
                }

                // check for name string
                if (lexlings[currentIndex].type == CompilerLabs::LexlingType::String) {
                    // set name
                    tempItem.name = lexlings[currentIndex];
                    currentIndex++;
                // error, missing name string
                } else {
                    log->LogError("JSON Error. Object member did not have a name string. Line Number: " + std::to_string(lexlings[currentIndex].location.lineNumber));
                    
                    return output;
                }

                // check for colon
                if (lexlings[currentIndex].type == CompilerLabs::LexlingType::Colon) {
                    // skip past colon
                    currentIndex++;
                // colon not found
                } else {
                    log->LogError("JSON Error. Colon was missing after object name's declaration. Line Number: " + std::to_string(lexlings[currentIndex].location.lineNumber));

                    return output;
                }

                // parse data
                if (lexlings[currentIndex].type == CompilerLabs::LexlingType::Null) {
                    // set type
                    tempItem.type = CompilerLabs::ItemType::Null;

                    // advance
                    currentIndex++;
                } else if (lexlings[currentIndex].type == CompilerLabs::LexlingType::False) {
                    // set type
                    tempItem.type = CompilerLabs::ItemType::False;
                    tempItem.booleanValue = false;

                    // advance
                    currentIndex++;
                } else if (lexlings[currentIndex].type == CompilerLabs::LexlingType::True) {
                    // set type
                    tempItem.type = CompilerLabs::ItemType::True;
                    tempItem.booleanValue = true;

                    // advance
                    currentIndex++;
                } else if (lexlings[currentIndex].type == CompilerLabs::LexlingType::String) {
                    // set type
                    tempItem.type = CompilerLabs::ItemType::String;
                    tempItem.stringValue = lexlings[currentIndex];

                    // advance
                    currentIndex++;
                } else if (lexlings[currentIndex].type == CompilerLabs::LexlingType::Number) {
                    // set type
                    tempItem.type = CompilerLabs::ItemType::Number;
                    tempItem.numberValue = lexlings[currentIndex];

                    // advance
                    currentIndex++;
                } else if (lexlings[currentIndex].type == CompilerLabs::LexlingType::LeftCurlyBracket) {
                    // set item
                    tempItem.type = CompilerLabs::ItemType::Object;
                    tempItem.objectBody = ParseObjectBody(log, lexlings, currentIndex);
                } else if (lexlings[currentIndex].type == CompilerLabs::LexlingType::LeftSquareBracket) {
                    // set item
                    tempItem.type = CompilerLabs::ItemType::Array;
                    tempItem.arrayBody = ParseArrayBody(log, lexlings, currentIndex);
                // error
                } else {
                    log->LogError("JSON Error. While parsing an object body, an invalid lexling appeared. Line Number: " + std::to_string(lexlings[currentIndex].location.lineNumber));

                    return output;
                }

                // append item to list
                output.push_back(tempItem);

                // check for error
                if (log->normalErrorOccured) {
                    return output;
                }

                // next comma index
                commaIndex++;
            }

            // check for right curly brace
            if (lexlings[currentIndex].type == CompilerLabs::LexlingType::RightCurlyBracket) {
                currentIndex++;
            }

            return output;
        }

        // parse array body
        std::vector<CompilerLabs::Item> ParseArrayBody(CompilerLabs::Log* log, std::vector<CompilerLabs::Lexling>& lexlings, CompilerLabs::Index& currentIndex) {
            std::vector<CompilerLabs::Item> output;

            // create comma tracker
            CompilerLabs::Index commaIndex = 0;

            // if object body is missing container
            if (lexlings[currentIndex].type != CompilerLabs::LexlingType::LeftSquareBracket) {
                log->LogError("JSON Error. Array body is missing opening left square bracket. Line Number: " + std::to_string(lexlings[currentIndex].location.lineNumber));

                return output;
            // otherwise, skip past bracket
            } else {
                currentIndex++;
            }

            // loop over body
            while (lexlings[currentIndex].type != CompilerLabs::LexlingType::RightSquareBracket) {
                // setup temp item
                CompilerLabs::Item tempItem;
                tempItem.type = CompilerLabs::ItemType::Invalid;

                // if requires comma
                if (commaIndex > 0 && lexlings[currentIndex].type == CompilerLabs::LexlingType::Comma) {
                    // next lexling index
                    currentIndex++;
                }

                // parse data
                if (lexlings[currentIndex].type == CompilerLabs::LexlingType::Null) {
                    // set type
                    tempItem.type = CompilerLabs::ItemType::Null;

                    // advance
                    currentIndex++;
                } else if (lexlings[currentIndex].type == CompilerLabs::LexlingType::False) {
                    // set type
                    tempItem.type = CompilerLabs::ItemType::False;
                    tempItem.booleanValue = false;

                    // advance
                    currentIndex++;
                } else if (lexlings[currentIndex].type == CompilerLabs::LexlingType::True) {
                    // set type
                    tempItem.type = CompilerLabs::ItemType::True;
                    tempItem.booleanValue = true;

                    // advance
                    currentIndex++;
                } else if (lexlings[currentIndex].type == CompilerLabs::LexlingType::String) {
                    // set type
                    tempItem.type = CompilerLabs::ItemType::String;
                    tempItem.stringValue = lexlings[currentIndex];

                    // advance
                    currentIndex++;
                } else if (lexlings[currentIndex].type == CompilerLabs::LexlingType::Number) {
                    // set type
                    tempItem.type = CompilerLabs::ItemType::Number;
                    tempItem.numberValue = lexlings[currentIndex];

                    // advance
                    currentIndex++;
                } else if (lexlings[currentIndex].type == CompilerLabs::LexlingType::LeftCurlyBracket) {
                    // set item
                    tempItem.type = CompilerLabs::ItemType::Object;
                    tempItem.objectBody = ParseObjectBody(log, lexlings, currentIndex);
                } else if (lexlings[currentIndex].type == CompilerLabs::LexlingType::LeftSquareBracket) {
                    // set item
                    tempItem.type = CompilerLabs::ItemType::Array;
                    tempItem.arrayBody = ParseArrayBody(log, lexlings, currentIndex);
                // error
                } else {
                    log->LogError("JSON Error. While parsing an array body, an invalid lexling appeared. Line Number: " + std::to_string(lexlings[currentIndex].location.lineNumber));

                    return output;
                }

                // append item to list
                output.push_back(tempItem);

                // check for errors
                if (log->normalErrorOccured) {
                    return output;
                }

                // next comma index
                commaIndex++;
            }

            // check for right square brace
            if (lexlings[currentIndex].type == CompilerLabs::LexlingType::RightSquareBracket) {
                currentIndex++;
            }

            return output;
        }

    public:
        // turn json root into json string
        std::string JSONToString(CompilerLabs::Root parsedJSON) {
            return JSONItemToString(parsedJSON.masterObject, false, 0) + "\n";
        }

    private:
        // stringify JSON
        std::string JSONItemToString(CompilerLabs::Item item, CompilerLabs::Boolean requiresName, CompilerLabs::TabDepth tabDepth) {
            std::string output = "";
            CompilerLabs::Tabs tabs = CompilerLabs::Tabs("\t", tabDepth);

            // add name if required
            if (requiresName) {
                output += tabs.GenerateTabs() + item.name.stringedValue + ": ";
            } else {
                output += tabs.GenerateTabs();
            }

            // append each item based on type
            if (item.type == CompilerLabs::ItemType::Object) {
                // append opening brace
                output += "{\n";

                // append sub-items
                for (CompilerLabs::Index i = 0; i < item.objectBody.size(); i++) {
                    // append sub-objects
                    output += JSONItemToString(item.objectBody[i], true, tabDepth + 1);

                    // append comma if necessary
                    if (i < item.objectBody.size() - 1) {
                        output += ",";
                    }

                    // next line
                    output += "\n";
                }

                // append closing brace
                output += tabs.GenerateTabs() + "}";
            } else if (item.type == CompilerLabs::ItemType::Array) {
                // append opening brace
                output += "[\n";

                // append sub-items
                for (CompilerLabs::Index i = 0; i < item.arrayBody.size(); i++) {
                    // append sub-objects
                    output += JSONItemToString(item.arrayBody[i], false, tabDepth + 1);

                    // append comma if necessary
                    if (i < item.arrayBody.size() - 1) {
                        output += ",";
                    }

                    // next line
                    output += "\n";
                }

                // append closing brace
                output += tabs.GenerateTabs() + "]";
            } else if (item.type == CompilerLabs::ItemType::False) {
                output += "false";
            } else if (item.type == CompilerLabs::ItemType::True) {
                output += + "true";
            } else if (item.type == CompilerLabs::ItemType::Null) {
                output += + "null";
            } else if (item.type == CompilerLabs::ItemType::String) {
                output += item.stringValue.stringedValue;
            } else if (item.type == CompilerLabs::ItemType::Number) {
                output += item.numberValue.stringedValue;
            } else {
                output += "[INVALID]";
            }

            return output;
        }
    };
}
