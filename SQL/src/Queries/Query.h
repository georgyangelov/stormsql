#ifndef _H_QUERIES_QUERY_INCLUDED
#define _H_QUERIES_QUERY_INCLUDED

#include "../StormSQL/SQL/Lexer.h"

using namespace StormSQL::SQL;

namespace StormSQL
{
	// Forward declarations
	class Table;

	namespace Queries
	{
		class Query
		{
		public:
			virtual string GetType() const = 0;
			virtual Table* Execute() = 0;

			virtual void Parse(Lexer&) = 0;
		};
	}
}

#endif