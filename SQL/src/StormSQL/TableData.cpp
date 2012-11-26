#include "TableData.h"
#include "Exceptions.h"
#include "Table.h"

namespace StormSQL
{
	/* TableDataRow */
	TableDataRow::TableDataRow(byte* _ptr, const vector<Field>& _columns)
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
}