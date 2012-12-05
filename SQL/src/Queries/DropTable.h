#ifndef _H_QUERIES_DROPTABLE_INCLUDED
#define _H_QUERIES_DROPTABLE_INCLUDED

#include "../StormSQL/Defines.h"
#include "../StormSQL/Table.h"
#include "Query.h"

namespace StormSQL
{
	// Forward declarations
	class Database;

	namespace Queries
	{
		class DropTable
			: public Query
		{
		protected:
			Database* db;
			string name;

		public:
			DropTable(Database*);

			void SetName(string);

			string GetType() const;

			void Parse(Lexer&);
			Table* Execute();
		};
	}
}

#endif