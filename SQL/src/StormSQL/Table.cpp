#include "Table.h"
#include "Exceptions.h"
#include "../Queries/Insert.h"

namespace StormSQL
{
	Table::Table()
	{
		data = NULL;
		rows = 0;
		tableRowSize = 0;
	}

	Table::Table(const Table& obj)
		: rows(obj.rows), tableRowSize(obj.tableRowSize), columns(obj.columns)
	{
		//strcpy(name, obj.name);

		if (obj.data)
			data = new DynamicBuffer(*obj.data);
		else
			data = NULL;
	}

	Table::~Table()
	{
		if (data)
			delete data;
	}

	void Table::createDataBuffer()
	{
		tableRowSize = 0;

		for (unsigned int i = 0; i < columns.size(); i++)
		{
			tableRowSize += columns[i].GetByteSize();
		}

		if (data)
			delete data;

		data = new DynamicBuffer(tableRowSize);
	}

	int Table::GetNumFields() const
	{
		return columns.size();
	}

	bool Table::HasField(const char* name, int& index) const
	{
		for (unsigned int i = 0; i < columns.size(); i++)
		{
			if (strcmp(columns[i].name, name) == 0)
			{
				index = i;
				return true;
			}
		}

		return false;
	}

	bool Table::HasField(const char* name) const
	{
		int tmp;
		return HasField(name, tmp);
	}

	void Table::AddField(const Field& field)
	{
		AddField(columns.size(), field);
	}

	void Table::AddField(int index, const Field& field)
	{
		if (HasField(field.name))
			throw FieldExists();

		Table old(*this);

		//columns.push_back(field);
		columns.insert(columns.begin() + index, field);
		createDataBuffer();

		// Copy data
		data->Expand(old.rows);
		
		// Calculate first part size
		unsigned int firstSize = 0;
		for (int i = 0; i < index; i++)
		{
			firstSize += columns[i].GetByteSize();
		}

		// Calculate second part size
		unsigned int secondSize = 0;
		for (int i = index + 1; i < columns.size(); i++)
		{
			secondSize += columns[i].GetByteSize();
		}

		int newColumnSize = columns[index].GetByteSize();

		// Copy data
		for (int i = 0; i < rows; i++)
		{
			byte* newPtr = data->GetElementPtr(i);
			byte* oldPtr = old.data->GetElementPtr(i);

			// Copy first part
			copy(oldPtr, oldPtr + firstSize, newPtr);

			// Set new column data to 0
			for (int j = 0; j < newColumnSize; j++)
			{
				*(newPtr + firstSize + j) = 0;
			}

			// Copy second part
			copy(oldPtr + firstSize, oldPtr + firstSize + secondSize, newPtr + firstSize + newColumnSize);
		}
	}

	void Table::RemoveField(const char* name)
	{
		int index;
		if (!HasField(name, index))
			throw FieldDoesntExists();
		
		columns.erase(columns.begin() + index);

		//TODO: Modify data
		createDataBuffer();
	}

	void Table::AlterField(const char* name, const Field& field)
	{
		int index;
		if (!HasField(name, index))
			throw FieldDoesntExists();
		
		columns[index] = field;

		//TODO: Modify data
		createDataBuffer();
	}


	void Table::WriteToStream(ostream& out) const
	{
		// Write schema version at beginning
		float ver = STORM_SQL_SCHEMA_VERSION;
		out.write((char*)&ver, sizeof(float));

		// Write columns count
		int num = columns.size();
		out.write((char*)&num, sizeof(int));

		// Write columns
		for (int i = 0; i < num; i++)
		{
			columns[i].WriteToStream(out);
		}

		// Write row count
		out.write((char*)&rows, sizeof(rowIndexType));

		// Write table data array
		out.write((const char*)data, rows * tableRowSize);
	}

	void Table::ReadFromStream(istream& in)
	{
		// Read schema version and verify
		float ver;
		in.read((char*)&ver, sizeof(float));

		if (ver != STORM_SQL_SCHEMA_VERSION)
			throw TableSchemaOldVersion();

		// Read column count
		int num;
		in.read((char*)&num, sizeof(int));

		// Read columns
		for (int i = 0; i < num; i++)
		{
			Field f;
			f.ReadFromStream(in);
			columns.push_back(f);
		}

		// (Re)create the data buffer
		createDataBuffer();

		// Read row count
		in.read((char*)&rows, sizeof(rowIndexType));

		// Read table data array
		in.read((char*)data, rows * tableRowSize);
	}

	TableDataIterator<TruePredicate> Table::GetIterator()
	{
		return TableDataIterator<TruePredicate>(this, TruePredicate());
	}
}