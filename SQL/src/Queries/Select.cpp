#include "Select.h"
#include "../StormSQL/Exceptions.h"
#include "Insert.h"
#include "../StormSQL/SQL/Expressions/Expression.h"

#include <string>

using namespace std;
using namespace StormSQL;
using namespace StormSQL::SQL;
using namespace StormSQL::Queries;

void Select::del()
{
	delete predicate;
	
	for (hash_map<string, Expression*>::iterator iter = columns.begin();
		iter != columns.end();
		iter++)
	{
		delete iter->second;
	}

	for (int i = 0; i < joins.size(); i++)
	{
		delete joins[i].second;
	}
}

void Select::copy(const Select& obj)
{
	db = obj.db;
	tableName = obj.tableName;
	columns = obj.columns;
	includeAllColumns = obj.includeAllColumns;

	predicate = obj.predicate->Clone();
}

Select::Select(Database* _db)
	: db(_db)
{
	includeAllColumns = false;
	predicate = new TruePredicate();
}

Select::Select(const Select& obj)
{
	copy(obj);
}

Select::~Select()
{
	del();
}

Select& Select::operator = (const Select& obj)
{
	if (this != &obj)
	{
		del();
		copy(obj);
	}

	return *this;
}

string Select::GetType() const
{
	return "select";
}

void Select::SetFrom(string tbl)
{
	tableName = tbl;
}

void Select::SetWhere(const ITableDataPredicate& pred)
{
	if (predicate)
		delete predicate;

	predicate = pred.Clone();
}

/*
	* SELECT // Read by parser
	* <columns>
	* FROM
	* <table>
	* (WHERE <expression>)
	*
	* <columns> := <columnName>, <columns> | <columnName>
	* <columnName> := identifier (AS identifier) | *
	* <table> := identifier
	*/
void Select::Parse(Lexer& lex)
{
	// <columns>
	ReadColumns(lex);

	Token t = lex.NextToken("from", TokenType::Keyword);

	t = lex.NextToken(TokenType::Identifier);
	SetFrom(t.strData);

	if (lex.endOfStream())
		return;

	ReadJoins(lex);

	if (lex.endOfStream())
		return;

	t = lex.NextToken();
	if (t.type == TokenType::Keyword && t.strData == "where")
	{
		SetWhere(ExpressionPredicate(lex, IOperation::GetStandardOperations()));
	}
	else
	{
		lex.PutBackToken(t);
	}
}

void Select::ReadColumns(Lexer& lex)
{
	Token t;
	includeAllColumns = false;

	do
	{
		// <columnName>
		t = lex.NextToken(false);
		string data = t.strData;

		if (t.type == TokenType::Operator && t.strData == "*")
		{
			includeAllColumns = true;
			
			// Should be separator if there are other columns
			t = lex.NextToken(false);

			continue;
		}
		else
		{
			lex.PutBackToken(t);
		}

		// Expression
		// Read the expression
		ExpressionParser p(&lex, IOperation::GetStandardOperations());
		Expression* expr = p.Parse();

		t = lex.NextToken();

		if (t.type == TokenType::Keyword && t.strData == "as")
		{
			t = lex.NextToken(TokenType::Identifier, false);
			string name = t.strData;

			columns[name] = expr;

			t = lex.NextToken();
		}
		else
		{
			columns[p.GetLastExpression()] = expr;
		}

	}
	while (t.type == TokenType::Separator);
			
	lex.PutBackToken(t);
}

// JOIN <tableName> (ON <expression>)
void Select::ReadJoins(Lexer& lex)
{
	Token t;
	t = lex.NextToken();
	pair< Table*, Expression* > joinInfo;
	bool putBack = true;

	while (t.type == TokenType::Keyword && t.strData == "join")
	{
		t = lex.NextToken(TokenType::Identifier);
		joinInfo.first = &db->GetTable(t.strData);

		if (!lex.endOfStream())
		{
			t = lex.NextToken();

			if (t.type == TokenType::Keyword && t.strData == "on")
			{
				ExpressionParser p(&lex, IOperation::GetStandardOperations());
				joinInfo.second = p.Parse();

				if (!lex.endOfStream())
					t = lex.NextToken();
				else
					putBack = false;
			}
			else
			{
				joinInfo.second = new ConstExpression(1);
			}
		}
		else
		{
			putBack = false;
			joinInfo.second = new ConstExpression(1);
		}

		joins.push_back(joinInfo);
	}

	if (putBack)
		lex.PutBackToken(t);
}

