#ifndef _H_QUERIES_SELECT_INCLUDED
#define _H_QUERIES_SELECT_INCLUDED

#include "../StormSQL/Defines.h"
#include "../StormSQL/Database.h"
#include "Query.h"
#include <hash_map>

using namespace std;

namespace StormSQL
{
	namespace Queries
	{
		class Select
			: public Query
		{
		private:
			void del();
			void copy(const Select&);

		protected:
			Database* db;
			string tableName;
			hash_map<string, string> columns;
			ITableDataPredicate* predicate;

			void ReadColumns(Lexer&);

		public:
			Select(Database*);
			Select(const Select&);
			~Select();

			Select& operator=(const Select&);

			void AddColumn(string, string);
			void SetFrom(string);
			void SetWhere(const ITableDataPredicate&);

			string GetType() const;
			void Parse(Lexer&);

			Table* Execute();
		};
	}
}

#endif