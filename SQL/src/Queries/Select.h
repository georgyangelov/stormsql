#ifndef _H_QUERIES_SELECT_INCLUDED
#define _H_QUERIES_SELECT_INCLUDED

#include "../StormSQL/Defines.h"
#include "../StormSQL/Table.h"
#include "Query.h"

namespace StormSQL
{
	namespace Queries
	{
		class Select
			: Query
		{
		protected:
			Table* const table;
			
		public:
			Select(Table* const);
			Select(const Select&);
			~Select();

			string GetType() const;
			Table* Execute();
		};
	}
}

#endif