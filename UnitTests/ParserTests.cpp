#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include <string>
#include <fstream>
#include "StormSQL/SQL/Parser.h"
#include <sstream>

using namespace std;
using namespace StormSQL;
using namespace StormSQL::SQL;

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

			delete query;
		}

	};
}