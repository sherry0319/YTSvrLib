#include "stdafx.h"
#include "TimerMgr.h"

void CTimerMgr::SetEvent()
{
	YTSvrLib::CServerApplication::GetInstance()->SetEvent(EAppEvent::eAppTimerMgrOnTimer);
}

CTimerMgr::CTimerMgr(void)
{
	m_tNow = time32();

	SYSTEMTIME st;
	GetLocalTime(&st);
	m_nToday = st.wYear * 10000 + st.wMonth * 100 + st.wDay;
	m_wCurday = st.wDay;
	m_wCurHour = st.wHour;
	m_wCurDayOfWeek = st.wDayOfWeek;

	st.wHour = 0;
	st.wMinute = 0;
	st.wSecond = 0;
	st.wMilliseconds = 0;
	m_tTodayZero = SystemTimeToTime_t(&st);
	m_tTimeZone = GetLocalTimeZone();

	m_nTomorrow = CalcTomorrowYYYYMMDD();
	m_tNextHour = (m_tNow / 3600 + 1) * 3600;
	m_tNext5Minute = (m_tNow / 300 + 1) * 300;
	m_tNext10Minute = (m_tNow / 600 + 1) * 600;
	m_tNextLegion10Minute = m_tNow + 600;
	m_tNextMinute = (m_tNow / 60 + 1) * 60;
	m_tNext15Minute = m_tNow + 900;
	m_tNext30Sec = m_tNow + 30;
	m_tNext10Sec = m_tNow + 10;

	LOG("TM_Init Now=%d TodayZero=%d LocalTimeZoneSeconds=%d", m_tNow, m_tTodayZero, m_tTimeZone);
}

CTimerMgr::~CTimerMgr(void)
{

}

void CTimerMgr::OnTimerCheckQueue()
{
	m_tNow = time32();
	CheckTimer((DOUBLE) m_tNow);
	if (m_tNow >= m_tNext10Sec)
	{
		CServerParser::GetInstance()->CheckSvrSocket();
		m_tNext10Sec += 10;

		char szTitle[127] = { 0 };
		_snprintf_s(szTitle, 127, "WSGatewaySvr=P[%d] L[%d] Online Client=%d ...", CConfig::GetInstance()->m_nPublicSvrID, CConfig::GetInstance()->m_nLocalSvrID, CPkgParser::GetInstance()->GetCurClientCount());
		SetConsoleTitleA(szTitle);

	}

	if (m_tNow >= m_tNextMinute)
	{
		m_tNextMinute += 60;
		if (m_tNow >= m_tNext5Minute)
		{
			m_tNext5Minute += 300;
		}
		if (m_tNow >= m_tNext10Minute)
		{
			SYSTEMTIME st;
			GetLocalTime(&st);
			if (m_tNow >= m_tNextHour)
			{
				if (st.wHour != m_wCurHour)
				{
					m_wCurHour = st.wHour;
					if (st.wDay != m_wCurday)
					{
						m_wCurday = st.wDay;
						m_wCurDayOfWeek = st.wDayOfWeek;
						m_nToday = st.wYear * 10000 + st.wMonth * 100 + st.wDay;
						m_nTomorrow = CalcTomorrowYYYYMMDD();
					}
					ReOpenLogFile();
					LOG("Timer New Day=%d Hour=%d", m_wCurday, m_wCurHour);
				}
				m_tNextHour += 3600;
				ArrangeTimer();
			}//if( tNow >= m_tNextHour )
			m_tNext10Minute += 600;
		} //if( tNow >= m_tNext10Minute )
		CPkgParser::GetInstance()->CheckIdleSocket(m_tNow);
	}//if( m_tNow >= m_tNextMinute )
}

void CTimerMgr::OnTimer(YTSvrLib::LPSTimerInfo pTimer)
{

}

DOUBLE CTimerMgr::GetNearTime()
{
	return (DOUBLE) m_tNextHour;
}