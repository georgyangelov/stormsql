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
			bool includeAllColumns;
			hash_map<string, Expression*> columns;
			ITableDataPredicate* predicate;

			void ReadColumns(Lexer&);

		public:
			Select(Database*);
			Select(const Select&);
			~Select();

			Select& operator=(const Select&);

			void SetFrom(string);
			void SetWhere(const ITableDataPredicate&);

			string GetType() const;
			void Parse(Lexer&);

			Table* Execute();
		};
	}
}

#endif