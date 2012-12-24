#include "stdafx.h"
#include "CppUnitTest.h"
#include "StormSQL/Database.h"
#include "StormSQL/SQL/Parser.h"
#include <sstream>

using namespace std;
using namespace StormSQL;
using namespace StormSQL::SQL;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{
	TEST_CLASS(QueryTests)
	{
	public:

		Database GetDatabase()
		{
			Database db;
			
			stringstream query("CREATE TABLE tests (`id` int, `student` char(200), fnum uint, grade byte)");
			Parser p(query, &db);

			Query* create = p.ParseQuery();
			Assert::IsNotNull(create);
			Assert::AreEqual("createTable", create->GetType().c_str());
			
			Assert::AreEqual(0, (int)db.GetTableNames().size());
			Table* res = create->Execute();
			delete create;
			Assert::IsNull(res);
			Assert::AreEqual(1, (int)db.GetTableNames().size());
			Assert::IsTrue(db.HasTable("tests"));

			Table tbl = db.GetTable("tests");
			Assert::AreEqual(4, tbl.GetNumFields());
			Assert::AreEqual(0, tbl.GetNumRows());

			Assert::IsTrue(tbl.HasField("id"));
			Assert::IsTrue(tbl.GetField(0).type == Field::FieldType::int32);

			Assert::IsTrue(tbl.HasField("student"));
			Assert::IsTrue(tbl.GetField(1).type == Field::FieldType::fixedchar);
			Assert::AreEqual(200, (int)tbl.GetField(1).size);

			Assert::IsTrue(tbl.HasField("fnum"));
			Assert::IsTrue(tbl.GetField(2).type == Field::FieldType::uint32);

			Assert::IsTrue(tbl.HasField("grade"));
			Assert::IsTrue(tbl.GetField(3).type == Field::FieldType::byte);

			return db;
		}

		void InsertData(Database& db, int id, string name, unsigned int fnum, char grade)
		{
			stringstream query;
			query << "INSERT INTO tests VALUES (" << id << ",'" << name << "'," << fnum << "," << (int)grade << ")";

			Parser p(query, &db);

			Query* insert = p.ParseQuery();
			Assert::AreEqual("insert", insert->GetType().c_str());
			
			Table& tbl = db.GetTable("tests");

			int num = tbl.GetNumRows();
			insert->Execute();
			Assert::AreEqual(num + 1, tbl.GetNumRows());

			TableDataIterator iter = tbl.GetIterator();
			while (iter.NextRow()) { } // Skip to last row
			TableDataRow row = iter.GetFullDataRow();
			
			Assert::AreEqual(id, row[0].GetInt32());
			Assert::AreEqual(name.c_str(), row[1].GetString().c_str());
			Assert::AreEqual(fnum, row[2].GetUInt32());
			Assert::AreEqual(grade, row[3].GetChar());
		}

		void InsertTestRows(Database& db)
		{
			InsertData(db, -123, "Georgy Angelov", 1233344, -126);
			InsertData(db, 333123, "Sa6o", 0, 128);
			InsertData(db, 0, "Pe6ooooooo", 11344, 0);
			InsertData(db, 23, "Elvisaaa", 34234, 1);
		}

		TEST_METHOD(CreateTable)
		{
			GetDatabase();
		}
		
		TEST_METHOD(DropTable)
		{
			Database db = GetDatabase();

			stringstream query("DROP TABLE tests");
			Parser p(query, &db);

			Query* dropTable = p.ParseQuery();
			Assert::AreEqual("dropTable", dropTable->GetType().c_str());

			Assert::IsTrue(db.HasTable("tests"));
			dropTable->Execute();
			Assert::IsFalse(db.HasTable("tests"));

			delete dropTable;
		}

		TEST_METHOD(Insert)
		{
			Database db = GetDatabase();

			InsertTestRows(db);
		}

		void VerifyDataRow(TableDataRow row, int id, string name, unsigned int fnum, char grade)
		{
			Assert::AreEqual(id, row[0].GetInt32());
			Assert::AreEqual(name.c_str(), row[1].GetString().c_str());
			Assert::AreEqual(fnum, row[2].GetUInt32());
			Assert::AreEqual(grade, row[3].GetChar());
		}

		TEST_METHOD(Select)
		{
			Database db = GetDatabase();
			InsertTestRows(db);

			stringstream query("SELECT * FROM tests");
			Parser p(query, &db);

			Query* select = p.ParseQuery();
			Assert::AreEqual("select", select->GetType().c_str());

			Table* res = select->Execute();
			Assert::IsNotNull(res);
			Assert::AreEqual(db.GetTable("tests").GetNumRows(), res->GetNumRows());

			TableDataIterator iter = res->GetIterator();
			Assert::IsTrue(iter.NextRow());
			TableDataRow row = iter.GetFullDataRow();
			VerifyDataRow(row, -123, "Georgy Angelov", 1233344, -126);

			Assert::IsTrue(iter.NextRow());
			row = iter.GetFullDataRow();
			VerifyDataRow(row, 333123, "Sa6o", 0, 128);
			
			Assert::IsTrue(iter.NextRow());
			row = iter.GetFullDataRow();
			VerifyDataRow(row, 0, "Pe6ooooooo", 11344, 0);
			
			Assert::IsTrue(iter.NextRow());
			row = iter.GetFullDataRow();
			VerifyDataRow(row, 23, "Elvisaaa", 34234, 1);

			Assert::IsFalse(iter.NextRow());
			row = iter.GetFullDataRow();
			VerifyDataRow(row, 23, "Elvisaaa", 34234, 1);

			Assert::IsTrue(iter.PrevRow());
			row = iter.GetFullDataRow();
			VerifyDataRow(row, 0, "Pe6ooooooo", 11344, 0);

			Assert::IsTrue(iter.PrevRow());
			row = iter.GetFullDataRow();
			VerifyDataRow(row, 333123, "Sa6o", 0, 128);

			Assert::IsTrue(iter.PrevRow());
			row = iter.GetFullDataRow();
			VerifyDataRow(row, -123, "Georgy Angelov", 1233344, -126);

			Assert::IsFalse(iter.PrevRow());
		}

		TEST_METHOD(SelectWhere)
		{
			Database db = GetDatabase();
			InsertTestRows(db);

			stringstream query("SELECT * FROM tests WHERE `student` = 'Georgy Angelov'");
			Parser p(query, &db);

			Query* select = p.ParseQuery();
			Assert::AreEqual("select", select->GetType().c_str());

			Table* res = select->Execute();
			Assert::IsNotNull(res);
			Assert::AreEqual(1, res->GetNumRows());

			TableDataIterator iter = res->GetIterator();
			Assert::IsTrue(iter.NextRow());
			TableDataRow row = iter.GetFullDataRow();
			VerifyDataRow(row, -123, "Georgy Angelov", 1233344, -126);
		}

		TEST_METHOD(SelectWhere2)
		{
			Database db = GetDatabase();
			InsertTestRows(db);

			stringstream query("SELECT * FROM tests WHERE strlen(student) > 1 OR id OR fnum OR grade");
			Parser p(query, &db);

			Query* select = p.ParseQuery();
			Assert::AreEqual("select", select->GetType().c_str());

			Table* res = select->Execute();
			Assert::IsNotNull(res);
			Assert::AreEqual(db.GetTable("tests").GetNumRows(), res->GetNumRows());

			TableDataIterator iter = res->GetIterator();
			Assert::IsTrue(iter.NextRow());
			TableDataRow row = iter.GetFullDataRow();
			VerifyDataRow(row, -123, "Georgy Angelov", 1233344, -126);
		}
	};
}