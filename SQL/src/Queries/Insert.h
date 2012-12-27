#ifndef _H_QUERIES_INSERT_INCLUDED
#define _H_QUERIES_INSERT_INCLUDED

#include "../StormSQL/Defines.h"
#include "../StormSQL/Table.h"
#include "Query.h"

namespace StormSQL
{
	namespace Queries
	{
		class Insert
			: public Query
		{
		protected:
			Table* const table;
			byte* buffer;
			bool* valuesSet;
			bool inserted;

			FieldData getColumn(int column) const;

		public:
			Insert(Table* const);
			Insert(const Insert&);
			~Insert();
			
			void Set(int column, const FieldData& row);
			void Set(int column, const Value& value);
			void Set(int column, Field::FieldType type, const byte* ptr);
			void SetByte(int column, byte value);
			void SetInt(int column, int value);
			void SetUInt(int column, unsigned int value);
			void SetString(int column, string value);

			bool AllColumnsSet() const;

			string GetType() const;

			Table* Execute();
			void Reset();

			void Parse(Lexer&);
		};
	}
}

#endif