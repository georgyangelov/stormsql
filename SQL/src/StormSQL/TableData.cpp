#include "TableData.h"
#include "Exceptions.h"

namespace StormSQL
{
	/* TableDataRow */
	TableDataRow::TableDataRow(byte* const _ptr, const vector<Field>& _columns)
		: ptr(_ptr), columns(_columns)
	{
	}

	Field::FieldType TableDataRow::GetType(int index) const
	{
		return columns[index].type;
	}

	const byte* TableDataRow::GetPtr(int index) const
	{
		byte* res = ptr;

		for (int i = 0; i < index; i++)
			res += columns[i].GetByteSize();

		return res;
	}

	char TableDataRow::GetChar(int index) const
	{
		if (columns[index].type != Field::FieldType::byte)
			throw InvalidFieldType();

		return *GetPtr(index);
	}

	int TableDataRow::GetInt32(int index) const
	{
		if (columns[index].type != Field::FieldType::int32)
			throw InvalidFieldType();

		return *((int*)GetPtr(index));
	}
	
	unsigned int TableDataRow::GetUInt32(int index) const
	{
		if (columns[index].type != Field::FieldType::uint32)
			throw InvalidFieldType();

		return *((unsigned int*)GetPtr(index));
	}
	
	string TableDataRow::GetString(int index) const
	{
		if (columns[index].type != Field::FieldType::fixedchar)
			throw InvalidFieldType();

		return (char*)GetPtr(index);
	}
	/* END TableDataRow */

	/* TableDataIterator */
	TableDataIterator::TableDataIterator(Table* const _data, ITableDataPredicate* const _predicate)
		: table(_data), predicate(_predicate)
	{
		rowIndex = 0;
	}

	bool TableDataIterator::TestCurrentRow() const
	{
		TableDataRow row = GetFullDataRow();

		return (*predicate)(table, row);
	}

	TableDataRow TableDataIterator::GetFullDataRow() const
	{
		return TableDataRow(table->data->GetElementPtr(rowIndex), table->columns);
	}

	bool TableDataIterator::NextRow()
	{
		rowIndexType i = rowIndex + 1;
		while (!TestCurrentRow())
		{
			if (i >= table->rows)
				return false;

			i++;
		}

		rowIndex = i;
		
		return true;
	}

	bool TableDataIterator::PrevRow()
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
	/* END TableDataIterator */
}