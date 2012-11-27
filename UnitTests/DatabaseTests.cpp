#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include <string>
#include <fstream>
#include "StormSQL/Table.h"
#include "StormSQL/TableData.h"
#include "Queries/Insert.h"
#include "StormSQL/Database.h"

using namespace std;
using namespace StormSQL;
using namespace StormSQL::Queries;

namespace UnitTests
{		
	TEST_CLASS(DatabaseTests)
	{
	public:
		Database GetDatabase()
		{
			Database db;

			Field f1 = { "testInt", Field::FieldType::int32, 10 };
			Field f2 = { "testStr", Field::FieldType::fixedchar, 30 };

			Table schema1;
			schema1.AddField(f1);
			schema1.AddField(f2);

			db.CreateTable("table1", schema1);

			Assert::IsTrue(db.HasTable("table1"));
			Assert::AreEqual(2, db.GetTable("table1").GetNumFields());

			Field f3 = { "strTest", Field::FieldType::fixedchar, 30 };
			Field f4 = { "intTest", Field::FieldType::int32, 10 };

			Table schema2;
			schema2.AddField(f3);
			schema2.AddField(f4);

			db.CreateTable("table2", schema2);

			Assert::IsTrue(db.HasTable("table2"));
			Assert::AreEqual(2, db.GetTable("table2").GetNumFields());

			return db;
		}

		void InsertData(Database& db)
		{
			Insert insert(&db.GetTable("table1"));
			insert.SetInt(0, 1);
			insert.SetString(1, "testString");
			insert.Commit();

			insert.SetInt(0, 2);
			insert.SetString(1, "anotherTest");
			insert.Commit();

			Insert insert2(&db.GetTable("table2"));
			insert2.SetString(0, "stringTest");
			insert2.SetInt(1, 3);
			insert2.Commit();

			insert2.SetString(0, "anotherTest");
			insert2.SetInt(1, 4);
			insert2.Commit();
		}

		TEST_METHOD(DropTableTest)
		{
			Database db = GetDatabase();
			
			db.DropTable("table1");
			Assert::IsFalse(db.HasTable("table1"));

			db.DropTable("table2");
			Assert::IsFalse(db.HasTable("table2"));
		}

		TEST_METHOD(GetTableNamesTest)
		{
			Database db = GetDatabase();

			vector<string> names = db.GetTableNames();

			Assert::AreEqual(2, (int)names.size());
			Assert::IsTrue(names[0] == "table1");
			Assert::IsTrue(names[1] == "table2");
		}

		TEST_METHOD(LoadStoreTest)
		{
			Database db = GetDatabase();
			InsertData(db);

			ofstream out("testDb.bin");
			db.Store(out);
			out.close();

			Database db2;
			
			ifstream in("testDb.bin");
			db2.Load(in);
			in.close();

			// Iterate and verify data
			TableDataIterator<> iter = db2.GetTable("table1").GetIterator();
			
			iter.NextRow();
			TableDataRow row = iter.GetFullDataRow();
			Assert::AreEqual(1, row.GetInt32(0));
			Assert::IsTrue(row.GetString(1) == "testString");

			iter.NextRow();
			row = iter.GetFullDataRow();
			Assert::AreEqual(2, row.GetInt32(0));
			Assert::IsTrue(row.GetString(1) == "anotherTest");

			// Iterate and verify data
			iter = db2.GetTable("table2").GetIterator();
			
			iter.NextRow();
			row = iter.GetFullDataRow();
			Assert::IsTrue(row.GetString(0) == "stringTest");
			Assert::AreEqual(3, row.GetInt32(1));

			iter.NextRow();
			row = iter.GetFullDataRow();
			Assert::IsTrue(row.GetString(0) == "anotherTest");
			Assert::AreEqual(4, row.GetInt32(1));
		}

	};
}