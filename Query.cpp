#include<bits/stdc++.h>
#include "Header/Query.h"
#include "Header/Table.h"

using namespace std;
//Spliting the given string with given delimiter and storing it in a vector
void split(string s, string delimiter, vector<string>& v)
{
	size_t pos = 0;
	string token;
	while ((pos = s.find(delimiter)) != string::npos) {
		token = s.substr(0, pos);
		v.push_back(token);
		s.erase(0, pos + delimiter.length());
	}
	v.push_back(s);
}


compare::compare(Table* in_t,int order,int type): t(in_t),order_by(order),type(type)
{
	order_type=t->col_type[order_by];
}
bool compare::operator()(int i,int j)
{
	//We store the elements of the table in string format. So if the column type is INT then \
	we need to convert the value from STRING to int. For that we use stoi function. \
	type =1 (Ascending) type=0 (Descending)
	if(order_type=="STRING")  
	{
		if (type)
			return t->table[i][order_by] > t->table[j][order_by];
		else
			return t->table[i][order_by] < t->table[j][order_by];
	}
	if(order_type=="INT")
	{
		int num1,num2;
		num1=stoi(t->table[i][order_by]);
		num2=stoi(t->table[j][order_by]);

		if (type)
			return num1 > num2;
		else
			return num1 < num2;
	}
}

Query::Query(Table* in_t,string s)
{
	t = in_t;
	query = s;
}

void Query::query_table(bool is_union=false)
{
	vector<string> queries;
	split(query, ";", queries); //Statements in the query are seperated with ';' as delimiter

	string start = queries[0].substr(0, 6);

	//We process WHERE and ORDER statements first. Then we prcess select or update or delete statement.
	for (int i = 1; i < queries.size(); i++)
	{
		int pos = queries[i].find(" ");
		string token = queries[i].substr(0, pos);

		if (token == "WHERE") //Processing WHERE statement
		{
			if_selected = true; //if_selected=true means we have selected one or more rows \
								 from table using the given condition
			_where(queries[i]);
		}
		if (token == "ORDER")  
		{
			vector<string> token;
			split(queries[i], " ", token);  //token[0]=ORDER token[1]=column_name token[2]=ASC|DSC

			if(token.size()!=3)
			{
				cerr<<"Invalid ORDER statement"<<endl;
				return;
			}

			order_by=t->col_names[token[1]];
			if (token[2]=="ASC")
				type = 0;
			else if (token[2]=="DSC")
				type = 1;
			else
			{
				cerr<<"Invalid Order type"<<endl;
				return;
			}
			
		}
	}

	if(!if_selected)   //If WHERE statement is not present then we select all rows
	{
		selected = vector<int>(t->table.size(),0);
		for (int i = 0; i < t->table.size(); i++)
		{
			selected[i] = i;
		}
	}

	if(~order_by)  //If order_by is not -1 then we call _order function
		_order();

	if (start == "SELECT")  //Processing select statement
	{
		_select(queries[0],is_union);
	}
	//Union of two select statements is only valid

	else if (start == "UPDATE")  //Processing update statement
	{
		if(is_union)
		{
			return;
		}
		_update(queries[0]);
	}
	else if (start == "DELETE") //Processing delete statement
	{
		if(is_union)
		{
			return;
		}
		_delete(queries[0]);
	}
	else
	{
		return;
	}
}

