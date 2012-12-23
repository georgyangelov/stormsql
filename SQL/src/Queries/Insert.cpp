#include "Insert.h"
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

		byte* Insert::getPtr(int column) const
		{
			if (column >= table->columns.size())
				throw ColumnIndexOutOfBounds();

			byte* res = buffer;

			for (int i = 0; i < column; i++)
				res += table->columns[i].GetByteSize();

			return res;
		}

		void Insert::Set(int column, Value value)
		{
			switch (table->columns[column].type)
			{
			case Field::FieldType::byte:
				SetByte(column, (int)value);
				break;
			case Field::FieldType::int32:
				SetInt(column, (int)value);
				break;
			case Field::FieldType::uint32:
				SetUInt(column, (int)value);
				break;
			case Field::FieldType::fixedchar:
				SetString(column, (string)value);
				break;
			default:
				throw InvalidFieldType();
			}
		}

		void Insert::Set(int column, Field::FieldType type, const byte* ptr)
		{
			if (table->columns[column].type != type)
				throw InvalidFieldType();

			switch (type)
			{
			case Field::FieldType::byte:
				*(byte*)getPtr(column) = *(byte*)ptr;
				break;
			case Field::FieldType::int32:
				*(int*)getPtr(column) = *(int*)ptr;
				break;
			case Field::FieldType::uint32:
				*(unsigned int*)getPtr(column) = *(unsigned int*)ptr;
				break;
			case Field::FieldType::fixedchar:
				strcpy((char*)getPtr(column), (const char*)ptr);
				break;
			default:
				throw InvalidFieldType();
			}

			valuesSet[column] = true;
		}
		
		void Insert::SetByte(int column, byte value)
		{
			if (table->columns[column].type != Field::byte)
				throw InvalidFieldType();

			*getPtr(column) = value;

			valuesSet[column] = true;
		}

		void Insert::SetInt(int column, int value)
		{
			if (table->columns[column].type != Field::int32)
				throw InvalidFieldType();

			*((int*)getPtr(column)) = value;

			valuesSet[column] = true;
		}
		
		void Insert::SetUInt(int column, unsigned int value)
		{
			if (table->columns[column].type != Field::uint32)
				throw InvalidFieldType();

			*((unsigned int*)getPtr(column)) = value;

			valuesSet[column] = true;
		}

		void Insert::SetString(int column, string value)
		{
			if (table->columns[column].type != Field::fixedchar)
				throw InvalidFieldType();

			if (value.size() >= table->columns[column].size)
				throw FieldDataTooLarge();

			strcpy((char*)getPtr(column), value.c_str());

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