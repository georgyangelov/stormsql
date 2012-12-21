#ifndef _H_SQL_EXPRESSIONS_EXPRESSION_INCLUDED
#define _H_SQL_EXPRESSIONS_EXPRESSION_INCLUDED

#include "../../Field.h"
#include "../Lexer.h"
#include "../../TableData.h"
#include "Value.h"
#include <hash_map>

using namespace StormSQL::SQL;
using namespace std;

namespace StormSQL
{
	namespace Queries
	{
		namespace Expressions
		{
			class Expression
			{
			public:
				virtual Value Compute(const hash_map<string, FieldData>&) const = 0;
			};

			class ConstExpression
				: public Expression
			{
			protected:
				FieldData val;

			public:
				ConstExpression(FieldData val);

				void SetValue(const FieldData&);
				Value Compute(const hash_map<string, FieldData>&);
			};

			class VarExpression
				: public Expression
			{
			protected:
				string name;

			public:
				VarExpression(string);

				Value Compute(const hash_map<string, FieldData>&);
			};

			class CompExpression
				: public Expression
			{
			protected:
				vector<Expression*> expressions;
				string operation;

			public:
				Value Compute(const hash_map<string, FieldData>&);
			};

			class Where
			{
			protected:
				

			public:
				void Parse(Lexer&);
			};
		}
	}
}

#endif