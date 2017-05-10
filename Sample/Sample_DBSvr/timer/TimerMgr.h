#pragma once

typedef enum {
	eDBTimerIdle = 0,
	eDBTimerMax
} EM_DBTIMER_TYPE;

class CTimerMgr : public YTSvrLib::ITIMERTHREAD,public YTSvrLib::CSingle<CTimerMgr>
{
public:
    CTimerMgr(void);
    virtual ~CTimerMgr(void);
public:
	static void OnTimer()
	{
		GetInstance()->OnTimerCheckQueue();
	}

	virtual void SetEvent();

    void    OnTimerCheckQueue();
	
	WORD	GetCurrentHour() { return m_wCurHour; }
	__time32_t GetTodayZero() { return m_tTodayZero; };
	__time32_t GetTimeNow() { return m_tNow; }
	BOOL	IsNightNow() { return ( m_wCurHour>=1 && m_wCurHour < 8 ); }
	int		GetToday() { return m_nToday; }
	int		GetTomorrow() { return m_nTomorrow; }
	WORD	GetTodayOfWeek() { return m_wCurDayOfWeek; }
private:
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

	int		m_nLocalTimeZoneSeconds;	//exp: -8*3600 In BeiJing TimeZone
};


#define TIMER_NEW(t)		CTimerMgr::GetInstance()->AllocateTimer(t)
#define TIMER_DEL(t)		CTimerMgr::GetInstance()->ReleaseTimer(t)
#define TIMER_SET(t)		CTimerMgr::GetInstance()->AddNewTimer(t)
#define TIMER_RESET(t, n)	CTimerMgr::GetInstance()->UpdateTimer(t, n)
#define TIMER_UNSET(t)		CTimerMgr::GetInstance()->RemoveTimer(t)
#define GET_TIME_NOW		CTimerMgr::GetInstance()->GetTimeNow()