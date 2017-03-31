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
#ifndef _XML_PARSER_H
#define _XML_PARSER_H

#include<string>

#include "tinyxml2.h"

using std::string;

class CXMLParser
{
public:

	CXMLParser(void)
	{
		m_xml.Clear();
		m_pElement = NULL;
	}

	~CXMLParser(void)
	{
		m_xml.Clear();
	}

private:

	tinyxml2::XMLDocument m_xml;

	tinyxml2::XMLElement* m_pElement;

private:

	tinyxml2::XMLElement* GetFirstElement(LPCSTR ElementMark, tinyxml2::XMLElement* pElement);

public:

	//解析xml字符串
	bool ParseXmlStr(LPCSTR lpszXmlStr);

	//解析xml文件
	bool ParseXmlFile(LPCSTR lpszXmlFilePath);

	//根据标签取值
	tinyxml2::XMLElement* GetFirstElementValue(LPCSTR lpszElementMark, string& strValue);

	//针对同一标签的记录取值
	tinyxml2::XMLElement* GetNextElementValue(tinyxml2::XMLElement* pElement, LPCSTR lpszElementMark, string& strValue);

	//取得属性值
	bool GetElementAttributeValue(tinyxml2::XMLElement* pElement,LPCSTR lpszAttributeName,string& strValue);

	//获取根结点
	tinyxml2::XMLElement* GetRootElement();

	//返回当前的xml字符串
	string GetXmlStr();

	//清空解析的内容(注意:不要自己delete.tinyxml会遍历delete所有的链表)
	void Clear();

	//添加一个根节点
	tinyxml2::XMLElement* AddXmlRootElement(LPCSTR lpszElementMark);

	//添加子节点
	tinyxml2::XMLElement* AddXmlChildElement(tinyxml2::XMLElement* pElement, LPCSTR lpszElementMark);

	tinyxml2::XMLElement* AddXmlChildElement(tinyxml2::XMLElement* pElement,LPCSTR lpszElementMark,LPCSTR lpszValue);
	tinyxml2::XMLElement* AddXmlChildElement(tinyxml2::XMLElement* pElement,LPCSTR lpszElementMark,LPCWSTR lpwzValue);
	tinyxml2::XMLElement* AddXmlChildElement(tinyxml2::XMLElement* pElement,LPCSTR lpszElementMark,int nValue);
	tinyxml2::XMLElement* AddXmlChildElement(tinyxml2::XMLElement* pElement, LPCSTR lpszElementMark, LONGLONG nValue);
	tinyxml2::XMLElement* AddXmlChildElement(tinyxml2::XMLElement* pElement, LPCSTR lpszElementMark, long nValue);
	tinyxml2::XMLElement* AddXmlChildElement(tinyxml2::XMLElement* pElement, LPCSTR lpszElementMark, double dValue);

	//给节点添加值
	tinyxml2::XMLElement* AddElementValue(tinyxml2::XMLElement* pElement, LPCSTR lpszToAdd);

	//添加属性及属性值
	tinyxml2::XMLElement* AddXmlAttribute(tinyxml2::XMLElement* pElement,LPCSTR lpszAttributeKey,LPCSTR lpszAttributeValue);

	//添加声明
	tinyxml2::XMLDeclaration* AddXmlDeclaration(LPCSTR lpszVer,LPCSTR lpszEncode,LPCSTR lpszStandalone);

	//添加注释
	tinyxml2::XMLComment* AddXmlComment(tinyxml2::XMLElement* pElement,LPCSTR lpszComment);

	//将xml内容保存到文件
	bool SaveFile(LPCSTR lpszFileName);

};

#endif

