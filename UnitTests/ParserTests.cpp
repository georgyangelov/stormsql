#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include <string>
#include <fstream>
#include "StormSQL/SQL/Parser.h"
#include <sstream>
#include "StormSQL/SQL/Expressions/Operations.h"
#include "StormSQL/SQL/Expressions/Expression.h"

using namespace std;
using namespace StormSQL;
using namespace StormSQL::SQL;
using namespace StormSQL::SQL::Expressions;

namespace UnitTests
{		
	TEST_CLASS(ParserTests)
	{
	public:

		TEST_METHOD(CreateTableTest)
		{
			stringstream stream("CREATE TABLE `test` ( `id` int, `num` uint, `b` byte, `str` char(20) )");
			Database db;

			Parser p(stream, &db);

			Query* query = p.ParseQuery();
			
			Assert::IsTrue(query->GetType() == "createTable");

			query->Execute();

			Assert::IsTrue(db.HasTable("test"));
			Assert::IsTrue(db.GetTable("test").GetNumFields() == 4);
			Assert::IsTrue(db.GetTable("test").GetNumRows() == 0);

			delete query;
		}

		TEST_METHOD(ExpressionRPNTest)
		{
			stringstream stream("a + 50 = 35 AND test(`b`, 'c') = true");
			hash_map<string, operationInfo> ops;
			// The operations themselves aren't used here
			ops["+"] = operationInfo(new Plus(), 2, 2, true, false);
			ops["="] = operationInfo(new Equals(), 1, 2, true, false);
			ops["AND"] = operationInfo(new And(), 0, 2, true, false);
			ops["test"] = operationInfo(new And(), 0, 2, true, true);

			Lexer lex(stream);
			ExpressionParser p(lex, ops);

			stringstream rpn = p.GetRPN();
			Assert::AreEqual(" a 50 + 35 = `b` 'c' test true = AND", rpn.str().c_str());
		}

	};
}