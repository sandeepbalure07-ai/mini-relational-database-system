#ifndef TABLE_H
#define TABLE_H

#include "Record.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>

/**
 * @class Table
 * @brief Represents a database table containing schema and records.
 * 
 * Provides structural definitions (columns), row-level operations,
 * and maintains primary key uniqueness constraint validation.
 */
class Table {
private:
    std::string name;
    std::vector<std::string> columnNames;
    std::unordered_map<std::string, size_t> columnIndexMap;
    std::vector<Record> records;
    
    // Quick lookup for primary key constraints (first column is the primary key)
    std::unordered_set<std::string> primaryKeys;

    void rebuildPrimaryKeySet();

public:
    Table();
    Table(const std::string& tableName, const std::vector<std::string>& columns);

    // Schema Getters
    std::string getName() const;
    const std::vector<std::string>& getColumnNames() const;
    int getColumnIndex(const std::string& columnName) const;
    std::string getPrimaryKeyName() const;

    // Record Getters & Operations
    const std::vector<Record>& getRecords() const;
    
    // Core CRUD
    bool insertRecord(const Record& record, std::string& errorMsg);
    
    std::vector<Record> selectAll() const;
    
    std::vector<Record> selectWhere(const std::string& columnName, 
                                    const std::string& targetValue, 
                                    std::string& errorMsg) const;
                                    
    int updateRecordsWhere(const std::string& updateColumn, 
                           const std::string& newValue, 
                           const std::string& whereColumn, 
                           const std::string& whereValue, 
                           std::string& errorMsg);
                           
    int deleteRecordsWhere(const std::string& columnName, 
                           const std::string& targetValue, 
                           std::string& errorMsg);

    // Helpers for file loading
    void forceAddRecord(const Record& record);
};

#endif // TABLE_H
