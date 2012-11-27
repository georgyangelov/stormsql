#ifndef _H_QUERIES_INSERT_INCLUDED
#define _H_QUERIES_INSERT_INCLUDED

#include "../StormSQL/Defines.h"
#include "../StormSQL/Table.h"

namespace StormSQL
{
	namespace Queries
	{
		class Insert
		{
		protected:
			Table* const table;
			byte* buffer;
			bool* valuesSet;
			bool inserted;

			byte* getPtr(int column) const;

		public:
			Insert(Table* const);
			Insert(const Insert&);
			~Insert();

			void SetByte(int column, byte value);
			void SetInt(int column, int value);
			void SetUInt(int column, unsigned int value);
			void SetString(int column, char* const value);

			bool AllColumnsSet() const;

			void Commit(bool resetAfter = true);
			void Reset();
		};
	}
}

#endif