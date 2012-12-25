#include "DynamicBuffer.h"
#include <algorithm>

namespace StormSQL
{
	DynamicBuffer::DynamicBuffer(int _elementSize)
		: elementSize(_elementSize)
	{
	}

	void DynamicBuffer::copy(const DynamicBuffer& obj)
	{
		elementSize = obj.elementSize;

		for (unsigned int i = 0; i < obj.buffers.size(); i++)
		{
			int effectiveNumElements = obj.buffers[i].numElements + obj.buffers[i].removedIndices.size();
			byte* ptr = new byte[effectiveNumElements * elementSize];

			// Copy the data
			// Equivalent to memcpy(ptr, obj.buffers[i].second, obj.buffers[i].first * elementSize);
			std::copy(obj.buffers[i].ptr, 
				obj.buffers[i].ptr + effectiveNumElements * elementSize, 
				ptr);

			BufferInfo bi;
			bi.numElements = obj.buffers[i].numElements;
			bi.removedIndices = obj.buffers[i].removedIndices;
			bi.ptr = ptr;

			buffers.push_back(bi);
		}
	}

	DynamicBuffer::DynamicBuffer(const DynamicBuffer& obj)
	{
		copy(obj);
	}

	void DynamicBuffer::del()
	{
		for (unsigned int i = 0; i < buffers.size(); i++)
			delete [] buffers[i].ptr;
	}

	DynamicBuffer& DynamicBuffer::operator = (const DynamicBuffer& obj)
	{
		if (this != &obj)
		{
			del();
			copy(obj);
		}

		return *this;
	}

	DynamicBuffer::~DynamicBuffer()
	{
		del();
	}

	void DynamicBuffer::Expand(int numElements)
	{
		if (numElements == 0)
			return;

		byte* ptr = new byte[numElements * elementSize];

		BufferInfo bi;
		bi.numElements = numElements;
		bi.ptr = ptr;

		buffers.push_back(bi);
	}

	pair<int, int> DynamicBuffer::getBufferIndex(int index) const
	{
		int currentIndex = 0;

		for (unsigned int i = 0; i < buffers.size(); i++)
		{
			if (currentIndex + buffers[i].numElements > index)
			{
				int indexInBuffer = index - currentIndex;

				// Skip deleted elements
				for (set<int>::iterator iter = buffers[i].removedIndices.begin();
					iter != buffers[i].removedIndices.end() && *iter <= indexInBuffer;
					iter++)
				{
					indexInBuffer++;
				}

				return pair<int, int>(i, indexInBuffer);
			}

			currentIndex += buffers[i].numElements;
		}

		return pair<int, int>(-1, currentIndex);
	}

	void DynamicBuffer::Remove(int index)
	{
		pair<int, int> pos = getBufferIndex(index);

		buffers[pos.first].removedIndices.insert(pos.second);
		buffers[pos.first].numElements--;
	}

	byte* DynamicBuffer::GetElementPtr(int index)
	{
		pair<int, int> pos = getBufferIndex(index);

		if (pos.first == -1)
		{
			// Not big enough to contain index
			//TODO: Better expand logic here?
			Expand(max((index + 1) - pos.second, 100));
			return buffers[buffers.size() - 1].ptr + (index - pos.second) * elementSize;
		}

		return buffers[pos.first].ptr + pos.second * elementSize;
	}

	byte* DynamicBuffer::operator[] (int index)
	{
		return GetElementPtr(index);
	}

	void DynamicBuffer::Load(istream& in, int numElements)
	{
		del();
		Expand(numElements);

		in.read((char*)GetElementPtr(0), numElements * elementSize);
	}

	void DynamicBuffer::Store(ostream& out, int numElements)
	{
		int numLeft = numElements;

		for (unsigned int i = 0; i < buffers.size() && numLeft > 0; i++)
		{
			if (buffers[i].removedIndices.size() == 0)
			{
				out.write((const char*)buffers[i].ptr, min(numLeft, buffers[i].numElements) * elementSize);

				numLeft -= buffers[i].numElements;
			}
			else
			{
				int lastIndex = 0;

				for (set<int>::iterator iter = buffers[i].removedIndices.begin();
					iter != buffers[i].removedIndices.end();
					iter++)
				{
					if (*iter <= lastIndex)
					{
						lastIndex++;
						continue;
					}

					int numElements = min(numLeft, *iter - lastIndex);
					out.write((const char*)buffers[i].ptr + lastIndex * elementSize,
								numElements * elementSize);
					
					numLeft -= numElements;
					lastIndex += numElements + 1; // + 1 for the removed index
				}

				if (numLeft > 0 && lastIndex < buffers[i].numElements + buffers[i].removedIndices.size() - 1)
				{
					// Save the last batch of data
					int numElements = min((unsigned int)numLeft, 
						buffers[i].numElements + buffers[i].removedIndices.size() - lastIndex);
					out.write((const char*)buffers[i].ptr + lastIndex * elementSize,
								numElements * elementSize);

					numLeft -= numElements;
					lastIndex += numElements;
				}
			}
		}
	}
}