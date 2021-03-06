#ifndef _H_PREDICATES_INCLUDED
#define _H_PREDICATES_INCLUDED

#include "SQL/Expressions/Operations.h"
#include "SQL/Lexer.h"

using namespace StormSQL::SQL;
using namespace StormSQL::SQL::Expressions;

namespace StormSQL
{
	// Forward declarations
	class Table;
	class TableDataRow;

	class ITableDataPredicate
	{
	public:
		virtual ~ITableDataPredicate() { }

		virtual bool operator () (TableDataRow&) const = 0;
		virtual ITableDataPredicate* Clone() const = 0;
	};

	// Simple predicate that always returns true
	class TruePredicate
		: public ITableDataPredicate
	{
	public:
		bool operator () (TableDataRow& row) const
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

		bool operator () (TableDataRow& row) const;

		ITableDataPredicate* Clone() const;
	};
}

#endif

