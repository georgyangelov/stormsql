#include "Insert.h"
#include "../StormSQL/TableData.h"
#include "../StormSQL/Exceptions.h"

namespace StormSQL
{
	namespace Queries
	{
		Insert::Insert(Table* const _table)
			: table(_table)
		{
			if (table->tableRowSize == 0 || table->columns.size() == 0)
				throw NoColumnsInTable();

			buffer = new byte[table->tableRowSize];
			valuesSet = new bool[table->columns.size()];

			for (int i = 0; i < table->columns.size(); i++)
			{
				valuesSet[i] = false;
			}
		}

		Insert::Insert(const Insert& obj)
			: table(obj.table)
		{
			buffer = new byte[table->tableRowSize];
			valuesSet = new bool[table->columns.size()];

			copy(obj.buffer, obj.buffer + table->tableRowSize, buffer);
			copy(obj.valuesSet, obj.valuesSet + table->columns.size(), valuesSet);
		}

		Insert::~Insert()
		{
			delete [] buffer;
			delete [] valuesSet;
		}

		FieldData Insert::getColumn(int column) const
		{
			if (column >= table->columns.size())
				throw ColumnIndexOutOfBounds();

			byte* ptr = buffer;

			for (int i = 0; i < column; i++)
				ptr += table->columns[i].GetByteSize();

			return FieldData(ptr, table->columns[column]);
		}

		void Insert::Set(int column, const FieldData& data)
		{
			getColumn(column).Set(data);

			valuesSet[column] = true;
		}

		void Insert::Set(int column, const Value& value)
		{
			getColumn(column).Set(value);

			valuesSet[column] = true;
		}

		void Insert::Set(int column, Field::FieldType type, const byte* ptr)
		{
			getColumn(column).Set(type, ptr);

			valuesSet[column] = true;
		}
		
		void Insert::SetByte(int column, byte value)
		{
			getColumn(column).Set(value);

			valuesSet[column] = true;
		}

		void Insert::SetInt(int column, int value)
		{
			getColumn(column).Set(value);

			valuesSet[column] = true;
		}
		
		void Insert::SetUInt(int column, unsigned int value)
		{
			getColumn(column).Set(value);

			valuesSet[column] = true;
		}

		void Insert::SetString(int column, string value)
		{
			getColumn(column).Set(value);

			valuesSet[column] = true;
		}

		bool Insert::AllColumnsSet() const
		{
			for (int i = 0; i < table->columns.size(); i++)
			{
				if (!valuesSet[i])
					return false;
			}

			return true;
		}

		string Insert::GetType() const
		{
			return "insert";
		}

		Table* Insert::Execute()
		{
			if (!AllColumnsSet())
				throw NotAllColumnsSet();

			byte* ptr = table->data->GetElementPtr(table->rows);
			copy(buffer, buffer + table->tableRowSize, ptr);

			table->rows++;

			Reset();

			return NULL;
		}

		void Insert::Reset()
		{
			for (int i = 0; i < table->columns.size(); i++)
			{
				valuesSet[i] = false;
			}
		}

		/*
		 * INSERT INTO <tableName> // read by parser
		 * VALUES (<values>)
		 *
		 * <columns> := <columnName> | <columnName>, <columns>
		 * <values> := <value> | <value>, <values>
		 * <columnName> == identifier
		 * <value> == intValue | strValue
		 */
		void Insert::Parse(Lexer& lex)
		{
			lex.NextToken("values", TokenType::Keyword);
			lex.NextToken("(", TokenType::Parenthesis);

			Token t;
			int index = 0;
			do
			{
				/* BEGIN Read <value> */
				t = lex.NextToken();

				if (t.type == TokenType::IntValue)
				{
					switch (table->GetFieldType(index))
					{
					case Field::FieldType::byte:
						SetByte(index, (byte)t.longIntData);
						break;
						
					case Field::FieldType::int32:
						SetInt(index, (int)t.longIntData);
						break;
						
					case Field::FieldType::uint32:
						SetUInt(index, (unsigned int)t.longIntData);
						break;

					default:
						throw InvalidTokenException(t);
					}
				}
				else if (t.type == TokenType::StringValue)
				{
					SetString(index, t.strData);
				}
				else
					throw InvalidTokenException(t);
				
				index++;
				/* END Read <value> */

				t = lex.NextToken();
			}
			while (t.type == TokenType::Separator);

			if (t.type != TokenType::Parenthesis || t.strData != ")")
				throw InvalidTokenException(t);
		}
	}
}