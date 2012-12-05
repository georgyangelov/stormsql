#include "DropTable.h"
#include "../StormSQL/Database.h"

namespace StormSQL
{
	namespace Queries
	{
		DropTable::DropTable(Database* _db)
			: db(_db)
		{
		}

		void DropTable::SetName(string _name)
		{
			name = _name;
		}

		string DropTable::GetType() const
		{
			return "dropTable";
		}

		Table* DropTable::Execute()
		{
			db->DropTable(name);

			return NULL;
		}

		/*
		 * DROP TABLE <name>
		 */
		void DropTable::Parse(Lexer& lex)
		{
			Token t = lex.NextToken(TokenType::Identifier, false);
			SetName(t.strData);
		}
	}
}