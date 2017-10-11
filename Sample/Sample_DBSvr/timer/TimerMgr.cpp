#include "stdafx.h"
#include "TimerMgr.h"
#include "../db/DBLogMgr.h"


void CTimerMgr::SetEvent()
{
	YTSvrLib::CServerApplication::GetInstance()->SetEvent(EAppEvent::eAppTimerMgrOnTimer);
}

CTimerMgr::CTimerMgr(void)
{
#ifdef LIB_WINDOWS
	SYSTEMTIME st;
	GetLocalTime(&st);
	m_nToday = st.wYear * 10000 + st.wMonth * 100 + st.wDay;
	m_wCurday = st.wDay;
	m_wCurHour = st.wHour;
	m_wCurDayOfWeek = st.wDayOfWeek;
	m_tNow = _time32(NULL);
	st.wHour = 0;
	st.wMinute = 0;
	st.wSecond = 0;
	st.wMilliseconds = 0;
	m_tTodayZero = SystemTimeToTime_t(&st);

	TIME_ZONE_INFORMATION sLocalTimeZone;
	ZeroMemory(&sLocalTimeZone, sizeof(sLocalTimeZone));
	DWORD dw = GetTimeZoneInformation(&sLocalTimeZone);
	m_nLocalTimeZoneSeconds = sLocalTimeZone.Bias * 60;
#else
	m_tNow = time(NULL);
	tm st;
	localtime_r(&m_tNow, &st);

	m_nToday = (((st.tm_year + 1900) * 10000) + ((st.tm_mon + 1) * 100) + st.tm_mday);
	m_wCurday = st.tm_mday;
	m_wCurHour = st.tm_hour;
	m_wCurDayOfWeek = st.tm_wday;
	m_nLocalTimeZoneSeconds = st.tm_gmtoff;
	st.tm_hour = 0;
	st.tm_min = 0;
	st.tm_sec = 0;
	m_tTodayZero = mktime(&st);
	
#endif // LIB_WINDOWS

	m_nTomorrow = CalcTomorrowYYYYMMDD();
	m_tNextHour = (m_tNow/3600+1)*3600;
	m_tNext5Minute = (m_tNow/300+1)*300;
	m_tNext10Minute = (m_tNow/600+1)*600;
	m_tNextLegion10Minute = m_tNow + 600;
	m_tNextMinute = (m_tNow/60+1)*60;
	m_tNext15Minute = m_tNow + 900;

	LOG("TM_Init Now=%d TodayZero=%d LocalTimeZoneSeconds=%d", m_tNow, m_tTodayZero, m_nLocalTimeZoneSeconds );
}

CTimerMgr::~CTimerMgr(void)
{

}

void CTimerMgr::OnTimerCheckQueue()
{
#ifdef LIB_WINDOWS
	m_tNow = _time32(NULL);
#else
	m_tNow = time(NULL);
#endif // LIB_WINDOWS

	if( m_tNow >= m_tNextMinute )
	{
		if (CConfig::GetInstance()->m_bIsSQLCache)
			CDBCache::GetInstance()->RefreshSQLCache();
		m_tNextMinute += 60;
		if( m_tNow >= m_tNext10Minute )
		{
			SYSTEMTIME st;
			GetLocalTime( &st );
			if( m_tNow >= m_tNextHour )
			{
				if( st.wHour != m_wCurHour )
				{
					m_wCurHour = st.wHour;
					if( st.wDay != m_wCurday )
					{
						m_wCurday = st.wDay;
						m_wCurDayOfWeek = st.wDayOfWeek;
						m_nToday = st.wYear*10000 + st.wMonth*100 + st.wDay;
						m_nTomorrow = CalcTomorrowYYYYMMDD();
					}
					ReOpenLogFile();
					ReOpenDBLogFile();
					LOG("Timer New Day=%d Hour=%d", m_wCurday, m_wCurHour );
				}
				m_tNextHour += 3600;
			}//if( tNow >= m_tNextHour )
			m_tNext10Minute += 600;
		} //if( tNow >= m_tNext10Minute )
	} //if( tNow >= m_tNextMinute )
}