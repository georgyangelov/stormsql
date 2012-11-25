#include "Field.h"
#include "Exceptions.h"

namespace StormSQL
{
	unsigned int Field::GetByteSize() const
	{
		switch (type)
		{
		case FieldType::byte:
			return sizeof(char);
		case FieldType::int32:
			return sizeof(int);
		case FieldType::uint32:
			return sizeof(unsigned int);
		case FieldType::fixedchar:
			return size;

		default:
			throw InvalidFieldType();
		}
	}

	void Field::WriteToStream(ostream& out) const
	{
		out.write(name, STORM_SQL_FIELD_NAME_SIZE);
		out.write((const char*)&type, sizeof(FieldType));
		out.write((const char*)&size, sizeof(size));
	}

	void Field::ReadFromStream(istream& in)
	{
		in.read(name, STORM_SQL_FIELD_NAME_SIZE);
		in.read((char*)&type, sizeof(FieldType));
		in.read((char*)&size, sizeof(size));
	}
}