#include "StdAfx.h"
#include "Property.h"
#include <MAPI/ColumnTags.h>
#include <sstream>

wstring cdataopen = L"<![CDATA[";
wstring cdataclose = L"]]>";

wstring tagopen(_In_ const wstring& szTag, int iIndent)
{
	return indent(iIndent) + L"<" + szTag + L">";
}

wstring tagclose(_In_ const wstring& szTag, int iIndent)
{
	return indent(iIndent) + L"</" + szTag + L">\n";
}

Parsing::Parsing(const wstring& szParsing, bool bXMLSafe, const Attributes& attributes)
{
	m_szParsing = szParsing;
	m_bXMLSafe = bXMLSafe;
	m_attributes = attributes;
}

Parsing::Parsing(const Parsing& other)
{
	m_szParsing = other.m_szParsing;
	m_bXMLSafe = other.m_bXMLSafe;
	m_attributes = other.m_attributes;
}

wstring Parsing::toXML(UINT uidTag, int iIndent) const
{
	if (m_szParsing.empty()) return L"";

	auto szTag = loadstring(uidTag);
	std::wstringstream szXML;
	szXML << tagopen(szTag + m_attributes.toXML(), iIndent);

	if (!m_bXMLSafe)
	{
		szXML << cdataopen;
	}

	szXML << ScrubStringForXML(m_szParsing);

	if (!m_bXMLSafe)
	{
		szXML << cdataclose;
	}

	szXML << tagclose(szTag, 0);

	return szXML.str();
}

wstring Parsing::toString() const
{
	auto cb = m_attributes.GetAttribute(L"cb");
	if (!cb.empty())
	{
		return L"cb: " + cb + L" lpb: " + m_szParsing;
	}

	auto err = m_attributes.GetAttribute(L"err");
	if (!err.empty())
	{
		return L"Err: " + err + L"=" + m_szParsing;
	}

	return m_szParsing;
}

void Property::AddParsing(const Parsing& mainParsing, const Parsing& altParsing)
{
	m_MainParsing.push_back(mainParsing);
	m_AltParsing.push_back(altParsing);
}

// Only used to add a MV row parsing to the main set, so we only
// need to copy the first entry and no attributes
void Property::AddMVParsing(const Property& Property)
{
	m_MainParsing.push_back(Property.m_MainParsing[0]);
	m_AltParsing.push_back(Property.m_AltParsing[0]);
}

void Property::AddAttribute(const wstring& key, const wstring& value)
{
	m_attributes.AddAttribute(key, value);
}

wstring Property::toXML(int iIndent) const
{
	std::wstringstream szXML;
	auto mv = m_attributes.GetAttribute(L"mv");

	if (mv == L"true")
	{
		auto szValue = loadstring(PropXMLNames[pcPROPVAL].uidName);
		auto szRow = loadstring(IDS_ROW);

		szXML << tagopen(szValue + m_attributes.toXML(), iIndent) + L"\n";

		for (const auto& parsing : m_MainParsing)
		{
			szXML << tagopen(szRow, iIndent + 1) + L"\n";
			szXML << parsing.toXML(PropXMLNames[pcPROPVAL].uidName, iIndent + 2);
			szXML << parsing.toXML(PropXMLNames[pcPROPVALALT].uidName, iIndent + 2);
			szXML << tagclose(szRow, iIndent + 1);
		}

		szXML << tagclose(szValue, iIndent);
		return szXML.str();
	}

	if (m_MainParsing.size() == 1) szXML << m_MainParsing[0].toXML(PropXMLNames[pcPROPVAL].uidName, iIndent);
	if (m_AltParsing.size() == 1) szXML << m_AltParsing[0].toXML(PropXMLNames[pcPROPVALALT].uidName, iIndent);

	return szXML.str();
}

wstring Property::toString() const
{
	return toString(m_MainParsing);
}

wstring Property::toAltString() const
{
	return toString(m_AltParsing);
}

wstring Property::toString(const vector<Parsing>& parsings) const
{
	auto mv = m_attributes.GetAttribute(L"mv");
	if (mv == L"true")
	{
		std::wstringstream szString;
		szString << m_attributes.GetAttribute(L"count") + L": ";
		auto first = true;
		for (const auto& parsing: parsings)
		{
			if (!first)
			{
				szString << L"; ";
			}

			szString << parsing.toString();
			first = false;
		}

		return szString.str();
	}

	if (parsings.size() == 1)
	{
		return parsings[0].toString();
	}

	return L"";
}