void Query::_select(string &query,bool is_union)
{
	vector<string> v;
	split(query, " ", v);  //Spliting the select statement
	bool is_distinct= 0;
	
	if(v.size()<2)
	{
		cerr<<"Invalid SELECT statement"<<endl;
		return;
	}

	auto itr = find(v.begin(),v.end(),"DISTINCT"); //Checking whether DISTINCT word is present in the select statement
	if (itr != v.end())
	{
		is_distinct = true;
		v.erase(itr);
	}

	int _type = 0;
	int ind = 1;

	// _type=0 Normal Select statement \
	   _type=1 Count number of selected rows \
	   _type=2 Min of selected rows ordered by given column name \
	   _type=3 Max of selected rows ordered by given column name

	if (v[1].find("COUNT") != string::npos)   
	{
		_type = 1;
		ind++;
	}
	else if (v[1].find("MIN") != string::npos)
	{
		_type = 2;
		ind++;
	}
	else if (v[1].find("MAX") != string::npos)
	{
		_type = 3;
		ind++;
	}
	
	if(v.size()!=ind+1)
	{
		cerr<<"Column names missing"<<endl;
		return;
	}
	vector<int> cols_select;  //cols_select stores the name of columns from which \
								we have to select data. * means all columns
	if (v[ind] == "*") //v[ind] stores the column names to be selected
	{
		for (int i = 0; i < t->col_type.size(); i++)
		{
			cols_select.push_back(i);
		}
	}
	else
	{
		vector<string> v1;
		split(v[ind], ",", v1);
		for (auto ele : v1)
		{
			cols_select.push_back(t->col_names[ele]);
		}
	}

	if (is_distinct)
	{
		unordered_set<string> u; //We hash each row as string and insert into unordered_set u.\
								  Then we eliminate row indices from selected to make selected columns of \
								  selected rows Distinct
		vector<int> select_new;  
		for (auto itr = selected.begin(); itr != selected.end(); itr++)
		{
			string s="";
			for (int i = 0; i < cols_select.size(); i++)
			{
				s += "#" + t->table[*itr][cols_select[i]];
			}
			if (u.find(s) == u.end())
			{
				select_new.push_back(*itr);
				u.insert(s);
			}
		}
		selected=select_new;
	}
	if(is_union) //If we have Union statement then Select should be normal select statement (i.e _type=0) 
	{
		if(_type==0)
		{
			union_table=new Table();
			for (int i = 0; i < selected.size(); i++) //We store selected rows and columns into union_table
			{
				vector<string> v_row;
				for (int j = 0; j < cols_select.size(); j++)
				{
					v_row.push_back(t->table[selected[i]][cols_select[j]]);
				}
				union_table->table.push_back(v_row);
			}
			for(int k=0;k<cols_select.size();k++)
			{
				string type=t->col_type[cols_select[k]];
				union_table->col_type.push_back(type);
			}
			return;
		}
		else{
			return;
		}
	}
	if (selected.size() == 0 )
	{
		cout << "No elements found" << endl;
		return;
	}
	if(_type ==0)  //Normal select
	{
		for (int i = 0; i < selected.size(); i++)
		{
			for (int j = 0; j < cols_select.size(); j++)
			{
				cout << t->table[selected[i]][cols_select[j]] << '\t';
			}
			cout << endl;
		}
	}
	else if (_type == 1) //Count type
	{
		cout << "Number of elements: "<<selected.size() << endl;
	}
	else if (_type == 2) //Finding min.
	{
		int position = v[1].find("(");
		int len=v[1].find(")")-position -1;

		string _col = v[1].substr(position + 1, len); //Here we display all selected rows and columns \
														 whose given column element is minimum 

		int order_column=t->col_names[_col];

		position = *min_element(selected.begin(), selected.end(), compare(t,order_column,0));
		for(auto i:selected)
		{
			if(t->table[position][order_column]==t->table[i][order_column])
			{
				for (int j = 0; j < cols_select.size(); j++)
				{
					cout << t->table[i][cols_select[j]] << '\t';
				}
				cout << endl;
			}
		}
	}
	else  //Finding max
	{
		int position = v[1].find("(");
		int len=v[1].find(")")-position -1;

		string _col = v[1].substr(position + 1, len); //Here we display all selected rows and columns \
														 whose given column element is maximum 

		int order_column=t->col_names[_col];

		position = *max_element(selected.begin(), selected.end(), compare(t,order_column,0));
		for(auto i:selected)
		{
			if(t->table[position][order_column]==t->table[i][order_column])
			{
				for (int j = 0; j < cols_select.size(); j++)
				{
					cout << t->table[i][cols_select[j]] << '\t';
				}
				cout << endl;
			}
		}
	}
}
void Query::_update(string &query)
{
	if (selected.size())
		is_edited = true;
	else
		return;

	changed = vector<bool>(t->table.size(),0);

	vector<string> update;
	int pos = query.find(" ");
	split(query.substr(pos + 1, query.size()), ",", update);
	if(update.size()<2)
	{
		cerr<<"Column names is not given for Update statement"<<endl;
		return;
	}

	vector<int> cols;         //Column numbers for which value need to be updated
	vector<string> values;    //New value to which it need to be updated

	string temp;
	
	for (auto ele : update)
	{
		pos = ele.find("=");  //column_name=value
		int col_no = t->col_names[ele.substr(0, pos)];
		cols.push_back(col_no);
		temp=(ele.substr(pos + 1, ele.size()));			
		values.push_back(temp);
	}
	for (int i = 0; i < selected.size(); i++)
	{
		for (int j=0;j<cols.size();j++)
		{
			t->table[selected[i]][cols[j]] = values[j];
		}
	}
}
void Query::_delete(string &query)
{
	if (selected.size())
		is_edited = true;

	changed = vector<bool>(t->table.size(),0);

	for (int i = 0; i < selected.size(); i++) //Making the changed[i]=1 if i is in selected vector. \
	                                            So in Table_manager class the selected row is not written
	{
		changed[selected[i]]=1;
	}

}
void Query::_where(string s) //I have used stack based arithmetic evaluation for \
								evaluating conditions\
								So if we have k conditions on k columns then we evalutate \
								all the conditions on each row. If final stack value is 1 then \
								that row is selected.
{
	vector<string> conditions; //condition is colum_name(type)value type is <=,>=,<,>,=,!=
	split(s, " ", conditions);
	
	if(conditions.size()<2)
	{
		cerr<<"Conditions is not given for WHERE statement"<<endl;
		return;
	}
	vector<int> conds;   //conds stores column names and operation type. Operation type= AND | OR
	vector<int> op_type; //op_type stores type of inequality
	vector<string> value; //It stores the RHS value of the condition

	for (int i = 1; i < conditions.size(); i++)  //We split condtions with space as delimiter.
	{
		int type_no, col_name;
		string val;
		if (conditions[i] == "NOT") //If keyword is NOT then we take inverse of the next condition \
									and push it into the stack
		{
			int arr[6] = { 4,3,5,1,0,2 }; //0 = <,1 = >,2 = =,3 = <= ,4 = >=,5 = !=
			parse_conditions(conditions[i+1], type_no, col_name, val); 
			conds.push_back(col_name);
			op_type.push_back(arr[type_no]);
			value.push_back(val);

			i++;
		}
		else if (conditions[i] == "AND")
		{
			conds.push_back(1);
		}
		else if (conditions[i] == "OR")
		{
			conds.push_back(0);
		}
		else
		{
			parse_conditions(conditions[i], type_no, col_name, val);
			conds.push_back(col_name);
			op_type.push_back(type_no);
			value.push_back(val);
		}
	}
	for (int i = 0; i < t->table.size(); i++)
	{
		stack<bool> _operand;
		stack<int> _operator;
		for (int j = 0; j < conds.size(); j++) //In conds vector if index i is even then conds[i] is column name \
																		  i is odd then conds[i] is operation type (Operator)
		{
			if (j % 2==0)
			{
				if (t->col_type[conds[j]]=="STRING")
				{
					string val = t->table[i][conds[j]];
					_operand.push(evaluate<string>(val, value[j / 2], op_type[j / 2]));  //Evaluate function evaluates the condition
				}
				else //If column type is INT the we convert both value in table and RHS value of condition to int
				{
					int val= stoi(t->table[i][conds[j]]);
					int value_int=stoi(value[j / 2]);
					_operand.push(evaluate<int>(val, value_int, op_type[j / 2]));
				}
			}
			else
			{
				while (!_operator.empty() && _operator.top()>=conds[j])  
				{
					
					bool v1 = _operand.top();
					_operand.pop();

					bool v2 = _operand.top();
					_operand.pop();

					bool op = _operator.top();
					_operator.pop();

					if (op)
						v1 = v1 & v2;
					else
						v1 = v1 | v2;

					_operand.push(v1);
				}
				_operator.push(conds[j]);
			}
		}
		while (!_operator.empty())
		{
			bool v1 = _operand.top();
			_operand.pop();

			bool v2 = _operand.top();
			_operand.pop();

			bool op = _operator.top();
			_operator.pop();

			if (op)
				v1 = v1 & v2;
			else
				v1 = v1 | v2;

			_operand.push(v1);
		}
		if (_operand.top())
			selected.push_back(i);
	}
}
void Query::_order()  //Sorting the rows based on given column name
{
	sort(selected.begin(), selected.end(), compare(t,order_by,type));
}

