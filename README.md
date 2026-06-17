# Mini Relational Database System

# Overview

Mini Relational Database System is a lightweight SQL-like database engine developed in C++. The system supports table creation, record insertion, querying, updating, deletion, and persistent storage using CSV files. The project demonstrates object-oriented design, file handling, query parsing, and core database management concepts.

---

## Technologies Used

- C++
- Object-Oriented Programming (OOP)
- STL
- File Handling
- Regular Expressions (Regex)
- CSV Storage

## 🚀 Key Features

* **Dynamic Table Creation**: Create custom tables with user-defined columns on the fly.
* **SQL Command Parsing**: Parse and execute core SQL-like operations: `CREATE TABLE`, `INSERT`, `SELECT`, `UPDATE`, `DELETE`, and `DROP TABLE`.
* **Primary Key Constraint Validation**: Ensures the first column acts as a primary key, automatically throwing validation errors on duplicate inserts/updates.
* **Persistent File I/O Engine**: Each table is mapped to a dedicated `.csv` file in the `data/` folder, which is auto-saved on edits and auto-loaded on engine startup.
* **Robust CSV Serializer**: Correctly handles cell values containing quotes or commas by escaping and wrapping tokens using double quotes.
* **Interactive CLI Terminal**: Features a welcoming command prompt with active execution timing down to the microsecond level.
* **Visual Grid Layout**: Automatically calculates columns width and formats queries into a aligned ASCII table border.

---
## Supported Commands

CREATE TABLE students(id,name,age)

INSERT INTO students VALUES(1,Sandeep,21)

SELECT * FROM students

UPDATE students SET age=22 WHERE id=1

DELETE FROM students WHERE id=1

DROP TABLE students

EXIT

## Project Structure

MiniDataBaseEngine/
│
├── src/
│   ├── main.cpp
│   ├── Database.cpp
│   ├── Database.h
│   ├── Table.cpp
│   ├── Table.h
│   ├── Record.cpp
│   ├── Record.h
│   ├── QueryParser.cpp
│   ├── QueryParser.h
│   ├── FileManager.cpp
│   └── FileManager.h
│
├── data/
│
├── screenshots/
│
├── README.md
└── .gitignore

## ⚙️ Compilation & Running

Make sure you have `g++` (GCC 8.0+) supporting C++17 installed.



## 🔍 Supported SQL-Like Dialect & Examples

Run the following commands sequentially in the prompt:

1. **Create Table**:
   ```sql
   CREATE TABLE students (id,name,age,cgpa)
   ```
2. **Insert Records**:
   ```sql
   INSERT INTO students VALUES (1,Sandeep,20,6.5)
   INSERT INTO students VALUES (2,Aditya,21,8.0)
   ```
3. **Primary Key Validation (Fails)**:
   ```sql
   INSERT INTO students VALUES (1,DuplicateID,22,9.0)
   ```
   *Expected Output: `Duplicate primary key error: Value '1' already exists...`*
4. **Select All Records**:
   ```sql
   SELECT * FROM students
   ```
5. **Filtered Selection**:
   ```sql
   SELECT * FROM students WHERE id=1
   ```
6. **Update Record**:
   ```sql
   UPDATE students SET cgpa=8.5 WHERE id=1
   ```
7. **Delete Record**:
   ```sql
   DELETE FROM students WHERE id=1
   ```
8. **Drop Table**:
   ```sql
   DROP TABLE students
   ```
9. **Exit Session**:
   ```sql
   EXIT
   ```

---

## 🧱 Architectural Components

Here's how data flows during a query execution:

```mermaid
sequenceDiagram
    participant CLI as CLI (main.cpp)
    participant DB as Database Engine (Database.cpp)
    participant QP as Query Parser (QueryParser.cpp)
    participant TB as Table Memory (Table.cpp)
    participant FM as File Manager (FileManager.cpp)

    CLI->>DB: executeQuery("INSERT INTO students VALUES (1, Sandeep, 20, 6.5)")
    DB->>QP: parse("INSERT INTO students VALUES...")
    QP-->>DB: return ParsedQuery (INSERT, tableName, values)
    DB->>TB: insertRecord(Record)
    Note over TB: Validates Column Count<br/>Validates Primary Key Uniqueness
    TB-->>DB: Success / Error
    DB->>FM: saveTable("data", Table)
    Note over FM: Writes to data/students.csv
    FM-->>DB: Save status
    DB-->>CLI: Return OK message + time elapsed




