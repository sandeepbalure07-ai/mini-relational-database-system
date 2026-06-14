#include "Database.h"
#include "QueryParser.h"
#include "FileManager.h"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <sstream>

// Helper to convert a string to lowercase for case-insensitive lookup
static std::string toLowerCase(const std::string& str) {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return lower;
}

Database::Database(const std::string& directory) : dataDir(directory) {}

/**
 * @brief Discovers and loads all existing tables from disk upon startup.
 */
void Database::loadAllTables() {
    std::vector<std::string> discovered = FileManager::discoverTables(dataDir);
    for (const auto& name : discovered) {
        Table table;
        if (FileManager::loadTable(dataDir, name, table)) {
            tables[toLowerCase(name)] = table;
        }
    }
}

bool Database::hasTable(const std::string& tableName) const {
    return tables.find(toLowerCase(tableName)) != tables.end();
}

/**
 * @brief Parses and executes user query strings, updating state and disk.
 * @param queryStr The SQL-like string to parse.
 * @param outputMsg Out-parameter to contain execution reports or error logs.
 * @return True if query execution succeeded; false if syntax or validation failed.
 */
bool Database::executeQuery(const std::string& queryStr, std::string& outputMsg) {
    ParsedQuery pq = QueryParser::parse(queryStr);
    
    if (pq.type == QueryType::INVALID) {
        outputMsg = pq.errorMessage;
        return false;
    }

    if (pq.type == QueryType::EXIT) {
        outputMsg = "Goodbye!";
        return true;
    }

    std::string key = toLowerCase(pq.tableName);

    switch (pq.type) {
        case QueryType::CREATE_TABLE: {
            if (hasTable(key)) {
                outputMsg = "Error: Table '" + pq.tableName + "' already exists.";
                return false;
            }
            
            // Instantiating a new table
            Table newTable(pq.tableName, pq.columns);
            
            // Persist schema to file
            if (!FileManager::saveTable(dataDir, newTable)) {
                outputMsg = "Error: Failed to write table file to disk.";
                return false;
            }
            
            tables[key] = newTable;
            outputMsg = "Table '" + pq.tableName + "' created successfully.";
            return true;
        }

        case QueryType::DROP_TABLE: {
            if (!hasTable(key)) {
                outputMsg = "Error: Table '" + pq.tableName + "' does not exist.";
                return false;
            }
            
            // Remove file
            FileManager::deleteTableFile(dataDir, tables[key].getName());
            
            // Remove from memory
            tables.erase(key);
            outputMsg = "Table '" + pq.tableName + "' dropped successfully.";
            return true;
        }

        case QueryType::INSERT: {
            if (!hasTable(key)) {
                outputMsg = "Error: Table '" + pq.tableName + "' does not exist.";
                return false;
            }
            
            Table& targetTable = tables[key];
            Record newRecord(pq.values);
            
            std::string err;
            if (!targetTable.insertRecord(newRecord, err)) {
                outputMsg = err;
                return false;
            }
            
            // Persist modification
            if (!FileManager::saveTable(dataDir, targetTable)) {
                outputMsg = "Error: Failed to write table updates to disk.";
                return false;
            }
            
            outputMsg = "Query OK: 1 record inserted.";
            return true;
        }

        case QueryType::SELECT_ALL: {
            if (!hasTable(key)) {
                outputMsg = "Error: Table '" + pq.tableName + "' does not exist.";
                return false;
            }
            
            const Table& targetTable = tables[key];
            std::vector<Record> records = targetTable.selectAll();
            outputMsg = formatTableGrid(targetTable.getColumnNames(), records);
            return true;
        }

        case QueryType::SELECT_WHERE: {
            if (!hasTable(key)) {
                outputMsg = "Error: Table '" + pq.tableName + "' does not exist.";
                return false;
            }
            
            const Table& targetTable = tables[key];
            std::string err;
            std::vector<Record> records = targetTable.selectWhere(pq.whereColumn, pq.whereValue, err);
            
            if (!err.empty()) {
                outputMsg = "Error: " + err;
                return false;
            }
            
            outputMsg = formatTableGrid(targetTable.getColumnNames(), records);
            return true;
        }

        case QueryType::UPDATE: {
            if (!hasTable(key)) {
                outputMsg = "Error: Table '" + pq.tableName + "' does not exist.";
                return false;
            }
            
            Table& targetTable = tables[key];
            std::string err;
            int count = targetTable.updateRecordsWhere(pq.updateColumn, pq.updateValue, 
                                                       pq.whereColumn, pq.whereValue, err);
            if (count == -1) {
                outputMsg = "Error: " + err;
                return false;
            }
            
            if (count > 0) {
                if (!FileManager::saveTable(dataDir, targetTable)) {
                    outputMsg = "Error: Failed to write table updates to disk.";
                    return false;
                }
            }
            
            outputMsg = "Query OK: " + std::to_string(count) + " record(s) updated.";
            return true;
        }

        case QueryType::DELETE_WHERE: {
            if (!hasTable(key)) {
                outputMsg = "Error: Table '" + pq.tableName + "' does not exist.";
                return false;
            }
            
            Table& targetTable = tables[key];
            std::string err;
            int count = targetTable.deleteRecordsWhere(pq.whereColumn, pq.whereValue, err);
            if (count == -1) {
                outputMsg = "Error: " + err;
                return false;
            }
            
            if (count > 0) {
                if (!FileManager::saveTable(dataDir, targetTable)) {
                    outputMsg = "Error: Failed to write table updates to disk.";
                    return false;
                }
            }
            
            outputMsg = "Query OK: " + std::to_string(count) + " record(s) deleted.";
            return true;
        }

        default:
            outputMsg = "Error: Execution case unhandled.";
            return false;
    }
}

