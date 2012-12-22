#include "ShowCreate.h"
#include "../StormSQL/Database.h"
#include "../StormSQL/Exceptions.h"
#include "../Queries/Insert.h"
#include "../StormSQL/SQL/Lexer.h"

#include <sstream>

using namespace std;
using namespace StormSQL::SQL;

namespace StormSQL
{
	namespace Queries
	{
		ShowCreate::ShowCreate(Database* _db)
			: db(_db)
		{
		}

		void ShowCreate::SetTableName(string _tableName)
		{
			tableName = _tableName;
		}

		string ShowCreate::GetType() const
		{
			return "showCreate";
		}

		Table* ShowCreate::Execute()
		{
			stringstream create;
			create << "CREATE TABLE `" << tableName << "` (\n";

			vector<Field> columns = db->GetTable(tableName).GetFields();
			for (int i = 0; i < columns.size(); i++)
			{
				create << columns[i].name;
				create << " ";
				create << columns[i].GetStringType();
				
				if (columns[i].type == Field::fixedchar)
				{
					create << "(";
					create << columns[i].size;
					create << ")";
				}

				if (i < columns.size() - 1)
					create << ", \n";
			}

			create << "\n)";
			
			Table* res = new Table();

			res->AddField(Field("Table", Field::fixedchar, 255));

			Insert ins(res);
			ins.SetString(0, create.str());
			ins.Execute();

			return res;
		}

		/*
		 * SHOW CREATE // Read by the parser
		 * TABLE <tableName>
		 */
		void ShowCreate::Parse(Lexer& lex)
		{
			lex.NextToken("table", TokenType::Keyword);

			Token tableName = lex.NextToken(TokenType::Identifier, false);
			SetTableName(tableName.strData);
		}
	}
}