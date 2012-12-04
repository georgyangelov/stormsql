#include "CreateTable.h"
#include "../StormSQL/Database.h"
#include "../StormSQL/Exceptions.h"

namespace StormSQL
{
	namespace Queries
	{
		CreateTable::CreateTable(Database* _db)
			: db(_db)
		{
		}

		string CreateTable::GetType() const
		{
			return "createTable";
		}

		void CreateTable::SetName(string _name)
		{
			name = _name;
		}

		void CreateTable::AddField(const Field& field)
		{
			tbl.AddField(field);
		}

		Table* CreateTable::Execute()
		{
			db->CreateTable(name, tbl);

			return NULL;
		}

		/*
		 * CREATE TABLE <name> (
		 *    <columnDefinitions>
		 * )
		 *
		 * <name> == <columnName> == identifier
		 * <columnDefinitions> := <columnName> <columnType> [, <columnDefinitions>]
		 * <columnType> := byte | int | uint | char(<length>)
		 * <length> == int
		 */
		void CreateTable::Parse(Lexer& lex)
		{
			Token t;
			
			// <name>
			t = lex.NextToken(TokenType::Identifier, false);
			name = t.strData;
			
			// (
			lex.NextToken(TokenType::Parenthesis, "(");

			string tableName;
			// While not )
			do
			{
				Field f = parseField(lex);
				AddField(f);
			
				// , or )
				t = lex.NextToken();
			}
			while (t.type == TokenType::Separator);

			if (t.type != TokenType::Parenthesis || t.strData != ")")
				throw InvalidTokenException(t);
		}

		Field CreateTable::parseField(Lexer& lex)
		{
			Field res;
			res.size = 0;
			Token t;

			// <columnName>
			t = lex.NextToken(TokenType::Identifier, false);
			
			if (t.strData.size() >= STORM_SQL_FIELD_NAME_SIZE)
				throw NameTooLong();

			strcpy(res.name, t.strData.c_str());

			// <columnType>
			t = lex.NextToken(TokenType::Keyword);

			if (t.strData == "byte")
				res.type = Field::FieldType::byte;
			else if (t.strData == "int")
				res.type = Field::FieldType::int32;
			else if (t.strData == "uint")
				res.type = Field::FieldType::uint32;
			else if (t.strData == "char")
			{
				res.type = Field::FieldType::fixedchar;

				// (<length>)
				lex.NextToken(TokenType::Parenthesis, "(");
				t = lex.NextToken(TokenType::IntValue);
				lex.NextToken(TokenType::Parenthesis, ")");

				res.size = t.longIntData;
			}
			else
				throw InvalidTokenException(t);

			return res;
		}
	}
}