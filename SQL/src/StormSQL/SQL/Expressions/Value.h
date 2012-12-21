#ifndef _H_SQL_EXPRESSIONS_VALUE_INCLUDED
#define _H_SQL_EXPRESSIONS_VALUE_INCLUDED

#include "../../Exceptions.h"
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
				enum Type
				{
					integer,
					string
				};

				int intVal;
				std::string strVal;

				Type type;
				
				Value(int _int)
					: intVal(_int), type(Type::integer)
				{
				}

				Value(std::string _str)
					: strVal(_str), type(Type::string)
				{
				}

				operator bool()
				{
					if (type == integer)
						return intVal != 0;
					else
						return strVal.size() > 0;
				}

				operator int()
				{
					if (type != Value::integer)
						throw InvalidType("int");

					return intVal;
				}

				operator std::string()
				{
					if (type != Value::string)
						throw InvalidType("str");

					return strVal;
				}

				

			bool operator ==(const Value& v2)
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
			};
		}
	}
}

#endif