#include<bits/stdc++.h>
#include<windows.h>
#include "Header/DTBase.h"
#include "Header/Table_Manager.h"
#include "Header/Table.h"

using namespace std;

Database::Database() {}

//We read table names from meta file and store it into table_name vector.

Database::Database(string d_name, string pass) : database_name(d_name), password(pass)
{
    string file_name = "Database/" + d_name + "/" + "meta.txt";

    ifstream database_meta;
    database_meta.open(file_name.c_str());

    string s;
    getline(database_meta, s); // Reading d_name and password
    getline(database_meta, s); // Reading Last edit time

    getline(database_meta, s);
    while (!database_meta.eof())
    {
        table_name.push_back(s);
        getline(database_meta, s);
    }

    database_meta.close();
}

//We create a meta file inside d_name folder and store database name , password and last edit time in it.

bool Database::create_database(string d_name, string pass)
{
    time_t curr_time;

    time(&curr_time);
    
    tm* tm_local = localtime(&curr_time);

    database_name = d_name;
    password = pass;

    string file_name = "Database/" + d_name + "/" + "meta.txt";

    ofstream database_meta;
    database_meta.open(file_name.c_str(), ios::trunc);

    if (database_meta.is_open())
    {
        cout << "Creating Database " << d_name << endl;

        database_meta << d_name << ' ' << pass + "\n";
        database_meta << "Last Edit Time" << ' ' << tm_local->tm_mon << '/' << tm_local->tm_year << ' ' << tm_local->tm_hour << ':' << tm_local->tm_min << endl;

        database_meta.close();
        return true;
    }
    else
    {
        cerr << "Unable to create Database" << endl;
        return false;
    }
}
/* We first delete all the table file and table's meta file. And then we delete database's meta file.
 Finally we delete database's folder using RemoveDirectoryA function (Works only in windows).
 */
bool Database::delete_database()
{
    string file_name = "Database/" + database_name;
    
    for(auto table:table_name)
    {
        Table_Manager T(table,database_name);
        if(!T.delete_table())
        {
            cerr<<"Unable to delete table "<<table<<endl;
            return false;
        }
    }

    string meta_file = "Database/" + database_name + "/" + "meta.txt";
    if (remove(meta_file.c_str())!=0)
    {
        cerr << "Unable to delete metaFile" << endl;
        return false;
    }

    if(RemoveDirectoryA(file_name.c_str())==0)
    {
        cerr << "Unable to delete Database" << endl;
        return false;
    }
    return true;

}
// We insert table name into table_name vector and call create_table function of Table_Manager class.
bool Database::add_table(string t_name, vector<string>& columns, vector<string>& column_type)
{
    if (find(table_name.begin(), table_name.end(), t_name) != table_name.end())
    {
        cerr << "Table already present" << endl;
        return false;
    }
    else
    {
        changed=true;

        table_name.push_back(t_name);

        Table_Manager T(t_name, columns, column_type);
        
        T.create_table(database_name);

        return true;
    }
}
/*
Here we rewrite all the table names in meta file to present table names before closing the database. 
This is only done if we have created or deleted any new tables.
*/
void Database::comit_changes()
{
    if(!changed)
        return;

    time_t curr_time;
    time(&curr_time);
    tm* tm_local = localtime(&curr_time);

    string file_name = "Database/" + database_name + "/" + "meta.txt";

    ofstream database_meta;
    database_meta.open(file_name.c_str(),ios::trunc);

    database_meta << database_name << ' ' << password + "\n";
    database_meta << "Last Edit Time" << ' ' << tm_local->tm_mon << '/' << tm_local->tm_year << ' ' << tm_local->tm_hour << ':' << tm_local->tm_min << endl;

    for(int i=0;i<table_name.size();i++)
    {
        database_meta<<table_name[i]<<endl;
    }
    database_meta.close();
}
// We delete the table name from table_name vector and the call delete_function of Table_Manager class \
   to delete corresponding files

bool Database::delete_table(string t_name)
{
    vector<string>::iterator it=find(table_name.begin(), table_name.end(), t_name);
    if (it == table_name.end())
    {
        cerr << "Table Not present" << endl;
        return false;
    }
    else
    {
        changed=true;
        table_name.erase(it);

        Table_Manager T(t_name,database_name);
        if(T.delete_table())
            cout<<"Deleted table "<<t_name<<" sucessfully"<<endl;
        else
            cerr<<"Unable to delete table"<<endl;

        return true;
    }
}

