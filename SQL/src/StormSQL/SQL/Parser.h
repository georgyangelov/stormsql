#ifndef _H_PARSER_INCLUDED
#define _H_PARSER_INCLUDED

#include <istream>
#include "Lexer.h"
#include "../../Queries/Query.h"
#include "../Database.h"

using namespace std;
using namespace StormSQL;
using namespace StormSQL::Queries;

namespace StormSQL
{
	namespace SQL
	{
		class Parser
		{
		protected:
			Lexer lexer;
			Database* db;

		public:
			Parser(istream&, Database*);

			// Creates new Query in heap
			// Should be deleted manually
			Query* ParseQuery();
		};

		class UnknownQueryException
			: public runtime_error
		{
		public:
			UnknownQueryException(string query)
				: runtime_error("Unknown query: " + query)
			{
			}
		};
	}
}

#endif