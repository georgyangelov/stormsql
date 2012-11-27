#include "Database.h"
#include "Exceptions.h"

namespace StormSQL
{
	/* Protected */
	void Database::del()
	{
		hash_map<string, Table*>::iterator iter;

		for (iter = tables.begin(); iter != tables.end(); iter++)
		{
			delete iter->second;
		}
	}

	void Database::copy(const Database& db)
	{
		tables.clear();

		hash_map<string, Table*>::const_iterator iter;
		Table* tbl;

		for (iter = db.tables.begin(); iter != db.tables.end(); iter++)
		{
			tbl = new Table(*iter->second);
			tables[iter->first] = tbl;
		}
	}

	/* Public */
	Database::Database()
	{
	}

	Database::Database(const Database& db)
	{
		copy(db);
	}

	Database::~Database()
	{
		del();
	}


	void Database::Store(ostream& out)
	{
		// Write number of tables
		int num = tables.size();
		out.write((const char*)&num, sizeof(int));

		// Write actual tables
		hash_map<string, Table*>::const_iterator iter;
		for (iter = tables.begin(); iter != tables.end(); iter++)
		{
			// Get table name in c string
			char name[STORM_SQL_TABLE_NAME_SIZE];
			strcpy(name, iter->first.c_str());
			// Write table name
			out.write(name, STORM_SQL_TABLE_NAME_SIZE);

			// Write table
			iter->second->Store(out);
		}
	}

	void Database::Load(istream& in)
	{
		// Read number of tables
		int num;
		in.read((char*)&num, sizeof(int));

		// Read tables
		for (int i = 0; i < num; i++)
		{
			// Read table name
			char name[STORM_SQL_TABLE_NAME_SIZE];
			in.read(name, STORM_SQL_TABLE_NAME_SIZE);

			// Read table
			Table* tbl = new Table;
			tbl->Load(in);

			tables[name] = tbl;
		}
	}

	void Database::CreateTable(string name, Table& table)
	{
		if (name.size() > STORM_SQL_TABLE_NAME_SIZE - STORM_SQL_STRING_TERMINATOR_SIZE)
			throw NameTooLong();

		// Copy table and add to database
		tables[name] = new Table(table);
	}

	void Database::DropTable(string name)
	{
		// Delete Table object
		delete tables[name];

		// Delete the table from the hash_map
		tables.erase(name);
	}

	Table& Database::GetTable(string name)
	{
		if (!HasTable(name))
			throw TableDoesntExist();

		return *tables[name];
	}

	Table& Database::operator[](string name)
	{
		return GetTable(name);
	}

	bool Database::HasTable(string name) const
	{
		return tables.find(name) != tables.end();
	}

	vector<string> Database::GetTableNames() const
	{
		vector<string> names;

		hash_map<string, Table*>::const_iterator iter;
		for (iter = tables.begin(); iter != tables.end(); iter++)
		{
			names.push_back(iter->first);
		}

		return names;
	}
}