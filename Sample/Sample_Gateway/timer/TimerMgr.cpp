#include "stdafx.h"
#include "TimerMgr.h"

extern YTSvrLib::CServerApplication gApp;

void CTimerMgr::SetEvent()
{
	gApp.SetEvent(EAppEvent::eAppTimerMgrOnTimer);
}

CTimerMgr::CTimerMgr(void) : m_poolTimers("STimerInfo")
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
 	m_nSysCloseWaitMinutes = -1;
// 	m_tSysQuit = 0;
	m_tNext30Sec = m_tNow + 30;
	m_tNext10Sec = m_tNow + 10;

	LOG("TM_Init Now=%d TodayZero=%d LocalTimeZoneSeconds=%d", m_tNow, m_tTodayZero, m_nLocalTimeZoneSeconds );
}

CTimerMgr::~CTimerMgr(void)
{
	for( SetTimer::iterator it = m_setTimers.begin();it!=m_setTimers.end(); it++ )
		if( *it )
			m_poolTimers.ReclaimObj( *it );
	m_setTimers.clear();
}

LPSTimerInfo CTimerMgr::AllocateTimer(EM_TIMER_TYPE emType)
{
	LPSTimerInfo pTimer = m_poolTimers.ApplyObj();
	if (pTimer)
	{
		pTimer->m_emType = emType;
		LOGTRACE("Allocate Timer=0x%x Type=%d", pTimer, emType);
	}
	return pTimer;
}
void CTimerMgr::ReleaseTimer(LPSTimerInfo pTimer)
{
	if (pTimer == NULL)
	{
		return;
	}

	LOGTRACE("Release Timer=0x%x Type=%d", pTimer, pTimer->m_emType);
	m_poolTimers.ReclaimObj(pTimer);
}

void CTimerMgr::AddNewTimer(LPSTimerInfo pTimer)
{
	if (pTimer->GetEnd() < m_tNextHour)
	{
		m_setTimers.insert(pTimer);
	}
	else
	{
		m_listFarTimer.push_back(pTimer);
	}

	LOGTRACE("Set Timer=0x%x Type=%d UserID=%d Total=%d",
			 pTimer, pTimer->m_emType, pTimer->m_nUserID, m_setTimers.size());
}

void CTimerMgr::ArrangeTimer()
{
	LOG("Arranging Timer...");
	ListTimer::iterator it = m_listFarTimer.begin();
	while (it != m_listFarTimer.end())
	{
		if (*it)
		{
			LPSTimerInfo pTimer = (*it);
			if (pTimer->GetEnd() < m_tNextHour)
			{
				it = m_listFarTimer.erase(it);
				m_setTimers.insert(pTimer);
				continue;
			}
		}
		++it;
	}
}

LPSTimerInfo CTimerMgr::SetNewTimer(EM_TIMER_TYPE emTimerType, UINT nUserID, __time32_t tBegin, __time32_t tEnd, TIMER_PARAM nParam1, TIMER_PARAM nParam2, TIMER_PARAM nParam3, TIMER_PARAM nParam4)
{
	LPSTimerInfo pTimer = AllocateTimer(emTimerType);
	if (pTimer == NULL)
	{
		LOG("Allocate Timer Failed : %d", emTimerType);
		return NULL;
	}

	pTimer->m_emType = emTimerType;
	pTimer->m_nUserID = nUserID;
	pTimer->m_unTimerParam.m_ayParams[0] = nParam1;
	pTimer->m_unTimerParam.m_ayParams[1] = nParam2;
	pTimer->m_unTimerParam.m_ayParams[2] = nParam3;
	pTimer->m_unTimerParam.m_ayParams[3] = nParam4;
	pTimer->ResetTimer(tBegin, tEnd);

	AddNewTimer(pTimer);

	return pTimer;
}

