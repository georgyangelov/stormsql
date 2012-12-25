#include "Update.h"
#include "../StormSQL/Exceptions.h"
#include "../StormSQL/SQL/Expressions/Expression.h"

#include <string>

using namespace std;
using namespace StormSQL;
using namespace StormSQL::SQL;
using namespace StormSQL::Queries;

void Update::del()
{
	delete predicate;
}

void Update::copy(const Update& obj)
{
	db = obj.db;
	tableName = obj.tableName;
	columns = obj.columns;

	predicate = obj.predicate->Clone();
}

Update::Update(Database* _db)
	: db(_db)
{
	predicate = new TruePredicate();
}

Update::Update(const Update& obj)
{
	copy(obj);
}

Update::~Update()
{
	del();
}

Update& Update::operator = (const Update& obj)
{
	if (this != &obj)
	{
		del();
		copy(obj);
	}

	return *this;
}

string Update::GetType() const
{
	return "update";
}

void Update::SetFrom(string tbl)
{
	tableName = tbl;
}

void Update::SetWhere(const ITableDataPredicate& pred)
{
	if (predicate)
		delete predicate;

	predicate = pred.Clone();
}

/*
	* Update // Read by parser
	* <table>
	* SET
	* [<column>, <column>, ...]
	* WHERE <expression>
	*
	* <column> := <columnName> = <expression>
	* <table> := identifier
	*/
void Update::Parse(Lexer& lex)
{
	Token t;

	t = lex.NextToken(TokenType::Identifier);
	SetFrom(t.strData);

	// <column>s
	t = lex.NextToken("set", TokenType::Keyword);
	ReadColumns(lex);

	// Don't allow UPDATE without WHERE clause
	//if (lex.endOfStream())
	//	return;

	t = lex.NextToken("where", TokenType::Keyword);
	//if (t.type == TokenType::Keyword && t.strData == "where")
	//{
	SetWhere(ExpressionPredicate(lex, IOperation::GetStandardOperations()));
	//}
	//else
	//{
	//	lex.PutBackToken(t);
	//}
}

void Update::ReadColumns(Lexer& lex)
{
	Token t;
	
	do
	{
		// <columnName>
		t = lex.NextToken(TokenType::Identifier);
		string name = t.strData;

		t = lex.NextToken("=", TokenType::Operator);

		// Expression
		// Read the expression
		ExpressionParser p(&lex, IOperation::GetStandardOperations());
		Expression* expr = p.Parse();

		columns[name] = expr;

		t = lex.NextToken();
	}
	while (t.type == TokenType::Separator);
			
	lex.PutBackToken(t);
}

Table* Update::Execute()
{
	Table* source = &db->GetTable(tableName);

	/*hash_map<string, Field> fields = source->GetFields();
	for (hash_map<string, Expression*>::iterator colIter = columns.begin(); colIter != columns.end(); colIter++)
	{
		Field suitableField = colIter->second->GetSuitableField(colIter->first, fields);
		
		fields[suitableField.name] = suitableField;
	}*/

	TableDataIterator iter = source->GetIterator(*predicate);
	while (iter.NextRow())
	{
		TableDataRow row = iter.GetFullDataRow();

		hash_map<string, Value> fieldsData;
		int numFields = source->GetNumFields();

		for (int i = 0; i < numFields; i++)
		{
			fieldsData[source->GetField(i).name] = row[i];
		}

		for (hash_map<string, Expression*>::iterator colIter = columns.begin(); colIter != columns.end(); colIter++)
		{
			Value val = colIter->second->Compute(fieldsData);
			
			row[source->GetColumnIndex(colIter->first.c_str())].Set(val);
		}
	}

	return NULL;
}