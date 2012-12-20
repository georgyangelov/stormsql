#include "Select.h"
#include "../StormSQL/Exceptions.h"
#include "../StormSQL/SQL/Lexer.h"
#include "Insert.h"

#include <string>

using namespace std;

namespace StormSQL
{
	namespace Queries
	{
		Select::Select(Database* _db)
			: db(_db)
		{
		}

		Select::~Select()
		{
		}

		string Select::GetType() const
		{
			return "select";
		}

		void Select::AddColumn(string name, string column)
		{
			columns[name] = column;
		}

		void Select::SetFrom(string tbl)
		{
			tableName = tbl;
		}

		/*
		 * SELECT // Read by parser
		 * <columns>
		 * FROM
		 * <table>
		 * //(WHERE <expressions>)
		 *
		 * <columns> := <columnName>, <columns> | <columnName>
		 * <columnName> := identifier (AS identifier) | *
		 * <table> := identifier
		 * //<expressions> := <boolExpr> | <boolExpr> (AND|OR) <expressions>
		 * //<boolExpr> := <boolValue> | <boolValue> (==|<=|<|>|>=|!=) <boolValue>
		 */
		void Select::Parse(Lexer& lex)
		{
			// <columns>
			ReadColumns(lex);

			Token t = lex.NextToken(TokenType::Keyword, "from");

			t = lex.NextToken(TokenType::Identifier);
			SetFrom(t.strData);
		}

		void Select::ReadColumns(Lexer& lex)
		{
			Token t;

			do
			{
				// <columnName>
				t = lex.NextToken(false);
				string data = t.strData;

				if (t.type != TokenType::Identifier && t.strData != "*")
					throw InvalidTokenException(t);

				t = lex.NextToken();

				if (t.type == TokenType::Keyword && t.strData == "as")
				{
					t = lex.NextToken(TokenType::Identifier, false);
					string name = t.strData;

					AddColumn(name, data);

					t = lex.NextToken();
				}
				else
				{
					AddColumn(data, data);
				}

			}
			while (t.type == TokenType::Separator);
			
			lex.PutBackToken(t);
		}

		Table* Select::Execute()
		{
			Table* source = &db->GetTable(tableName);
			Table* data = new Table();

			for (hash_map<string, string>::iterator colIter = columns.begin(); colIter != columns.end(); colIter++)
			{
				if (colIter->second == "*")
				{
					int numCols = source->GetNumFields();
					for (int i = 0; i < numCols; i++)
					{
						Field f = source->GetField(i);

						data->AddField(f);
					}
				}
				else
				{
					if (!source->HasField(colIter->second.c_str()))
						throw UnknownTokenException(colIter->second);

					int index = source->GetColumnIndex(colIter->second.c_str());
					Field f = source->GetField(index);

					data->AddField(Field(colIter->first.c_str(), f.type, f.size));
				}
			}

			TableDataIterator<> iter = source->GetIterator();
			while (iter.NextRow())
			{
				TableDataRow row = iter.GetFullDataRow();
				Insert ins(data);
				int columnNum = 0;

				for (hash_map<string, string>::iterator colIter = columns.begin(); colIter != columns.end(); colIter++)
				{
					if (colIter->second == "*")
					{
						int numCols = source->GetNumFields();
						for (int i = 0; i < numCols; i++)
						{
							ins.Set(columnNum, row.GetType(i), row.GetPtr(i));
							
							columnNum++;
						}
					}
					else
					{
						if (!source->HasField(colIter->second.c_str()))
							throw UnknownTokenException(colIter->second);

						int index = source->GetColumnIndex(colIter->second.c_str());
						
						ins.Set(columnNum, row.GetType(index), row.GetPtr(index));

						columnNum++;
					}
				}

				ins.Execute();
			}

			return data;
		}
	}
}