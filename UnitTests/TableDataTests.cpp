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
			Field f2 = { "test 2 ", Field::FieldType::fixedchar, 30 };

			Table schema;
			schema.AddField(f1);
			schema.AddField(f2);

			return schema;
		}
		
		TEST_METHOD(InsertIterateTest)
		{
			Table schema = GetTableSchema();

			// Insert rows
			Queries::Insert insert(&schema);
			insert.SetInt(0, 1234);
			insert.SetString(1, "test string");
			insert.Commit();

			Queries::Insert insert2(&schema);
			insert2.SetInt(0, 5678);
			insert2.SetString(1, "another test string");
			insert2.Commit();

			// Iterate and verify data
			TableDataIterator<TruePredicate> iter = schema.GetIterator(TruePredicate());
			//TableDataIterator<TruePredicate> iter(&schema, TruePredicate());

			iter.NextRow();
			TableDataRow row = iter.GetFullDataRow();
			Assert::AreEqual(1234, row.GetInt32(0));
			Assert::IsTrue(row.GetString(1).compare("test string") == 0);

			iter.NextRow();
			TableDataRow row2 = iter.GetFullDataRow();
			Assert::AreEqual(5678, row2.GetInt32(0));
			Assert::IsTrue(row2.GetString(1).compare("another test string") == 0);
		}

	};
}