#ifndef QUERYPARSER_H
#define QUERYPARSER_H

#include <string>
#include <vector>

/**
 * @enum QueryType
 * @brief Categorizes different SQL-like queries supported by the database engine.
 */
enum class QueryType {
    CREATE_TABLE,
    INSERT,
    SELECT_ALL,
    SELECT_WHERE,
    UPDATE,
    DELETE_WHERE,
    DROP_TABLE,
    EXIT,
    INVALID
};

/**
 * @struct ParsedQuery
 * @brief Structured representation of a parsed user query.
 */
struct ParsedQuery {
    QueryType type = QueryType::INVALID;
    std::string tableName = "";
    std::vector<std::string> columns = {}; // Used for CREATE TABLE
    std::vector<std::string> values = {};  // Used for INSERT
    std::string updateColumn = "";         // Used for UPDATE SET
    std::string updateValue = "";          // Used for UPDATE SET
    std::string whereColumn = "";          // Used for WHERE clause filter
    std::string whereValue = "";           // Used for WHERE clause value
    std::string rawQuery = "";
    std::string errorMessage = "";
};

/**
 * @class QueryParser
 * @brief Lexes and parses SQL-like query strings into ParsedQuery structs.
 */
class QueryParser {
private:
    static std::string trim(const std::string& str);
    static std::string stripQuotes(const std::string& str);
    static std::vector<std::string> splitCSV(const std::string& csvStr);

public:
    static ParsedQuery parse(const std::string& queryStr);
};

#endif // QUERYPARSER_H
