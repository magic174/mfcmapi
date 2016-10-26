#include "stdafx.h"
#include "GlobalObjectId.h"
#include "String.h"
#include "InterpretProp.h"
#include "InterpretProp2.h"
#include "ExtraPropTags.h"

GlobalObjectId::GlobalObjectId()
{
	m_Year = 0;
	m_Month = 0;
	m_Day = 0;
	m_CreationTime = { 0 };
	m_X = { 0 };
	m_dwSize = 0;
}

void GlobalObjectId::Parse()
{
	m_Id = m_Parser.GetBYTES(16);
	BYTE b1 = NULL;
	BYTE b2 = NULL;
	m_Parser.GetBYTE(&b1);
	m_Parser.GetBYTE(&b2);
	m_Year = static_cast<WORD>(b1 << 8 | b2);
	m_Parser.GetBYTE(&m_Month);
	m_Parser.GetBYTE(&m_Day);
	m_CreationTime = m_Parser.Get<FILETIME>();
	m_X = m_Parser.Get<LARGE_INTEGER>();
	m_Parser.GetDWORD(&m_dwSize);
	m_lpData = m_Parser.GetBYTES(m_dwSize, _MaxBytes);
}

_Check_return_ wstring GlobalObjectId::ToStringInternal()
{
	wstring szGlobalObjectId;

	szGlobalObjectId = formatmessage(IDS_GLOBALOBJECTIDHEADER);

	szGlobalObjectId += BinToHexString(m_Id, true);

	auto szFlags = InterpretFlags(flagGlobalObjectIdMonth, m_Month);

	wstring PropString;
	wstring AltPropString;
	FileTimeToString(&m_CreationTime, PropString, AltPropString);
	szGlobalObjectId += formatmessage(IDS_GLOBALOBJECTIDDATA1,
		m_Year,
		m_Month, szFlags.c_str(),
		m_Day,
		m_CreationTime.dwHighDateTime, m_CreationTime.dwLowDateTime, PropString.c_str(),
		m_X.HighPart, m_X.LowPart,
		m_dwSize);

	if (m_lpData.size())
	{
		szGlobalObjectId += formatmessage(IDS_GLOBALOBJECTIDDATA2);
		szGlobalObjectId += BinToHexString(m_lpData, true);
	}

	return szGlobalObjectId;
}