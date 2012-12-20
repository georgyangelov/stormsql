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
		protected:
			Database* db;
			string tableName;
			hash_map<string, string> columns;

			void ReadColumns(Lexer&);
			
		public:
			Select(Database*);
			~Select();

			void AddColumn(string, string);
			void SetFrom(string);

			string GetType() const;
			void Parse(Lexer&);

			Table* Execute();
		};
	}
}

#endif