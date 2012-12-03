#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include <string>
#include <fstream>
#include "StormSQL/SQL/Lexer.h"
#include <sstream>

using namespace std;
using namespace StormSQL;
using namespace StormSQL::SQL;

namespace UnitTests
{		
	TEST_CLASS(LexerTests)
	{
	public:

		TEST_METHOD(LexerTest1)
		{
			stringstream stream("SELECT * FROM `tableName` WHERE `id` = 1, user = 'pe6o'");

			Lexer lex(stream);

			Token t = lex.NextToken();
			Assert::IsTrue(TokenType::Keyword == t.type);
			Assert::AreEqual("select", t.strData.c_str());

			t = lex.NextToken();
			Assert::IsTrue(TokenType::Operator == t.type);
			Assert::AreEqual("*", t.strData.c_str());

			t = lex.NextToken();
			Assert::IsTrue(TokenType::Keyword == t.type);
			Assert::AreEqual("from", t.strData.c_str());

			t = lex.NextToken();
			Assert::IsTrue(TokenType::Identifier == t.type);
			Assert::AreEqual("tableName", t.strData.c_str());

			t = lex.NextToken();
			Assert::IsTrue(TokenType::Keyword == t.type);
			Assert::AreEqual("where", t.strData.c_str());

			t = lex.NextToken();
			Assert::IsTrue(TokenType::Identifier == t.type);
			Assert::AreEqual("id", t.strData.c_str());

			t = lex.NextToken();
			Assert::IsTrue(TokenType::Operator == t.type);
			Assert::AreEqual("=", t.strData.c_str());

			t = lex.NextToken();
			Assert::IsTrue(TokenType::IntValue == t.type);
			Assert::AreEqual("1", t.strData.c_str());
			Assert::AreEqual((long)1, t.longIntData);

			t = lex.NextToken();
			Assert::IsTrue(TokenType::Separator == t.type);
			Assert::AreEqual(",", t.strData.c_str());

			t = lex.NextToken();
			Assert::IsTrue(TokenType::Keyword == t.type);
			Assert::AreEqual("user", t.strData.c_str());

			t = lex.NextToken();
			Assert::IsTrue(TokenType::Operator == t.type);
			Assert::AreEqual("=", t.strData.c_str());

			t = lex.NextToken();
			Assert::IsTrue(TokenType::StringValue == t.type);
			Assert::AreEqual("pe6o", t.strData.c_str());
			
		}

	};
}