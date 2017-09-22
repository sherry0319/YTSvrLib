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

/***********************************************************************
用户自处理TIMER类

***********************************************************************/
#pragma once

#ifndef __TIMER_HANDLER_H_
#define __TIMER_HANDLER_H_

#define TIMER_PARAM_COUNT 4
typedef LONGLONG TIMER_PARAM;

namespace YTSvrLib
{
	struct YTSVRLIB_EXPORT STimerInfo : public CRecycle
	{
		int m_nType;
		int m_nUserID;

		TIMER_PARAM m_ayParams[TIMER_PARAM_COUNT];

		DOUBLE GetEnd() const
		{
			return m_tEnd;
		}
		DOUBLE GetBegin() const
		{
			return m_tBegin;
		}

		BOOL IsCalling() const
		{
			return m_bCalling;
		}

		void SetCalling(BOOL bCall)
		{
			m_bCalling = bCall;
		}

		void ResetTimer(__time32_t tBegin, __time32_t tEnd)
		{
			m_tBegin = (DOUBLE) tBegin;
			m_tEnd = (DOUBLE) tEnd;
		}
		void ResetTimer(DOUBLE tBegin, DOUBLE tEnd)
		{
			m_tBegin = tBegin;
			m_tEnd = tEnd;
		}

		void Clean()
		{
			m_nType = 0;
			m_nUserID = 0;
			ZeroMemory(&m_ayParams, sizeof(m_ayParams));
			m_tEnd = 0;
			m_tBegin = 0;
			m_bCalling = FALSE;
		}

		STimerInfo()
		{
			Clean();
		}

		virtual void Init()
		{
			Clean();
		};
	private:
		DOUBLE m_tBegin;
		DOUBLE m_tEnd;
		BOOL m_bCalling;
	};
	typedef STimerInfo* LPSTimerInfo;

	struct YTSVRLIB_EXPORT Timer_Less
	{
		bool operator() (const LPSTimerInfo& _Left, const LPSTimerInfo& _Right) const
		{
			if (_Left->GetEnd() == _Right->GetEnd())
				return _Left < _Right;
			return _Left->GetEnd() < _Right->GetEnd();
		}
	};

	class YTSVRLIB_EXPORT ITIMERHANDLER
	{
	public:
		ITIMERHANDLER();
		virtual ~ITIMERHANDLER();

	public:
		// 响应定时器
		virtual void OnTimer(LPSTimerInfo pTimer) = 0;

		// 返回接近的时间点.低于这个时间放到排序集合里作为预备响应的定时器.建议1小时.
		virtual DOUBLE GetNearTime() = 0;
	public:
		void	RemoveTimer(LPSTimerInfo pTimer);
		void	UpdateTimer(LPSTimerInfo pTimer, DOUBLE tNewBegin, DOUBLE tNewEnd);
		void	UpdateTimer(LPSTimerInfo pTimer, DOUBLE tNewEnd);
		LPSTimerInfo SetNewTimer(int nType, int nUserID, DOUBLE tBegin, DOUBLE tEnd, TIMER_PARAM nParam1 = 0, TIMER_PARAM nParam2 = 0, TIMER_PARAM nParam3 = 0, TIMER_PARAM nParam4 = 0);
		LPSTimerInfo SetNewTimer(int nType, int nUserID, __time32_t tBegin, __time32_t tEnd, TIMER_PARAM nParam1 = 0, TIMER_PARAM nParam2 = 0, TIMER_PARAM nParam3 = 0, TIMER_PARAM nParam4 = 0);
	protected:
		void	ArrangeTimer();

		void	CheckTimer(DOUBLE tNow);
	private:
		LPSTimerInfo AllocateTimer(int nType);
		void    ReleaseTimer(LPSTimerInfo pTimer);
		void    AddNewTimer(LPSTimerInfo pTimer);

		CPool<STimerInfo, 256> m_poolTimers;

		typedef std::list<LPSTimerInfo> ListTimer;
		ListTimer m_listFarTimer;

		typedef std::set<LPSTimerInfo, Timer_Less> SetTimer;
		SetTimer    m_setTimers;
	};
}

#endif