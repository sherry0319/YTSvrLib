#include "stdafx.h"
#include "TimerMgr.h"

extern YTSvrLib::CServerApplication gApp;

void CTimerMgr::SetEvent()
{
	gApp.SetEvent(EAppEvent::eAppTimerMgrOnTimer);
}

void CTimerMgr::RegistTimerFunc(EM_TIMER_TYPE eTimerType,TimerFunc pFunc)
{
	m_mapTimerFunc[eTimerType] = pFunc;
}

CTimerMgr::CTimerMgr(void) : m_poolTimers("STimerInfo")
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
	for( SetTimer::iterator it = m_setTimers.begin();it!=m_setTimers.end(); it++ )
		if( *it )
			m_poolTimers.ReclaimObj( *it );
	m_setTimers.clear();
}

LPSTimerInfo CTimerMgr::AllocateTimer( EM_TIMER_TYPE emType )
{
	LPSTimerInfo pTimer = m_poolTimers.ApplyObj();
	if( pTimer )
	{
		pTimer->m_emType = emType;
		LOGTRACE("Allocate Timer=0x%x Type=%d", pTimer, emType);
	}
    return pTimer;
}
void CTimerMgr::ReleaseTimer( LPSTimerInfo pTimer )
{
	if (pTimer == NULL)
	{
		return;
	}

	LOGTRACE("Release Timer=0x%x Type=%d", pTimer, pTimer->m_emType);
	m_poolTimers.ReclaimObj( pTimer );
}

void CTimerMgr::AddNewTimer( LPSTimerInfo pTimer )
{
	if (pTimer->GetEnd() < m_tNextHour)
	{
		m_setTimers.insert( pTimer );
	}
	else
	{
		m_listFarTimer.push_back( pTimer );
	}

	LOGTRACE("Set Timer=0x%x Type=%d UserID=%d Total=%d",
			 pTimer, pTimer->m_emType, pTimer->m_nUserID, m_setTimers.size());
}

LPSTimerInfo CTimerMgr::SetNewTimer(EM_TIMER_TYPE emTimerType, UINT nUserID, DOUBLE tBegin, DOUBLE tEnd, TIMER_PARAM nParam1, TIMER_PARAM nParam2, TIMER_PARAM nParam3, TIMER_PARAM nParam4)
{
	LPSTimerInfo pTimer = AllocateTimer(emTimerType);
	if (pTimer == NULL)
	{
		LOG("Allocate Timer Failed : %d",emTimerType);
		return NULL;
	}

	pTimer->m_emType = emTimerType;
	pTimer->m_nUserID = nUserID;
	pTimer->m_unTimerParam.m_ayParams[0] = nParam1;
	pTimer->m_unTimerParam.m_ayParams[1] = nParam2;
	pTimer->m_unTimerParam.m_ayParams[2] = nParam3;
	pTimer->m_unTimerParam.m_ayParams[3] = nParam4;
	pTimer->ResetTimer(tBegin,tEnd);

	AddNewTimer(pTimer);

	return pTimer;
}

void CTimerMgr::RemoveTimer( LPSTimerInfo pTimer )
{
	if (pTimer == NULL)
	{
		return;
	}

	LOGTRACE("Remove Timer=0x%x Type=%d", pTimer, pTimer->m_emType);
	if (pTimer->IsCalling() == TRUE)
	{
		LOG("Timer 0x%x is calling Type=%d",pTimer, pTimer->m_emType);
		return;
	}

	if (pTimer->GetEnd() < m_tNextHour)
	{
		SetTimer::iterator it = m_setTimers.find( pTimer );
		if( it != m_setTimers.end() )
			m_setTimers.erase( it );
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

void CTimerMgr::UpdateTimer(LPSTimerInfo pTimer, DOUBLE tNewBegin, DOUBLE tNewEnd)
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

void CTimerMgr::UpdateTimer(LPSTimerInfo pTimer, DOUBLE tNewEnd)
{
	UpdateTimer(pTimer, pTimer->GetBegin(), tNewEnd);
}

void CTimerMgr::OnTimerCheckQueue()
{
	timeval tv;
	gettimeofday(&tv, NULL);

	m_tNow = ((DOUBLE) tv.tv_sec + ((DOUBLE) tv.tv_usec / 1000000.000));

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
		DOUBLE tCurEnd = (*iter)->GetEnd();
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

void CTimerMgr::OnTimer( LPSTimerInfo pTimer )
{
	if (m_mapTimerFunc.find(pTimer->m_emType) != m_mapTimerFunc.end())
	{
		LOGTRACE("CTimerMgr::OnTimer=0x%x :: User=%d Type=%d Param=[%lld|%lld|%lld|%lld]",pTimer,pTimer->m_nUserID,pTimer->m_emType,
			pTimer->m_unTimerParam.m_ayParams[0],pTimer->m_unTimerParam.m_ayParams[1],pTimer->m_unTimerParam.m_ayParams[2],pTimer->m_unTimerParam.m_ayParams[3]);
		(this->*m_mapTimerFunc[pTimer->m_emType])(pTimer);
	}
	else
	{
		LOG("TM_OnTimer Invalid Type=%d Error!", pTimer->m_emType);
	}
}

void  CTimerMgr::ArrangeTimer()
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

void CTimerMgr::OnTimerTest(LPSTimerInfo pTimer)
{
	LOG("OnTimerTest Callback");
}