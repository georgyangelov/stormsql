#include "Select.h"
#include "../StormSQL/Exceptions.h"
#include "Insert.h"
#include "../StormSQL/SQL/Expressions/Expression.h"

#include <string>

using namespace std;
using namespace StormSQL;
using namespace StormSQL::SQL;
using namespace StormSQL::Queries;

void Select::del()
{
	delete predicate;
}

void Select::copy(const Select& obj)
{
	db = obj.db;
	tableName = obj.tableName;
	columns = obj.columns;
	includeAllColumns = obj.includeAllColumns;

	predicate = obj.predicate->Clone();
}

Select::Select(Database* _db)
	: db(_db)
{
	includeAllColumns = false;
	predicate = new TruePredicate();
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

	Token t = lex.NextToken("from", TokenType::Keyword);

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
	includeAllColumns = false;

	do
	{
		// <columnName>
		t = lex.NextToken(false);
		string data = t.strData;

		if (t.type == TokenType::Operator && t.strData == "*")
		{
			includeAllColumns = true;
			
			// Should be separator if there are other columns
			t = lex.NextToken(false);

			continue;
		}
		else
		{
			lex.PutBackToken(t);
		}

		// Expression
		// Read the expression
		ExpressionParser p(lex, IOperation::GetStandardOperations());
		Expression* expr = p.Parse();

		t = lex.NextToken();

		if (t.type == TokenType::Keyword && t.strData == "as")
		{
			t = lex.NextToken(TokenType::Identifier, false);
			string name = t.strData;

			columns[name] = expr;

			t = lex.NextToken();
		}
		else
		{
			columns[p.GetLastExpression()] = expr;
		}

	}
	while (t.type == TokenType::Separator);
			
	lex.PutBackToken(t);
}

Table* Select::Execute()
{
	Table* source = &db->GetTable(tableName);
	Table* data = new Table();

	if (includeAllColumns)
	{
		int numCols = source->GetNumFields();
		for (int i = 0; i < numCols; i++)
		{
			Field f = source->GetField(i);

			data->AddField(f);
		}
	}

	hash_map<string, Field> fields = source->GetFields();
	for (hash_map<string, Expression*>::iterator colIter = columns.begin(); colIter != columns.end(); colIter++)
	{
		Field suitableField = colIter->second->GetSuitableField(colIter->first, fields);
		data->AddField(suitableField);

		fields[suitableField.name] = suitableField;
	}

	TableDataIterator iter = source->GetIterator(*predicate);
	while (iter.NextRow())
	{
		TableDataRow row = iter.GetFullDataRow();
		Insert ins(data);
		int columnNum = 0;

		if (includeAllColumns)
		{
			int numCols = source->GetNumFields();
			for (int i = 0; i < numCols; i++)
			{
				ins.Set(columnNum, source->GetFieldType(i), row[i].GetPtr());
							
				columnNum++;
			}
		}

		hash_map<string, Value> fieldsData;
		int numFields = source->GetNumFields();

		for (int i = 0; i < numFields; i++)
		{
			fieldsData[source->GetField(i).name] = row[i];
		}

		for (hash_map<string, Expression*>::iterator colIter = columns.begin(); colIter != columns.end(); colIter++)
		{
			Value val = colIter->second->Compute(fieldsData);
			ins.Set(columnNum, val);

			fieldsData[colIter->first] = val;

			columnNum++;
		}

		ins.Execute();
	}

	return data;
}