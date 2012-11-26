#include "Insert.h"
#include "../StormSQL/Exceptions.h"

namespace StormSQL
{
	namespace Queries
	{
		Insert::Insert(Table* const _table)
			: table(_table)
		{
			inserted = false;
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
			inserted = obj.inserted;
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

		void Insert::SetString(int column, char* const value)
		{
			if (table->columns[column].type != Field::fixedchar)
				throw InvalidFieldType();

			if (strlen(value) >= table->columns[column].size)
				throw FieldDataTooLarge();

			strcpy((char*)getPtr(column), value);

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

		void Insert::Commit(bool resetAfter)
		{
			if (!AllColumnsSet())
				throw NotAllColumnsSet();

			if (inserted)
				throw RowAlreadyInserted();

			byte* ptr = table->data->GetElementPtr(table->rows);
			copy(buffer, buffer + table->tableRowSize, ptr);

			table->rows++;

			inserted = true;

			if (resetAfter)
				Reset();
		}

		void Insert::Reset()
		{
			inserted = false;
			for (int i = 0; i < table->columns.size(); i++)
			{
				valuesSet[i] = false;
			}
		}
	}
}