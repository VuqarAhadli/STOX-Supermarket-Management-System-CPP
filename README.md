STOX Supermarket Management System
==================================

A console-based C++ application demonstrating a complete supermarket workflow using object-oriented programming. 
The system manages product inventory, employees, transactions, barcode generation (Code39), and CSV-based data storage.
It uses inheritance, polymorphism, abstract classes, and custom exceptions to structure the application.

CORE COMPONENTS
---------------

PRODUCT MODULE
- Product creation with auto-increment IDs
- Stock update and validation
- Code39 barcode generation in PBM format
- CSV serialisation/deserialisation

EMPLOYEE MODULE
- Manager and Cashier classes derived from Employee
- Abstract salary calculation method
- Cashier transaction counter affecting salary

TRANSACTION MODULE
- Item list with quantity validation and stock deduction
- Transaction queue for history
- Receipt generation as formatted text output
- Timestamping using <ctime> utilities

ERROR HANDLING
Custom exceptions:
- InvalidPriceException
- InvalidQuantityException
- InsufficientStockException
- ProductNotFoundException
- FileOperationException

BUILD INSTRUCTIONS
------------------
Compile with any modern C++ compiler:

    g++ STOX.cpp -o STOX
    ./STOX

DATA FILES
----------
The system automatically creates:

products.csv    Product storage
employees.csv   Employee storage
receipt_*.txt   Generated receipts
*.pbm           Generated barcodes

TECHNICAL HIGHLIGHTS
--------------------
- Uses STL containers: map, vector, queue
- Applies RAII for cleanup in destructors
- Uses ANSI escape codes for terminal styling
- Implements polymorphic behavior for salary calculation
- Handles I/O with <fstream> and CSV parsing using stringstream


