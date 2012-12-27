#ifndef _H_TABLE_DATA_INCLUDED
#define _H_TABLE_DATA_INCLUDED

#include "Defines.h"
#include <vector>
#include "Field.h"
#include <istream>
#include <ostream>

using namespace std;

namespace StormSQL
{
	// Forward declarations
	class Table;
	class ITableDataPredicate;

	namespace SQL
	{
		namespace Expressions
		{
			struct Value;
		}
	}
	
	using namespace StormSQL::SQL::Expressions;

	class FieldData
	{
	private:
		byte* ptr;
		Field field;

	public:
		FieldData(byte*, const Field&);

		/* Getters */
		Field::FieldType GetType() const;
		const Field& GetField() const;

		const byte* GetPtr() const;
		
		char GetChar() const;
		int GetInt32() const;
		unsigned int GetUInt32() const;
		string GetString() const;

		/* Setters */
		void Set(const FieldData& value);
		void Set(const Value& value);
		void Set(Field::FieldType type, const byte* ptr);
		void SetByte(byte value);
		void SetInt(int value);
		void SetUInt(unsigned int value);
		void SetString(string value);
	};

	class TableDataRow
	{
	protected:
		vector<FieldData> columns;
		
		void AppendRow(byte* _ptr, const vector<Field>& _columns);
	public:
		TableDataRow(byte* _ptr, const vector<Field>& _columns);
		void Append(const TableDataRow&);

		int GetNumFields() const;
		FieldData& operator [](int);
	};

	class TableDataIterator
	{
	protected:
		bool started;
		Table* table;
		ITableDataPredicate* predicate;
		
		rowIndexType rowIndex;

		bool TestCurrentRow() const;

		void del();

		void copy(const TableDataIterator&);
		
		TableDataIterator(Table*, const ITableDataPredicate&);

		friend class Table;
	public:
		TableDataIterator(const TableDataIterator&);
		~TableDataIterator();

		TableDataIterator& operator = (const TableDataIterator&);

		// Get data
		TableDataRow GetFullDataRow() const;
		rowIndexType GetRowIndex() const;

		// Move pointer
		bool Seek(rowIndexType);

		bool NextRow();
		bool PrevRow();
	};

}
#endif

