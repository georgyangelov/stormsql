#ifndef _H_FIELD_INCLUDED
#define _H_FIELD_INCLUDED

#include <istream>
#include <ostream>
#include "Defines.h"

using namespace std;

namespace StormSQL
{

	struct Field
	{
		enum FieldType
		{
			byte = 0,
			int32 = 1,
			uint32 = 4,
			fixedchar = 8
		};

		char name[STORM_SQL_FIELD_NAME_SIZE];
		FieldType type;

		unsigned int size;

		int GetByteSize() const;

		void WriteToStream(ostream&) const;
		void ReadFromStream(istream&);
	};
}

#endif

