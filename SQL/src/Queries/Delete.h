#ifndef _H_QUERIES_DELETE_INCLUDED
#define _H_QUERIES_DELETE_INCLUDED

#include "../StormSQL/Defines.h"
#include "../StormSQL/Database.h"
#include "Query.h"

using namespace std;

namespace StormSQL
{
	namespace Queries
	{
		class Delete
			: public Query
		{
		private:
			void del();
			void copy(const Delete&);

		protected:
			Database* db;
			string tableName;
			ITableDataPredicate* predicate;

		public:
			Delete(Database*);
			Delete(const Delete&);
			~Delete();

			Delete& operator=(const Delete&);

			void SetFrom(string);
			void SetWhere(const ITableDataPredicate&);

			string GetType() const;
			void Parse(Lexer&);

			Table* Execute();
		};
	}
}

#endif