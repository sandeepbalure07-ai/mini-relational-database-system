#include "QueryParser.h"
#include <regex>
#include <sstream>
#include <algorithm>
#include <iostream>

std::string QueryParser::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

std::string QueryParser::stripQuotes(const std::string& str) {
    std::string s = trim(str);
    if (s.size() >= 2) {
        if ((s.front() == '"' && s.back() == '"') || 
            (s.front() == '\'' && s.back() == '\'')) {
            return s.substr(1, s.size() - 2);
        }
    }
    return s;
}

/**
 * @brief Splits a comma-separated values string.
 * Accounts for quoted strings that may contain nested commas.
 */
std::vector<std::string> QueryParser::splitCSV(const std::string& csvStr) {
    std::vector<std::string> result;
    std::string current = "";
    bool inQuotes = false;
    
    for (size_t i = 0; i < csvStr.size(); ++i) {
        char c = csvStr[i];
        if (inQuotes) {
            if (c == '"') {
                // Escaped double quotes
                if (i + 1 < csvStr.size() && csvStr[i + 1] == '"') {
                    current += '"';
                    i++;
                } else {
                    inQuotes = false;
                }
            } else {
                current += c;
            }
        } else {
            if (c == '"') {
                inQuotes = true;
            } else if (c == ',') {
                result.push_back(stripQuotes(current));
                current = "";
            } else {
                current += c;
            }
        }
    }
    result.push_back(stripQuotes(current));
    
    // Clean up empty fields if they were just whitespace
    for (auto& item : result) {
        item = trim(item);
    }
    
    return result;
}

/**
 * @brief Parses a query string into a ParsedQuery structure.
 * Employs case-insensitive regular expressions for SQL matching.
 */
ParsedQuery QueryParser::parse(const std::string& queryStr) {
    ParsedQuery pq;
    pq.rawQuery = queryStr;
    std::string trimmedQuery = trim(queryStr);
    if (!trimmedQuery.empty() && trimmedQuery.back() == ';')
{
    trimmedQuery.pop_back();
}
    
    // Define case-insensitive regex patterns for each command type
    std::regex createRegex(R"(^\s*CREATE\s+TABLE\s+(\w+)\s*\(([^)]+)\)\s*$)", std::regex::icase);
    std::regex insertRegex(R"(^\s*INSERT\s+INTO\s+(\w+)\s+VALUES\s*\(([^)]+)\)\s*$)", std::regex::icase);
    std::regex selectWhereRegex(R"(^\s*SELECT\s+\*\s+FROM\s+(\w+)\s+WHERE\s+(\w+)\s*=\s*(.+?)\s*$)", std::regex::icase);
    std::regex selectAllRegex(R"(^\s*SELECT\s+\*\s+FROM\s+(\w+)\s*$)", std::regex::icase);
    std::regex updateRegex(R"(^\s*UPDATE\s+(\w+)\s+SET\s+(\w+)\s*=\s*(.+?)\s+WHERE\s+(\w+)\s*=\s*(.+?)\s*$)", std::regex::icase);
    std::regex deleteRegex(R"(^\s*DELETE\s+FROM\s+(\w+)\s+WHERE\s+(\w+)\s*=\s*(.+?)\s*$)", std::regex::icase);
    std::regex dropRegex(R"(^\s*DROP\s+TABLE\s+(\w+)\s*$)", std::regex::icase);
    std::regex exitRegex(R"(^\s*EXIT\s*$)", std::regex::icase);

    std::smatch match;

    if (std::regex_match(trimmedQuery, match, createRegex)) {
        pq.type = QueryType::CREATE_TABLE;
        pq.tableName = match[1].str();
        pq.columns = splitCSV(match[2].str());
        
        // Validation: Verify there are columns
        if (pq.columns.empty() || (pq.columns.size() == 1 && pq.columns[0].empty())) {
            pq.type = QueryType::INVALID;
            pq.errorMessage = "Syntax Error: CREATE TABLE requires at least one column definition.";
        }
    } 
    else if (std::regex_match(trimmedQuery, match, insertRegex)) {
        pq.type = QueryType::INSERT;
        pq.tableName = match[1].str();
        pq.values = splitCSV(match[2].str());
    } 
    else if (std::regex_match(trimmedQuery, match, selectWhereRegex)) {
        pq.type = QueryType::SELECT_WHERE;
        pq.tableName = match[1].str();
        pq.whereColumn = match[2].str();
        pq.whereValue = stripQuotes(match[3].str());
    } 
    else if (std::regex_match(trimmedQuery, match, selectAllRegex)) {
        pq.type = QueryType::SELECT_ALL;
        pq.tableName = match[1].str();
    } 
    else if (std::regex_match(trimmedQuery, match, updateRegex)) {
        pq.type = QueryType::UPDATE;
        pq.tableName = match[1].str();
        pq.updateColumn = match[2].str();
        pq.updateValue = stripQuotes(match[3].str());
        pq.whereColumn = match[4].str();
        pq.whereValue = stripQuotes(match[5].str());
    } 
    else if (std::regex_match(trimmedQuery, match, deleteRegex)) {
        pq.type = QueryType::DELETE_WHERE;
        pq.tableName = match[1].str();
        pq.whereColumn = match[2].str();
        pq.whereValue = stripQuotes(match[3].str());
    } 
    else if (std::regex_match(trimmedQuery, match, dropRegex)) {
        pq.type = QueryType::DROP_TABLE;
        pq.tableName = match[1].str();
    } 
    else if (std::regex_match(trimmedQuery, match, exitRegex)) {
        pq.type = QueryType::EXIT;
    } 
    else {
        pq.type = QueryType::INVALID;
        pq.errorMessage = "Syntax Error: Command unrecognized. Please verify syntax.";
    }

    return pq;
}
