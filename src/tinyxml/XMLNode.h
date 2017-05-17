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

#ifndef _XML_NODE_H
#define _XML_NODE_H

#include "tinyxml2.h"

namespace YTSvrLib
{
	class CXMLNode
	{
	public:
		typedef std::unordered_map<std::string, std::unordered_set<CXMLNode*> > _XMLPPChildList;
	public:
		CXMLNode(tinyxml2::XMLElement* pCur, tinyxml2::XMLDocument& doc) : m_pCurElement(pCur), m_doc(doc)
		{
			m_mapChildElement.clear();
		}

		CXMLNode(const CXMLNode& base) : m_pCurElement(base.m_pCurElement), m_doc(base.m_doc)
		{
	
		}

		virtual ~CXMLNode();

		CXMLNode& operator=(const CXMLNode& base);
	private:
		//添加一个根节点
		CXMLNode* AddElement(const char* key);
		//找到一个根节点
		CXMLNode* GetElement(const char* key);
	public:
		//给当前根节点设置一个值
		void SetValue(const char* value);
		//给当前根节点设置一个属性
		template<typename T>
		void SetAttribute(const char* name, T value)
		{
			m_pCurElement->SetAttribute(name, value);
		}
		//给当前根节点设置一个注释
		void SetComment(const char* comment);
		//获得当前节点的值
		const char* GetValue();
		//获得当前节点的key
		const char* GetName();
		//获得当前属性的值
		const char* GetAttribute(const char* name);
		//添加一个新节点
		CXMLNode& Append(const char* key);

		bool operator==(const CXMLNode& o);

		operator const char*() const;

		operator int() const;

		operator double() const;

		operator long() const;

		operator LONGLONG() const;

		CXMLNode& operator[](const char* key);

		CXMLNode& operator[](std::string key);

		template<typename __VALUETYPE>
		CXMLNode& operator=(__VALUETYPE value)
		{
			std::ostringstream s;
			s << value;
			SetValue(s.str().c_str());
			return (*this);
		}
	private:
		tinyxml2::XMLDocument& m_doc;

		tinyxml2::XMLElement* m_pCurElement;

		_XMLPPChildList m_mapChildElement;
	};
}



#endif