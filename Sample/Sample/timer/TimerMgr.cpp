#include "stdafx.h"
#include "TimerMgr.h"

void CTimerMgr::SetEvent()
{
	YTSvrLib::CServerApplication::GetInstance()->SetEvent(EAppEvent::eAppTimerMgrOnTimer);
}

void CTimerMgr::RegistTimerFunc(EM_TIMER_TYPE eTimerType,TimerFunc pFunc)
{
	m_mapTimerFunc[eTimerType] = pFunc;
}

CTimerMgr::CTimerMgr(void)
{
#ifdef LIB_WINDOWS
	SYSTEMTIME st;
	GetLocalTime(&st);
	m_wCurday = st.wDay;
	m_wCurHour = st.wHour;
	m_wCurDayOfWeek = st.wDayOfWeek;
	m_tNow = ((DOUBLE)_time32(NULL) + ((DOUBLE) st.wMilliseconds / 1000.000));

	TIME_ZONE_INFORMATION sLocalTimeZone;
	ZeroMemory(&sLocalTimeZone, sizeof(sLocalTimeZone));
	DWORD dw = GetTimeZoneInformation(&sLocalTimeZone);
	m_nLocalTimeZoneSeconds = sLocalTimeZone.Bias * 60;
#else
	__time32_t tNow = time(NULL);
	tm st;
	localtime_r(&tNow, &st);

	m_wCurday = st.tm_mday;
	m_wCurHour = st.tm_hour;
	m_wCurDayOfWeek = st.tm_wday;

	struct timeval tv;
	gettimeofday(&tv, NULL);

	m_tNow = ((DOUBLE) tv.tv_sec + ((DOUBLE) tv.tv_usec / 1000000.000));
	m_nLocalTimeZoneSeconds = st.tm_gmtoff;
#endif // LIB_WINDOWS
	
	m_tNextMinute = (GetTimeNowSec() / 60 + 1) * 60;
	m_tNextHour = (GetTimeNowSec() / 3600 + 1) * 3600;

	LOG("TimeMgr_Init Now=%.6f LocalTimeZoneSeconds=%d", m_tNow, m_nLocalTimeZoneSeconds);

#undef TIMER_FUNC_CALLBACK
#define TIMER_FUNC_CALLBACK(_Timer,_Func) RegistTimerFunc(_Timer,&CTimerMgr::_Func);
#include "TimerFuncTable.h"
}

CTimerMgr::~CTimerMgr(void)
{

}

void CTimerMgr::OnTimerCheckQueue()
{
	timeval tv;
	gettimeofday(&tv, NULL);

	m_tNow = ((DOUBLE) tv.tv_sec + ((DOUBLE) tv.tv_usec / 1000000.000));

	CheckTimer(m_tNow);

	if( (__time32_t)m_tNow >= m_tNextMinute )
	{
		m_tNextMinute += 60;
		
		SYSTEMTIME st;
		GetLocalTime(&st);
		if ((__time32_t) m_tNow >= m_tNextHour)
		{
			if (st.wHour != m_wCurHour)
			{
				m_wCurHour = st.wHour;
				if (st.wDay != m_wCurday)
				{
					m_wCurday = st.wDay;
					m_wCurDayOfWeek = st.wDayOfWeek;
				}
				ReOpenLogFile();
				ReOpenDBLogFile();

				LOG("Timer New Day=%d Hour=%d", m_wCurday, m_wCurHour);
			}
			m_tNextHour += 3600;
			ArrangeTimer();
		}//if( tNow >= m_tNextHour )
	} //if( tNow >= m_tNextMinute )
}

DOUBLE CTimerMgr::GetNearTime()
{
	return (DOUBLE) m_tNextHour;
}

void CTimerMgr::OnTimer( YTSvrLib::LPSTimerInfo pTimer )
{
	if (m_mapTimerFunc.find((EM_TIMER_TYPE)pTimer->m_nType) != m_mapTimerFunc.end())
	{
		LOGTRACE("CTimerMgr::OnTimer=0x%x :: User=%d Type=%d Param=[%lld|%lld|%lld|%lld]",pTimer,pTimer->m_nUserID,pTimer->m_nType,
			pTimer->m_ayParams[0],pTimer->m_ayParams[1],pTimer->m_ayParams[2],pTimer->m_ayParams[3]);
		(this->*m_mapTimerFunc[(EM_TIMER_TYPE)pTimer->m_nType])(pTimer);
	}
	else
	{
		LOG("TM_OnTimer Invalid Type=%d Error!", pTimer->m_nType);
	}
}

void CTimerMgr::OnTimerTest(YTSvrLib::LPSTimerInfo pTimer)
{
	LOG("OnTimerTest Callback");
}