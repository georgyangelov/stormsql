#ifndef _H_TABLE_SCHEMA_INCLUDED
#define _H_TABLE_SCHEMA_INCLUDED

#include <vector>
#include "Field.h"
#include "Defines.h"
#include <istream>
#include <ostream>
#include "DynamicBuffer.h"
#include "TableData.h"
#include "Predicates.h"

using namespace std;

namespace StormSQL
{
	// Forward declarations
	namespace Queries
	{
		class Insert;
	}

	class Table
	{
	protected:
		//char name[STORM_SQL_TABLE_NAME_SIZE];
		vector<Field> columns;

		DynamicBuffer* data;
		rowIndexType rows;

		int tableRowSize;

		void createDataBuffer();
		
		// Iterator
		template <class TPredicate>
		friend class TableDataIterator;

		// Queries
		friend class Queries::Insert;
	public:
		Table();
		Table(const Table&);
		~Table();

		// Info
		int GetNumFields() const;

		// Test
		bool HasField(const char*) const;
		bool HasField(const char*, int&) const;

		// Alter
		void AddField(const Field&);
		void RemoveField(const char*);
		void AlterField(const char*, const Field&);

		// Store/Load
		void ReadFromStream(istream& in);
		void WriteToStream(ostream& out) const;

		// Construct iterator
		template <class TPredicate>
		TableDataIterator<TPredicate> GetIterator(const TPredicate& predicate)
		{
			return TableDataIterator<TPredicate>(this, predicate);
		}

		TableDataIterator<TruePredicate> GetIterator();
	};

}
#endif

