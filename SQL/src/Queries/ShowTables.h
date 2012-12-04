#ifndef _H_QUERIES_SHOWTABLES_INCLUDED
#define _H_QUERIES_SHOWTABLES_INCLUDED

#include "../StormSQL/Defines.h"
#include "../StormSQL/Table.h"
#include "Query.h"

namespace StormSQL
{
	// Forward declarations
	class Database;

	namespace Queries
	{
		class ShowTables
			: public Query
		{
		protected:
			Database* db;

		public:
			ShowTables(Database*);

			string GetType() const;

			void Parse(Lexer&);
			Table* Execute();
		};
	}
}

#endif