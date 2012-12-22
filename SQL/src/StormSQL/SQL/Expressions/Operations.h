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

			class IOperation
			{
			public:
				static hash_map<string, OperationInfo> GetStandardOperations()
				{
					hash_map<string, OperationInfo> ops;
					ops["AND"] = OperationInfo(And(), 1, 2, true, false);
					ops["="] = OperationInfo(Equals(), 0, 2, true, false);
					ops["+"] = OperationInfo(Equals(), 4, 2, true, false);

					return ops;
				}

				virtual Value operator () (const vector<Value>&) const = 0;
				virtual IOperation* Clone() const = 0;
			};

			struct OperationInfo
			{
			private:
				void copy(const OperationInfo& obj)
				{
					if (obj.op)
						op = obj.op->Clone();
					else
						op = NULL;

					priority = obj.priority;
					arguments = obj.arguments;
					leftAssoc = obj.leftAssoc;
					isFunction = obj.isFunction;
				}

			public:
				IOperation* op;
				int priority;
				int arguments;
				bool leftAssoc;
				bool isFunction;

				OperationInfo()
				{
					op = NULL;
				}
				
				OperationInfo(const IOperation& _op, int _priority, int _arguments, bool _leftAssoc, bool _isFunction)
					: op(_op.Clone()), priority(_priority), arguments(_arguments), 
					leftAssoc(_leftAssoc), isFunction(_isFunction)
				{
				}

				OperationInfo(const OperationInfo& obj)
				{
					copy(obj);
				}

				~OperationInfo()
				{
					if (op)
						delete op;
				}

				OperationInfo& operator = (const OperationInfo& obj)
				{
					if (this != &obj)
					{
						if (op)
							delete op;
						copy(obj);
					}

					return *this;
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