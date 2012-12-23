#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <hash_map>
#include "boost/filesystem.hpp"
#include "StormSQL/Database.h"
#include "StormSQL/SQL/Parser.h"

using namespace std;
using namespace StormSQL;
using namespace StormSQL::SQL;
using namespace boost::filesystem;

hash_map<string, Database*> dbs;

void loadDBs()
{
	dbs.clear();

	path dir("data");
	
	if (exists(dir))
	{
		directory_iterator end_itr; // default construction yields past-the-end
		for ( directory_iterator iter( dir ); iter != end_itr; iter++ )
		{
			if (extension(*iter) == ".stormdb")
			{
				Database* db = new Database();

				ifstream in(iter->path().string());
				db->Load(in);

				dbs[basename(*iter)] = db;
			}
		}
	}

	if (dbs.size() == 0)
	{
		cout << "No databases found!" << endl;
	}
	else
	{
		cout << "Loaded database: ";

		for (hash_map<string, Database*>::iterator iter = dbs.begin(); iter != dbs.end(); iter++)
		{
			cout << iter->first << ' ';
		}

		cout << endl;
	}
}

void ignoreWhitespace()
{
	char c = cin.peek();
	while (c == ' ' || c == '\t' || c == '\n')
	{
		cin.get();
		c = cin.peek();
	}
}

void saveDatabases()
{
	for (hash_map<string, Database*>::iterator iter = dbs.begin(); iter != dbs.end(); iter++)
	{
		ofstream out("data/" + iter->first + ".stormdb");
		iter->second->Store(out);
		out.close();
	}
}

ostream& operator << (ostream& out, Table tbl)
{
	if (tbl.GetNumRows() == 0)
	{
		out << "No rows in result set" << endl;

		return out;
	}

	int num = tbl.GetNumFields();

	out << left;
	out << setw(num * 13) << setfill('-') << '+' << setfill(' ');
	out << '+' << endl;

	for (int i = 0; i < num; i++)
	{
		out << "| " << setw(10) << tbl.GetField(i).name << " ";
	}

	out << "|" << endl;
	out << setw(num * 13) << setfill('-') << '+' << setfill(' ');
	out << '+' << endl;

	TableDataIterator iter = tbl.GetIterator();
	while (iter.NextRow())
	{
		TableDataRow row = iter.GetFullDataRow();

		for (int i = 0; i < num; i++)
		{
			out << "| ";
			out << setw(10);

			switch (tbl.GetField(i).type)
			{
			case Field::FieldType::byte:
				out << (int)row[i].GetChar() << " ";
				break;
			case Field::FieldType::int32:
				out << row[i].GetInt32() << " ";
				break;
			case Field::FieldType::uint32:
				out << row[i].GetUInt32() << " ";
				break;
			case Field::FieldType::fixedchar:
				out << row[i].GetString() << " ";
				break;
			}
		}

		 out << "|" << endl;
	}
	out << setw(num * 13) << setfill('-') << '+' << setfill(' ');
	out << '+' << endl;

	return out;
}

int main()
{
	cout << "StormSQL v.0.1 ALPHA" << endl;

	cout << "Loading database..." << endl;
	loadDBs();

	if (dbs.size() == 0)
	{
		// Create test database
		Database* db = new Database();
		dbs["test"] = db;
	}

	char str[1000];
	while (true)
	{
		try
		{
			cout << endl << "> ";
			ignoreWhitespace();
			cin.getline(str, 1000, ';');
			
			if (strcmp(str, "save") == 0)
			{
				saveDatabases();

				cout << "Databases saved" << endl;
			}
			else if (strcmp(str, "exit") == 0)
			{
				saveDatabases();

				break;
			}
			else
			{
				stringstream strIn(str);

				Parser p(strIn, dbs["test"]);
				Query* query = p.ParseQuery();

				Table* res = query->Execute();

				if (res)
				{
					cout << *res;

					delete res;
				}
			}
		}
		catch (const runtime_error& ex)
		{
			cout << "ERROR: " << ex.what() << endl;
		}
	}

	return 0;
}