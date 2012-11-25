#ifndef _H_DYNAMIC_BUFFER_INCLUDED
#define _H_DYNAMIC_BUFFER_INCLUDED

#include "Defines.h"
#include <vector>
#include <istream>
#include <ostream>

using namespace std;

namespace StormSQL
{
	class DynamicBuffer
	{
	protected:
		int elementSize;
		vector< pair<int, byte*> > buffers;

		void del();
	public:
		DynamicBuffer(int);
		DynamicBuffer(const DynamicBuffer&);

		~DynamicBuffer();

		void Expand(int);
		byte* GetElementPtr(int);

		void ReadFromStream(istream&, int);
		void WriteToStream(ostream&, int);
	};
}
#endif

