/*MIT License

Copyright (c) 2016 Archer Xu

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#pragma once

#ifndef _XML_DOCUMENT_H
#define _XML_DOCUMENT_H

#include "XMLNode.h"

namespace YTSvrLib
{
	class YTSVRLIB_EXPORT CXMLDocument
	{
	public:
		CXMLDocument();
		CXMLDocument(const char* version, const char* encode, const char* standalone = NULL);

		virtual ~CXMLDocument();
	private:
		//添加一个根节点
		CXMLNode* AddElement(const char* key);
		//找到一个根节点
		CXMLNode* GetElement(const char* key);
	public:
		//设置文档的属性
		void SetDeclaration(const char* version,const char* encode,const char* standalone = NULL);

		//设置文档的注释
		void SetComment(const char* comment);

		CXMLNode& operator[](const char* key);

		operator const char*();
	private:
		bool m_bHaveDeclaration;
		tinyxml2::XMLDocument m_xml;
		tinyxml2::XMLPrinter m_printer;
		CXMLNode::_XMLPPChildList m_mapChildElement;
	};
}

#endif