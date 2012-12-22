#ifndef _H_PREDICATES_INCLUDED
#define _H_PREDICATES_INCLUDED

#include "SQL/Expressions/Operations.h"

using namespace StormSQL::SQL::Expressions;

namespace StormSQL
{
	// Forward declarations
	class Table;
	class TableDataRow;

	class ITableDataPredicate
	{
	public:
		virtual bool operator () (const Table*, TableDataRow&) const = 0;
		virtual ITableDataPredicate* Clone() const = 0;
	};

	// Simple predicate that always returns true
	class TruePredicate
		: public ITableDataPredicate
	{
	public:
		bool operator () (const Table* table, TableDataRow& row) const
		{
			return true;
		}

		ITableDataPredicate* Clone() const
		{
			return new TruePredicate(*this);
		}
	};

	// Predicate that uses an Expression
	class ExpressionPredicate
		: public ITableDataPredicate
	{
	protected:
		Expression* expression;

		void del();
		void copy(const ExpressionPredicate&);

	public:
		ExpressionPredicate(Lexer&, const hash_map<string, OperationInfo>&);
		ExpressionPredicate(Expression&);
		ExpressionPredicate(const ExpressionPredicate&);
		~ExpressionPredicate();
		ExpressionPredicate& operator = (const ExpressionPredicate&);

		bool operator () (const Table* table, TableDataRow& row) const;

		ITableDataPredicate* Clone() const;
	};
}

#endif

