#include "Delete.h"

using namespace std;
using namespace StormSQL;
using namespace StormSQL::Queries;

void Delete::del()
{
	delete predicate;
}

void Delete::copy(const Delete& obj)
{
	db = obj.db;
	tableName = obj.tableName;
	predicate = obj.predicate->Clone();
}

Delete::Delete(Database* _db)
	: db(_db)
{
	predicate = new TruePredicate();
}

Delete::Delete(const Delete& obj)
{
	copy(obj);
}

Delete::~Delete()
{
	del();
}

Delete& Delete::operator = (const Delete& obj)
{
	if (this != &obj)
	{
		del();
		copy(obj);
	}

	return *this;
}

void Delete::SetFrom(string _tableName)
{
	tableName = _tableName;
}

void Delete::SetWhere(const ITableDataPredicate& _predicate)
{
	predicate = _predicate.Clone();
}

string Delete::GetType() const
{
	return "delete";
}

void Delete::Parse(Lexer& lex)
{
	Token t = lex.NextToken("from", TokenType::Keyword);

	t = lex.NextToken(TokenType::Identifier);
	SetFrom(t.strData);
	
	// Don't allow DELETE without WHERE clause
	
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

Table* Delete::Execute()
{
	Table* source = &db->GetTable(tableName);

	TableDataIterator iter = source->GetIterator(*predicate);
	while (iter.NextRow())
	{
		source->DeleteBufferIndex(iter.GetRowIndex());
	}

	return NULL;
}