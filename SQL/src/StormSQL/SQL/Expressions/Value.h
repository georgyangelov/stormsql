#ifndef _H_SQL_EXPRESSIONS_VALUE_INCLUDED
#define _H_SQL_EXPRESSIONS_VALUE_INCLUDED

#include "../../Exceptions.h"
#include <string>

using namespace std;

namespace StormSQL
{
	namespace Queries
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
						return intVal;
					else
						return strVal.size() > 0;
				}

				operator int()
				{
					if (type != integer)
						throw InvalidType("int");

					return intVal;
				}

				operator std::string()
				{
					if (type != string)
						throw InvalidType("str");

					return strVal;
				}
			};
		}
	}
}

#endif