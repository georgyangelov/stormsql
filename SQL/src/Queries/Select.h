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
			vector< pair< Table*, Expression* > > joins;
			ITableDataPredicate* predicate;

			void ReadColumns(Lexer&);
			void ReadJoins(Lexer&);
			string GetHashableData(Table&, TableDataRow&, const vector< pair<string, string> >&, bool firstTable) const;
			Table* Join(Table&, Table&, const Expression&, bool filterOnJoin = false) const;

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