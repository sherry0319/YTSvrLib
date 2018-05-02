// Utility.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Utility.h"

typedef void(*TEST_FUNCTION)();

void Test_StrDelimiter()
{
	const char* pszStr = "aaa|bbb^ddd|eee^fff|ggg|hhh";
	vector<string> vctList;

	StrDelimiter(pszStr, "^", vctList);
	cout << pszStr << endl;
	for (size_t i = 0; i < vctList.size();++i)
	{
		vector<string> vctSubList;

		cout << "\t" << vctList[i] << endl;

		StrDelimiter(vctList[i], "|", vctSubList);

		for (size_t j = 0; j < vctSubList.size();++j)
		{
			cout << "\t\t" << vctSubList[j] << endl;
		}
	}
}

void Test_StringReplace()
{
	string str = "hello sam hello kitty hello tommy";

	cout << "\tbefore : " << str << endl;

	StrReplace(str, "hello", "goodbye");

	cout << "\tafter : " << str << endl;
}

void Test_Random()
{
#define GET_RANDOM_COUNT	10
	cout << "get " << GET_RANDOM_COUNT<<" random int value between 0-10000 >> " << endl;

	for (int i = 0; i < GET_RANDOM_COUNT; ++i)
	{
		cout << "[" << Random2(10000) << "] ";
	}
	cout << endl;
	cout << "get " << GET_RANDOM_COUNT<<" random double value between 0-1" << endl;

	for (int i = 0; i < GET_RANDOM_COUNT; ++i)
	{
		cout << "[" << Random2(1.0000,0.0000) << "] ";
	}
	cout << endl;
}

void Test_Datatime()
{
	char buffer[1024] = { 0 };
	char type[] = "ASDTWN";
	for (int i = 0; i < strlen(type);++i)
	{
		cout << "TYPE " << type[i] << " : " << GetDateTime(buffer, 1023, type[i]) << endl;
	}
}

void Test_MD5()
{
	char output[34] = { 0 };
	
	cout << "string MD5 : " << MD5("hello world",output) << endl;

	struct
	{
		int numbervalue;
		char buffer[128];
		double doublevalue;
	}test;

	test.numbervalue = 100;
	test.doublevalue = 23.4122;
	strncpy_s(test.buffer, "hello world", 127);

	cout << "binary MD5 : " << MD5((LPCSTR)&test, sizeof(test),output) << endl;
}

void Test_UrlEncode()
{
	std::string orignal = "name=你好";
	
	cout << "orignal string : " << orignal << endl;

	std::string encode = URLEncode(orignal);

	cout << "encoded string : " << encode << endl;

	cout << "decoded string : " << URLDecode(encode) << endl;
}

void Test_ProcessNamePath()
{
	char path[MAX_PATH] = { 0 };

	GetModuleFilePath(path, MAX_PATH);

	char name[MAX_PATH] = { 0 };

	GetModuleFileName(name, MAX_PATH);

	cout << "cur process path : " << path << endl;
	cout << "cur process name : " << name << endl;
}

void Test_CovertUTC2String()
{
	__time32_t tNow = (__time32_t)time(NULL);

	tNow -= Random2(SEC_DAY);// reduce random seconds in a day

	cout << "UTC time is : " << tNow << endl;

	char buffer[128] = { 0 };
	cout << "String local time is : " << CovertUTC2String(tNow, buffer, 127) << endl;
}

void Test_MakeRandomKey()
{
#define RANDOM_KEY_MAX_LENGTH	256
	char key[4][256] = { 0 };

	for (int i = 0; i < 4;++i)
	{
		int length = (int)pow(2, 4 + i);
		MakeRandomKey(key[i], RANDOM_KEY_MAX_LENGTH, length);

		cout << "Random key length " << length << " : " << key[i] << endl;
	}
}

struct
{
	const char* name;
	TEST_FUNCTION func;
}g_ayFunctions[] = {
	{ "string split",Test_StrDelimiter},
	{ "string replace",Test_StringReplace },
	{ "get random number",Test_Random },
	{ "get datetime", Test_Datatime },
	{ "md5", Test_MD5 },
	{ "urlencode", Test_UrlEncode },
	{ "process name & path", Test_ProcessNamePath },
	{ "covert utc time to string local time", Test_CovertUTC2String},
	{ "make random key", Test_MakeRandomKey }
};

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	BEGIN_WATCH(PERFORMANCE_MICRO_SECONDS);

	for (size_t i = 0; i < _countof(g_ayFunctions);++i)
	{
		if (g_ayFunctions[i].func && g_ayFunctions[i].name)
		{
			cout << "begin " << g_ayFunctions[i].name << " test >>>>>" << endl;

			g_ayFunctions[i].func();

			CHECK_WATCH("function [%s] cost", g_ayFunctions[i].name);

			cout << "end " << g_ayFunctions[i].name << " test <<<<<" << endl;

			cout << "###########################################################################" << endl;
		}
	}

	END_WATCH();

	getchar();

	return 0;
}