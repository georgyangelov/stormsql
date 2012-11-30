#include "Lexer.h"
#include <algorithm>
#include <sstream>

namespace StormSQL
{
	namespace SQL
	{
		/* Private */
		void Lexer::ignoreWhitespace()
		{
			char c = in->peek();
			while (c == ' ' || c == '\t' || c == '\n')
			{
				in->get();
				c = in->peek();
			}
		}

		Token Lexer::getToken(const string& str, TokenType type) const
		{
			Token res;
			res.strData = str;

			if (type == TokenType::IntValue)
			{
				// Parse string to int
				stringstream strStream(str);
				strStream >> res.longIntData;
			}
			else if (type == TokenType::Keyword)
			{
				toLower(res.strData);
			}

			res.type = type;

			return res;
		}
		
		inline bool Lexer::isLetter(char c) const
		{
			return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z'; 
		}

		inline bool Lexer::isNumber(char c) const
		{
			return c >= '0' && c <= '9';
		}

		void Lexer::toLower(string& str) const
		{
			std::transform(str.begin(), str.end(), str.begin(), ::tolower);
		}

		/* Public */
		Lexer::Lexer(istream& _in)
		{
			in = &_in;
		}

		Token Lexer::NextToken()
		{
			ignoreWhitespace();

			char c = in->get();
			string tmp;

			if (c == '(' || c == ')')
				return getToken(&c, TokenType::Parenthesis);
			else if (c == ',')
				return getToken(&c, TokenType::Separator);
			else if (c == '`')
			{
				while ((c = in->get()) != '`')
					tmp += c;

				return getToken(tmp, TokenType::Identifier);
			}
			else if (c == '\'' || c == '\"')
			{
				char endP = c;
				char last = endP;

				while ((c = in->get()) != endP && last != '\\')
				{
					tmp += c;
					last = c;
				}

				return getToken(tmp, TokenType::StringValue);
			}
			else if (isNumber(c))
			{
				while (isNumber(c))
				{
					tmp += c;
					c = in->get();
				}

				return getToken(tmp, TokenType::IntValue);
			}
			else if (isLetter(c))
			{
				while (isLetter(c))
				{
					tmp += c;
					c = in->get();
				}

				return getToken(tmp, TokenType::Keyword);
			}
			else
			{
				tmp += c;

				switch (c)
				{
				case '-':
				case '+':
				case '*':
				case '/':
					return getToken(tmp, TokenType::Operator);

				case '=':
				case '!':
				case '<':
				case '>':

					if (in->peek() == '=')
						tmp += in->get();

					return getToken(tmp, TokenType::Operator);

				default:
					throw UnknownTokenException(tmp);
				}
			}
		}
	}
}