void CTimerMgr::RemoveTimer(LPSTimerInfo pTimer)
{
	if (pTimer == NULL)
	{
		return;
	}

	LOGTRACE("Remove Timer=0x%x Type=%d", pTimer, pTimer->m_emType);
	if (pTimer->IsCalling() == TRUE)
	{
		LOG("Timer 0x%x is calling Type=%d", pTimer, pTimer->m_emType);
		return;
	}

	if (pTimer->GetEnd() < m_tNextHour)
	{
		SetTimer::iterator it = m_setTimers.find(pTimer);
		if (it != m_setTimers.end())
			m_setTimers.erase(it);
	}
	else
	{
		ListTimer::iterator itFar = m_listFarTimer.begin();
		while (itFar != m_listFarTimer.end())
		{
			if ((*itFar) == pTimer)
			{
				m_listFarTimer.erase(itFar);
				break;
			}
			++itFar;
		}
	}

	ReleaseTimer(pTimer);
}

void CTimerMgr::UpdateTimer(LPSTimerInfo pTimer, __time32_t tNewBegin, __time32_t tNewEnd)
{
	if (pTimer->GetEnd() < m_tNextHour)
	{
		SetTimer::iterator it = m_setTimers.find(pTimer);
		if (it == m_setTimers.end())
		{
			LOGTRACE("TimerMgr_UpdateTimer Cannot Find Timer=0x%x Type=%d UserID=%d Error!", pTimer, pTimer->m_emType, pTimer->m_nUserID);
			return;
		}
		m_setTimers.erase(it);
		pTimer->ResetTimer(tNewBegin, tNewEnd);
	}
	else
	{
		ListTimer::iterator itFar = m_listFarTimer.begin();
		while (itFar != m_listFarTimer.end())
		{
			if ((*itFar) == pTimer)
			{
				m_listFarTimer.erase(itFar);
				pTimer->ResetTimer(tNewBegin, tNewEnd);
				break;
			}
			++itFar;
		}
	}

	AddNewTimer(pTimer);
}

void CTimerMgr::UpdateTimer(LPSTimerInfo pTimer, __time32_t tNewEnd)
{
	UpdateTimer(pTimer, pTimer->GetBegin(), tNewEnd);
}

void CTimerMgr::OnTimerCheckQueue()
{
#ifdef LIB_WINDOWS
	m_tNow = _time32(NULL);
#else
	m_tNow = time(NULL);
#endif // LIB_WINDOWS
   
	while (false == m_setTimers.empty())
	{
		SetTimer::iterator iter = m_setTimers.begin();
		if ((*iter) && (*iter)->GetEnd() <= m_tNow)
		{
			LPSTimerInfo pTimer = (*iter);
			m_setTimers.erase(iter);
			pTimer->SetCalling(TRUE);
			OnTimer(pTimer);
			ReleaseTimer(pTimer);
			continue;
		}
		__time32_t tCurEnd = (*iter)->GetEnd();
		SetTimer::iterator iterPre = iter;
		LPSTimerInfo pTimerPre = (*iter);
		iter++;
		if (iter == m_setTimers.end())
		{
			break;
		}
		if (tCurEnd > (*iter)->GetEnd() || (tCurEnd - m_tNow) > (SEC_MINUTE * 3))
		{
			m_setTimers.erase(iterPre);
			m_setTimers.insert(pTimerPre);
		}
		break;
	}

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

void CTimerMgr::OnTimer(LPSTimerInfo pTimer)
{
	if (m_mapTimerFunc.find(pTimer->m_emType) != m_mapTimerFunc.end())
	{
		LOGTRACE("CTimerMgr::OnTimer=0x%x :: User=%d Type=%d Param=[%lld|%lld|%lld|%lld]", pTimer, pTimer->m_nUserID, pTimer->m_emType,
				 pTimer->m_unTimerParam.m_ayParams[0], pTimer->m_unTimerParam.m_ayParams[1], pTimer->m_unTimerParam.m_ayParams[2], pTimer->m_unTimerParam.m_ayParams[3]);
		(this->*m_mapTimerFunc[pTimer->m_emType])(pTimer);
	}
	else
	{
		LOG("TM_OnTimer Invalid Type=%d Error!", pTimer->m_emType);
	}
}