#ifndef _H_DYNAMIC_BUFFER_INCLUDED
#define _H_DYNAMIC_BUFFER_INCLUDED

#include "Defines.h"
#include <vector>
#include <istream>
#include <ostream>
#include <set>

using namespace std;

namespace StormSQL
{
	class DynamicBuffer
	{
	protected:
		struct BufferInfo
		{
			int numElements;
			set<int> removedIndices;

			byte* ptr;
		};

		int elementSize;
		vector< BufferInfo > buffers;

		void del();
		void copy(const DynamicBuffer&);

		// pair<BufferIndex, IndexInBuffer>
		// If out of bounds returns pair<-1, maxIndexAvailable>
		pair<int, int> getBufferIndex(int elementIndex) const;
	public:
		DynamicBuffer(int);
		DynamicBuffer(const DynamicBuffer&);
		DynamicBuffer& operator = (const DynamicBuffer&);
		~DynamicBuffer();

		void Expand(int);
		void Remove(int);
		byte* GetElementPtr(int);
		byte* operator [](int i);

		void Load(istream&, int);
		void Store(ostream&, int);
	};
}
#endif

