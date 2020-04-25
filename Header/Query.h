#ifndef Q_Manager
#define Q_Manager

#include<bits/stdc++.h>
#include "Table.h"
using namespace std;

void split(string , string , vector<string>& );

class compare //Compare class has the comparator to comapare two elements \
				in the same column of different rows in table t
{
    Table *t;
    int order_by;  //Column by which we have to compare two rows
    string order_type;  // DataType of the column
    int type;    //Either ascending or descending

public:
    compare(Table* ,int ,int );

    bool operator()(int ,int );
};

class Query {

private:
	Table* t=NULL;
	string query;
	vector<int> selected;  //It contains the row indices corresponding to the rows selected by WHERE statement \
							If there is no WHERE statement then it contains all the row indices
	bool if_selected;   //It is set true if there is where statement in the query
	int order_by=-1;    //Column number by which we have to order the rows
	bool type;         // Ascending or descending
	
	void _select(string& ,bool );

	void _update(string& );
	
	void _delete(string& );

	void _where(string );

	void _order();

	void parse_conditions(string , int& , int& , string& );

	template <typename T>
	bool evaluate(T , T , int );

public:

    Table* union_table=NULL;  //Output table is generated when we have an Union type Query
	bool is_edited=false;     //true if we have any Update or Delete statement
	vector<bool> changed;     //If row i is deleted then changed[i]=1

	Query(Table* ,string );
	
	void query_table(bool );
};
#endif