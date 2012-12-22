#include "Predicates.h"
#include "SQL/Expressions/Expression.h"
#include "Table.h"

using namespace std;

namespace StormSQL
{
	void ExpressionPredicate::del()
	{
		delete expression;
	}

	void ExpressionPredicate::copy(const ExpressionPredicate& obj)
	{
		expression = obj.expression->Clone();
	}

	ExpressionPredicate::ExpressionPredicate(Lexer& lex, const hash_map<string, OperationInfo>& ops)
	{
		ExpressionParser parser(lex, ops);
		expression = parser.Parse();
	}

	ExpressionPredicate::ExpressionPredicate(Expression& _expression)
		: expression(_expression.Clone())
	{
	}

	ExpressionPredicate::ExpressionPredicate(const ExpressionPredicate& obj)
	{
		copy(obj);
	}

	ExpressionPredicate::~ExpressionPredicate()
	{
		del();
	}

	ExpressionPredicate& ExpressionPredicate::operator = (const ExpressionPredicate& obj)
	{
		if (this != &obj)
		{
			del();
			copy(obj);
		}

		return *this;
	}

	bool ExpressionPredicate::operator () (const Table* table, TableDataRow& row) const
	{
		hash_map<string, Value> fields;
		int numFields = table->GetNumFields();

		for (int i = 0; i < numFields; i++)
		{
			fields[table->GetField(i).name] = row[0];
		}

		return (bool)expression->Compute(fields);
	}

	ITableDataPredicate* ExpressionPredicate::Clone() const
	{
		return new ExpressionPredicate(*this);
	}
}