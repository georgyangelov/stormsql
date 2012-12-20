#ifndef _H_LEXER_INCLUDED
#define _H_LEXER_INCLUDED

#include <string>
#include <stack>

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
			stack<Token> putBackTokens;

			// Helper functions
			void ignoreWhitespace();
			//Token getToken(const string&) const;
			Token getToken(const string&, TokenType type, bool toLower = true) const;
			long toLong(const string&) const;

			bool isLetter(char c) const;
			bool isNumber(char c) const;
			void toLower(string& c) const;

		public:
			Lexer(istream&);
			
			Token NextToken(bool toLower = true);
			Token NextToken(TokenType expected, bool toLower = true);
			Token NextToken(TokenType expected, string strData, bool toLower = true);

			void PutBackToken(const Token&);
		};

		class LexerException
			: public runtime_error
		{
		public:
			LexerException(string msg)
				: runtime_error(msg)
			{
			}
		};

		class UnknownTokenException
			: public LexerException
		{
		public:
			UnknownTokenException(string token)
				: LexerException("Unknown token" + token)
			{
			}
		};

		class InvalidTokenException
			: public LexerException
		{
		public:
			InvalidTokenException(Token token)
				: LexerException("Invalid token: " + token.strData)
			{
			}
		};

		class UnexpectedEndOfStreamException
			: public LexerException
		{
		public:
			UnexpectedEndOfStreamException()
				: LexerException("Unexpected end of stream")
			{
			}
		};
	}
}

#endif