#include "Record.h"
#include <sstream>

Record::Record() {}

Record::Record(const std::vector<std::string>& values) : values(values) {}

const std::vector<std::string>& Record::getValues() const {
    return values;
}

std::string Record::getValue(size_t index) const {
    if (index < values.size()) {
        return values[index];
    }
    return "";
}

void Record::setValue(size_t index, const std::string& value) {
    if (index < values.size()) {
        values[index] = value;
    }
}

size_t Record::size() const {
    return values.size();
}

/**
 * @brief Serializes a record to a delimited string.
 * Supports standard CSV escaping (wrapping in double quotes and escaping inner quotes).
 */
std::string Record::serialize(char delimiter) const {
    std::string result = "";
    for (size_t i = 0; i < values.size(); ++i) {
        std::string val = values[i];
        bool needsQuotes = false;
        
        // Check if value contains special chars needing wrapping
        if (val.find(delimiter) != std::string::npos || 
            val.find('"') != std::string::npos || 
            val.find('\n') != std::string::npos || 
            val.find('\r') != std::string::npos) {
            needsQuotes = true;
        }
        
        if (needsQuotes) {
            result += "\"";
            for (char c : val) {
                if (c == '"') {
                    result += "\"\""; // Standard CSV quote escape
                } else {
                    result += c;
                }
            }
            result += "\"";
        } else {
            result += val;
        }
        
        if (i < values.size() - 1) {
            result += delimiter;
        }
    }
    return result;
}

/**
 * @brief Parses a CSV string line into a Record.
 * Handles double-quoted cells containing delimiters and escaped quotes.
 */
Record Record::deserialize(const std::string& csvLine, char delimiter) {
    std::vector<std::string> parsedValues;
    std::string currentField = "";
    bool inQuotes = false;
    
    for (size_t i = 0; i < csvLine.size(); ++i) {
        char c = csvLine[i];
        if (inQuotes) {
            if (c == '"') {
                // Look ahead to check for escaped double quotes
                if (i + 1 < csvLine.size() && csvLine[i + 1] == '"') {
                    currentField += '"';
                    i++; // Skip the next quote
                } else {
                    inQuotes = false; // Closing quote
                }
            } else {
                currentField += c;
            }
        } else {
            if (c == '"') {
                inQuotes = true;
            } else if (c == delimiter) {
                parsedValues.push_back(currentField);
                currentField = "";
            } else if (c == '\r' || c == '\n') {
                // Discard line endings
                continue;
            } else {
                currentField += c;
            }
        }
    }
    // Push the remaining field
    parsedValues.push_back(currentField);
    
    return Record(parsedValues);
}
