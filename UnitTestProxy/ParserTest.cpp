#include "stdafx.h"
#include "CppUnitTest.h"
#include <map>
#include <Parser.h>

using namespace std;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestProxy
{
	TEST_CLASS(UnitTest1)
	{
		map<string, string> preparedata()
		{
			map<string, string> data;
			data["Event:"] = "UserEvent";
			data["UserEvent:"] = "initcall";
			data["src"] = "8987654321";
			data["dst"] = "8123456789";
			data["userid"] = "22";
			data["time"] = "123456789123456789";
			data["callid"] = "111222333";

			return data;
		}

		map<string, string> preparedata_finishcall()
		{
			map<string, string> data;
			data["Event:"] = "UserEvent";
			data["UserEvent:"] = "finishcall";
			data["src"] = "8987654321";
			data["dst"] = "8123456789";
			data["userid"] = "22";
			data["time"] = "123456789123456789";
			data["callid"] = "111222333";
			data["callstart"] = "123456789123456799";
			data["callanswer"] = "123456789123456999";
			data["status"] = "ANSERED";
			data["calltype"] = "1";

			return data;
		}
	public:

		TEST_METHOD(ConcurentCalls_INSERT)
		{
			// TODO: Your test code here
			

			Parser parser;
			auto data = preparedata();

			parser.parsedata(data);

			auto calls = parser.getCallRecords();
			
			Assert::AreEqual(static_cast<int>(calls.size()),1, L"CallRecords size not equal 1");

			auto Call = calls.begin();

			Assert::AreEqual(static_cast<int>((*Call).first==data.at("src")),1,L"SRC not equal");
		}

		TEST_METHOD(ConcurentCalls_DELETE)
		{
			Parser parser;
			auto data = preparedata();
			parser.parsedata(data);

			auto data_finishcall = preparedata_finishcall();
			parser.parsedata(data_finishcall);

			auto calls = parser.getCallRecords();
			Assert::AreEqual(static_cast<int>(calls.size()), 0, L"CallRecords size not equal 0");

		}

	};
}