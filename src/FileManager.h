#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "Table.h"
#include <string>
#include <vector>

/**
 * @class FileManager
 * @brief Handles I/O operations for persisting database tables to files.
 * 
 * Responsible for saving tables to CSV-like formatted files, reading them
 * back to restore state, and discovering existing tables in the data directory.
 */
class FileManager {
public:
    // Saves a single table to data/<tableName>.csv
    static bool saveTable(const std::string& directory, const Table& table);

    // Loads a table schema and contents from data/<tableName>.csv
    static bool loadTable(const std::string& directory, const std::string& tableName, Table& table);

    // Deletes the file associated with a dropped table
    static bool deleteTableFile(const std::string& directory, const std::string& tableName);

    // Discovers and returns all table names (files ending in .csv) in data/
    static std::vector<std::string> discoverTables(const std::string& directory);
};

#endif // FILEMANAGER_H
