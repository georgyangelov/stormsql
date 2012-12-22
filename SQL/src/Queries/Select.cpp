#include "Select.h"
#include "../StormSQL/Exceptions.h"
#include "../StormSQL/SQL/Lexer.h"
#include "Insert.h"
#include "../StormSQL/SQL/Expressions/Expression.h"

#include <string>

using namespace std;

namespace StormSQL
{
	namespace Queries
	{
		void Select::del()
		{
			if (predicate)
				delete predicate;
		}

		void Select::copy(const Select& obj)
		{
			db = obj.db;
			tableName = obj.tableName;
			columns = obj.columns;

			if (obj.predicate)
				predicate = obj.predicate->Clone();
			else
				predicate = NULL;
		}

		Select::Select(Database* _db)
			: db(_db)
		{
		}

		Select::Select(const Select& obj)
		{
			copy(obj);
		}

		Select::~Select()
		{
			del();
		}

		Select& Select::operator = (const Select& obj)
		{
			if (this != &obj)
			{
				del();
				copy(obj);
			}

			return *this;
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

		void Select::SetWhere(const ITableDataPredicate& pred)
		{
			if (predicate)
				delete predicate;

			predicate = pred.Clone();
		}

		/*
		 * SELECT // Read by parser
		 * <columns>
		 * FROM
		 * <table>
		 * (WHERE <expression>)
		 *
		 * <columns> := <columnName>, <columns> | <columnName>
		 * <columnName> := identifier (AS identifier) | *
		 * <table> := identifier
		 */
		void Select::Parse(Lexer& lex)
		{
			// <columns>
			ReadColumns(lex);

			Token t = lex.NextToken(TokenType::Keyword, "from");

			t = lex.NextToken(TokenType::Identifier);
			SetFrom(t.strData);

			if (lex.endOfStream())
				return;

			t = lex.NextToken();
			if (t.type == TokenType::Keyword && t.strData == "where")
			{
				SetWhere(ExpressionPredicate(lex, IOperation::GetStandardOperations()));
			}
			else
			{
				lex.PutBackToken(t);
			}
		}

		void Select::ReadColumns(Lexer& lex)
		{
			Token t;

			do
			{
				// <columnName>
				t = lex.NextToken(false);
				string data = t.strData;

				if (t.type != TokenType::Identifier && t.type != TokenType::Keyword && t.strData != "*")
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
							ins.Set(columnNum, row[i].GetType(), row[i].GetPtr());
							
							columnNum++;
						}
					}
					else
					{
						if (!source->HasField(colIter->second.c_str()))
							throw UnknownTokenException(colIter->second);

						int index = source->GetColumnIndex(colIter->second.c_str());
						
						ins.Set(columnNum, row[index].GetType(), row[index].GetPtr());

						columnNum++;
					}
				}

				ins.Execute();
			}

			return data;
		}
	}
}