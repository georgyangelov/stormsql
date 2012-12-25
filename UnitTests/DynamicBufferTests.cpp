#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include <fstream>
#include "StormSQL/DynamicBuffer.h"

using namespace std;
using namespace StormSQL;

namespace UnitTests
{		
	TEST_CLASS(DynamicBufferTests)
	{
	public:
		
		TEST_METHOD(AddExpandTest)
		{
			DynamicBuffer obj(sizeof(int));
			
			*((int*)obj.GetElementPtr(0)) = 1;
			Assert::AreEqual(*((int*)obj.GetElementPtr(0)), 1);
			
			*((int*)obj.GetElementPtr(10)) = 11;
			Assert::AreEqual(*((int*)obj.GetElementPtr(10)), 11);

			*((int*)obj.GetElementPtr(1000)) = 1122;
			Assert::AreEqual(*((int*)obj.GetElementPtr(1000)), 1122);
		}
		
		TEST_METHOD(CopyConstructorTest)
		{
			DynamicBuffer obj(sizeof(int));
			
			*((int*)obj.GetElementPtr(0)) = 1;
			*((int*)obj.GetElementPtr(10)) = 11;
			*((int*)obj.GetElementPtr(1000)) = 1122;

			DynamicBuffer obj2(obj);

			Assert::AreEqual(*((int*)obj2.GetElementPtr(0)), 1);
			Assert::AreEqual(*((int*)obj2.GetElementPtr(10)), 11);
			Assert::AreEqual(*((int*)obj2.GetElementPtr(1000)), 1122);
		}

		TEST_METHOD(LoadSaveTest)
		{
			DynamicBuffer obj(sizeof(int));
			*((int*)obj.GetElementPtr(0)) = 1;
			*((int*)obj.GetElementPtr(9)) = 9;
			*((int*)obj.GetElementPtr(10)) = 10;
			*((int*)obj.GetElementPtr(11)) = 11;
			*((int*)obj.GetElementPtr(999)) = 1122;
			*((int*)obj.GetElementPtr(1000)) = 11232;

			obj.Remove(10);
			Assert::AreEqual(*((int*)obj.GetElementPtr(998)), 1122);
			Assert::AreEqual(*((int*)obj.GetElementPtr(999)), 11232);

			obj.Remove(9);
			Assert::AreEqual(*((int*)obj.GetElementPtr(997)), 1122);
			Assert::AreEqual(*((int*)obj.GetElementPtr(998)), 11232);

			ofstream out("test.bin");
			obj.Store(out, 999);
			out.close();


			DynamicBuffer obj2(sizeof(int));

			ifstream in("test.bin");
			obj2.Load(in, 999);
			in.close();
			
			Assert::AreEqual(*((int*)obj2.GetElementPtr(0)), 1);
			Assert::AreEqual(*((int*)obj2.GetElementPtr(9)), 11);
			Assert::AreEqual(*((int*)obj2.GetElementPtr(997)), 1122);
			Assert::AreEqual(*((int*)obj2.GetElementPtr(998)), 11232);
		}

		TEST_METHOD(BufferDeleteTest)
		{
			DynamicBuffer obj(sizeof(int));
			
			*((int*)obj.GetElementPtr(0)) = 1;
			*((int*)obj.GetElementPtr(10)) = 11;
			*((int*)obj.GetElementPtr(11)) = 12;

			*((int*)obj.GetElementPtr(1000)) = 1122;

			Assert::AreEqual(*((int*)obj.GetElementPtr(0)), 1);
			Assert::AreEqual(*((int*)obj.GetElementPtr(10)), 11);
			Assert::AreEqual(*((int*)obj.GetElementPtr(11)), 12);

			obj.Remove(10);

			Assert::AreEqual(*((int*)obj.GetElementPtr(10)), 12);
			Assert::AreEqual(*((int*)obj.GetElementPtr(999)), 1122);
		}

	};
}