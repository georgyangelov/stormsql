#ifndef _H_SQL_EXPRESSIONS_EXPRESSION_INCLUDED
#define _H_SQL_EXPRESSIONS_EXPRESSION_INCLUDED

#include "../../Field.h"
#include "../Lexer.h"
#include "../../TableData.h"
#include "Value.h"
#include "Operations.h"
#include <hash_map>
#include <sstream>
#include <queue>

using namespace StormSQL::SQL;
using namespace std;

namespace StormSQL
{
	namespace SQL
	{
		namespace Expressions
		{
			class Expression
			{
			public:
				virtual Value Compute(const hash_map<string, Value>&) const = 0;
				virtual Expression* Clone() const = 0;
			};

			class ExpressionParser
			{
			private:
				Lexer* lex;
				hash_map<string, operationInfo> ops;

				void ExpressionParser::InsertOp(Token t, stack<Expression*>& values);
			public:
				queue<Token> GetRPN();
				static string Implode(queue<Token>&);

				ExpressionParser(Lexer&, const hash_map<string, operationInfo>&);

				Expression* Parse();
			};

			class ConstExpression
				: public Expression
			{
			protected:
				Value val;

			public:
				ConstExpression(Value);

				Value Compute(const hash_map<string, Value>&) const;
				Expression* Clone() const;
			};

			class VarExpression
				: public Expression
			{
			protected:
				string name;

			public:
				VarExpression(string);

				Value Compute(const hash_map<string, Value>&) const;
				Expression* Clone() const;
			};

			class CompExpression
				: public Expression
			{
			protected:
				vector<Expression*> expressions;
				IOperation* operation;

				void del();
				void copy(const CompExpression&);

			public:
				CompExpression(const vector<Expression*>, IOperation*);
				CompExpression(const CompExpression&);
				~CompExpression();
				
				CompExpression& operator = (const CompExpression&);

				Value Compute(const hash_map<string, Value>&) const;
				Expression* Clone() const;
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