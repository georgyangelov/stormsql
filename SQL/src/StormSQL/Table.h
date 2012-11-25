#ifndef _H_TABLE_SCHEMA_INCLUDED
#define _H_TABLE_SCHEMA_INCLUDED

#include <vector>
#include "Field.h"
#include "Defines.h"
#include <istream>
#include <ostream>
#include "DynamicBuffer.h"

using namespace std;

namespace StormSQL
{
	class Table
	{
	protected:
		char name[STORM_SQL_TABLE_NAME_SIZE];
		vector<Field> columns;

		DynamicBuffer* data;
		rowIndexType rows;

		int tableRowSize;

		void createDataBuffer();
		
		friend class TableDataIterator;
	public:
		Table();
		Table(const Table&);
		~Table();

		// Info
		int GetNumFields() const;

		// Test
		bool HasField(const char*) const;
		bool HasField(const char*, int&) const;

		// Alter
		void AddField(const Field&);
		void RemoveField(const char*);
		void AlterField(const char*, const Field&);

		// Store/Load
		void ReadFromStream(istream& in);
		void WriteToStream(ostream& out) const;
	};

}
#endif

