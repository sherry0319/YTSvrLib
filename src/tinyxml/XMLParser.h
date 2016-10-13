/*MIT License

Copyright (c) 2016 Zhe Xu

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

#include "tinyxml.h"

using std::string;

class CXMLParser
{
public:

	CXMLParser(void)
	{
		m_xml.Clear();
	}

	~CXMLParser(void)
	{
		m_xml.Clear();
	}

private:

	TiXmlDocument m_xml;

	TiXmlElement* m_pElement;

private:

	TiXmlElement* GetFirstElement(LPCSTR ElementMark,TiXmlElement* pElement);

public:

	//解析xml字符串
	bool ParseXmlStr(LPCSTR lpszXmlStr);

	//解析xml文件
	bool ParseXmlFile(LPCSTR lpszXmlFilePath);

	//根据标签取值
	TiXmlElement* GetFirstElementValue(LPCSTR lpszElementMark,string& strValue);

	//针对同一标签的记录取值
	TiXmlElement* GetNextElementValue(TiXmlElement* pElement,LPCSTR lpszElementMark,string& strValue);

	//取得属性值
	bool GetElementAttributeValue(TiXmlElement* pElement,LPCSTR lpszAttributeName,string& strValue);

	//获取根结点
	TiXmlElement* GetRootElement();

	//返回当前的xml字符串
	string GetXmlStr();

	//清空解析的内容(注意:不要自己delete.tinyxml会遍历delete所有的链表)
	void Clear();

	//添加一个根节点
	TiXmlElement* AddXmlRootElement(LPCSTR lpszElementMark);

	//添加子节点
	TiXmlElement* AddXmlChildElement(TiXmlElement* pElement,LPCSTR lpszElementMark,LPCSTR lpszValue = NULL);
	TiXmlElement* AddXmlChildElementW(TiXmlElement* pElement,LPCSTR lpszElementMark,LPCWSTR lpwzValue);
	TiXmlElement* AddXmlChildElementInt(TiXmlElement* pElement,LPCSTR lpszElementMark,int nValue);

	//给节点添加值
	TiXmlText* AddElementValue(TiXmlElement* pElement,LPCSTR lpszToAdd);

	//添加属性及属性值
	TiXmlElement* AddXmlAttribute(TiXmlElement* pElement,LPCSTR lpszAttributeKey,LPCSTR lpszAttributeValue);

	//添加声明
	TiXmlDeclaration* AddXmlDeclaration(LPCSTR lpszVer,LPCSTR lpszEncode,LPCSTR lpszStandalone);

	//添加注释
	TiXmlComment* AddXmlComment(TiXmlElement* pElement,LPCSTR lpszComment);

	//将xml内容保存到文件
	bool SaveFile(LPCSTR lpszFileName);

};

#endif

