#include "Lexer.h"
#include <algorithm>
#include <sstream>

namespace StormSQL
{
	namespace SQL
	{
		/* Token struct */
		bool Token::operator == (const Token& obj) const
		{
			if (type != obj.type)
				return false;

			if (type == TokenType::IntValue)
				return longIntData == obj.longIntData;
			else
				return strData == obj.strData;
		}

		Token::operator string() const
		{
			switch (type)
			{
			case TokenType::StringValue:
				return (string)"'" + strData + "'";
			case TokenType::Identifier:
				return (string)"`" + strData + "`";
			default:
				return strData;
			}
		}

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

		Token Lexer::getToken(const string& str, TokenType type, bool toLow) const
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
				if (toLow)
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

		bool Lexer::endOfStream()
		{
			ignoreWhitespace();

			return putBackTokens.empty() && in->eof();
		}

		Token Lexer::NextToken(TokenType expected, bool toLower)
		{
			Token t = NextToken(toLower);

			// Keywords map to Identifiers (<name> vs `<name>`)
			if (expected == TokenType::Identifier && t.type == TokenType::Keyword)
				t.type = TokenType::Identifier;
			
			if (t.type != expected)
				throw InvalidTokenException(t);

			return t;
		}

		Token Lexer::NextToken(string strData, TokenType expected, bool toLower)
		{
			Token t = NextToken(expected, toLower);
			
			if (t.strData != strData)
				throw InvalidTokenException(t);

			return t;
		}

		inline string toString(char c)
		{
			char tmp[2];
			tmp[0] = c;
			tmp[1] = 0;

			return tmp;
		}

		Token Lexer::NextToken(bool toLower)
		{
			if (!putBackTokens.empty())
			{
				Token t = putBackTokens.top();
				putBackTokens.pop();

				if (t.type == TokenType::Keyword && toLower)
					this->toLower(t.strData);

				return t;
			}

			ignoreWhitespace();

			if (in->eof())
				throw UnexpectedEndOfStreamException();

			int c = in->get();

			if (c < 0)
				throw UnexpectedEndOfStreamException();

			string tmp;

			if (c == '(' || c == ')')
				return getToken(toString(c), TokenType::Parenthesis);
			else if (c == ',')
				return getToken(toString(c), TokenType::Separator);
			else if (c == '`')
			{
				while ((c = in->get()) != '`' && c != -1)
					tmp += c;

				return getToken(tmp, TokenType::Identifier);
			}
			else if (c == '\'' || c == '\"')
			{
				char endP = c;
				char last = endP;

				while ((c = in->get()) != endP && c != -1 && last != '\\')
				{
					tmp += c;
					last = c;
				}

				return getToken(tmp, TokenType::StringValue);
			}
			else if ( (c == '-' && isNumber(in->peek())) || isNumber(c))
			{
				tmp += c;
				while (isNumber(in->peek()))
				{
					tmp += in->get();
				}

				return getToken(tmp, TokenType::IntValue);
			}
			else if (isLetter(c))
			{
				tmp += c;
				while (isLetter(in->peek()) || isNumber(in->peek()) || in->peek() == '_')
				{
					tmp += in->get();
				}

				return getToken(tmp, TokenType::Keyword, toLower);
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

			throw UnexpectedEndOfStreamException();
		}

		void Lexer::PutBackToken(const Token& token)
		{
			putBackTokens.push(token);
		}
	}
}