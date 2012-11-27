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
			*((int*)obj.GetElementPtr(10)) = 11;
			*((int*)obj.GetElementPtr(999)) = 1122;
			*((int*)obj.GetElementPtr(1000)) = 11232;


			ofstream out("test.bin");
			obj.Store(out, 1001);
			out.close();


			DynamicBuffer obj2(sizeof(int));

			ifstream in("test.bin");
			obj2.Load(in, 1001);
			in.close();
			
			Assert::AreEqual(*((int*)obj2.GetElementPtr(0)), 1);
			Assert::AreEqual(*((int*)obj2.GetElementPtr(10)), 11);
			Assert::AreEqual(*((int*)obj2.GetElementPtr(999)), 1122);
			Assert::AreEqual(*((int*)obj2.GetElementPtr(1000)), 11232);
		}

	};
}