#pragma once

enum EM_TIMER_TYPE
{
	eTimerIdle = 0,
	eTimerMax,
};

class CTimerMgr : public YTSvrLib::ITIMERTHREAD, public YTSvrLib::ITIMERHANDLER,public YTSvrLib::CSingle<CTimerMgr>
{
public:
    CTimerMgr(void);
    virtual ~CTimerMgr(void);
private:
	virtual void OnTimer(YTSvrLib::LPSTimerInfo pTimer) override;

	virtual DOUBLE GetNearTime() override;
public:
	typedef void (CTimerMgr::*TimerFunc)(YTSvrLib::LPSTimerInfo pTimer);
	void RegistTimerFunc(EM_TIMER_TYPE eTimerType, TimerFunc pFunc);
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

	int		m_nLocalTimeZoneSeconds;
};


#define TIMER_RESET(_Timer,_NewEnd)	CTimerMgr::GetInstance()->UpdateTimer(_Timer,_NewEnd)
#define TIMER_RESETEX(_Timer,_NewBegin,_NewEnd)	CTimerMgr::GetInstance()->UpdateTimer(_Timer,_NewBegin,_NewEnd)
#define GET_TIME_NOW		CTimerMgr::GetInstance()->GetTimeNow()