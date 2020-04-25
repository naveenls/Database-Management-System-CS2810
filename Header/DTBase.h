#ifndef DTBASE
#define DTBASE

#include<bits/stdc++.h>
#include "Table_Manager.h"
#include "Table.h"

using namespace std;

class Database {

private:
    string database_name;
    string password;

    vector<string> table_name; // Name of tables stored in the database
    bool changed=false;   // It is true if we have created a table or deleted it.

    bool add_table(string , vector<string>& , vector<string>& );
    void comit_changes();
    bool delete_table(string );

public:

    Database();
    Database(string , string );      

    bool create_database(string , string );

    bool delete_database();
    
    void handle_queries();

};
#endif
