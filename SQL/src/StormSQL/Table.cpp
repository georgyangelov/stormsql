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

	void Table::createDataBuffer(bool deleteOld)
	{
		tableRowSize = 0;

		for (unsigned int i = 0; i < columns.size(); i++)
		{
			tableRowSize += columns[i].GetByteSize();
		}

		if (data && deleteOld)
			delete data;

		data = new DynamicBuffer(tableRowSize);
	}
	
	int Table::GetNumRows() const
	{
		return rows;
	}

	int Table::GetNumFields() const
	{
		return columns.size();
	}

	Field Table::GetField(int index) const
	{
		return columns[index];
	}

	hash_map<string, Field> Table::GetFields() const
	{
		hash_map<string, Field> ret;

		for (int i = 0; i < columns.size(); i++)
		{
			ret[columns[i].name] = columns[i];
		}

		return ret;
	}

	int Table::GetColumnIndex(const char* name) const
	{
		for (int i = 0; i < columns.size(); i++)
		{
			if (strcmp(columns[i].name, name) == 0)
				return i;
		}

		throw ColumnIndexOutOfBounds();
	}
	
	Field::FieldType Table::GetFieldType(int i) const
	{
		return columns[i].type;
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

		// No need to copy everything
		// Table old(*this);
		DynamicBuffer* oldBuffer = data;

		columns.insert(columns.begin() + index, field);
		createDataBuffer(false);

		if (oldBuffer == NULL)
			return;

		// Expand the new data buffer
		data->Expand(rows);
		
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
			byte* newPtr = (*data)[i];
			byte* oldPtr = (*oldBuffer)[i];

			// Copy first part
			copy(oldPtr, 
				 oldPtr + firstSize, 
				 newPtr);

			// Set new column data to 0
			for (int j = 0; j < newColumnSize; j++)
			{
				*(newPtr + firstSize + j) = 0;
			}

			// Copy second part
			copy(oldPtr + firstSize, 
				 oldPtr + firstSize + secondSize, 
				 newPtr + firstSize + newColumnSize);
		}

		// Delete the old buffer
		delete oldBuffer;
	}

	void Table::RemoveField(const char* name)
	{
		int index;
		if (!HasField(name, index))
			throw FieldDoesntExist();

		Field deletedColumn = columns[index];
		
		// Store pointer to old data
		DynamicBuffer* oldBuffer = data;

		columns.erase(columns.begin() + index);
		createDataBuffer(false);

		if (oldBuffer == NULL)
			return;

		data->Expand(rows);

		// Calculate first part size
		unsigned int firstSize = 0;
		for (int i = 0; i < index; i++)
		{
			firstSize += columns[i].GetByteSize();
		}

		// Calculate second part size
		unsigned int secondSize = 0;
		for (int i = index; i < columns.size(); i++)
		{
			secondSize += columns[i].GetByteSize();
		}

		// Copy data
		for (int i = 0; i < rows; i++)
		{
			byte* newPtr = (*data)[i];
			byte* oldPtr = (*oldBuffer)[i];

			// Copy first part
			copy(oldPtr, 
				 oldPtr + firstSize,
				 newPtr);

			// Copy second part
			copy(oldPtr + firstSize + deletedColumn.GetByteSize(), 
				 oldPtr + firstSize + deletedColumn.GetByteSize() + secondSize, 
				 newPtr + firstSize);
		}

		delete oldBuffer;
	}

	void Table::AlterField(const char* name, const Field& field)
	{
		int index;
		if (!HasField(name, index))
			throw FieldDoesntExist();
		
		columns[index] = field;

		//TODO: Modify data
		createDataBuffer();
	}


	void Table::Store(ostream& out) const
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
		data->Store(out, rows);
	}

	void Table::Load(istream& in)
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
		data->Load(in, rows);
	}

	TableDataIterator Table::GetIterator()
	{
		return TableDataIterator(this, TruePredicate());
	}
}