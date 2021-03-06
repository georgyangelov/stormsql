#ifndef _H_SQL_EXPRESSIONS_VALUE_INCLUDED
#define _H_SQL_EXPRESSIONS_VALUE_INCLUDED

#include "../../Exceptions.h"
#include "../../TableData.h"
#include <string>

using namespace std;

namespace StormSQL
{
	namespace SQL
	{
		namespace Expressions
		{
			class InvalidType
				: public Exception
			{
			public:
				InvalidType(string type)
					: Exception("Invalid type of expression. Expected " + type)
				{
				}
			};

			struct Value
			{
			private:
				void copy(const FieldData& field)
				{
					switch (field.GetType())
					{
					case Field::FieldType::byte:
						intVal = field.GetChar();
						type = Type::integer;
						break;
					case Field::FieldType::int32:
						intVal = field.GetInt32();
						type = Type::integer;
						break;
					case Field::FieldType::uint32:
						//TODO: Safer conversion (no conversion at all)
						intVal = field.GetUInt32();
						type = Type::integer;
						break;
					case Field::FieldType::fixedchar:
						strVal = field.GetString();
						type = Type::string;
						break;
					default:
						throw InvalidFieldType();
					}
				}

			public:

				enum Type
				{
					integer,
					string
				};

				int intVal;
				std::string strVal;

				Type type;

				Value()
					: intVal(0), type(Type::integer)
				{
				}
				
				Value(int _int)
					: intVal(_int), type(Type::integer)
				{
				}

				Value(std::string _str)
					: strVal(_str), type(Type::string)
				{
				}

				Value(const FieldData& field)
				{
					copy(field);
				}

				const Value& operator = (const FieldData& field)
				{
					copy(field);

					return *this;
				}

				operator bool() const
				{
					if (type == integer)
						return intVal != 0;
					else
						return strVal.length() > 0;
				}

				operator int() const
				{
					if (type != Value::integer)
						throw InvalidType("int");

					return intVal;
				}

				operator std::string() const
				{
					if (type != Value::string)
						throw InvalidType("str");

					return strVal;
				}

				bool operator ==(const Value& v2) const
				{
					if (type != v2.type)
						return false;

					if (type == Value::integer)
						return intVal == v2.intVal;
					else if (type == Value::string)
						return strVal == v2.strVal;
					else
						throw runtime_error("Unknown Value type");
				}

				bool operator < (const Value& v2) const
				{
					if (type != v2.type)
						return false;

					if (type == Value::integer)
						return intVal < v2.intVal;
					else if (type == Value::string)
						return strVal.compare(v2.strVal) == -1;
					else
						throw runtime_error("Unknown Value type");
				}

				bool operator != (const Value& v2) const
				{
					return !(*this == v2);
				}

				bool operator <= (const Value& v2) const
				{
					return *this < v2 || *this == v2;
				}

				bool operator > (const Value& v2) const
				{
					return !(*this < v2 || *this == v2);
				}

				bool operator >= (const Value& v2) const
				{
					return !(*this < v2);
				}

				Field GetSuitableField(std::string name) const
				{
					if (type == Type::integer)
						return Field(name.c_str(), Field::FieldType::int32, 0);
					else if (type == Type::string)
						return Field(name.c_str(), Field::FieldType::fixedchar, name.length() + 1);
					else
						throw runtime_error("Unknown Value type");
				}
			};
		}
	}
}

#endif