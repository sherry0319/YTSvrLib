#pragma once

#include "resource.h"

class CURLWriter : public YTSvrLib::CURLWriterFactory, public YTSvrLib::CSingle<CURLWriter>
{
public:
	CURLWriter() :YTSvrLib::CURLWriterFactory()
	{

	}

	virtual ~CURLWriter()
	{

	}


};