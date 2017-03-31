#include "stdafx.h"
#include "XMLDocument.h"

namespace YTSvrLib
{
	CXMLDocument::CXMLDocument()
	{
		m_bHaveDeclaration = false;
		m_xml.Clear();
		m_mapChildElement.clear();
	}

	CXMLDocument::CXMLDocument(const char* version, const char* encode, const char* standalone)
	{
		m_xml.Clear();

		m_mapChildElement.clear();

		m_bHaveDeclaration = false;

		SetDeclaration(version, encode, standalone);

	}

	CXMLDocument::~CXMLDocument()
	{
		m_xml.Clear();

		for (auto& pair : m_mapChildElement)
		{
			for (auto& node : pair.second)
			{
				delete node;
			}
		}

		m_mapChildElement.clear();
		m_bHaveDeclaration = false;
	}

	CXMLDocument::operator const char*()
	{
		m_printer.ClearBuffer();
		m_xml.Print(&m_printer);
		return m_printer.CStr();
	}

	//添加子节点
	CXMLNode* CXMLDocument::AddElement(const char* key)
	{
		tinyxml2::XMLElement* pRootElement = m_xml.NewElement(key);

		m_xml.InsertEndChild(pRootElement);

		CXMLNode* pNode = new CXMLNode(pRootElement,m_xml);

		m_mapChildElement[key].insert(pNode);

		return pNode;
	}

	CXMLNode* CXMLDocument::GetElement(const char* key)
	{
		auto it = m_mapChildElement.find(key);
		if (it != m_mapChildElement.end() && !it->second.empty())
		{
			return (*(it->second.begin()));
		}

		return NULL;
	}

	void CXMLDocument::SetDeclaration(const char* version, const char* encode, const char* standalone)
	{
		char buffer[256] = { 0 };
		if (standalone == NULL)
		{
			_snprintf_s(buffer,255,"xml version=\"%s\" encoding=\"%s\"",version,encode);
		}
		else
		{
			_snprintf_s(buffer, 255, "xml version=\"%s\" encoding=\"%s\" standalone=\"%s\"", version, encode,standalone);
		}

		if (m_bHaveDeclaration)
		{
			tinyxml2::XMLDeclaration* pDec = (tinyxml2::XMLDeclaration*)m_xml.FirstChild();

			pDec->SetValue(buffer);
		}
		else
		{
			m_xml.InsertFirstChild(m_xml.NewDeclaration(buffer));

			m_bHaveDeclaration = true;
		}
	}

	//设置文档的注释
	void CXMLDocument::SetComment(const char* comment)
	{
		if (m_bHaveDeclaration)
		{
			m_xml.InsertAfterChild(m_xml.FirstChildElement(), m_xml.NewComment(comment));
		}
		else
		{
			m_xml.InsertFirstChild(m_xml.NewComment(comment));
		}
	}

	CXMLNode& CXMLDocument::operator[](const char* key)
	{
		CXMLNode* pNode = GetElement(key);
		if (pNode == NULL)
		{
			pNode = AddElement(key);
		}

		return (*pNode);
	}
}