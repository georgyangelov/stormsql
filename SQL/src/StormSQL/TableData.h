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

	class TableDataRow
	{
	protected:
		byte* ptr;
		vector<Field> columns;

	public:
		TableDataRow(byte* _ptr, const vector<Field>& _columns);
		
		Field::FieldType GetType(int columnIndex) const;

		const byte* GetPtr(int columnIndex) const;

		char GetChar(int columnIndex) const;
		int GetInt32(int columnIndex) const;
		unsigned int GetUInt32(int columnIndex) const;
		string GetString(int columnIndex) const;
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

