#include "stdafx.h"
#include "CppUnitTest.h"
#include <string>
#include <sstream>
#include "StormSQL/SQL/Expressions/Expression.h"
#include "StormSQL/SQL/Expressions/Operations.h"
#include <hash_map>

using namespace std;
using namespace StormSQL::SQL::Expressions;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Microsoft
{
	namespace VisualStudio
	{
		namespace CppUnitTestFramework
		{
			template<>
			static std::wstring ToString<Value>(const Value& val)
			{
				wstringstream str;
				str << val.type << L"--";

				if (val.type == Value::Type::integer)
					str << (int)val;
				else if (val.type == Value::Type::string)
					str << ((string)val).c_str();
				else
					Assert::Fail(L"Invalid Value::Type");

				return str.str();
			}
		}
	}
}

namespace UnitTests
{
	TEST_CLASS(OperationTests)
	{
	public:
		void TestOperation(string expr, const hash_map<string, Value>& vars, Value expected)
		{
			stringstream str(expr);
			Lexer l(str);
			ExpressionParser p(&l, IOperation::GetStandardOperations());

			Expression* e = p.Parse();

			Value res = e->Compute(vars);
			Assert::AreEqual(expected, res);

			delete e;
		}

		void TestOperation(string expr, Value expected)
		{
			TestOperation(expr, hash_map<string, Value>(), expected);
		}

		TEST_METHOD(TrueAndFalse)
		{
			TestOperation("true", 1);
			TestOperation("tRuE", 1);
			TestOperation("false", 0);
			TestOperation("FaLSe", 0);
		}

		TEST_METHOD(StringToBool)
		{
			TestOperation("'str' and 'str2'", 1);
			TestOperation("'str' and ''", 0);
		}
		
		TEST_METHOD(And)
		{
			TestOperation("0 and 0", 0);
			TestOperation("1 and 0", 0);
			TestOperation("0 and 1", 0);
			TestOperation("1 and 1", 1);
			TestOperation("1 and 1 and 1 and 1", 1);
			TestOperation("1 and 1 and 0 and 1", 0);
		}
		
		TEST_METHOD(Or)
		{
			TestOperation("0 or 0", 0);
			TestOperation("1 or 0", 1);
			TestOperation("0 or 1", 1);
			TestOperation("1 or 1", 1);
			TestOperation("1 and 1 or 1 and 0", 1);
			TestOperation("0 or 0 and 0 or 1", 1);
			TestOperation("(0 or 0) and (0 or 1)", 0);
		}

		TEST_METHOD(Not)
		{
			TestOperation("!0", 1);
			TestOperation("!true", 0);
			TestOperation("!false", 1);
		}

		TEST_METHOD(Mathematical)
		{
			TestOperation("1 + 2 * 25 / 2 - 1", 25);
		}

		TEST_METHOD(Comparisons)
		{
			TestOperation("'a' < 'z' and 'a' <= 'z'", 1);
			TestOperation("0 < 5 and -5 < -1 and -6 <= 0 and 0 <= 0", 1);
			TestOperation("0 = 1 or -1 = 10 or -1 >= 10 or 'a' > 'z' or 'b' >= 'g'", 0);
			TestOperation("1 = 1 and 5 = 5 and 1233456 = 1233456", 1);
		}

		TEST_METHOD(Conversions)
		{
			TestOperation("toInt(123) + 1", 124);
			TestOperation("toInt('1234') - 1", 1233);
			TestOperation("toStr(1234)", (string)"1234");
			TestOperation("toStr('123')", (string)"123");
			TestOperation("toBool('false')", 1);
			TestOperation("toBool('')", 0);
			TestOperation("toBool(true)", 1);
			TestOperation("toBool(false)", 0);
		}

		TEST_METHOD(StringFuncs)
		{
			TestOperation("strcat(strcat('test', '2'), 'go')", (string)"test2go");
			TestOperation("strlen('test2')", 5);
			TestOperation("strlen('')", 0);
			TestOperation("substr('test2tes', 4, 4)", (string)"2tes");

			hash_map<string, Value> vars;
			vars["testVar"] = (string)"test";
			TestOperation("strcat(testVar, toStr(strlen(testVar)))", vars, (string)"test4");
		}
	};
}