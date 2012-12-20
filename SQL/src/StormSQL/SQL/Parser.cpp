#include "Parser.h"
#include "../../Queries/CreateTable.h"
#include "../../Queries/DropTable.h"
#include "../../Queries/ShowTables.h"
#include "../../Queries/ShowCreate.h"
#include "../../Queries/Insert.h"
#include "../../Queries/Select.h"

namespace StormSQL
{
	namespace SQL
	{
		/* Public */
		Parser::Parser(istream& in, Database* _db)
			: lexer(in), db(_db)
		{
		}

		Query* Parser::ParseQuery()
		{
			Token t = lexer.NextToken(TokenType::Keyword);

			Query* q = NULL;

			if (t.strData == "create")
			{
				Token t2 = lexer.NextToken(TokenType::Keyword);

				if (t2.strData == "table")
					q = new CreateTable(db);
				else
					throw InvalidTokenException(t2);
			}
			else if (t.strData == "show")
			{
				Token t2 = lexer.NextToken(TokenType::Keyword);

				if (t2.strData == "tables")
					q = new ShowTables(db);
				else if (t2.strData == "create")
					q = new ShowCreate(db);
				else
					throw InvalidTokenException(t2);
			}
			else if (t.strData == "drop")
			{
				Token t2 = lexer.NextToken(TokenType::Keyword);

				if (t2.strData == "table")
					q = new DropTable(db);
				else
					throw InvalidTokenException(t2);
			}
			else if (t.strData == "insert")
			{
				lexer.NextToken(TokenType::Keyword, "into");
				Token tblName = lexer.NextToken(TokenType::Identifier, false);

				q = new Insert(&db->GetTable(tblName.strData));
			}
			else if (t.strData == "select")
			{
				q = new Select(db);
			}

			if (q == NULL)
				throw UnknownQueryException(t.strData);

			q->Parse(lexer);
			return q;
		}
	}
}