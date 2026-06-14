#include "Table.h"
#include <stdexcept>
#include <algorithm>

Table::Table() : name("") {}

Table::Table(const std::string& tableName, const std::vector<std::string>& columns) 
    : name(tableName), columnNames(columns) {
    for (size_t i = 0; i < columns.size(); ++i) {
        columnIndexMap[columns[i]] = i;
    }
}

void Table::rebuildPrimaryKeySet() {
    primaryKeys.clear();
    for (const auto& rec : records) {
        if (rec.size() > 0) {
            primaryKeys.insert(rec.getValue(0));
        }
    }
}

std::string Table::getName() const {
    return name;
}

const std::vector<std::string>& Table::getColumnNames() const {
    return columnNames;
}

int Table::getColumnIndex(const std::string& columnName) const {
    auto it = columnIndexMap.find(columnName);
    if (it != columnIndexMap.end()) {
        return static_cast<int>(it->second);
    }
    return -1;
}

std::string Table::getPrimaryKeyName() const {
    if (!columnNames.empty()) {
        return columnNames[0];
    }
    return "";
}

const std::vector<Record>& Table::getRecords() const {
    return records;
}

/**
 * @brief Inserts a Record into the table.
 * Validates column count and primary key uniqueness constraint.
 */
bool Table::insertRecord(const Record& record, std::string& errorMsg) {
    if (record.size() != columnNames.size()) {
        errorMsg = "Column count mismatch: Table '" + name + "' expects " + 
                   std::to_string(columnNames.size()) + " values, query provided " + 
                   std::to_string(record.size()) + ".";
        return false;
    }

    if (columnNames.empty()) {
        errorMsg = "Table has no columns defined.";
        return false;
    }

    // First column is the primary key
    std::string pkValue = record.getValue(0);
    if (primaryKeys.count(pkValue) > 0) {
        errorMsg = "Duplicate primary key error: Value '" + pkValue + 
                   "' already exists for primary key column '" + columnNames[0] + "'.";
        return false;
    }

    records.push_back(record);
    primaryKeys.insert(pkValue);
    return true;
}

std::vector<Record> Table::selectAll() const {
    return records;
}

/**
 * @brief Filters and retrieves records matching a WHERE clause condition.
 */
std::vector<Record> Table::selectWhere(const std::string& columnName, 
                                      const std::string& targetValue, 
                                      std::string& errorMsg) const {
    int colIdx = getColumnIndex(columnName);
    if (colIdx == -1) {
        errorMsg = "Column '" + columnName + "' does not exist in table '" + name + "'.";
        return {};
    }

    std::vector<Record> result;
    for (const auto& rec : records) {
        if (rec.getValue(colIdx) == targetValue) {
            result.push_back(rec);
        }
    }
    return result;
}

/**
 * @brief Updates records matching the WHERE filter.
 * Ensures that if the primary key is updated, it does not violate uniqueness.
 */
int Table::updateRecordsWhere(const std::string& updateColumn, 
                             const std::string& newValue, 
                             const std::string& whereColumn, 
                             const std::string& whereValue, 
                             std::string& errorMsg) {
    int updateColIdx = getColumnIndex(updateColumn);
    if (updateColIdx == -1) {
        errorMsg = "Update column '" + updateColumn + "' does not exist in table '" + name + "'.";
        return -1;
    }

    int whereColIdx = getColumnIndex(whereColumn);
    if (whereColIdx == -1) {
        errorMsg = "Where column '" + whereColumn + "' does not exist in table '" + name + "'.";
        return -1;
    }

    // Identify which records will be updated
    std::vector<size_t> indicesToUpdate;
    for (size_t i = 0; i < records.size(); ++i) {
        if (records[i].getValue(whereColIdx) == whereValue) {
            indicesToUpdate.push_back(i);
        }
    }

    if (indicesToUpdate.empty()) {
        return 0;
    }

    // Check primary key uniqueness if updating the primary key (index 0)
    if (updateColIdx == 0) {
        // If updating multiple records to the same primary key, it's an immediate conflict
        if (indicesToUpdate.size() > 1) {
            errorMsg = "Constraint violation: Updating multiple records to the same primary key value '" + 
                       newValue + "' is not allowed.";
            return -1;
        }

        // Single record update: check if new PK exists elsewhere in the table
        std::string currentPkVal = records[indicesToUpdate[0]].getValue(0);
        if (currentPkVal != newValue && primaryKeys.count(newValue) > 0) {
            errorMsg = "Duplicate primary key error: Value '" + newValue + 
                       "' already exists for primary key column '" + columnNames[0] + "'.";
            return -1;
        }
    }

    // Apply updates
    int updatedCount = 0;
    for (size_t idx : indicesToUpdate) {
        records[idx].setValue(updateColIdx, newValue);
        updatedCount++;
    }

    // Rebuild the primary key cache if index 0 was updated
    if (updateColIdx == 0) {
        rebuildPrimaryKeySet();
    }

    return updatedCount;
}

/**
 * @brief Deletes records matching the filter.
 */
int Table::deleteRecordsWhere(const std::string& columnName, 
                             const std::string& targetValue, 
                             std::string& errorMsg) {
    int colIdx = getColumnIndex(columnName);
    if (colIdx == -1) {
        errorMsg = "Column '" + columnName + "' does not exist in table '" + name + "'.";
        return -1;
    }

    std::vector<Record> keptRecords;
    int deletedCount = 0;
    for (const auto& rec : records) {
        if (rec.getValue(colIdx) == targetValue) {
            deletedCount++;
        } else {
            keptRecords.push_back(rec);
        }
    }

    if (deletedCount > 0) {
        records = std::move(keptRecords);
        rebuildPrimaryKeySet();
    }

    return deletedCount;
}

/**
 * @brief Direct addition bypasses check during loading file.
 */
void Table::forceAddRecord(const Record& record) {
    if (record.size() == columnNames.size() && !columnNames.empty()) {
        records.push_back(record);
        primaryKeys.insert(record.getValue(0));
    }
}
