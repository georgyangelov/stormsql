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
}