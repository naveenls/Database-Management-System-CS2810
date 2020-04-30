#include<bits/stdc++.h>
#include "Header/Table_Manager.h"
#include "Header/Table.h"
#include "Header/Query.h"

#define database "/Database"

using namespace std;

Table_Manager::Table_Manager(string T_name,string d_name) : table_name(T_name),d_name(d_name) {}
//We use the below constructor only when we create the table. Rest of the time we use the \
 above constuctor.
Table_Manager::Table_Manager(string T_Name, vector<string>& columns, vector<string>& type)
{
    table_name = T_Name;
    col_names = columns;
    col_type = type;
}

//Deleting the in_table and out_table which is stored in heap memory
Table_Manager::~Table_Manager()
{
    if(in_table)
        delete in_table;

    if(out_table) 
        delete out_table;
}
// Here we read the column name and its type from the meta file coressponding to the table \
and store it in col_names and col_type vector
void Table_Manager::open_meta()
{
    ifstream table_meta;

    string meta_file = "Database/" + d_name + "/" + table_name + "_meta.txt";

    table_meta.open(meta_file,ios::in);

    string s;
    string s1, s2;

    getline(table_meta, s);
    while (!table_meta.eof())
    {
        int pos = s.find(" ");
        s1 = s.substr(0, pos);
        s.erase(0, pos + 1); 
        s2 = s;
        col_names.push_back(s1);
        col_type.push_back(s2);
        getline(table_meta, s);
    }

    table_meta.close();
}
//We read the table from the text file and store it in Table class object (in_table).
void Table_Manager::open_table()
{
    ifstream table;
    string file_name = "Database/" + d_name + "/" + table_name + ".txt";

    table.open(file_name, ios::in);

    in_table=new Table();

    in_table->table_name=table_name;
    for(int i=0;i<col_names.size();i++)
    {
        in_table->col_names[col_names[i]]=i;   // We map each column name to an integer. i'th column maps to integer i
    }
    
    in_table->col_type=col_type;

    string s;
    getline(table, s);
    while (!table.eof())
    {
        vector<string> v;
        split(s," ",v);         // Each tuple is splited with space as deliminer and stored in vector v

        in_table->table.push_back(v);
        getline(table, s);
    }
    table.close();

}
//Here string s stores a tuple. We split string s with "," as delimiter and get value of each column.
void Table_Manager::add_record(string s)
{
    vector<string> v;
    split(s,",",v);  //v stores the value of each column

    ofstream table;

    string file_name = "Database/" + d_name + "/" + table_name + ".txt";

    table.open(file_name,ios::app);

    if(v.size()!=col_type.size()) //Number of values given and number of columns should match
    {
        cerr<<"Number of columns mismatch"<<endl;
        table.close();
        return;
    }
    for (int i=0;i<v.size();i++)
    {
        // If the column type is string we directly write into the file.\
            If column type is INT we convert the string to int and write into the file.
        if(col_type[i]=="STRING")
        {
            table << v[i] << ' ';
        }
        else if(col_type[i]=="INT")
        {
            stringstream ss(v[i]);

            int val;
            ss>>val;
            table << val << ' ';
        }
    }
    table << '\n';

    table.close();

}
//Here we delete the table text file and its meta file in database d_name
bool Table_Manager::delete_table()
{

    string file_name = "Database/" + d_name + "/" + table_name + ".txt" ;
    string meta_file = "Database/" + d_name + "/" + table_name + "_meta.txt";
    if (remove(file_name.c_str())!=0)
    {
        return false;
    }

    if (remove(meta_file.c_str())!=0)
    {
        return false;
    }
    return true;
}
// We create two files named table_name.txt and table_name_meta.txt inside d_name folder. \
table_name.txt stores the table while meta file stores the column names and its type.
bool Table_Manager::create_table(string d_name)
{
    ofstream new_table,table_meta;

    string file_name = "Database/" + d_name + "/" + table_name + ".txt" ;
    string meta_file = "Database/" + d_name + "/" + table_name + "_meta.txt";

    new_table.open(file_name, ios::trunc);
    table_meta.open(meta_file, ios::trunc);

    if (new_table.is_open() && table_meta.is_open())
    {
        cout << "Creating new table " << table_name << " in database " << d_name << endl;

        cout << "Creating meta file" << endl;

        for (int i = 0; i < col_names.size(); i++)
        {
            table_meta << col_names[i] << ' ' << col_type[i] << '\n';
        }

        new_table.close();
        table_meta.close();

        return true;
    }
    else
    {
        cerr << "Unable to create table" << endl;
        return false;
    }
}
// Database class handles Union operations. So Database class requires the selected \
tuples and columns from Table_Manager. We store this table in out_table variable.
Table* Table_Manager::get_table()
{
    return out_table;
}

void Table_Manager::query(string s,bool is_union=false) //If is_union=flase then out_table is not generated by Query class.
{
    open_meta(); //Reading column names and it's type from meta file
    if(s.size() > 6 && s.substr(0,6)=="INSERT")  //Insert type query. Here we no need to read the table
    {
        int len=s.size()-8;
        add_record(s.substr(7,len));  //s.substr(7,len) is value of each column of a row and is seperated by ","
        return;
    }

    open_table();  //Reading the table from table_name.txt
    Query q = Query(in_table, s);
    q.query_table(is_union);

    if(is_union)  //If is_union is true then the Query class object creates a table which contains selected rows and columns
    {
        out_table=q.union_table;
        return;
    }
    if (q.is_edited)  //q.edited is true if Query is either update or delete type. \
                        In this case we update all the elements in the table 
    {
        ofstream edit_table;

        string file_name = "Database/" + d_name + "/" + table_name + ".txt";
        edit_table.open(file_name, ios::trunc);

        cout << "Updating table " << table_name << " in database " << d_name << endl;
        for (int i = 0; i < in_table->table.size(); i++)
        {
            if (!q.changed[i])   //if a row i is deleted then changed[i] becomes 1 and we no need to write that row \
                                    into the file
            {
                for (int j = 0; j < in_table->table[i].size(); j++)
                {
                    edit_table << in_table->table[i][j] << ' ';
                }
                edit_table << '\n';
            }
        }
    }
}