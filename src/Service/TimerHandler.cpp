/*MIT License

Copyright (c) 2016 Archer Xu

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#include "stdafx.h"
#include "TimerHandler.h"

namespace YTSvrLib
{
	ITIMERHANDLER::ITIMERHANDLER() : m_poolTimers("LPSTimerInfo")
	{
		m_setTimers.clear();
		m_listFarTimer.clear();
		m_setHighTimers.clear();
	}

	ITIMERHANDLER::~ITIMERHANDLER()
	{
		for (SetTimer::iterator it = m_setTimers.begin(); it != m_setTimers.end(); it++)
		{
			if (*it)
			{
				m_poolTimers.ReclaimObj(*it);
			}
		}

		m_setTimers.clear();

		for (SetTimer::iterator it = m_setHighTimers.begin(); it != m_setHighTimers.end(); it++)
		{
			if (*it)
			{
				m_poolTimers.ReclaimObj(*it);
			}
		}

		m_setHighTimers.clear();

		for (ListTimer::iterator it = m_listFarTimer.begin(); it != m_listFarTimer.end();++it)
		{
			if (*it)
			{
				m_poolTimers.ReclaimObj(*it);
			}
		}

		m_listFarTimer.clear();
	}

	void  ITIMERHANDLER::ArrangeTimer()
	{
		LOG("Arranging Timer...");
		ListTimer::iterator it = m_listFarTimer.begin();
		while (it != m_listFarTimer.end())
		{
			if (*it)
			{
				LPSTimerInfo pTimer = (*it);
				if (pTimer->GetEnd() < GetNearTime())
				{
					it = m_listFarTimer.erase(it);
					m_setTimers.insert(pTimer);
					continue;
				}
			}
			++it;
		}
	}

	LPSTimerInfo ITIMERHANDLER::AllocateTimer(int nType)
	{
		LPSTimerInfo pTimer = m_poolTimers.ApplyObj();
		if (pTimer)
		{
			pTimer->m_nType = nType;
			LOGTRACE("Allocate Timer=0x%x Type=%d", pTimer, nType);
		}
		return pTimer;
	}

	void ITIMERHANDLER::ReleaseTimer(LPSTimerInfo pTimer)
	{
		if (pTimer == NULL)
		{
			return;
		}

		LOGTRACE("Release Timer=0x%x Type=%d", pTimer, pTimer->m_nType);

		m_poolTimers.ReclaimObj(pTimer);
	}

	void ITIMERHANDLER::AddNewTimer(LPSTimerInfo pTimer)
	{
		if (pTimer->GetLevel() == TLEVEL_NORMAL)
		{
			if (pTimer->GetEnd() < GetNearTime())
			{
				m_setTimers.insert(pTimer);
			}
			else
			{
				m_listFarTimer.push_back(pTimer);
			}
		}
		else
		{
			m_setHighTimers.insert(pTimer);
		}

		LOGTRACE("Set Timer=0x%x Level=%d Type=%d UserID=%d Total=%d",
				 pTimer, pTimer->GetLevel(),pTimer->m_nType, pTimer->m_nUserID, m_setTimers.size());
	}

	LPSTimerInfo ITIMERHANDLER::SetNewTimer(EM_TIMER_LEVEL emLevel, int nTimerType, int nUserID, DOUBLE tBegin, DOUBLE tEnd, TIMER_PARAM nParam1, TIMER_PARAM nParam2, TIMER_PARAM nParam3, TIMER_PARAM nParam4)
	{
		LPSTimerInfo pTimer = AllocateTimer(nTimerType);
		if (pTimer == NULL)
		{
			LOG("Allocate Timer Failed : %d", nTimerType);
			return NULL;
		}

		pTimer->m_emLevel = emLevel;
		pTimer->m_nType = nTimerType;
		pTimer->m_nUserID = nUserID;
		pTimer->m_ayParams[0] = nParam1;
		pTimer->m_ayParams[1] = nParam2;
		pTimer->m_ayParams[2] = nParam3;
		pTimer->m_ayParams[3] = nParam4;
		pTimer->ResetTimer(tBegin, tEnd);

		AddNewTimer(pTimer);

		return pTimer;
	}

	LPSTimerInfo ITIMERHANDLER::SetNewTimer(EM_TIMER_LEVEL emLevel, int nTimerType, int nUserID, __time32_t tBegin, __time32_t tEnd, 
											TIMER_PARAM nParam1, TIMER_PARAM nParam2, TIMER_PARAM nParam3, TIMER_PARAM nParam4)
	{
		return SetNewTimer(emLevel, nTimerType, nUserID, (DOUBLE) tBegin, (DOUBLE) tEnd, nParam1, nParam2, nParam3, nParam4);
	}

	void ITIMERHANDLER::RemoveTimer(LPSTimerInfo pTimer)
	{
		if (pTimer == NULL)
		{
			return;
		}

		LOGTRACE("Remove Timer=0x%x Type=%d", pTimer, pTimer->m_nType);
		if (pTimer->IsCalling() == TRUE)
		{
			LOG("Timer 0x%x is calling Type=%d", pTimer, pTimer->m_nType);
			return;
		}

		if (pTimer->GetLevel() == TLEVEL_NORMAL)
		{
			if (pTimer->GetEnd() < GetNearTime())
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
		}
		else
		{
			SetTimer::iterator it = m_setHighTimers.find(pTimer);
			if (it != m_setHighTimers.end())
				m_setHighTimers.erase(it);
		}

		ReleaseTimer(pTimer);
	}

	void ITIMERHANDLER::UpdateTimer(LPSTimerInfo pTimer, DOUBLE tNewBegin, DOUBLE tNewEnd)
	{
		if (pTimer->GetLevel() == TLEVEL_NORMAL)
		{
			if (pTimer->GetEnd() < GetNearTime())
			{
				SetTimer::iterator it = m_setTimers.find(pTimer);
				if (it == m_setTimers.end())
				{
					LOGTRACE("TimerMgr_UpdateTimer Cannot Find Timer=0x%x Type=%d UserID=%d Error!", pTimer, pTimer->m_nType, pTimer->m_nUserID);
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
		}
		else
		{
			SetTimer::iterator it = m_setHighTimers.find(pTimer);
			if (it == m_setHighTimers.end())
			{
				LOGTRACE("TimerMgr_UpdateTimer Cannot Find Timer=0x%x Type=%d UserID=%d Error!", pTimer, pTimer->m_nType, pTimer->m_nUserID);
				return;
			}
			m_setHighTimers.erase(it);
			pTimer->ResetTimer(tNewBegin, tNewEnd);
		}
		
		AddNewTimer(pTimer);
	}

	void ITIMERHANDLER::UpdateTimer(LPSTimerInfo pTimer, DOUBLE tNewEnd)
	{
		UpdateTimer(pTimer, pTimer->GetBegin(), tNewEnd);
	}

	void ITIMERHANDLER::CheckHighTimer(DOUBLE tNow)
	{
		while (false == m_setHighTimers.empty())
		{
			SetTimer::iterator iter = m_setHighTimers.begin();
			if ((*iter) && (*iter)->GetEnd() <= tNow)
			{
				LPSTimerInfo pTimer = (*iter);
				m_setHighTimers.erase(iter);
				pTimer->SetCalling(TRUE);
				OnTimer(pTimer);
				ReleaseTimer(pTimer);
				continue;
			}
			DOUBLE tCurEnd = (*iter)->GetEnd();
			SetTimer::iterator iterPre = iter;
			LPSTimerInfo pTimerPre = (*iter);
			iter++;
			if (iter == m_setHighTimers.end())
			{
				break;
			}
			if (tCurEnd > (*iter)->GetEnd() || (tCurEnd - tNow) > (SEC_MINUTE * 3))
			{
				m_setHighTimers.erase(iterPre);
				m_setHighTimers.insert(pTimerPre);
			}
			break;
		}
	}

	void ITIMERHANDLER::CheckTimer(DOUBLE tNow)
	{
		while (false == m_setTimers.empty())
		{
			SetTimer::iterator iter = m_setTimers.begin();
			if ((*iter) && (*iter)->GetEnd() <= tNow)
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
			if (tCurEnd > (*iter)->GetEnd() || (tCurEnd - tNow) > (SEC_MINUTE * 3))
			{
				m_setTimers.erase(iterPre);
				m_setTimers.insert(pTimerPre);
			}
			break;
		}
	}
}