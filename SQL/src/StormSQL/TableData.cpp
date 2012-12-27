#include "TableData.h"
#include "Exceptions.h"
#include "Table.h"

namespace StormSQL
{
	/* FieldData */
	FieldData::FieldData(byte* _ptr, const Field& _field)
		: ptr(_ptr), field(_field)
	{
	}

	Field::FieldType FieldData::GetType() const
	{
		return field.type;
	}

	const Field& FieldData::GetField() const
	{
		return field;
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

	void FieldData::Set(const FieldData& value)
	{
		if (field.type != value.field.type)
			throw InvalidFieldType();

		switch (field.type)
		{
		case Field::FieldType::byte:
			SetByte(value.GetChar());
			break;
		case Field::FieldType::int32:
			SetInt(value.GetInt32());
			break;
		case Field::FieldType::uint32:
			SetUInt(value.GetUInt32());
			break;
		case Field::FieldType::fixedchar:
			SetString(value.GetString());
			break;
		default:
			throw InvalidFieldType();
		}
	}

	void FieldData::Set(const Value& value)
	{
		switch (field.type)
		{
		case Field::FieldType::byte:
			SetByte((int)value);
			break;
		case Field::FieldType::int32:
			SetInt((int)value);
			break;
		case Field::FieldType::uint32:
			SetUInt((int)value);
			break;
		case Field::FieldType::fixedchar:
			SetString((string)value);
			break;
		default:
			throw InvalidFieldType();
		}
	}

	void FieldData::Set(Field::FieldType type, const byte* ptr)
	{
		if (field.type != type)
			throw InvalidFieldType();

		switch (type)
		{
		case Field::FieldType::byte:
			SetByte(*(char*)ptr);
			break;
		case Field::FieldType::int32:
			SetInt(*(int*)ptr);
			break;
		case Field::FieldType::uint32:
			SetUInt(*(unsigned int*)ptr);
			break;
		case Field::FieldType::fixedchar:
			SetString((const char*)ptr);
			break;
		default:
			throw InvalidFieldType();
		}
	}

	void FieldData::SetByte(byte value)
	{
		if (field.type != Field::byte)
			throw InvalidFieldType();

		*ptr = value;
	}

	void FieldData::SetInt(int value)
	{
		if (field.type != Field::int32)
			throw InvalidFieldType();

		*((int*)ptr) = value;
	}
		
	void FieldData::SetUInt(unsigned int value)
	{
		if (field.type != Field::uint32)
			throw InvalidFieldType();

		*((unsigned int*)ptr) = value;
	}

	void FieldData::SetString(string value)
	{
		if (field.type != Field::fixedchar)
			throw InvalidFieldType();

		if (value.size() >= field.size)
			throw FieldDataTooLarge();

		strcpy((char*)ptr, value.c_str());
	}
	/* END FieldData */

	/* TableDataRow */
	TableDataRow::TableDataRow(byte* _ptr, const vector<Field>& _columns)
	{
		AppendRow(_ptr, _columns);
	}

	void TableDataRow::AppendRow(byte* _ptr, const vector<Field>& _columns)
	{
		byte* res = _ptr;

		for (int i = 0; i < _columns.size(); i++)
		{
			columns.push_back(FieldData(res, _columns[i]));
			res += _columns[i].GetByteSize();
		}
	}

	void TableDataRow::Append(const TableDataRow& row)
	{
		for (int i = 0; i < row.columns.size(); i++)
		{
			columns.push_back(row.columns[i]);
		}
	}

	int TableDataRow::GetNumFields() const
	{
		return columns.size();
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

		return (*predicate)(row);
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
	bool TableDataIterator::Seek(rowIndexType index)
	{
		if (index < 0 || index >= table->rows)
			throw out_of_range("The index is out of bounds of the table");

		rowIndex = index;

		return TestCurrentRow();
	}

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