//Finds the type of condition,colum_name and RHS value
void Query::parse_conditions(string s, int& type_no, int& col_name, string& val) 
{
	bool j=0;
	for (int i = 0; i < s.size(); i++)
	{
		if (s[i] == '<')
		{
			if (s[i + 1] == '=')
			{
				type_no = 3;
				j = 1;
			}
			else
				type_no = 0;
		}
		else if (s[i] == '>')
		{
			if (s[i + 1] == '=')
			{
				type_no = 4;
				j = 1;
			}
			else
				type_no = 1;
		}
		else if (s[i] == '=')
		{
			type_no = 2;
		}
		else if (s[i] == '!')
		{
			type_no = 5;
			j = 1;
		}
		else
			continue;

		col_name = t->col_names[s.substr(0, i)];
		int len=s.size()-(i + j + 1);
		val = s.substr(i + j + 1, len);
		break;
	}
}

template <typename T>
bool Query::evaluate(T lhs, T rhs, int type_no)
{
	switch (type_no)
	{
		case 0:
			return lhs < rhs;
		case 1:
			return lhs > rhs;
		case 2:
			return lhs == rhs;
		case 3:
			return lhs <= rhs;
		case 4:
			return lhs >= rhs;
		case 5:
			return lhs != rhs;
	}
}
