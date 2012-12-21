#include "Expression.h"
#include <stack>

using namespace std;
using namespace StormSQL::SQL::Expressions;

ExpressionParser::ExpressionParser(Lexer& _lex, const hash_map<string, operationInfo>& _ops)
	: lex(&_lex), ops(_ops)
{
}

stringstream ExpressionParser::GetRPN()
{
	stringstream out;
	stack<string> operationStack;

	while (!lex->endOfStream())
	{
		string op;
		Token t = lex->NextToken(false);
		
		switch (t.type)
		{
		case TokenType::IntValue:
		case TokenType::StringValue:
			out << " " << t.strData;
			break;

		case TokenType::Identifier:
			out << " " << '`' << t.strData << '`';
			break;

		case TokenType::Keyword:

			// Function or operator
			if (ops.find(t.strData) != ops.end())
			{
				if (ops[t.strData].isFunction)
				{
					// Function name (next token should be '(')
					Token temp = lex->NextToken(TokenType::Parenthesis, "(");
					lex->PutBackToken(temp);
				}
				else
				{
					while (!operationStack.empty() && ( 
								(ops[t.strData].leftAssoc && ops[t.strData].priority <= ops[operationStack.top()].priority) 
								|| 
								(ops[t.strData].priority < ops[operationStack.top()].priority) 
							) )
					{
						out << " " << operationStack.top();
						operationStack.pop();
					}
				}

				operationStack.push(t.strData);
			}
			else
			{
				// Value or constant
				out << " " << t.strData;
			}

			break;

		case TokenType::Separator:

			if (operationStack.empty())
				throw InvalidTokenException(t);

			op = operationStack.top();
			while (op != "(")
			{
				operationStack.pop();
				out << " " << op;
				
				if (operationStack.empty())
					throw InvalidTokenException(t);

				op = operationStack.top();
			}

			break;

		case TokenType::Operator:
			
			if (ops.find(t.strData) == ops.end())
				throw InvalidTokenException(t);

			while (!operationStack.empty() && ( 
						(ops[t.strData].leftAssoc && ops[t.strData].priority <= ops[operationStack.top()].priority) 
						|| 
						(ops[t.strData].priority < ops[operationStack.top()].priority) 
					) )
			{
				out << " " << operationStack.top();
				operationStack.pop();
			}

			operationStack.push(t.strData);
			
			break;

		case TokenType::Parenthesis:
			
			if (t.strData == "(")
			{
				operationStack.push("(");
			}
			else 
			{
				if (operationStack.empty())
				throw InvalidTokenException(t);

				op = operationStack.top();
				while (op != "(")
				{
					operationStack.pop();
					out << " " << op;
				
					if (operationStack.empty())
						throw InvalidTokenException(t);

					op = operationStack.top();
				}

				// Pop (
				operationStack.pop();

				if (!operationStack.empty() && ops.find(operationStack.top()) != ops.end())
				{
					// Token at top of stack is function name
					out << " " << operationStack.top();
					operationStack.pop();
				}
			}

			break;

		default:
			throw InvalidTokenException(t);
		}
	}

	while (!operationStack.empty())
	{
		if (operationStack.top() == "(")
			throw LexerException("Mismatched parenthesis");

		out << " " << operationStack.top();
		operationStack.pop();
	}

	return out;
}

Expression* ExpressionParser::Parse()
{
	return NULL;
}