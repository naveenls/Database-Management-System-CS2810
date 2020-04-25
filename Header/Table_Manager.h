#ifndef TABLE_MANAGER
#define TABLE_MANAGER

#include<bits/stdc++.h>
#include "Table.h"
#include "Query.h"

using namespace std;

class Table_Manager {

    Table* in_table=NULL; // Table stored in the text file. This is given as input to Query class.

    string table_name; //Table name
    string d_name;  //Database name
    vector<string> col_names; // Stores the column names of the table.
    vector<string> col_type;  // Stores the column types of the columns.
    Table* out_table=NULL; // This is output table created for union operations.

    void open_meta();
    void open_table();
    void add_record(string );
    
public:

    Table_Manager(string ,string );

    Table_Manager(string , vector<string>& , vector<string>& );

    ~Table_Manager();

    bool delete_table();

    bool create_table(string );

    Table* get_table();

    void query(string ,bool );
};
#endif