#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include <fstream>
#include "StormSQL/Table.h"

using namespace std;
using namespace StormSQL;

namespace UnitTests
{		
	TEST_CLASS(TableSchemaTests)
	{
	public:
		
		TEST_METHOD(AddDeleteTest)
		{
			Field f1("test", Field::FieldType::int32, 10);
			Field f2("test 2 ", Field::FieldType::fixedchar, 15);

			Table schema;
			schema.AddField(f1);
			
			Assert::IsTrue(schema.HasField(f1.name));
			Assert::IsTrue(schema.GetNumFields() == 1);
			
			schema.AddField(f2);

			Assert::IsTrue(schema.HasField(f2.name));
			Assert::IsTrue(schema.GetNumFields() == 2);
			
			schema.RemoveField(f1.name);
			
			Assert::IsFalse(schema.HasField(f1.name));
			Assert::IsTrue(schema.GetNumFields() == 1);
		}

		TEST_METHOD(LoadSaveTest)
		{
			Field f1("test", Field::FieldType::int32, 10);
			Field f2("test 2 ", Field::FieldType::fixedchar, 15);

			ofstream out("test.bin");
			Table schema;
			schema.AddField(f1);
			schema.AddField(f2);

			schema.Store(out);
			out.close();

			ifstream in("test.bin");
			Table schema2;

			schema2.Load(in);

			Assert::IsTrue(schema2.HasField(f1.name));
			Assert::IsTrue(schema2.HasField(f2.name));
			Assert::IsTrue(schema2.GetNumFields() == 2);
		}
		
		TEST_METHOD(CopyConstructorTest)
		{
			Field f1("test", Field::FieldType::int32, 10);
			Field f2("test 2 ", Field::FieldType::fixedchar, 15);

			Table schema;
			schema.AddField(f1);
			schema.AddField(f2);

			Table schema2(schema);
			
			Assert::IsTrue(schema2.HasField(f1.name));
			Assert::IsTrue(schema2.HasField(f2.name));
			Assert::AreEqual(schema2.GetNumFields(), 2);
		}

	};
}