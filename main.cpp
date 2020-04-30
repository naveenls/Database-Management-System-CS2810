#include<bits/stdc++.h>
#include "Header/DTBase.h"
#include<windows.h>

using namespace std;

string location = "Database/meta.txt";
vector< string> d_names;  //d_names stores the names of database present.
vector< string> pass;
bool is_edited = false;   /* It is set true if we create or delete a database. It helps 
							for updating the meta file */

/*
We take Database name and password as input. If Database is not present or if there is
password mismatch then corresponding error is displayed. If password matches then we create 
Database class object and handle queries there.
*/
void openDatabase()	
{
	cout << "Enter Database name:"<<endl;;
	string d_name;
	cin >> d_name;

	vector<string>::iterator it=find(d_names.begin(),d_names.end(),d_name) ;

	if (it!= d_names.end())
	{
		string password;
		cout<< "Enter password:"<<endl;
		cin>>password;
		int i=it-d_names.begin();
		if(password==pass[i])
		{
			cout<<"Opening database "<<d_name<<endl;
			Database D(d_name,password);
			getline(cin,password);
			D.handle_queries();

		}
		else
			cerr<<"Password mismatch!!"<<endl;
	}
	else
		cerr << "Database is not present"<<endl;
}
/*
We take Database name and password as input. If Database is not present or if there is
password mimatch then corresponding error is displayed. If password matches then we delete the 
database using Delete function defined in database class and erase this from vector d_names and pass. 
*/
void deleteDatabase()
{
	cout << "Enter Database name:"<<endl;;
	string d_name;
	cin >> d_name;

	vector<string>::iterator it=find(d_names.begin(),d_names.end(),d_name) ;

	if (it!= d_names.end())
	{
		string password;
		cout<< "Enter password:"<<endl;
		cin>>password;
		int i=it-d_names.begin();

		if(password==pass[i])
		{
			Database d(d_name,password);
			if(d.delete_database())
			{
				is_edited=true;
				d_names.erase(it);
				pass.erase(pass.begin()+i);
			}
		}
		else
			cerr<<"Password mismatch!!"<<endl;
	}
	else
		cerr << "Database is not present"<<endl;
}
/*
We input database name(d_name) and password(pass). If given database is not present then we create a folder named
d_name inside database folder. And then we call create_database function of Database class to create required files
inside d_name folder and we insert d_name and password into d_names and pass vector.
 Note: This folder creation works only on windows. 
*/
void createDatabase()
{
	cout << "Enter Database name:"<<endl;
	string d_name;
	cin >> d_name;

	if (find(d_names.begin(),d_names.end(),d_name) == d_names.end())
	{
		string password;
		cout<< "Enter password:"<<endl;
		cin>>password;

		string location="Database/"+d_name;
		if (CreateDirectory(location.c_str(), NULL) ||ERROR_ALREADY_EXISTS == GetLastError())
		{
			Database D;
			if(D.create_database(d_name,password))
			{
				is_edited=true;
				d_names.push_back(d_name);
				pass.push_back(password);
			}

		}
		else
		{
			cerr<<"Failed to create directory!!"<<endl;
		}
	}
	else
		cerr << "Database is already present"<<endl;
}

void displayOption()
{
	cout<<endl;
	cout << "******************************"<<endl;
	cout << "* Choose one of the options: *" << endl;
	cout << "* 1) Open Database           *" << endl;
	cout << "* 2) Create Database         *" << endl;
	cout << "* 3) Delete Database         *" << endl;
	cout << "* 4) Exit                    *" << endl;
	cout << "******************************"<<endl;
	cout<<endl;
}
/*
We create a meta file inside Database folder to store the names of database and corresponding password.
*/
void createMeta()
{
	ofstream m_file(location.c_str(),ios::trunc);
	
	m_file << "Database Management System" << endl;
	m_file.close();
}
/*
Reads the database names and password from meta file and stores it into a vectors d_names and pass.
*/
void openMeta()
{
	ifstream meta_file;
	meta_file.open(location.c_str(),ios::in);

	string s;
	getline(meta_file, s);

	getline(meta_file, s); 	// Each line is database name and password
	while (!meta_file.eof())
	{
		int pos=s.find(" ");

		d_names.push_back(s.substr(0,pos));
		pass.push_back(s.substr(pos+1,s.size()));

		getline(meta_file, s);
	}

	meta_file.close();
}
/*
Here we rewrite all the records in meta file to present database names before exiting the main program. 
This is only done if we have created or deleted any new database.
*/
void closeMeta()
{
	if (!is_edited)
	{
		return;
	}

	ofstream meta_file;
	meta_file.open(location.c_str(),ios::trunc);

	meta_file << "Database Management System" << endl;

	for (int i=0;i<d_names.size();i++)
	{
		meta_file << d_names[i]<<' '<<pass[i] <<'\n';
	}

	meta_file.close();
}
int main()
{
	cout << "**************************"<<endl;
	cout << "Database Management System" << endl;
	cout << "**************************"<<endl;
	openMeta();
	int option;
	while (1)
	{
		displayOption();
		cin >> option;

		switch (option)
		{
			case 1:
				openDatabase();
				break;
			case 2:
				createDatabase();
				break;
			case 3:
				deleteDatabase();
				break;
			case 4:
				goto end;
				break;
			default:
				cerr << "Invalid Option" << endl;
				break;
		}

	}

	end:
		closeMeta();
		cout << "Exited" << endl;
	return 0;
}