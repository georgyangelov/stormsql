#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include <string>
#include <fstream>
#include "StormSQL/Table.h"
#include "StormSQL/TableData.h"
#include "Queries/Insert.h"

using namespace std;
using namespace StormSQL;

namespace UnitTests
{		
	TEST_CLASS(TableDataTests)
	{
	public:
		Table GetTableSchema()
		{
			Field f1 = { "test", Field::FieldType::int32, 10 };
			Field f2 = { "test2", Field::FieldType::fixedchar, 30 };

			Table schema;
			schema.AddField(f1);
			schema.AddField(f2);

			return schema;
		}
		Table GetTable()
		{
			Table schema = GetTableSchema();

			// Insert rows
			Queries::Insert insert(&schema);
			insert.SetInt(0, 1234);
			insert.SetString(1, "test string");
			insert.Commit();

			insert.SetInt(0, 5678);
			insert.SetString(1, "another test string");
			insert.Commit();

			return schema;
		}
		
		TEST_METHOD(InsertIterateTest)
		{
			Table table = GetTable();

			// Iterate and verify data
			TableDataIterator<> iter = table.GetIterator();

			iter.NextRow();
			TableDataRow row = iter.GetFullDataRow();
			Assert::AreEqual(1234, row.GetInt32(0));
			Assert::IsTrue(row.GetString(1).compare("test string") == 0);

			iter.NextRow();
			TableDataRow row2 = iter.GetFullDataRow();
			Assert::AreEqual(5678, row2.GetInt32(0));
			Assert::IsTrue(row2.GetString(1).compare("another test string") == 0);
		}

		TEST_METHOD(AddColumnTest)
		{
			Table schema = GetTable();
			
			Field f3 = { "test12", Field::FieldType::int32, 10 };
			schema.AddField(1, f3);

			// Iterate and verify data
			TableDataIterator<> iter = schema.GetIterator();
			
			iter.NextRow();
			TableDataRow row = iter.GetFullDataRow();
			Assert::AreEqual(1234, row.GetInt32(0));
			Assert::AreEqual(row.GetInt32(1), 0);
			Assert::IsTrue(row.GetString(2).compare("test string") == 0);

			iter.NextRow();
			TableDataRow row2 = iter.GetFullDataRow();
			Assert::AreEqual(5678, row2.GetInt32(0));
			Assert::AreEqual(row2.GetInt32(1), 0);
			Assert::IsTrue(row2.GetString(2).compare("another test string") == 0);
		}

		TEST_METHOD(RemoveColumnTest)
		{
			Table schema = GetTable();
			
			schema.RemoveField("test");
			
			Assert::AreEqual(1, schema.GetNumFields());
			Assert::IsTrue(schema.HasField("test2"));
			Assert::IsFalse(schema.HasField("test"));
			
			// Iterate and verify data
			TableDataIterator<> iter = schema.GetIterator();
			
			iter.NextRow();
			TableDataRow row = iter.GetFullDataRow();
			Assert::IsTrue(row.GetString(0).compare("test string") == 0);

			iter.NextRow();
			TableDataRow row2 = iter.GetFullDataRow();
			Assert::IsTrue(row2.GetString(0).compare("another test string") == 0);
		}

	};
}