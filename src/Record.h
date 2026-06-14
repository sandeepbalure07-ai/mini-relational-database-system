#ifndef RECORD_H
#define RECORD_H

#include <vector>
#include <string>

/**
 * @class Record
 * @brief Represents a single row in a database table.
 * 
 * Contains a dynamic array of string values representing the columns of a single row.
 * Includes helpers for value access, size, and serialization.
 */
class Record {
private:
    std::vector<std::string> values;

public:
    // Constructors
    Record();
    explicit Record(const std::vector<std::string>& values);

    // Getters and Setters
    const std::vector<std::string>& getValues() const;
    std::string getValue(size_t index) const;
    void setValue(size_t index, const std::string& value);
    
    // Helpers
    size_t size() const;
    
    // Serialization helpers
    std::string serialize(char delimiter = ',') const;
    static Record deserialize(const std::string& csvLine, char delimiter = ',');
};

#endif // RECORD_H
