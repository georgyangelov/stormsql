#ifndef _H_QUERIES_SHOWCREATE_INCLUDED
#define _H_QUERIES_SHOWCREATE_INCLUDED

#include "../StormSQL/Defines.h"
#include "../StormSQL/Table.h"
#include "Query.h"

namespace StormSQL
{
	// Forward declarations
	class Database;

	namespace Queries
	{
		class ShowCreate
			: public Query
		{
		protected:
			Database* db;
			string tableName;

		public:
			ShowCreate(Database*);

			void SetTableName(string);

			string GetType() const;

			void Parse(Lexer&);
			Table* Execute();
		};
	}
}

#endif