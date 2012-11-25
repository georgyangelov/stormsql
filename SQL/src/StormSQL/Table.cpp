#include "Table.h"
#include "Exceptions.h"

namespace StormSQL
{
	Table::Table()
	{
		data = NULL;
		rows = 0;
		tableRowSize = 0;
	}

	Table::Table(const Table& obj)
		: rows(obj.rows), tableRowSize(obj.tableRowSize)
	{
		if (obj.data)
			data = new DynamicBuffer(*obj.data);
	}

	Table::~Table()
	{
		if (data)
			delete data;
	}

	void Table::createDataBuffer()
	{
		tableRowSize = 0;

		for (int i = 0; i < columns.size(); i++)
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
		for (int i = 0; i < columns.size(); i++)
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
		if (HasField(field.name))
			throw FieldExists();

		columns.push_back(field);

		//TODO: Modify data
		//createDataBuffer();
	}

	void Table::RemoveField(const char* name)
	{
		int index;
		if (!HasField(name, index))
			throw FieldDoesntExists();
		
		columns.erase(columns.begin() + index);

		//TODO: Modify data
		//createDataBuffer();
	}

	void Table::AlterField(const char* name, const Field& field)
	{
		int index;
		if (!HasField(name, index))
			throw FieldDoesntExists();
		
		columns[index] = field;

		//TODO: Modify data
		//createDataBuffer();
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
}