/**
 * @brief Pretty prints the schema and records in a visual ASCII grid.
 */
std::string Database::formatTableGrid(const std::vector<std::string>& headers, 
                                     const std::vector<Record>& records) const {
    if (headers.empty()) {
        return "Empty schema.";
    }

    size_t numCols = headers.size();
    std::vector<size_t> colWidths(numCols, 0);

    // Initialise widths based on headers
    for (size_t i = 0; i < numCols; ++i) {
        colWidths[i] = headers[i].size();
    }

    // Adjust widths based on records content
    for (const auto& rec : records) {
        for (size_t i = 0; i < numCols; ++i) {
            std::string val = rec.getValue(i);
            if (val.size() > colWidths[i]) {
                colWidths[i] = val.size();
            }
        }
    }

    // Build the ASCII grid borders and cell content
    std::stringstream ss;
    
    // Top border helper
    auto printBorder = [&]() {
        ss << "+";
        for (size_t i = 0; i < numCols; ++i) {
            ss << std::string(colWidths[i] + 2, '-') << "+";
        }
        ss << "\n";
    };

    printBorder();

    // Headers line
    ss << "|";
    for (size_t i = 0; i < numCols; ++i) {
        ss << " " << std::left << std::setw(colWidths[i]) << headers[i] << " |";
    }
    ss << "\n";

    printBorder();

    // Data lines
    if (records.empty()) {
        ss << "|";
        size_t totalWidth = 0;
        for (size_t w : colWidths) totalWidth += w + 2;
        totalWidth += (numCols - 1); // add spacing inside bars
        
        std::string emptyMsg = "No records found";
        if (emptyMsg.size() > totalWidth) {
            emptyMsg = emptyMsg.substr(0, totalWidth);
        }
        
        size_t padding = (totalWidth - emptyMsg.size()) / 2;
        ss << std::string(padding, ' ') << emptyMsg 
           << std::string(totalWidth - emptyMsg.size() - padding, ' ') << "|\n";
        printBorder();
    } else {
        for (const auto& rec : records) {
            ss << "|";
            for (size_t i = 0; i < numCols; ++i) {
                ss << " " << std::left << std::setw(colWidths[i]) << rec.getValue(i) << " |";
            }
            ss << "\n";
        }
        printBorder();
        ss << records.size() << " rows in set.\n";
    }

    return ss.str();
}
