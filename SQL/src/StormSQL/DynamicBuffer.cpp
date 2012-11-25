#include "DynamicBuffer.h"
#include <algorithm>

namespace StormSQL
{
	DynamicBuffer::DynamicBuffer(int _elementSize)
		: elementSize(_elementSize)
	{
	}

	DynamicBuffer::DynamicBuffer(const DynamicBuffer& obj)
	{
		elementSize = obj.elementSize;
		
		for (unsigned int i = 0; i < obj.buffers.size(); i++)
		{
			byte* ptr = new byte[obj.buffers[i].first * elementSize];

			// Copy the data
			copy(obj.buffers[i].second, obj.buffers[i].second + obj.buffers[i].first * elementSize, ptr);

			buffers.push_back(pair<int, byte*>(obj.buffers[i].first, ptr));
		}
	}

	void DynamicBuffer::del()
	{
		for (unsigned int i = 0; i < buffers.size(); i++)
			delete buffers[i].second;
	}

	DynamicBuffer::~DynamicBuffer()
	{
		del();
	}

	void DynamicBuffer::Expand(int numElements)
	{
		byte* ptr = new byte[numElements * elementSize];
		buffers.push_back(pair<int, byte*>(numElements, ptr));
	}

	byte* DynamicBuffer::GetElementPtr(int index)
	{
		int currentIndex = 0;

		for (unsigned int i = 0; i < buffers.size(); i++)
		{
			if (currentIndex + buffers[i].first > index)
			{
				return buffers[i].second + (index - currentIndex) * elementSize;
			}

			currentIndex += buffers[i].first;
		}

		// Not big enough to contain index
		//TODO: Better expand logic here?
		Expand(max((index + 1) - currentIndex, 100));

		return buffers[buffers.size() - 1].second + (index - currentIndex) * elementSize;
	}

	void DynamicBuffer::ReadFromStream(istream& in, int numElements)
	{
		del();
		Expand(numElements);

		in.read((char*)GetElementPtr(0), numElements * elementSize);
	}

	void DynamicBuffer::WriteToStream(ostream& out, int numElements)
	{
		int numLeft = numElements;

		for (unsigned int i = 0; i < buffers.size() && numLeft > 0; i++)
		{
			out.write((const char*)buffers[i].second, min(numLeft, buffers[i].first) * elementSize);

			numLeft -= buffers[i].first;
		}
	}
}