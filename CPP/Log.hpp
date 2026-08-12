#pragma once

#include "Basic.hpp"
#include <initializer_list>
#include <string>
#include <vector>
#include <iostream>

namespace CompilerLabs {
    // log entry type
    enum class EntryType {
        Note,
        Warning,
        Error,
        CriticalError,
    };

    // extra data pertaining to the note
    class LogEntrySubdata {
    public:
        std::string name;
        std::string value;

        // constructors
        LogEntrySubdata() {
            name = "[INVLAID]";
            value = "[INVALID]";
        }
        LogEntrySubdata(std::string _name, std::string _value) {
            name = _name;
            value = _value;
        }

        // to string
        std::string ToString() {
            return "\"" + name + "\": \"" + value + "\"";
        }
    };

    // one log entry
    class LogEntry {
    public:
        CompilerLabs::EntryType type;
        std::string message;
        std::vector<CompilerLabs::LogEntrySubdata> extras;

        // constructors
        LogEntry() {
            message = "(No message.)";
            type = CompilerLabs::EntryType::Note;
        }

        // print entry
        std::string ToString(CompilerLabs::Tabs tabs) {
            // give correct type string
            std::string entryTypeString;

            // check type
            switch (type) {
            case CompilerLabs::EntryType::Note:
                entryTypeString = "Note";
                break;
            case CompilerLabs::EntryType::Warning:
                entryTypeString = "Warning";
                break;
            case CompilerLabs::EntryType::Error:
                entryTypeString = "Error";
                break;
            case CompilerLabs::EntryType::CriticalError:
                entryTypeString = "CriticalError";
                break;
            default:
                entryTypeString = "InvalidLogEntryType";
                break;
            }

            // create string output
            // generate tabs
            std::string output = tabs.GenerateTabs();

            // add basics
            output += "{ \"type\": \"" + entryTypeString + "\", \"message\": \"" + message + "\"";

            // add extras
            for (auto extra : extras) {
                output += ", " + extra.ToString();
            }

            // return data after adding closing bracket
            return output + " }";
        }
    };

    // application wide log
    class Log {
        CompilerLabs::Tabs tabs;

    public:
        std::vector<CompilerLabs::LogEntry> entries;
        CompilerLabs::Tabs tabManager;
        CompilerLabs::Boolean loggingEnabled;
        CompilerLabs::Boolean printLogLive;
        CompilerLabs::Boolean saveLogToJSONFile;
        CompilerLabs::Boolean normalErrorOccured;
        CompilerLabs::Boolean criticalErrorOccured;

        // constructors
        Log() {
            tabs = CompilerLabs::Tabs(1);
            loggingEnabled = true;
            printLogLive = true;
            saveLogToJSONFile = false;
            normalErrorOccured = false;
            criticalErrorOccured = false;
        }
        Log(CompilerLabs::Boolean _loggingEnabled, CompilerLabs::Boolean _printLogLive, CompilerLabs::Boolean _saveLogToJSONFile, CompilerLabs::Boolean _normalErrorOccured, CompilerLabs::Boolean _criticalErrorOccured) {
            tabs = CompilerLabs::Tabs(1);
            loggingEnabled = _loggingEnabled;
            printLogLive = _printLogLive;
            saveLogToJSONFile = _saveLogToJSONFile;
            normalErrorOccured = _normalErrorOccured;
            criticalErrorOccured = _criticalErrorOccured;
        }

        // add any type of entry to log
        void LogAny(CompilerLabs::EntryType type, std::string message, CompilerLabs::Boolean isNormalError, CompilerLabs::Boolean isCriticalError, std::initializer_list<CompilerLabs::LogEntrySubdata> extras) {
            // check for logging enabled or not
            if (loggingEnabled) {
                // create log entry
                CompilerLabs::LogEntry entry;
                entry.type = type;
                entry.message = message;
                normalErrorOccured = isNormalError;
                criticalErrorOccured = isCriticalError;
                
                // append extras
                for (auto extra : extras) {
                    entry.extras.push_back(extra);
                }

                // log data
                entries.push_back(entry);

                // if printing log live
                if (printLogLive) {
                    std::string colorValue;

                    // select color based on entry type
                    switch (entry.type) {
                    case CompilerLabs::EntryType::Note:
                        colorValue = "\033[32m";

                        break;
                    case CompilerLabs::EntryType::Warning:
                        colorValue = "\033[33m";

                        break;
                    case CompilerLabs::EntryType::Error:
                        colorValue = "\033[1;31m";

                        break;
                    case CompilerLabs::EntryType::CriticalError:
                        colorValue = "\033[4;31m";

                        break;
                    default:
                        colorValue = "";

                        break;
                    }

                    // print entry
                    std::cout << colorValue << entry.ToString(tabs.tabDepth) << "\033[0m" << std::endl;
                }
            }
        }

        // add note to log
        void LogNote(std::string message, std::initializer_list<CompilerLabs::LogEntrySubdata> extras) {
            // log
            LogAny(CompilerLabs::EntryType::Note, message, false, false, extras);
        }
        void LogNote(std::string message) {
            // log
            LogAny(CompilerLabs::EntryType::Note, message, false, false, {});
        }

        // add warning to log
        void LogWarning(std::string message, std::initializer_list<CompilerLabs::LogEntrySubdata> extras) {
            // log
            LogAny(CompilerLabs::EntryType::Warning, message, false, false, extras);
        }
        void LogWarning(std::string message) {
            // log
            LogAny(CompilerLabs::EntryType::Warning, message, false, false, {});
        }

        // add error to log
        void LogError(std::string message, std::initializer_list<CompilerLabs::LogEntrySubdata> extras) {
            // log
            LogAny(CompilerLabs::EntryType::Error, message, true, false, extras);
        }
        void LogError(std::string message) {
            // log
            LogAny(CompilerLabs::EntryType::Error, message, true, false, {});
        }

        // add critical error to log
        void LogCriticalError(std::string message, std::initializer_list<CompilerLabs::LogEntrySubdata> extras) {
            // log
            LogAny(CompilerLabs::EntryType::CriticalError, message, false, true, extras);
        }
        void LogCriticalError(std::string message) {
            // log
            LogAny(CompilerLabs::EntryType::CriticalError, message, false, true, {});
        }

        // check any error occured
        CompilerLabs::Boolean CheckAnyErrorOccured() {
            return normalErrorOccured || criticalErrorOccured;
        }

        // turn entire log into json string
        std::string ToString() {
            // header
            std::string output = "\"Log\": [\n";

            // append each entry
            for (auto entry : entries) {
                // append entry
                output += entry.ToString(tabs.tabDepth) + "\n";
            }

            // footer
            return output + "]\n";
        }
    };
}
