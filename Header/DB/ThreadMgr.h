#ifndef INC_THREADMGR_H
#define INC_THREADMGR_H

#define COMMAND_QUITMGR		0x00000000

#include <windows.h>
#include "../Service/SyncObj.h"

struct SCommand
{
	UINT nCommand;
	BOOL bSync;			//ÊÇ·ñÍ¬²½
	WPARAM wParam;
	LPARAM lParam;

	struct SCommand* pNext;
	SCommand(UINT n, WPARAM w, LPARAM l)
	{
		nCommand = n;
		wParam = w;
		lParam = l;
		bSync = FALSE;
		pNext = NULL;
	}
};

#define TMTYPE_MESSAGELOOP		0x00000001
#define TMTYPE_WAITCMDEVENT		0x00000002

DWORD WINAPI ThreadMgrRoutine(LPVOID lpParam);

class CThreadMgr
{
public:
	CThreadMgr();
	virtual ~CThreadMgr();
    
public:
	virtual BOOL Init() = 0;
	virtual BOOL Uninit() = 0;
	virtual BOOL OnTimer() = 0;
	virtual BOOL Destroy();		//Must Call At Other Thread
	virtual BOOL Create(DWORD dwFlag);

	BOOL PostCommand(UINT nCmd, WPARAM wParam, LPARAM lParam);
	BOOL SendCommand(UINT nCmd, WPARAM wParam, LPARAM lParam);	//Must Call At Other Thread
	BOOL MessageLoop();
	BOOL CommandLoop();
	BOOL IsDestroying();
	DWORD GetThreadID() { return m_dwThreadID; };
	UINT GetCommandListCount();
	UINT GetCommandListCountMax();
	void	ClearCommandListCountMax();

protected:
	virtual BOOL ProcessCommand(const SCommand* pCmd) = 0;
	virtual void ProcessException(PEXCEPTION_RECORD pEptRec, PCONTEXT pCxtRec);
protected:

	void FreeCommandList();
	BOOL SetThreadPriority(int nPriority);

	SCommand * SCmdNew( UINT n, WPARAM w, LPARAM l );
	void	SCmdRelease( SCommand* pCmd );
	void FreeCmdBuf();
	void AddSCmdTail( SCommand* pCmd );
	void AddSCmdHead( SCommand* pCmd );
	SCommand* GetSCmdHead();

protected:
	YTSvrLib::CLock       m_csCmd;
//	CPtrList	m_lsCommand;
	HANDLE		m_hThread;
	DWORD		m_dwThreadID;
	HANDLE		m_hCmdEvent;
	HANDLE		m_hSyncEvent;
	DWORD		m_dwFlag;
	UINT		m_nSleep;
	BOOL		m_bDestroying;

	struct SCommand * m_pCmdBuf;
	UINT	m_nCmdBuf;
	struct SCommand * m_pCmdListHead;
	struct SCommand * m_pCmdListTail;
	UINT	m_nCmdList;
	UINT	m_nCmdListMax;

	friend DWORD WINAPI ThreadMgrRoutine(LPVOID lpParam);
};


#endif