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
#include "stdafx.h"
#include "XMLParser.h"

bool CXMLParser::ParseXmlFile( LPCSTR lpszXmlFilePath )
{
	return m_xml.LoadFile(lpszXmlFilePath);
}

bool CXMLParser::ParseXmlStr( LPCSTR lpszXmlStr )
{
	if(m_xml.Parse(lpszXmlStr))
		return true;
	else
		return false;
}

TiXmlElement* CXMLParser::GetFirstElement( LPCSTR lpszElementMark,TiXmlElement* pElement )
{
    TiXmlElement* pElementTmp=NULL;

    pElementTmp=pElement;

    while(pElementTmp)
    {
        if(strcmp(pElementTmp->Value(),lpszElementMark)==0)
        {
            return pElementTmp;
        }
        else
        {
            TiXmlElement* pElementNext=pElementTmp->FirstChildElement();

            while(pElementNext)
            {
                if(strcmp(pElementNext->Value(),lpszElementMark)==0)
                {
                    return pElementNext;
                }
                else
                {
                    TiXmlElement* reElement=NULL;

                    reElement=GetFirstElement(lpszElementMark,pElementNext);

                    if(reElement)
                    {
                        return reElement;
                    }
                }

                pElementNext=pElementNext->NextSiblingElement();
            }

        }

        pElementTmp=pElementTmp->NextSiblingElement();

    }

    return NULL;
}

//根据标签取值

TiXmlElement* CXMLParser::GetFirstElementValue( LPCSTR lpszElementMark,string& strValue )
{
	strValue.clear();
    try
    {
        TiXmlElement* pElement=m_xml.RootElement();

        pElement=GetFirstElement(lpszElementMark,pElement);

        if(pElement)
        {
            m_pElement=pElement;

            strValue=m_pElement->GetText();

			return pElement;
        }

    }
    catch(...)
    {
		return NULL;
    }

    return NULL;
}

TiXmlElement* CXMLParser::GetNextElementValue(TiXmlElement* pElement, LPCSTR lpszElementMark,string& strValue )
{
    strValue.clear();

	if (pElement)
	{
		pElement = pElement->NextSiblingElement(lpszElementMark);

		if (pElement)
		{
			strValue=pElement->GetText();

			return pElement;
		}
	}
	else
	{
		m_pElement=m_pElement->NextSiblingElement(lpszElementMark);

		if(m_pElement)
		{
			strValue=m_pElement->GetText();

			return m_pElement;
		}
	}

    return NULL;
}

string CXMLParser::GetXmlStr()
{
    string result;
    try
    {
        TiXmlPrinter printer;

        m_xml.Accept(&printer);

        result=printer.CStr();
    }
    catch(...)
    {
		return string("");
    }

    return result;
}

void CXMLParser::Clear()
{
    m_xml.Clear();
}

//添加子节点

TiXmlElement* CXMLParser::AddXmlRootElement( LPCSTR lpszElementMark )
{
    TiXmlElement* pRootElement = new TiXmlElement(lpszElementMark);

    m_xml.LinkEndChild(pRootElement);

    return pRootElement;
}

TiXmlElement* CXMLParser::AddXmlChildElementW(TiXmlElement* pElement,LPCSTR lpszElementMark,LPCWSTR lpwzValue)
{
	CHAR szValue[2048] = {0};
	WChar2Ansi(lpwzValue,szValue,2047);

	return AddXmlChildElement(pElement,lpszElementMark,szValue);
}

TiXmlElement* CXMLParser::AddXmlChildElement( TiXmlElement* pElement,LPCSTR lpszElementMark,LPCSTR lpszValue/* = NULL*/ )
{
    if(pElement)
    {
        TiXmlElement* pElementAdd=new TiXmlElement(lpszElementMark);

        pElement->LinkEndChild(pElementAdd);

		if (lpszValue)
		{
			TiXmlText *pContent=new TiXmlText(lpszValue);

			pElementAdd->LinkEndChild(pContent);
		}

        return pElementAdd;
    }

    return NULL;
}

TiXmlElement* CXMLParser::AddXmlChildElementInt(TiXmlElement* pElement,LPCSTR lpszElementMark,int nValue/*=0*/)
{
	if(pElement)
	{
		TiXmlElement* pElementAdd=new TiXmlElement(lpszElementMark);

		pElement->LinkEndChild(pElementAdd);

		char szValue[32] = {0};
		int nSize = _snprintf_s(szValue, 31, "%d", nValue);
		if (nSize > 0)
		{
			TiXmlText *pContent=new TiXmlText(szValue);

			pElementAdd->LinkEndChild(pContent);
		}
		
		return pElementAdd;
	}

	return NULL;
}

TiXmlText* CXMLParser::AddElementValue( TiXmlElement* pElement,LPCSTR lpszToAdd )
{
    if(pElement)
    {
        TiXmlText *pContent=new TiXmlText(lpszToAdd);

        pElement->LinkEndChild(pContent);

		return pContent;
    }

	return NULL;
}

//添加属性及属性值
TiXmlElement* CXMLParser::AddXmlAttribute( TiXmlElement* pElement,LPCSTR lpszAttributeKey,LPCSTR lpszAttributeValue )
{
    if(pElement)
    {
        pElement->SetAttribute(lpszAttributeKey,lpszAttributeValue);
		
		return pElement;
    }

	return NULL;
}

//添加声明
TiXmlDeclaration* CXMLParser::AddXmlDeclaration( LPCSTR lpszVer,LPCSTR lpszEncode,LPCSTR lpszStandalone )
{
    TiXmlDeclaration *pDeclaration=new TiXmlDeclaration(lpszVer,lpszEncode,lpszStandalone);

    m_xml.LinkEndChild(pDeclaration);

	return pDeclaration;
}

//添加注释
TiXmlComment* CXMLParser::AddXmlComment( TiXmlElement* pElement,LPCSTR lpszComment )
{
	TiXmlComment *pComment=new TiXmlComment(lpszComment);
    if(pElement)
    {
        pElement->LinkEndChild(pComment);
    }
	else
	{
		m_xml.LinkEndChild(pComment);
	}

	return pComment;
}

TiXmlElement* CXMLParser::GetRootElement()
{
    return m_xml.RootElement();
}

//取得属性值

bool CXMLParser::GetElementAttributeValue( TiXmlElement* pElement,LPCSTR lpszAttributeName,string& strValue )
{
	strValue.clear();
    if(pElement)
    {
        strValue=pElement->Attribute(lpszAttributeName);

        return true;
    }

    return false;
}

bool CXMLParser::SaveFile( LPCSTR lpszFileName )
{
   return m_xml.SaveFile(lpszFileName);
}

//////////////////////////////////////////