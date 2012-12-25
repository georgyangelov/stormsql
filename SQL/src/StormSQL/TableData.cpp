#include "TableData.h"
#include "Exceptions.h"
#include "Table.h"

namespace StormSQL
{
	/* FieldData */
	FieldData::FieldData(const byte* _ptr, const Field& _field)
		: ptr(_ptr), field(_field)
	{
	}

	Field::FieldType FieldData::GetType() const
	{
		return field.type;
	}

	const byte* FieldData::GetPtr() const
	{
		return ptr;
	}

	char FieldData::GetChar() const
	{
		if (field.type != Field::FieldType::byte)
			throw InvalidFieldType();

		return *(char*)ptr;
	}

	int FieldData::GetInt32() const
	{
		if (field.type != Field::FieldType::int32)
			throw InvalidFieldType();

		return *(int*)ptr;
	}
	
	unsigned int FieldData::GetUInt32() const
	{
		if (field.type != Field::FieldType::uint32)
			throw InvalidFieldType();

		return *((unsigned int*)ptr);
	}
	
	string FieldData::GetString() const
	{
		if (field.type != Field::FieldType::fixedchar)
			throw InvalidFieldType();

		return (const char*)ptr;
	}
	/* END FieldData */

	/* TableDataRow */
	TableDataRow::TableDataRow(byte* _ptr, const vector<Field>& _columns)
	{
		byte* res = _ptr;

		for (int i = 0; i < _columns.size(); i++)
		{
			columns.push_back(FieldData(res, _columns[i]));
			res += _columns[i].GetByteSize();
		}
	}

	FieldData& TableDataRow::operator [](int i)
	{
		return columns[i];
	}
	/* END TableDataRow */

	/* TableDataIterator */
	bool TableDataIterator::TestCurrentRow() const
	{
		TableDataRow row = GetFullDataRow();

		return (*predicate)(table, row);
	}

	void TableDataIterator::del()
	{
		delete predicate;
	}

	void TableDataIterator::copy(const TableDataIterator& obj)
	{
		started = obj.started;
		table = obj.table;
		predicate = obj.predicate->Clone();
		rowIndex = obj.rowIndex;
	}
		
	TableDataIterator::TableDataIterator(Table* _table, const ITableDataPredicate& _predicate)
		: table(_table), predicate(_predicate.Clone())
	{
		started = false;
		rowIndex = -1;
	}

	TableDataIterator::TableDataIterator(const TableDataIterator& obj)
	{
		copy(obj);
	}
	TableDataIterator::~TableDataIterator()
	{
		del();
	}

	TableDataIterator& TableDataIterator::operator = (const TableDataIterator& obj)
	{
		if (this != &obj)
		{
			del();
			copy(obj);
		}

		return *this;
	}

	// Get data
	TableDataRow TableDataIterator::GetFullDataRow() const
	{
		return TableDataRow(table->data->GetElementPtr(rowIndex), table->columns);
	}

	rowIndexType TableDataIterator::GetRowIndex() const
	{
		return rowIndex;
	}

	// Move pointer
	bool TableDataIterator::NextRow()
	{
		rowIndexType lastIndex = rowIndex;
		if (!started)
		{
			rowIndex = 0;
			started = true;
		}
		else
		{
			rowIndex++;
		}

		if (rowIndex >= table->rows)
		{
			rowIndex = lastIndex;
			return false;
		}

		while (!TestCurrentRow())
		{
			rowIndex++;

			if (rowIndex >= table->rows)
			{
				rowIndex = lastIndex;
				return false;
			}
		}

		return true;
	}

	bool TableDataIterator::PrevRow()
	{
		if (rowIndex == 0)
		{
			return false;
		}
		
		rowIndex--;

		while (!TestCurrentRow())
		{
			if (rowIndex == 0)
			{
				return false;
			}

			rowIndex--;
		}

		return true;
	}
}