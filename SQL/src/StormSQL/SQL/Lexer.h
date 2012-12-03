#ifndef _H_LEXER_INCLUDED
#define _H_LEXER_INCLUDED

#include <string>

using namespace std;

namespace StormSQL
{
	namespace SQL
	{
		enum TokenType
		{
			// 'SELECT', 'INSERT', 'INTO', 'FROM', 'VALUES', ...
			Keyword,
			// table names, database names, field names
			Identifier,
			// ','
			Separator,
			// '(' and ')'
			Parenthesis,
			// ==, !=, !, +, -, /, *, <, >, <=, >=
			Operator,
			StringValue,
			IntValue
		};

		struct Token
		{
			TokenType type;

			string strData;
			long longIntData;
		};

		class Lexer
		{
		private:
			istream* in;

			// Helper functions
			void ignoreWhitespace();
			//Token getToken(const string&) const;
			Token getToken(const string&, TokenType type) const;
			long toLong(const string&) const;

			bool isLetter(char c) const;
			bool isNumber(char c) const;
			void toLower(string& c) const;

		public:
			Lexer(istream&);
			
			Token NextToken();
			Token NextToken(TokenType expected);
			Token NextToken(TokenType expected, string strData);
		};

		class UnknownTokenException
			: public runtime_error
		{
		public:
			UnknownTokenException(string token)
				: runtime_error("Unknown token" + token)
			{
			}
		};

		class InvalidTokenException
			: public runtime_error
		{
		public:
			InvalidTokenException(Token token)
				: runtime_error("Invalid token: " + token.strData)
			{
			}
		};
	}
}

#endif