#pragma once

typedef enum 
{
	eTimerIdle = 0,
	eTimerTest,
	eTimerMax = 1024,
} EM_TIMER_TYPE;

class CTimerMgr : public YTSvrLib::ITIMERTHREAD,public YTSvrLib::ITIMERHANDLER,public YTSvrLib::CSingle<CTimerMgr>
{
public:
    CTimerMgr(void);
    virtual ~CTimerMgr(void);
private:
	virtual void OnTimer(YTSvrLib::LPSTimerInfo pTimer) override;

	virtual DOUBLE GetNearTime() override;
public:
	typedef void (CTimerMgr::*TimerFunc)(YTSvrLib::LPSTimerInfo pTimer);
	void RegistTimerFunc(EM_TIMER_TYPE eTimerType,TimerFunc pFunc);

	void    OnTimerCheckQueue();
	virtual void SetEvent();
	static void OnTimer()
	{
		GetInstance()->OnTimerCheckQueue();
	}

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
#define TIMER_FUNC_CALLBACK(_Timer,_Func) void _Func(YTSvrLib::LPSTimerInfo pTimer);
#include "TimerFuncTable.h"
private:
	DOUBLE m_tNow;
	WORD	m_wCurday;
	WORD	m_wCurHour;
	WORD	m_wCurDayOfWeek;
	__time32_t m_tNextMinute;
	__time32_t m_tNextHour;

	int		m_nLocalTimeZoneSeconds;
	map<EM_TIMER_TYPE, TimerFunc> m_mapTimerFunc;
};