#pragma once

#include "Service/TimerThread.h"

typedef LONGLONG TIMER_PARAM;

typedef enum 
{
	eTimerIdle = 0,
	eTimerTest,
	eTimerMax = 1024,
} EM_TIMER_TYPE;

typedef struct STimerInfo : public YTSvrLib::CRecycle
{
	EM_TIMER_TYPE m_emType;
	int m_nUserID;

	union unTimerParam
	{
		TIMER_PARAM m_ayParams[4];
	}m_unTimerParam;

	DOUBLE GetEnd() const
	{
		return m_tEnd;
	}
	DOUBLE GetBegin() const
	{
		return m_tBegin;
	}

	BOOL IsCalling() const {return m_bCalling;}

	void SetCalling(BOOL bCall) {m_bCalling = bCall;} 

	void ResetTimer(__time32_t tBegin,__time32_t tEnd)
	{
		m_tBegin = (DOUBLE) tBegin;
		m_tEnd = (DOUBLE) tEnd;
	}
	void ResetTimer(DOUBLE tBegin, DOUBLE tEnd)
	{
		m_tBegin = tBegin;
		m_tEnd = tEnd;
	}

	STimerInfo() {/* memset(this,0,sizeof(STimerInfo)); */}
	virtual void Init()
	{
		m_emType = eTimerIdle;
		m_nUserID = 0;
		ZeroMemory(&m_unTimerParam,sizeof(m_unTimerParam));
		m_tEnd = 0;
		m_tBegin = 0;
		m_bCalling = FALSE;
	};	
private:
	DOUBLE m_tBegin;
	DOUBLE m_tEnd;
	BOOL m_bCalling;
} STimerInfo;
typedef STimerInfo* LPSTimerInfo;

typedef set<LPSTimerInfo> CSetTimerMgr;

struct Timer_Less
{
	bool operator() (const LPSTimerInfo& _Left, const LPSTimerInfo& _Right) const
	{
		if( _Left->GetEnd() == _Right->GetEnd())
			return _Left < _Right;
		return _Left->GetEnd() < _Right->GetEnd();
	}
};

class CTimerMgr : public YTSvrLib::ITIMERTHREAD,public YTSvrLib::CSingle<CTimerMgr>
{
public:
    CTimerMgr(void);
    virtual ~CTimerMgr(void);
public:
	typedef void (CTimerMgr::*TimerFunc)(LPSTimerInfo pTimer);
	void RegistTimerFunc(EM_TIMER_TYPE eTimerType,TimerFunc pFunc);

	void    OnTimerCheckQueue();
	virtual void SetEvent();
	static void OnTimer()
	{
		GetInstance()->OnTimerCheckQueue();
	}

	void	OnTimer(LPSTimerInfo pTimer);
	void	RemoveTimer( LPSTimerInfo pTimer );
	void	UpdateTimer(LPSTimerInfo pTimer, DOUBLE tNewBegin,DOUBLE tNewEnd);
	void	UpdateTimer(LPSTimerInfo pTimer, DOUBLE tNewEnd);
	LPSTimerInfo	SetNewTimer(EM_TIMER_TYPE emTimerType, UINT nUserID, DOUBLE tBegin, DOUBLE tEnd, TIMER_PARAM nParam1 = 0, TIMER_PARAM nParam2 = 0, TIMER_PARAM nParam3 = 0, TIMER_PARAM nParam4 = 0);

	__time32_t GetTimeNowSec(){ return (__time32_t)m_tNow;}
	DOUBLE GetTimeNow() { return m_tNow;}// 获取当前时间(UTC秒.毫秒 例如 1476027497.330)

	WORD	GetCurrentHour()
	{
		return m_wCurHour;
	}

	WORD	GetCurDay()
	{
		return m_wCurday;
	}
	
	WORD	GetTodayOfWeek()
	{
		return m_wCurDayOfWeek;
	}

	__time32_t GetTimeZone()
	{
		return (-m_nLocalTimeZoneSeconds);
	}
public:
#undef TIMER_FUNC_CALLBACK
#define TIMER_FUNC_CALLBACK(_Timer,_Func) void _Func(LPSTimerInfo pTimer);
#include "TimerFuncTable.h"
private:
	void	ArrangeTimer();
	LPSTimerInfo AllocateTimer( EM_TIMER_TYPE emType );
	void    ReleaseTimer( LPSTimerInfo pTimer );
	void    AddNewTimer( LPSTimerInfo pTimer );
	
	typedef std::list<LPSTimerInfo> ListTimer;
	ListTimer   m_listFarTimer;// 超过1小时之外的Timer都放到fartimer
	YTSvrLib::CPool<STimerInfo,1024> m_poolTimers;

	typedef std::set<LPSTimerInfo,Timer_Less> SetTimer;
    SetTimer    m_setTimers;

	DOUBLE m_tNow;
	WORD	m_wCurday;
	WORD	m_wCurHour;
	WORD	m_wCurDayOfWeek;
	__time32_t m_tNextMinute;
	__time32_t m_tNextHour;

	int		m_nLocalTimeZoneSeconds;
	map<EM_TIMER_TYPE, TimerFunc> m_mapTimerFunc;
};