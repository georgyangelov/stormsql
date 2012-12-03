#ifndef _H_QUERIES_CREATETABLE_INCLUDED
#define _H_QUERIES_CREATETABLE_INCLUDED

#include "../StormSQL/Defines.h"
#include "../StormSQL/Table.h"
#include "Query.h"

namespace StormSQL
{
	// Forward declarations
	class Database;

	namespace Queries
	{
		class CreateTable
			: public Query
		{
		protected:
			Database* db;
			string name;
			Table tbl;

			Field parseField(Lexer&);

		public:
			CreateTable(Database*);

			void SetName(string name);
			void AddField(const Field&);

			string GetType() const;

			void Parse(Lexer&);
			void Execute();
		};
	}
}

#endif