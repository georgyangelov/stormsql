#ifndef _H_TABLE_DATA_INCLUDED
#define _H_TABLE_DATA_INCLUDED

#include "Defines.h"
#include <vector>
#include "Field.h"
#include <istream>
#include <ostream>

using namespace std;

namespace StormSQL
{
	// Forward declarations
	class Table;
	class ITableDataPredicate;

	class FieldData
	{
	private:
		const byte* ptr;
		Field field;

	public:
		FieldData(const byte*, const Field&);

		Field::FieldType GetType() const;

		const byte* GetPtr() const;
		
		char GetChar() const;
		int GetInt32() const;
		unsigned int GetUInt32() const;
		string GetString() const;
	};

	class TableDataRow
	{
	protected:
		vector<FieldData> columns;

	public:
		TableDataRow(byte* _ptr, const vector<Field>& _columns);

		FieldData& operator [](int);
	};

	class TableDataIterator
	{
	protected:
		bool started;
		Table* table;
		ITableDataPredicate* predicate;
		
		rowIndexType rowIndex;

		bool TestCurrentRow() const;

		void del();

		void copy(const TableDataIterator&);
		
		TableDataIterator(Table*, const ITableDataPredicate&);

		friend class Table;
	public:
		TableDataIterator(const TableDataIterator&);
		~TableDataIterator();

		TableDataIterator& operator = (const TableDataIterator&);

		// Get data
		TableDataRow GetFullDataRow() const;
		rowIndexType GetRowIndex() const;

		// Move pointer
		bool NextRow();

		bool PrevRow();
	};

}
#endif

