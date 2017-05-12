// URLWriter.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "URLWriter.h"

struct
{
	const char* url;
	const char* post;
}data[] = {
	{"http://v.juhe.cn/postcode/query?postcode=215001&key=%E7%94%B3%E8%AF%B7%E7%9A%84KEY",NULL},
	{"http://v.juhe.cn/postcode/query","postcode=215001&key=%E7%94%B3%E8%AF%B7%E7%9A%84KEY"},
};

void OnURLRequestCallback(YTSvrLib::CURLRequest* pReq)
{
	cout << "request index = " << pReq->m_ayParam[0] << endl;

	cout << "request code = " << pReq->m_nReturnCode << endl;

	cout << "return field = " << pReq->m_strReturn << endl;
}

void OnURLRequestSync()
{
	std::string outdata;
	int nResponseCode = YTSvrLib::CGlobalCURLRequest::GetInstance()->SendHTTPGETMessage(data[0].url, outdata);

	cout << "sync get request code = " << nResponseCode << endl;

	cout << "sync get request return field = " << outdata << endl;

	outdata.clear();
	outdata.shrink_to_fit();
	nResponseCode = YTSvrLib::CGlobalCURLRequest::GetInstance()->SendHTTPPOSTMessage(data[1].url, data[1].post, outdata);

	cout << "sync post request code = " << nResponseCode << endl;

	cout << "sync post request return field = " << outdata << endl;
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	OnURLRequestSync();

	CURLWriter::GetInstance()->StartURLWriter(5);

	int index = 0;

	while (true)
	{
		index++;
		for (int i = 0; i < _countof(data);++i)
		{
			CURLWriter::GetInstance()->AddURLRequest(data[i].url, data[i].post, (YTSvrLib::URLPARAM)index, 0, 0, 0, OnURLRequestCallback);
		}
		CURLWriter::GetInstance()->WaitForAllRequestDone();
	}

	return 0;
}