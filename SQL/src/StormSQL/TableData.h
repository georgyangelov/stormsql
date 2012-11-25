#ifndef _H_TABLE_DATA_INCLUDED
#define _H_TABLE_DATA_INCLUDED

#include "Defines.h"
#include <vector>
#include "Field.h"
#include "Table.h"
#include <istream>
#include <ostream>

using namespace std;

namespace StormSQL
{
	class TableDataRow
	{
	protected:
		byte* const ptr;
		vector<Field> columns;

	public:
		TableDataRow(byte* const _ptr, const vector<Field>& _columns);
		
		Field::FieldType GetType(int columnIndex) const;

		const byte* GetPtr(int columnIndex) const;

		char GetChar(int columnIndex) const;
		int GetInt32(int columnIndex) const;
		unsigned int GetUInt32(int columnIndex) const;
		string GetString(int columnIndex) const;
	};

	class ITableDataPredicate
	{
	public:
		virtual bool operator () (const Table*, TableDataRow&) = 0;
	};

	class TableDataIterator
	{
	protected:
		Table* const table;
		ITableDataPredicate* const predicate;
		
		rowIndexType rowIndex;

		bool TestCurrentRow() const;

	public:
		TableDataIterator(Table* const, ITableDataPredicate* const);

		// Get data
		TableDataRow GetFullDataRow() const;

		// Move pointer
		bool NextRow();
		bool PrevRow();
	};

}
#endif

