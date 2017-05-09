#pragma once

#include "Service/TimerThread.h"

typedef LONGLONG TIMER_PARAM;

enum EM_TIMER_TYPE
{
	eTimerIdle = 0,
	eTimerMax,
};

struct STimerInfo : public YTSvrLib::CRecycle
{
	EM_TIMER_TYPE m_emType;
	int m_nUserID;

	union
	{
		TIMER_PARAM m_ayParams[4];
	}m_unTimerParam;

	__time32_t GetEnd() const
	{
		return m_tEnd;
	}
	__time32_t GetBegin() const
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
		m_tBegin = tBegin;
		m_tEnd = tEnd;
	}

	STimerInfo()
	{/* memset(this,0,sizeof(STimerInfo)); */
	}
	virtual void Init()
	{
		m_emType = eTimerIdle;
		m_nUserID = 0;
		ZeroMemory(&m_unTimerParam, sizeof(m_unTimerParam));
		m_tEnd = 0;
		m_tBegin = 0;
		m_bCalling = FALSE;
	};
private:
	__time32_t m_tBegin;
	__time32_t m_tEnd;
	BOOL m_bCalling;
};
typedef STimerInfo* LPSTimerInfo;

struct Timer_Less
{
	bool operator() (const LPSTimerInfo& _Left, const LPSTimerInfo& _Right) const
	{
		if (_Left->GetEnd() == _Right->GetEnd())
			return _Left < _Right;
		return _Left->GetEnd() < _Right->GetEnd();
	}
};

class CTimerMgr : public YTSvrLib::ITIMERTHREAD, public YTSvrLib::CSingle<CTimerMgr>
{
public:
    CTimerMgr(void);
    virtual ~CTimerMgr(void);

	typedef void (CTimerMgr::*TimerFunc)(LPSTimerInfo pTimer);
	void RegistTimerFunc(EM_TIMER_TYPE eTimerType, TimerFunc pFunc);
public:
	void RemoveTimer(LPSTimerInfo pTimer);
	void UpdateTimer(LPSTimerInfo pTimer, __time32_t tNewBegin, __time32_t tNewEnd);
	void UpdateTimer(LPSTimerInfo pTimer, __time32_t tNewEnd);
	LPSTimerInfo SetNewTimer(EM_TIMER_TYPE emTimerType, UINT nUserID, __time32_t tBegin, __time32_t tEnd, TIMER_PARAM nParam1 = 0, TIMER_PARAM nParam2 = 0, TIMER_PARAM nParam3 = 0, TIMER_PARAM nParam4 = 0);
private:
	void OnTimer(LPSTimerInfo pTimer);
	void ArrangeTimer();
	LPSTimerInfo AllocateTimer(EM_TIMER_TYPE emType);
	void ReleaseTimer(LPSTimerInfo pTimer);
	void AddNewTimer(LPSTimerInfo pTimer);
public:
	static void  OnTimer()
	{
		GetInstance()->OnTimerCheckQueue();
	}

	virtual void SetEvent();

    void    OnTimerCheckQueue();

	WORD	GetCurrentHour() { return m_wCurHour; }
	__time32_t GetTodayZero() { return m_tTodayZero; };
	__time32_t GetTimeNow() { return m_tNow; }
	int		GetToday() { return m_nToday; }
	int		GetTomorrow() { return m_nTomorrow; }
	WORD	GetTodayOfWeek() { return m_wCurDayOfWeek; } //
private:
	YTSvrLib::CPool<STimerInfo,256> m_poolTimers;

	typedef std::set<LPSTimerInfo,Timer_Less> SetTimer;
    SetTimer    m_setTimers;

	typedef std::list<LPSTimerInfo> ListTimer;
	ListTimer   m_listFarTimer;// restore timer after a hour

	map<EM_TIMER_TYPE, TimerFunc> m_mapTimerFunc;

	int		m_nToday;	//yyyymmdd;
	int		m_nTomorrow;
	WORD	m_wCurDayOfWeek;
	WORD	m_wCurday;
	WORD	m_wCurHour;
	__time32_t m_tTodayZero;
	__time32_t m_tNextHour;
	__time32_t m_tNextMinute;
	__time32_t m_tNext5Minute;
	__time32_t m_tNext10Minute;
	__time32_t m_tNextLegion10Minute;
	__time32_t m_tNow;
	__time32_t m_tNext15Minute;

	__time32_t m_tNext10Sec;
	__time32_t m_tNext30Sec;


	int	m_nSysCloseWaitMinutes;
	__time32_t m_tSysQuit;

	int		m_nLocalTimeZoneSeconds;
};


#define TIMER_RESET(_Timer,_NewEnd)	CTimerMgr::GetInstance()->UpdateTimer(_Timer,_NewEnd)
#define TIMER_RESETEX(_Timer,_NewBegin,_NewEnd)	CTimerMgr::GetInstance()->UpdateTimer(_Timer,_NewBegin,_NewEnd)
#define GET_TIME_NOW		CTimerMgr::GetInstance()->GetTimeNow()