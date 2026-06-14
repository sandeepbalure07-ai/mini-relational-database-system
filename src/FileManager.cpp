#include "FileManager.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

/**
 * @brief Saves a Table object to disk under directory/<tableName>.csv.
 * Creates directories if they do not exist.
 */
bool FileManager::saveTable(const std::string& directory, const Table& table) {
    try {
        fs::create_directories(directory);
        std::string filePath = directory + "/" + table.getName() + ".csv";
        
        std::ofstream outFile(filePath, std::ios::out | std::ios::trunc);
        if (!outFile.is_open()) {
            return false;
        }

        // Write header (column names)
        const auto& columns = table.getColumnNames();
        for (size_t i = 0; i < columns.size(); ++i) {
            // Apply standard serialisation rules even to column names (e.g. if they contain commas)
            std::string col = columns[i];
            bool needsQuotes = (col.find(',') != std::string::npos || col.find('"') != std::string::npos);
            if (needsQuotes) {
                outFile << "\"";
                for (char c : col) {
                    if (c == '"') outFile << "\"\"";
                    else outFile << c;
                }
                outFile << "\"";
            } else {
                outFile << col;
            }
            
            if (i < columns.size() - 1) {
                outFile << ",";
            }
        }
        outFile << "\n";

        // Write data records
        for (const auto& rec : table.getRecords()) {
            outFile << rec.serialize(',') << "\n";
        }

        outFile.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[FileManager Error] Failed to save table '" << table.getName() 
                  << "': " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief Loads a Table object from disk.
 * Restores schema and populates records.
 */
bool FileManager::loadTable(const std::string& directory, const std::string& tableName, Table& table) {
    try {
        std::string filePath = directory + "/" + tableName + ".csv";
        if (!fs::exists(filePath)) {
            return false;
        }

        std::ifstream inFile(filePath);
        if (!inFile.is_open()) {
            return false;
        }

        std::string line;
        // Read columns header line
        if (!std::getline(inFile, line)) {
            inFile.close();
            return false;
        }

        Record headerRec = Record::deserialize(line, ',');
        std::vector<std::string> columns = headerRec.getValues();

        // Instantiate Table with discovered schema
        table = Table(tableName, columns);

        // Read all record lines
        while (std::getline(inFile, line)) {
            // Skip empty rows (e.g. trailing empty line)
            if (line.empty() || std::all_of(line.begin(), line.end(), ::isspace)) {
                continue;
            }
            Record rec = Record::deserialize(line, ',');
            table.forceAddRecord(rec);
        }

        inFile.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[FileManager Error] Failed to load table '" << tableName 
                  << "': " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief Removes the CSV file for a dropped table.
 */
bool FileManager::deleteTableFile(const std::string& directory, const std::string& tableName) {
    try {
        std::string filePath = directory + "/" + tableName + ".csv";
        if (fs::exists(filePath)) {
            return fs::remove(filePath);
        }
        return true; // Already gone
    } catch (const std::exception& e) {
        std::cerr << "[FileManager Error] Failed to delete file for '" << tableName 
                  << "': " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief Scans the data directory and returns names of all tables (.csv files).
 */
std::vector<std::string> FileManager::discoverTables(const std::string& directory) {
    std::vector<std::string> tableNames;
    try {
        if (!fs::exists(directory) || !fs::is_directory(directory)) {
            return tableNames;
        }

        for (const auto& entry : fs::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                auto path = entry.path();
                if (path.extension() == ".csv") {
                    tableNames.push_back(path.stem().string());
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[FileManager Error] Error scanning directory '" << directory 
                  << "': " << e.what() << std::endl;
    }
    return tableNames;
}
