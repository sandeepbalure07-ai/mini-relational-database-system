#include "Database.h"
#include <iostream>
#include <string>
#include <chrono>

/**
 * @brief Main function. Launches the interactive command line query interface.
 */
int main() {
    // Initialise the Database Manager with a custom storage path
    Database db("data");
    
    // Auto-load any existing tables on disk
    db.loadAllTables();

    // Welcome Screen Visuals
    std::cout << "===============================================================\n";
    std::cout << "                  MINI DATABASE ENGINE v1.0                    \n";
    std::cout << "       An SDE-Grade relational C++ DBMS built from scratch     \n";
    std::cout << "===============================================================\n";
    std::cout << "  Supports CREATE TABLE, INSERT INTO, SELECT, UPDATE, DELETE,  \n";
    std::cout << "  and DROP TABLE. Primary key constraints are active.\n";
    std::cout << "  Type 'EXIT' to terminate. All queries run in-memory and sync.\n";
    std::cout << "===============================================================\n\n";

    std::string query;
    while (true) {
        std::cout << "mini_db> ";
        std::string line;
        if (!std::getline(std::cin, line)) {
            break; // Handle EOF (Ctrl+Z or Ctrl+D)
        }

        // Clean query line
        // Trim leading space
        size_t start = line.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) {
            continue; // Empty query, repeat prompt
        }
        
        // Check if query is EXIT immediately
        std::string queryClean = line.substr(start);
        
        std::string outputMsg;
        
        // Track execution duration using high-resolution chrono timers
        auto startTime = std::chrono::high_resolution_clock::now();
        
        bool success = db.executeQuery(queryClean, outputMsg);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

        // Print execution results
        std::cout << outputMsg << "\n";
        
        if (success) {
            double ms = duration / 1000.0;
            std::cout << "Execution time: " << ms << " ms (" << duration << " us)\n\n";
        } else {
            std::cout << "Query execution failed.\n\n";
        }

        // Graceful loop break if EXIT was matched
        // Note: QueryParser returns success for EXIT
        if (queryClean.size() >= 4) {
            std::string upperQuery = "";
            for (int i = 0; i < 4; ++i) upperQuery += std::toupper(queryClean[i]);
            if (upperQuery == "EXIT") {
                break;
            }
        }
    }

    return 0;
}
