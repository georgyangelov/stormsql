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
				virtual Value operator () (const vector<Value>&) const = 0;
				virtual IOperation* Clone() const = 0;
			};

			struct operationInfo
			{
				IOperation* op;
				int priority;
				int arguments;
				bool leftAssoc;
				bool isFunction;

				operationInfo() { };
				operationInfo(IOperation* _op, int _priority, int _arguments, bool _leftAssoc, bool _isFunction)
					: op(_op), priority(_priority), arguments(_arguments), 
					  leftAssoc(_leftAssoc), isFunction(_isFunction)
				{
				}
			};

			class InvalidNumberOfArguments
				: public Exception
			{
			public:
				InvalidNumberOfArguments(string func)
					: Exception("Invalid number of arguments for function " + func)
				{
				}

				InvalidNumberOfArguments(string func, string num)
					: Exception("Invalid number of arguments for function " + func + ". " + num + " expected")
				{
				}
			};

			class And
				: public IOperation
			{
			public:
				Value operator()(const vector<Value>& v) const
				{
					if (v.size() != 2)
						throw InvalidNumberOfArguments("AND", "2");

					Value v1 = v[0], v2 = v[1];
					return (bool)v1 && (bool)v2;
				}

				IOperation* Clone() const
				{
					return new And();
				}
			};

			class Equals
				: public IOperation
			{
			public:
				Value operator()(const vector<Value>& v) const
				{
					if (v.size() != 2)
						throw InvalidNumberOfArguments("=", "2");

					Value v1 = v[0], v2 = v[1];
					return v1 == v2;
				}

				IOperation* Clone() const
				{
					return new Equals();
				}
			};

			class Plus
				: public IOperation
			{
			public:
				Value operator()(const vector<Value>& v) const
				{
					if (v.size() != 2)
						throw InvalidNumberOfArguments("+", "2");
					
					Value v1 = v[0], v2 = v[1];
					return (int)v1 + (int)v2;
				}

				IOperation* Clone() const
				{
					return new Plus();
				}
			};
		}
	}
}

#endif