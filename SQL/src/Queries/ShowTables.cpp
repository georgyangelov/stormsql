#include "ShowTables.h"
#include "../StormSQL/Database.h"
#include "../StormSQL/Exceptions.h"
#include "../Queries/Insert.h"

namespace StormSQL
{
	namespace Queries
	{
		ShowTables::ShowTables(Database* _db)
			: db(_db)
		{
		}

		string ShowTables::GetType() const
		{
			return "showTables";
		}

		Table* ShowTables::Execute()
		{
			Table* res = new Table();

			res->AddField(Field("Table", Field::fixedchar, STORM_SQL_TABLE_NAME_SIZE));

			Insert ins(res);
			
			vector<string> tables = db->GetTableNames();
			for (int i = 0; i < tables.size(); i++)
			{
				ins.SetString(0, tables[i].c_str());
				ins.Execute();
			}

			return res;
		}

		/*
		 * SHOW TABLES
		 */
		void ShowTables::Parse(Lexer& lex)
		{
			
		}
	}
}