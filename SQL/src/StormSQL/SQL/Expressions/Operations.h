#ifndef _H_SQL_EXPRESSIONS_OPERATIONS_INCLUDED
#define _H_SQL_EXPRESSIONS_OPERATIONS_INCLUDED

#include <vector>
#include <tuple>
#include "Value.h"
#include "../../Exceptions.h"

using namespace std;

namespace StormSQL
{
	namespace SQL
	{
		namespace Expressions
		{
			// Forward declarations
			class Expression;

			class IOperation
			{
			public:
				virtual Value operator () (const vector<Expression*>&) const = 0;
			};

			class InvalidNumberOfArguments
				: public Exception
			{
			public:
				InvalidNumberOfArguments(string func, string num)
					: Exception("Invalid number of arguments for function " + func + ". " + num + " expected")
				{
				}
			};

			class And
				: public IOperation
			{
			public:
				Value operator()(const vector<Expression*>& v) const
				{
					if (v.size() != 2)
						throw InvalidNumberOfArguments("and", "2");

					return (bool)v[0] && (bool)v[1];
				}
			};

			typedef tuple<string, IOperation*, int, int> operationInfo; 
			operationInfo operations[] = {
				operationInfo("and", new And(), 2, 1)
			};
			const int numOperations = 1;
		}
	}
}

#endif