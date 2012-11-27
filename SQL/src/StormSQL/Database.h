#ifndef _H_DATABASE_INCLUDED
#define _H_DATABASE_INCLUDED

#include <vector>
#include <string>
#include <hash_map>
#include "Table.h"

using namespace std;

namespace StormSQL
{
	class Database
	{
	protected:
		hash_map< string, Table* > tables;

		void del();
		void copy(const Database&);

	public:
		Database();
		Database(const Database&);
		~Database();

		void Load(istream&);
		void Store(ostream&);

		void CreateTable(string name, Table& table);
		void DropTable(string name);

		Table& GetTable(string);
		Table& operator [] (string);

		bool HasTable(string) const;
		
		vector<string> GetTableNames() const;
	};
}

#endif