/*string Select::GetHashableData(Table& table, TableDataRow& row, const vector< pair<string, string> >& columns, bool firstTable) const
{
	stringstream data;

	// Append data from each column
	for (int i = 0; i < columns.size(); i++)
	{
		int colIndex;
		if (firstTable)
			colIndex = table.GetColumnIndex(columns[i].first.c_str());
		else
			colIndex = table.GetColumnIndex(columns[i].second.c_str());

		data << '|';

		switch (row[colIndex].GetType())
		{
		case Field::FieldType::byte:
			data << (int)row[colIndex].GetChar();
			break;
		case Field::FieldType::int32:
			data << row[colIndex].GetInt32();
			break;
		case Field::FieldType::uint32:
			data << row[colIndex].GetUInt32();
			break;
		case Field::FieldType::fixedchar:
			data << row[colIndex].GetString();
			break;
		default:
			throw InvalidFieldType();
		}
	}

	return data.str();
}*/

// Does a simple nested loops join because.. well... it's simple
Table* Select::Join(Table& table1, Table& table2, const Expression& expr, bool filterOnJoin) const
{
	/* Prepare destination table */
	Table* dest = new Table();
	
	// Add the columns from both tables to dest
	for (int i = 0; i < table1.GetNumFields(); i++)
	{
		dest->AddField(i, table1.GetField(i));
	}
	for (int i = 0; i < table2.GetNumFields(); i++)
	{
		dest->AddField(table1.GetNumFields() + i, table2.GetField(i));
	}

	Insert insert(dest);

	for (TableDataIterator iter = table1.GetIterator(); iter.NextRow();)
	{
		TableDataRow row1 = iter.GetFullDataRow();

		/* Iterate table2, join and apply predicate */
		for (TableDataIterator iter2 = table2.GetIterator(); iter2.NextRow();)
		{
			TableDataRow row2 = iter2.GetFullDataRow();
			
			// Construct data for expression from rows
			hash_map<string, Value> data;
			for (int i = 0; i < row1.GetNumFields(); i++)
			{
				data[row1[i].GetField().name] = row1[i];
			}
			for (int i = 0; i < row2.GetNumFields(); i++)
			{
				data[row2[i].GetField().name] = row2[i];
			}

			bool shouldJoin = (bool)expr.Compute(data);
			
			if (!shouldJoin)
				continue;

			// Filter with expression and predicate
			TableDataRow joinedRow(row1);
			joinedRow.Append(row2);

			// Check if should be in result set (if filterOnJoin == true, otherwise just join full tables)
			if (!filterOnJoin || (*predicate)(joinedRow))
			{
				// Add to result table
				for (int i = 0; i < joinedRow.GetNumFields(); i++)
				{
					insert.Set(i, joinedRow[i]);
				}

				insert.Execute();
			}
		}
	}

	return dest;
}

