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

tinyxml2::XMLElement* CXMLParser::GetFirstElement(LPCSTR lpszElementMark, tinyxml2::XMLElement* pElement)
{
	tinyxml2::XMLElement* pElementTmp = NULL;

    pElementTmp=pElement;

    while(pElementTmp)
    {
        if(strcmp(pElementTmp->Value(),lpszElementMark)==0)
        {
            return pElementTmp;
        }
        else
        {
			tinyxml2::XMLElement* pElementNext = pElementTmp->FirstChildElement();

            while(pElementNext)
            {
                if(strcmp(pElementNext->Value(),lpszElementMark)==0)
                {
                    return pElementNext;
                }
                else
                {
					tinyxml2::XMLElement* reElement = NULL;

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

tinyxml2::XMLElement* CXMLParser::GetFirstElementValue(LPCSTR lpszElementMark, string& strValue)
{
	strValue.clear();
    try
    {
		tinyxml2::XMLElement* pElement = m_xml.RootElement();

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

tinyxml2::XMLElement* CXMLParser::GetNextElementValue(tinyxml2::XMLElement* pElement, LPCSTR lpszElementMark, string& strValue)
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
        tinyxml2::XMLPrinter printer;

        m_xml.Accept(&printer);

        result=printer.CStr();
    }
    catch(...)
    {
		return std::string("");
    }

    return result;
}

void CXMLParser::Clear()
{
    m_xml.Clear();
}

//添加子节点

tinyxml2::XMLElement* CXMLParser::AddXmlRootElement( LPCSTR lpszElementMark )
{
	tinyxml2::XMLElement* pRootElement = m_xml.NewElement(lpszElementMark);

	m_xml.LinkEndChild(pRootElement);

    return pRootElement;
}

tinyxml2::XMLElement* CXMLParser::AddXmlChildElement(tinyxml2::XMLElement* pElement, LPCSTR lpszElementMark)
{
	if (pElement)
	{
		tinyxml2::XMLElement* pElementAdd = m_xml.NewElement(lpszElementMark);

		pElement->LinkEndChild(pElementAdd);

		return pElementAdd;
	}

	return NULL;
}

tinyxml2::XMLElement* CXMLParser::AddXmlChildElement( tinyxml2::XMLElement* pElement,LPCSTR lpszElementMark,LPCSTR lpszValue )
{
    if(pElement)
    {
		tinyxml2::XMLElement* pElementAdd = m_xml.NewElement(lpszElementMark);

        pElement->LinkEndChild(pElementAdd);

		if (lpszValue)
		{
			pElementAdd->SetText(lpszValue);
		}

        return pElementAdd;
    }

    return NULL;
}


tinyxml2::XMLElement* CXMLParser::AddXmlChildElement(tinyxml2::XMLElement* pElement, LPCSTR lpszElementMark, LPCWSTR lpwzValue)
{
	CHAR szValue[2048] = { 0 };
	WChar2Ansi(lpwzValue, szValue, 2047);

	return AddXmlChildElement(pElement, lpszElementMark, szValue);
}

tinyxml2::XMLElement* CXMLParser::AddXmlChildElement(tinyxml2::XMLElement* pElement, LPCSTR lpszElementMark, LONGLONG nValue)
{
	if (pElement)
	{
		tinyxml2::XMLElement* pElementAdd = m_xml.NewElement(lpszElementMark);

		pElement->LinkEndChild(pElementAdd);

		pElementAdd->SetText((int64_t)nValue);

		return pElementAdd;
	}

	return NULL;
}

tinyxml2::XMLElement* CXMLParser::AddXmlChildElement(tinyxml2::XMLElement* pElement, LPCSTR lpszElementMark, long nValue)
{
	if (pElement)
	{
		tinyxml2::XMLElement* pElementAdd = m_xml.NewElement(lpszElementMark);

		pElement->LinkEndChild(pElementAdd);

		pElementAdd->SetText(nValue);

		return pElementAdd;
	}

	return NULL;
}

tinyxml2::XMLElement* CXMLParser::AddXmlChildElement(tinyxml2::XMLElement* pElement, LPCSTR lpszElementMark, double dValue)
{
	if (pElement)
	{
		tinyxml2::XMLElement* pElementAdd = m_xml.NewElement(lpszElementMark);

		pElement->LinkEndChild(pElementAdd);

		pElementAdd->SetText(dValue);

		return pElementAdd;
	}

	return NULL;
}

tinyxml2::XMLElement* CXMLParser::AddXmlChildElement(tinyxml2::XMLElement* pElement,LPCSTR lpszElementMark,int nValue)
{
	if(pElement)
	{
		tinyxml2::XMLElement* pElementAdd = m_xml.NewElement(lpszElementMark);

		pElement->LinkEndChild(pElementAdd);

		pElementAdd->SetText(nValue);
		
		return pElementAdd;
	}

	return NULL;
}

tinyxml2::XMLElement* CXMLParser::AddElementValue(tinyxml2::XMLElement* pElement, LPCSTR lpszToAdd)
{
    if(pElement)
    {
		pElement->SetText(lpszToAdd);
    }

	return pElement;
}

//添加属性及属性值
tinyxml2::XMLElement* CXMLParser::AddXmlAttribute( tinyxml2::XMLElement* pElement,LPCSTR lpszAttributeKey,LPCSTR lpszAttributeValue )
{
    if(pElement)
    {
        pElement->SetAttribute(lpszAttributeKey,lpszAttributeValue);
    }

	return pElement;
}

//添加声明
tinyxml2::XMLDeclaration* CXMLParser::AddXmlDeclaration( LPCSTR lpszVer,LPCSTR lpszEncode,LPCSTR lpszStandalone )
{
	char buffer[256] = { 0 };
	if (lpszStandalone == NULL)
	{
		_snprintf_s(buffer, 255, "xml version=\"%s\" encoding=\"%s\"", lpszVer, lpszEncode);
	}
	else
	{
		_snprintf_s(buffer, 255, "xml version=\"%s\" encoding=\"%s\" standalone=\"%s\"", lpszVer, lpszEncode, lpszStandalone);
	}

	return m_xml.NewDeclaration(buffer);
}

//添加注释
tinyxml2::XMLComment* CXMLParser::AddXmlComment( tinyxml2::XMLElement* pElement,LPCSTR lpszComment )
{
	if (pElement)
	{
		tinyxml2::XMLComment *pComment = m_xml.NewComment(lpszComment);

		pElement->LinkEndChild(pComment);

		return pComment;
	}
	else
	{
		return m_xml.NewComment(lpszComment);
	}
}

tinyxml2::XMLElement* CXMLParser::GetRootElement()
{
    return m_xml.RootElement();
}

//取得属性值

bool CXMLParser::GetElementAttributeValue( tinyxml2::XMLElement* pElement,LPCSTR lpszAttributeName,string& strValue )
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