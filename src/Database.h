#ifndef DATABASE_H
#define DATABASE_H

#include "Table.h"
#include <unordered_map>
#include <string>
#include <vector>

/**
 * @class Database
 * @brief Coordinates tables, manages execution dispatch, and syncs data via FileManager.
 * 
 * Provides the top-level interface for running SQL-like queries, loading tables on startup,
 * and pretty-printing query results.
 */
class Database {
private:
    std::string dataDir;
    std::unordered_map<std::string, Table> tables;

    // Helper to format table rows as a text grid
    std::string formatTableGrid(const std::vector<std::string>& headers, 
                                 const std::vector<Record>& records) const;

public:
    explicit Database(const std::string& directory = "data");

    // Scans directory and loads tables into memory
    void loadAllTables();

    // Executes queries and returns execution output
    bool executeQuery(const std::string& queryStr, std::string& outputMsg);
    
    // Check if table exists
    bool hasTable(const std::string& tableName) const;
};

#endif // DATABASE_H
