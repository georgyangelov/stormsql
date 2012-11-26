#ifndef _H_PREDICATES_INCLUDED
#define _H_PREDICATES_INCLUDED

#include "Defines.h"

namespace StormSQL
{
	// Forward declarations
	class Table;
	class TableDataRow;

	class ITableDataPredicate
	{
	public:
		virtual bool operator () (const Table*, TableDataRow&) const = 0;
	};

	// Simple predicate that always returns true
	class TruePredicate
		: ITableDataPredicate
	{
	public:
		bool operator () (const Table* table, TableDataRow& row) const
		{
			return true;
		}
	};
}

#endif

