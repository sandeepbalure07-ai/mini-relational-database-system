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

- Table Creation and Schema Management
- SQL-like Query Parsing
- CRUD Operations
- CSV-based Persistent Storage
- Primary Key Validation
- Object-Oriented Design
---

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

## How to Run

Compile: g++ -std=c++17 src/*.cpp -o mini_db | Run: .\mini_db.exe

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