void Database::handle_queries()
{
    cout<<"Enter Queries. Type \"STOP\" to end queries"<<endl;
    
    string q;
    getline(cin,q);

    while(q!="STOP")
    {
        if(q.size()>5 && q.substr(0,5)=="TABLE") //Queries for table creation and deletion
        {
            vector<string> v;
            split(q," ",v); // Spliting the query with space as delimiter. So v[0]=TABLE,v[1]=CREATE or DELETE \
                               v[2]=table_name, v[3]=col_name(col_type)(,col_name(col_type))* if it is CREATE query
            
            if(v.size()<3)
            {
                cerr<<"Invalid Query"<<endl;
                goto end;
            }
            if(v[1]=="DELETE")
            {
                v[2].pop_back();  //Deleting semicolon which is present at end of the query
                delete_table(v[2]);
            }
            else if(v[1]=="CREATE")
            {
                if(v.size()<4)
                {
                    cerr<<"Invalid query. Column names missing"<<endl;
                    goto end;
                }
                v[3].pop_back();  //Deleting semicolon which is present at end of the query
                vector<string> cols;

                vector<string> col_name;
                vector<string> col_ty;
                split(v[3],",",cols);  //Spliting the column names and their types with ',' as delimiter

                for(int i=0;i<cols.size();i++)
                {
                    vector<string> temp;
                    split(cols[i],"(",temp);

                    col_name.push_back(temp[0]);  //temp[0]=column name

                    temp[1].pop_back();   //tamp[1]=column_type
                    col_ty.push_back(temp[1]);
                }
                add_table(v[2],col_name,col_ty);
            }
            else
            {
                cerr<<"Invalid table query (Niether Delete nor Create)"<<endl;
            }
        }
        else if(q.find("UNION")!=string::npos) //Union type queries 
        {
            int pos=q.find("UNION");

            string q1=q.substr(0,pos); //q1=Select type query of first table
            q.erase(0,pos+6);
            string q2=q;  //q1=Select type query of second table

            int pos1,pos2; //pos1=Position of FROM statement in table 1 \
                            pos2=Position of FROM statement in table 2
            int end_pos1,end_pos2;

            pos1=q1.find("FROM ");

            end_pos1=pos1+5;
            while(q1[end_pos1]!=';')
                end_pos1++;

            pos2=q2.find("FROM ");
            
            if(pos1==-1 || pos2==-1)
            {
                cerr<<"Invalid Query"<<endl;
                goto end;
            }
            end_pos2=pos2+5;
            while(q2[end_pos2]!=';')
                end_pos2++;

            int len1=end_pos1-(pos1+5);
            int len2=end_pos2-(pos2+5);
            string t_name1=q1.substr(pos1+5,len1);   //Table name of first table
            q1.erase(pos1,end_pos1-pos1+1);
            string t_name2=q2.substr(pos2+5,len2);    // Table name of second table
            q2.erase(pos2,end_pos2-pos2+1);

            Table_Manager T1(t_name1,database_name);
            Table_Manager T2(t_name2,database_name);

            T1.query(q1,true);
            T2.query(q2,true);
            Table* first=T1.get_table();  //We get the Output table created by Query class of first table
            Table* second=T2.get_table();  //We get the Output table created by Query class of second table

            if(first->col_type.size()!=second->col_type.size()) // Checking wether the select statements of two tables selects \
                                                                    same number of columns 
            {
                cerr<<"Number of columns mismatch"<<endl;
                goto end;
            }
            for(int i=0;i<first->col_type.size();i++)  // Checking wether the select statements of two tables selects \
                                                        columns of same type 
            {
                if(first->col_type[i]!=second->col_type[i])
                {
                    cerr<<"Column Number "<<i+1<<" of first and second table type mismatch"<<endl;
                    goto end;
                }
            }

            unordered_set<string> u; //We hash each tuple as a string. And then we check for duplicate items. \
            If k columns are selected for each tuple and let their values by v1,v2,...,vk the the hash value is \
            #v1#v2#v3...#vk

            //Printing first table
            for(auto v_row:first->table)
            {
                string s="";
                for(auto ele:v_row)
                {
                    s+="#"+ele;
                }
                if(u.find(s)==u.end())
                {
                    u.insert(s);
                    for(auto ele:v_row)
                    {
                        cout<<ele<<'\t';
                    }
                    cout<<endl;
                }
            }
            //Printing second table
            for(auto v_row:second->table)
            {
                string s="";
                for(auto ele:v_row)
                {
                    s+="#"+ele;
                }
                if(u.find(s)==u.end())
                {
                    u.insert(s);
                    for(auto ele:v_row)
                    {
                        cout<<ele<<'\t';
                    }
                    cout<<endl;
                }
            }
        }
        else  // For handle Select,Update,Delete,Insert type queries.
        {
            int pos,end_pos;
            pos=q.find("FROM ");

            if(pos==-1)
                pos=q.find("INTO ");

            if(pos==-1)
            {
                cerr<<"Invalid Query"<<endl;
                goto end;
            }

            end_pos=pos+5;
            while(q[end_pos]!=';')
                end_pos++;

            int len=end_pos-(pos+5);
            string t_name=q.substr(pos+5,len); // Extracting table name from query.
            q.erase(pos,end_pos-pos+1);  //Deleting FROM and INTO statement from query.

            Table_Manager T(t_name,database_name);
            T.query(q,false);  //Processing the query
        }
        
        end:
        getline(cin,q);
    }
    comit_changes();
}
