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
		vector<Field> columns;

		DynamicBuffer* data;
		rowIndexType rows;

		int tableRowSize;

		void createDataBuffer(bool deleteOld = true);
		
		// Iterator
		friend class TableDataIterator;

		// Queries
		friend class Queries::Insert;
	public:
		Table();
		Table(const Table&);
		~Table();

		// Info
		int GetNumRows() const;
		int GetNumFields() const;
		Field GetField(int) const;
		hash_map<string, Field> GetFields() const;
		int GetColumnIndex(const char*) const;
		Field::FieldType GetFieldType(int) const;

		// Test
		bool HasField(const char*) const;
		bool HasField(const char*, int&) const;

		// Data
		void DeleteBufferIndex(rowIndexType);

		// Alter
		void AddField(const Field&);
		void AddField(int, const Field&);
		void RemoveField(const char*);
		void AlterField(const char*, const Field&);

		// Store/Load
		void Load(istream& in);
		void Store(ostream& out) const;

		// Construct iterator
		TableDataIterator GetIterator(const ITableDataPredicate& predicate)
		{
			return TableDataIterator(this, predicate);
		}

		TableDataIterator GetIterator();
	};

}
#endif

