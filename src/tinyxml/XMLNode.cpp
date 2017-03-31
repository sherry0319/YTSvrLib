#include "stdafx.h"
#include "XMLNode.h"

namespace YTSvrLib
{
	CXMLNode::~CXMLNode()
	{
		m_pCurElement = NULL;

		for (auto& pair : m_mapChildElement)
		{
			for (auto& node : pair.second)
			{
				delete node;
			}
		}

		m_mapChildElement.clear();
	}

	CXMLNode& CXMLNode::operator=(const CXMLNode& base)
	{
		m_pCurElement = base.m_pCurElement;

		return (*this);
	}

	bool CXMLNode::operator==(const CXMLNode& o)
	{
		return (m_pCurElement == o.m_pCurElement);
	}

	CXMLNode::operator const char*() const
	{
		const char* value = m_pCurElement->GetText();
		if (value == NULL)
		{
			return "";
		}
		return value;
	}

	CXMLNode::operator int() const
	{
		const char* value = m_pCurElement->GetText();
		if (value == NULL)
		{
			return 0;
		}
		return atoi(value);
	}

	CXMLNode::operator double() const
	{
		const char* value = m_pCurElement->GetText();
		if (value == NULL)
		{
			return 0.000000;
		}
		return atof(value);
	}

	CXMLNode::operator long() const
	{
		const char* value = m_pCurElement->GetText();
		if (value == NULL)
		{
			return 0;
		}
		return atol(value);
	}

	CXMLNode::operator LONGLONG() const
	{
		const char* value = m_pCurElement->GetText();
		if (value == NULL)
		{
			return 0;
		}
		return atoll(value);
	}

	//添加一个根节点
	CXMLNode* CXMLNode::AddElement(const char* key)
	{
		tinyxml2::XMLElement* pElementChild = m_doc.NewElement(key);

		m_pCurElement->LinkEndChild(pElementChild);

		CXMLNode* pNode = new CXMLNode(pElementChild,m_doc);

		m_mapChildElement[key].insert(pNode);

		return pNode;
	}

	//找到一个根节点
	CXMLNode* CXMLNode::GetElement(const char* key)
	{
		auto it = m_mapChildElement.find(key);
		if (it != m_mapChildElement.end() && !it->second.empty())
		{
			return (*(it->second.begin()));
		}

		return NULL;
	}

	//给当前根节点设置一个值
	void CXMLNode::SetValue(const char* value)
	{
		m_pCurElement->SetText(value);
	}

	//给当前根节点设置一个属性
	void CXMLNode::SetAttribute(const char* name, const char* value)
	{
		m_pCurElement->SetAttribute(name, value);
	}

	//给当前根节点设置一个注释
	void CXMLNode::SetComment(const char* comment)
	{
		m_pCurElement->LinkEndChild(m_doc.NewComment(comment));
	}

	const char* CXMLNode::GetValue()
	{
		return m_pCurElement->GetText();
	}

	const char* CXMLNode::GetName()
	{
		return m_pCurElement->Value();
	}

	CXMLNode& CXMLNode::Append(const char* key)
	{
		CXMLNode* pNewNode = AddElement(key);

		return (*pNewNode);
	}

	CXMLNode& CXMLNode::operator[](const char* key)
	{
		CXMLNode* pNode = GetElement(key);
		if (pNode == NULL)
		{
			pNode = AddElement(key);
		}

		return (*pNode);
	}

	CXMLNode& CXMLNode::operator[](std::string key)
	{
		CXMLNode* pNode = GetElement(key.c_str());
		if (pNode == NULL)
		{
			pNode = AddElement(key.c_str());
		}

		return (*pNode);
	}
}