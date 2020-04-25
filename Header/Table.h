#ifndef TABLE
#define TABLE

#include<bits/stdc++.h>
using namespace std;
class Table {

public:
	string table_name; //Name of the table
	vector< vector< string > > table; //Contents of the table

	unordered_map<string,int> col_names;  // Column number i is mapped to integer i
	vector<string> col_type;  //Data Type of the columns
};
#endif