/*Table* Select::HashJoin(Table& table1, Table& table2, const vector< pair<string, string> >& columns, bool filterOnJoin) const
{
	/* Prepare destination table /
	Table* dest = new Table();
	Insert insert(dest);
	
	// Add the columns from both tables to dest
	for (int i = 0; i < table1.GetNumFields(); i++)
	{
		dest->AddField(i, table1.GetField(i));
	}
	for (int i = 0; i < table2.GetNumFields(); i++)
	{
		dest->AddField(table1.GetNumFields() + i, table2.GetField(i));
	}

	/* Build hash table for table1 /
	// <stringified and appended data for each column, rowIndex>
	typedef hash_map<string, vector<int> > hash_table_type;
	hash_table_type indexTable;

	for (TableDataIterator iter = table1.GetIterator(); iter.NextRow();)
	{
		TableDataRow row = iter.GetFullDataRow();

		// Add row to hash table
		indexTable[GetHashableData(table1, row, columns, true)].push_back(iter.GetRowIndex());
	}

	// Get iterator for table1 to be used in getting join data
	TableDataIterator iterTable1 = table1.GetIterator();

	/* Iterate table2, join and apply predicate /
	for (TableDataIterator iter = table2.GetIterator(); iter.NextRow();)
	{
		TableDataRow row = iter.GetFullDataRow();
		
		hash_table_type::iterator joinRowsIter = indexTable.find(GetHashableData(table2, row, columns, false));
		if (joinRowsIter != indexTable.end())
		{
			vector<int>* joinRows = &joinRowsIter->second;
			
			// For each possible JOIN for this row
			for (int i = 0; i < joinRows->size(); i++)
			{
				// Filter with predicate
				TableDataRow joinedRow(iterTable1.GetFullDataRow());

				// Assert true:
				iterTable1.Seek((*joinRows)[i]);

				// Append the row from table2 to joinedRow
				joinedRow.Append(row);

				// Check if should be in result set (if filterOnJoin == true, otherwise just join full tables)
				if (filterOnJoin && (*predicate)(joinedRow))
				{
					// Add to result table
					for (int i = 0; i < joinedRow.GetNumFields(); i++)
					{
						insert.Set(i, joinedRow[i]);
					}

					insert.Execute();
				}
			}
		}
	}

	return dest;
}*/

Table* Select::Execute()
{
	// Get pointer to source table
	Table* source = &db->GetTable(tableName);
	// Create empty table to hold the results
	Table* data = new Table();
	bool isTempSource = false;

	for (int i = 0; i < joins.size(); i++)
	{
		isTempSource = true;
		Table* oldSource = source;

		source = Join(*source, *joins[i].first, *joins[i].second, i == joins.size() - 1); // Only filter on join if it's the last one

		// Delete the temp table for the last join
		if (i > 0)
			delete oldSource;
	}

	// Include all columns if there is a * in the columns list
	if (includeAllColumns)
	{
		// Iterate source columns and copy them to the result table
		int numCols = source->GetNumFields();
		for (int i = 0; i < numCols; i++)
		{
			Field f = source->GetField(i);

			data->AddField(f);
		}
	}

	// Add column definitions for the other specified columns through Expression::GetSuitableField
	hash_map<string, Field> fields = source->GetFields();
	for (hash_map<string, Expression*>::iterator colIter = columns.begin(); colIter != columns.end(); colIter++)
	{
		Field suitableField = colIter->second->GetSuitableField(colIter->first, fields);
		data->AddField(suitableField);

		fields[suitableField.name] = suitableField;
	}

	// Iterate the source table with the predicate and copy the data in `data`
	TableDataIterator iter = source->GetIterator(*predicate);
	while (iter.NextRow())
	{
		TableDataRow row = iter.GetFullDataRow();
		Insert ins(data);
		int columnNum = 0;

		// Copy all columns if there is a *
		if (includeAllColumns)
		{
			int numCols = source->GetNumFields();
			for (int i = 0; i < numCols; i++)
			{
				ins.Set(columnNum, source->GetFieldType(i), row[i].GetPtr());
							
				columnNum++;
			}
		}

		// fieldsData holds the column data that is passed to the Expressions
		hash_map<string, Value> fieldsData;
		int numFields = source->GetNumFields();

		// Copy data from each column in the row
		for (int i = 0; i < numFields; i++)
		{
			fieldsData[source->GetField(i).name] = row[i];
		}

		// Iterate all expressions (specified columns) and evaluate, then insert to new table
		for (hash_map<string, Expression*>::iterator colIter = columns.begin(); colIter != columns.end(); colIter++)
		{
			Value val = colIter->second->Compute(fieldsData);
			ins.Set(columnNum, val);

			// Add the result as value so that it can be used in next expressions
			fieldsData[colIter->first] = val;

			columnNum++;
		}

		// Insert data in rows
		ins.Execute();
	}

	if (isTempSource)
		delete source;

	return data;
}