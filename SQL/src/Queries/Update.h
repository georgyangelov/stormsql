#ifndef _H_QUERIES_UPDATE_INCLUDED
#define _H_QUERIES_UPDATE_INCLUDED

#include "../StormSQL/Defines.h"
#include "../StormSQL/Database.h"
#include "Query.h"
#include <hash_map>

using namespace std;

namespace StormSQL
{
	namespace Queries
	{
		class Update
			: public Query
		{
		private:
			void del();
			void copy(const Update&);

		protected:
			Database* db;
			string tableName;
			hash_map<string, Expression*> columns;
			ITableDataPredicate* predicate;

			void ReadColumns(Lexer&);

		public:
			Update(Database*);
			Update(const Update&);
			~Update();

			Update& operator=(const Update&);

			void SetFrom(string);
			void SetWhere(const ITableDataPredicate&);

			string GetType() const;
			void Parse(Lexer&);

			Table* Execute();
		};
	}
}

#endif