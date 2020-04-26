# Database-Management-System-CS2810
This project is an Object Orineted implementation of DBMS. It is a sofware for Storing and Retriving data from disk based on User queiries.
It allows users to Create Database,Delete Database and Query in a Database.
Also there are three classes named Database,Table_Manager,Queries for handling storage and retrival of data.

## How to run:
1. Download this repositiory and extract the zip file in **Windows** OS.
2. Note: A directory named **Database** and a **meta.txt** inside Database folder in the working Directory is necessary to run this program.(I have already created it)

3. Compiling: Execute the following command in the working directory
```
g++ .\main.cpp .\DTBase.cpp .\Table_Manager.cpp .\Query.cpp -std=c++14
```
4. Execution:
```
.\a.exe
```
5. After execution, options will be displayed for Creating database,Deleting database, Opening database and exiting. (Always use exit option to terminate the program) .
6. To query in a database use Open database option and enter your queries.

Valid queries and their syntax is given in "Queries.txt" file. 

**Note:** This works only on Windows. Also Database name and Table Name are case insensitive. Ex: Two tables with name table1 and Table1 are are not allowed.
