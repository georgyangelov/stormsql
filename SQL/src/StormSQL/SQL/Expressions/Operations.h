#ifndef _H_SQL_EXPRESSIONS_OPERATIONS_INCLUDED
#define _H_SQL_EXPRESSIONS_OPERATIONS_INCLUDED

#include <vector>
#include <tuple>
#include <hash_map>
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
			class IOperation;

			struct OperationInfo
			{
			private:
				void copy(const OperationInfo& obj);

			public:
				IOperation* op;
				int priority;
				int arguments;
				bool leftAssoc;
				bool isFunction;

				OperationInfo();
				OperationInfo(const IOperation& _op, int _priority, int _arguments, bool _leftAssoc, bool _isFunction);
				OperationInfo(const OperationInfo& obj);
				~OperationInfo();

				OperationInfo& operator = (const OperationInfo& obj);
			};

			class IOperation
			{
			public:
				static hash_map<string, OperationInfo> GetStandardOperations();

				virtual Value operator () (const vector<Value>&) const = 0;
				virtual IOperation* Clone() const = 0;
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