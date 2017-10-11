#include "stdafx.h"
#include "TimerMgr.h"



void CTimerMgr::SetEvent()
{
	YTSvrLib::CServerApplication::GetInstance()->SetEvent(EAppEvent::eAppTimerMgrOnTimer);
}

CTimerMgr::CTimerMgr(void)
{
#ifdef LIB_WINDOWS
	m_tNow = _time32(NULL);

	SYSTEMTIME st;
	GetLocalTime( &st );
	m_nToday = st.wYear*10000 + st.wMonth*100 + st.wDay;
	m_wCurday = st.wDay;
	m_wCurHour = st.wHour;
	m_wCurDayOfWeek = st.wDayOfWeek;

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

	st.tm_hour = 0;
	st.tm_min = 0;
	st.tm_sec = 0;
	m_tTodayZero = mktime(&st);

	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv, &tz);
	m_nLocalTimeZoneSeconds = tz.tz_minuteswest * 60;
#endif // LIB_WINDOWS

	m_nTomorrow = CalcTomorrowYYYYMMDD();
	m_tNextHour = (m_tNow/3600+1)*3600;
	m_tNext5Minute = (m_tNow/300+1)*300;
	m_tNext10Minute = (m_tNow/600+1)*600;
	m_tNextLegion10Minute = m_tNow + 600;
	m_tNextMinute = (m_tNow/60+1)*60;
	m_tNext15Minute = m_tNow + 900;
	m_tNext30Sec = m_tNow + 30;
	m_tNext10Sec = m_tNow + 10;

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
   
	CheckTimer((DOUBLE)m_tNow);

	if( m_tNow >= m_tNext10Sec )
	{
		CServerParser::GetInstance()->CheckSvrSocket();
		m_tNext10Sec += 10;

		char szTitle[127]={0};
		_snprintf_s( szTitle, 127, "GatewaySvr=P[%d] L[%d] Online Client=%d ...",CConfig::GetInstance()->m_nPublicSvrID,CConfig::GetInstance()->m_nLocalSvrID,CPkgParser::GetInstance()->GetCurClientCount() );
		SetConsoleTitleA( szTitle );
		
	}

	if( m_tNow >= m_tNextMinute )
	{// TODO every minute
		m_tNextMinute += 60;
		if( m_tNow >= m_tNext5Minute )
		{// TODO every 5 minute
			m_tNext5Minute += 300;
		}
		if( m_tNow >= m_tNext10Minute )
		{// TODO every 10 minute
			SYSTEMTIME st;
			GetLocalTime( &st );
			if( m_tNow >= m_tNextHour )
			{// TODO every hour
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
					
					LOG("Timer New Day=%d Hour=%d", m_wCurday, m_wCurHour );
				}
				m_tNextHour += 3600;
				ArrangeTimer();
			}//if( tNow >= m_tNextHour )
			m_tNext10Minute += 600;
		} //if( tNow >= m_tNext10Minute )
		CPkgParser::GetInstance()->CheckIdleSocket( m_tNow );
	}//if( m_tNow >= m_tNextMinute )
}

DOUBLE CTimerMgr::GetNearTime()
{
	return (DOUBLE) m_tNextHour;
}

void CTimerMgr::OnTimer(YTSvrLib::LPSTimerInfo pTimer)
{
	if (m_mapTimerFunc.find((EM_TIMER_TYPE)pTimer->m_nType) != m_mapTimerFunc.end())
	{
		LOGTRACE("CTimerMgr::OnTimer=0x%x :: User=%d Type=%d Param=[%lld|%lld|%lld|%lld]", pTimer, pTimer->m_nUserID, pTimer->m_nType,
				 pTimer->m_ayParams[0], pTimer->m_ayParams[1], pTimer->m_ayParams[2], pTimer->m_ayParams[3]);
		(this->*m_mapTimerFunc[(EM_TIMER_TYPE)pTimer->m_nType])(pTimer);
	}
	else
	{
		LOG("TM_OnTimer Invalid Type=%d Error!", pTimer->m_nType);
	}
}