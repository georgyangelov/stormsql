#include "Field.h"
#include "Exceptions.h"

namespace StormSQL
{
	Field::Field()
	{
	}
	
	Field::Field(const char* _name, FieldType _type, unsigned int _size)
		: type(_type), size(_size)
	{
		if (strlen(_name) >= STORM_SQL_FIELD_NAME_SIZE)
			throw NameTooLong();

		strcpy(name, _name);
	}

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

	string Field::GetStringType() const
	{
		switch (type)
		{
		case byte:
			return "byte";
		case int32:
			return "int";
		case uint32:
			return "uint";
		case fixedchar:
			return "char";

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