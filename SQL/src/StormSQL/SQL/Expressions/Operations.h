#ifndef _H_SQL_EXPRESSIONS_OPERATIONS_INCLUDED
#define _H_SQL_EXPRESSIONS_OPERATIONS_INCLUDED

#include <vector>
#include <tuple>
#include <hash_map>
#include <sstream>
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
				virtual Field GetSuitableField(const string& name, const vector<Field>&) const = 0;
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
						throw InvalidNumberOfArguments("and", "2");

					Value v1 = v[0], v2 = v[1];
					return (bool)v1 && (bool)v2;
				}

				Field GetSuitableField(const string& name, const vector<Field>& v) const
				{
					return Field(name.c_str(), Field::FieldType::byte, 0);
				}

				IOperation* Clone() const
				{
					return new And();
				}
			};
			class Or
				: public IOperation
			{
			public:
				Value operator()(const vector<Value>& v) const
				{
					if (v.size() != 2)
						throw InvalidNumberOfArguments("or", "2");

					Value v1 = v[0], v2 = v[1];
					return (bool)v1 || (bool)v2;
				}

				Field GetSuitableField(const string& name, const vector<Field>& v) const
				{
					return Field(name.c_str(), Field::FieldType::byte, 0);
				}

				IOperation* Clone() const
				{
					return new Or();
				}
			};
			class Not
				: public IOperation
			{
			public:
				Value operator()(const vector<Value>& v) const
				{
					if (v.size() != 1)
						throw InvalidNumberOfArguments("!", "1");

					Value v1 = v[0];
					return !(bool)v1;
				}

				Field GetSuitableField(const string& name, const vector<Field>& v) const
				{
					return Field(name.c_str(), Field::FieldType::byte, 0);
				}

				IOperation* Clone() const
				{
					return new Not();
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

				Field GetSuitableField(const string& name, const vector<Field>& v) const
				{
					return Field(name.c_str(), Field::FieldType::byte, 0);
				}

				IOperation* Clone() const
				{
					return new Equals();
				}
			};
			class LowerThan
				: public IOperation
			{
			public:
				Value operator()(const vector<Value>& v) const
				{
					if (v.size() != 2)
						throw InvalidNumberOfArguments("<", "2");

					Value v1 = v[0], v2 = v[1];

					return v1 < v2;
				}

				Field GetSuitableField(const string& name, const vector<Field>& v) const
				{
					return Field(name.c_str(), Field::FieldType::byte, 0);
				}

				IOperation* Clone() const
				{
					return new LowerThan();
				}
			};
			class LowerThanEqual
				: public IOperation
			{
			public:
				Value operator()(const vector<Value>& v) const
				{
					if (v.size() != 2)
						throw InvalidNumberOfArguments("<=", "2");

					Value v1 = v[0], v2 = v[1];

					return v1 <= v2;
				}

				Field GetSuitableField(const string& name, const vector<Field>& v) const
				{
					return Field(name.c_str(), Field::FieldType::byte, 0);
				}

				IOperation* Clone() const
				{
					return new LowerThanEqual();
				}
			};
			class GreaterThan
				: public IOperation
			{
			public:
				Value operator()(const vector<Value>& v) const
				{
					if (v.size() != 2)
						throw InvalidNumberOfArguments(">", "2");

					Value v1 = v[0], v2 = v[1];

					return v1 > v2;
				}

				Field GetSuitableField(const string& name, const vector<Field>& v) const
				{
					return Field(name.c_str(), Field::FieldType::byte, 0);
				}

				IOperation* Clone() const
				{
					return new GreaterThan();
				}
			};
			class GreaterThanEqual
				: public IOperation
			{
			public:
				Value operator()(const vector<Value>& v) const
				{
					if (v.size() != 2)
						throw InvalidNumberOfArguments(">=", "2");

					Value v1 = v[0], v2 = v[1];

					return v1 >= v2;
				}

				Field GetSuitableField(const string& name, const vector<Field>& v) const
				{
					return Field(name.c_str(), Field::FieldType::byte, 0);
				}

				IOperation* Clone() const
				{
					return new GreaterThanEqual();
				}
			};			
			class NotEqual
				: public IOperation
			{
			public:
				Value operator()(const vector<Value>& v) const
				{
					if (v.size() != 2)
						throw InvalidNumberOfArguments("!=", "2");

					Value v1 = v[0], v2 = v[1];

					return v1 != v2;
				}

				Field GetSuitableField(const string& name, const vector<Field>& v) const
				{
					return Field(name.c_str(), Field::FieldType::byte, 0);
				}

				IOperation* Clone() const
				{
					return new NotEqual();
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

				Field GetSuitableField(const string& name, const vector<Field>& v) const
				{
					return Field(name.c_str(), Field::FieldType::int32, 0);
				}

				IOperation* Clone() const
				{
					return new Plus();
				}
			};
			class Minus
				: public IOperation
			{
			public:
				Value operator()(const vector<Value>& v) const
				{
					if (v.size() != 2)
						throw InvalidNumberOfArguments("-", "2");
					
					Value v1 = v[0], v2 = v[1];
					
					return (int)v1 - (int)v2;
				}

				Field GetSuitableField(const string& name, const vector<Field>& v) const
				{
					return Field(name.c_str(), Field::FieldType::int32, 0);
				}

				IOperation* Clone() const
				{
					return new Minus();
				}
			};
			class Multiply
				: public IOperation
			{
			public:
				Value operator()(const vector<Value>& v) const
				{
					if (v.size() != 2)
						throw InvalidNumberOfArguments("*", "2");
					
					Value v1 = v[0], v2 = v[1];
					
					return (int)v1 * (int)v2;
				}

				Field GetSuitableField(const string& name, const vector<Field>& v) const
				{
					return Field(name.c_str(), Field::FieldType::int32, 0);
				}

				IOperation* Clone() const
				{
					return new Multiply();
				}
			};
			class Divide
				: public IOperation
			{
			public:
				Value operator()(const vector<Value>& v) const
				{
					if (v.size() != 2)
						throw InvalidNumberOfArguments("/", "2");
					
					Value v1 = v[0], v2 = v[1];
					
					return (int)v1 / (int)v2;
				}

				Field GetSuitableField(const string& name, const vector<Field>& v) const
				{
					return Field(name.c_str(), Field::FieldType::int32, 0);
				}

				IOperation* Clone() const
				{
					return new Divide();
				}
			};
		
			class ToInt
				: public IOperation
			{
			public:
				Value operator()(const vector<Value>& v) const
				{
					if (v.size() != 1)
						throw InvalidNumberOfArguments("toInt", "1");

					Value v1 = v[0];

					if (v1.type == Value::integer)
						return v1;

					stringstream str((string)v1);
					int tmp;
					str >> tmp;

					return tmp;
				}

				Field GetSuitableField(const string& name, const vector<Field>& v) const
				{
					return Field(name.c_str(), Field::FieldType::int32, 0);
				}

				IOperation* Clone() const
				{
					return new ToInt();
				}
			};
			class ToStr
				: public IOperation
			{
			public:
				Value operator()(const vector<Value>& v) const
				{
					if (v.size() != 1)
						throw InvalidNumberOfArguments("toStr", "1");

					Value v1 = v[0];

					if (v1.type == Value::string)
						return v1;

					stringstream str;
					str << (int)v1;

					return str.str();
				}

				Field GetSuitableField(const string& name, const vector<Field>& v) const
				{
					if (v.size() != 1)
						throw InvalidNumberOfArguments("toStr", "1");

					return Field(name.c_str(), Field::FieldType::fixedchar, v[0].size);
				}

				IOperation* Clone() const
				{
					return new ToStr();
				}
			};
			class ToBool
				: public IOperation
			{
			public:
				Value operator()(const vector<Value>& v) const
				{
					if (v.size() != 1)
						throw InvalidNumberOfArguments("toBool", "1");

					Value v1 = v[0];

					return (bool)v1;
				}

				Field GetSuitableField(const string& name, const vector<Field>& v) const
				{
					return Field(name.c_str(), Field::FieldType::byte, 0);
				}

				IOperation* Clone() const
				{
					return new ToBool();
				}
			};

			class StrCat
				: public IOperation
			{
			public:
				Value operator()(const vector<Value>& v) const
				{
					if (v.size() != 2)
						throw InvalidNumberOfArguments("strcat", "2");

					Value v1 = v[0], v2 = v[1];

					return (string)v1 + (string)v2;
				}

				Field GetSuitableField(const string& name, const vector<Field>& v) const
				{
					if (v.size() != 2)
						throw InvalidNumberOfArguments("strcat", "2");

					return Field(name.c_str(), Field::FieldType::fixedchar, v[0].size + v[1].size);
				}

				IOperation* Clone() const
				{
					return new StrCat();
				}
			};
			class StrLen
				: public IOperation
			{
			public:
				Value operator()(const vector<Value>& v) const
				{
					if (v.size() != 1)
						throw InvalidNumberOfArguments("strlen", "1");

					Value v1 = v[0];

					return ((string)v1).length();
				}

				Field GetSuitableField(const string& name, const vector<Field>& v) const
				{
					return Field(name.c_str(), Field::FieldType::int32, 0);
				}

				IOperation* Clone() const
				{
					return new StrLen();
				}
			};
			class SubStr
				: public IOperation
			{
			public:
				Value operator()(const vector<Value>& v) const
				{
					if (v.size() != 3)
						throw InvalidNumberOfArguments("substr", "3");

					Value v1 = v[0], v2 = v[1], v3 = v[2];

					try
					{
						return ((string)v1).substr((int)v2, (int)v3);
					}
					catch (out_of_range&)
					{
						return "";
					}
				}

				Field GetSuitableField(const string& name, const vector<Field>& v) const
				{
					if (v.size() != 3)
						throw InvalidNumberOfArguments("substr", "3");

					return Field(name.c_str(), Field::FieldType::int32, v[0].size);
				}

				IOperation* Clone() const
				{
					return new SubStr();
				}
			};
		}
	}
}

#endif