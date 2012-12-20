#ifndef _H_TABLE_DATA_INCLUDED
#define _H_TABLE_DATA_INCLUDED

#include "Defines.h"
#include <vector>
#include "Field.h"
#include "Predicates.h"
#include <istream>
#include <ostream>

using namespace std;

namespace StormSQL
{
	// Forward declarations
	class Table;

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

	template <class TPredicate = TruePredicate>
	class TableDataIterator
	{
	protected:
		bool started;
		Table* table;
		TPredicate predicate;
		
		rowIndexType rowIndex;

		bool TestCurrentRow() const
		{
			TableDataRow row = GetFullDataRow();

			return predicate(table, row);
		}
		
		TableDataIterator(Table* _table, const TPredicate& _predicate)
			: table(_table), predicate(_predicate)
		{
			started = false;
			rowIndex = -1;
		}

		friend class Table;
	public:

		// Get data
		TableDataRow GetFullDataRow() const
		{
			return TableDataRow(table->data->GetElementPtr(rowIndex), table->columns);
		}

		// Move pointer
		bool NextRow()
		{
			rowIndexType i;
			if (!started)
			{
				i = 0;
				started = true;
			}
			else
			{
				i = rowIndex + 1;
			}

			if (i >= table->rows)
				return false;

			while (!TestCurrentRow())
			{
				if (i >= table->rows)
					return false;

				i++;
			}

			rowIndex = i;
		
			return true;
		}

		bool PrevRow()
		{
			rowIndexType i = rowIndex - 1;
			while (!TestCurrentRow())
			{
				if (i < 0)
					return false;

				i--;
			}

			rowIndex = i;

			return true;
		}
	};

}
#endif

