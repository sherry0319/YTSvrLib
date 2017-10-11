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

#pragma once


#define CONFIG_FILE_NAME		".\\config.ini"
#define LOG_FILE_ASYN				"LogAsyn"	//日志文件夹名称
#define LOG_FILE_SYN				"LogSyn"	//日志文件夹名称
#define LOG_HANDLE_TIME			30
#define FILE_PREFIX_MAXLEN		64

namespace YTSvrLib
{
	typedef enum
	{
		IONewDay = 1,
		IOErrorData,
		IOCommonData,
		IODataCount,
		IOWrite
	} EM_IO_TYPE;

	template<typename _Ch>
	class CLogBuffer : 
#ifdef LIB_WINDOWS
		public OVERLAPPED,
#endif
		public CRecycle
	{
		std::vector<_Ch>   m_vctBuffer;
		const static DWORD  CAPICITY = 1 << 13;
		UINT               m_nSize;
	public:
		CLogBuffer() :m_vctBuffer(CAPICITY + 1)
		{
#ifdef LIB_WINDOWS
			Internal = InternalHigh = 0;
			//Pointer = 0;
			Offset = OffsetHigh = 0;
			hEvent = NULL;
#endif
			m_nSize = 0;
		}
		virtual ~CLogBuffer(){}

		virtual void Init(){}

		virtual void Clear(){}

		_Ch* GetBuffer()
		{
			return &m_vctBuffer[0];
		}

		void ReSize(UINT nSize)
		{
			m_nSize = nSize;
		}

		UINT GetCapcity()const
		{
			return CAPICITY;
		}

		UINT GetSize()const
		{
			return m_nSize;
		}
	};

	typedef CLogBuffer<char> CLogBufferA;
	typedef CLogBuffer<wchar_t> CLogBufferW;

#ifdef LIB_WINDOWS
	class YTSVRLIB_EXPORT CLogManager
	{
	public:
		CLogManager();

		virtual ~CLogManager();
	public:
		void ShutDown();

		void Init(LPCSTR pszPrefix);
	public:
		static BOOL PostBufferReady(CLogManager* pLogMgr, CLogBufferA* pBuffer, EM_IO_TYPE eIO);

		static void WriteSynLog(CLogManager* pLogMgr, CLogBufferA* pBuffer);

		void ReadConfigFile(const char* pstrFileName);

		CLogBufferA* ApplyObj()
		{
			return m_pool.ApplyObj();
		}

		HANDLE	OpenSynLogFile();

		void ReOpenSynLogFile();

		void SetFilePrefix(LPCSTR pszPrefix);
	public:
		static unsigned WINAPI ThreadIOCPProc(LPVOID pParam);

		BOOL AssociateDevice(HANDLE hDevice, EM_IO_TYPE eIO);

		void Write(EM_IO_TYPE /*eIO*/, CLogBufferA* pLogBuffer, PLARGE_INTEGER pliOffset = NULL);

		HANDLE OpenAsynLogFile(const char* pstrFileName);

		void ReOpenAsynLogFile();

		HANDLE GetIOCPHandle() const
		{
			return m_hIOCP;
		}

		HANDLE GetSyncFileHandle() const
		{
			return m_hSynFileHandle;
		}
		BOOL LockSyncFile()
		{
			m_lockSynFile.Lock();

			return TRUE;
		}
		BOOL UnlockSyncFile()
		{
			m_lockSynFile.UnLock();

			return TRUE;
		}
	protected:
		CHAR						m_wzFilePath[128];
		CPool< CLogBufferA, 128 >     m_pool;
		HANDLE					m_hIOCP;
		HANDLE                  m_hAsynFileHandle;
		HANDLE					m_hThread;

		HANDLE                  m_hSynFileHandle;
		YTSvrLib::CLock					m_lockSynFile;
		BOOL					m_bInited;
		char m_szFilePrefix[FILE_PREFIX_MAXLEN];
		char m_szFileExt[16];
	};

	class YTSVRLIB_EXPORT CLogManagerW
	{
	public:
		CLogManagerW();

		virtual ~CLogManagerW();
	public:
		void ShutDown();

		void Init(LPCSTR pszPrefix);
	public:
		static BOOL PostBufferReady(CLogManagerW* pLogMgr, CLogBufferW* pBuffer, EM_IO_TYPE eIO);

		static void WriteSynLog(CLogManagerW* pLogMgr, CLogBufferW* pBuffer);

		void ReadConfigFile(const char* pstrFileName);

		CLogBufferW* ApplyObj()
		{
			return m_pool.ApplyObj();
		}

		HANDLE	OpenSynLogFile();

		void ReOpenSynLogFile();

		void SetFilePrefix(LPCSTR pszPrefix);
	public:
		static unsigned WINAPI ThreadIOCPProc(LPVOID pParam);

		BOOL AssociateDevice(HANDLE hDevice, EM_IO_TYPE eIO);

		void Write(EM_IO_TYPE /*eIO*/, CLogBufferW* pLogBuffer, PLARGE_INTEGER pliOffset = NULL);

		HANDLE OpenAsynLogFile(const char* pstrFileName);

		void ReOpenAsynLogFile();

		HANDLE GetIOCPHandle() const
		{
			return m_hIOCP;
		}

		HANDLE GetSyncFileHandle() const
		{
			return m_hSynFileHandle;
		}

		BOOL LockSyncFile()
		{
			m_lockSynFile.Lock();

			return TRUE;
		}

		BOOL UnlockSyncFile()
		{
			m_lockSynFile.UnLock();

			return TRUE;
		}
	protected:
		CHAR						m_wzFilePath[128];
		CPool< CLogBufferW, 128 >     m_pool;
		HANDLE					m_hIOCP;
		HANDLE                  m_hAsynFileHandle;
		HANDLE					m_hThread;

		HANDLE                  m_hSynFileHandle;
		YTSvrLib::CLock					m_lockSynFile;
		BOOL					m_bInited;
		char m_szFilePrefix[FILE_PREFIX_MAXLEN];
		char m_szFileExt[16];
	};
#else //LINUX
	class CLogManager
	{
	public:
		CLogManager();

		virtual ~CLogManager();

		void Init(LPCSTR pszPrefix);

		void ShutDown();

		CLogBufferA* ApplyObj()
		{
			return m_pool.ApplyObj();
		}

		void ReclaimObj(CLogBufferA* pObj)
		{
			m_pool.ReclaimObj(pObj);
		}
	public:
		//异步LOG
		void	ThreadWriteAsynLOG();

		static void* ThreadFunction(void* arg);

		static BOOL PostBufferReady(CLogManager* pLogMgr, CLogBufferA* pBuffer, EM_IO_TYPE);

		BOOL PostBufferReady(CLogBufferA* pBuffer);

		FILE*	OpenAsynLogFile(const char* pstrFileName);

		void	ReOpenAsynLogFile();
	public:
		//同步LOG
		static void WriteSynLog(CLogManager* pLogMgr, CLogBufferA* pBuffer);

		FILE*	OpenSynLogFile();

		void	ReOpenSynLogFile();
	public:
		FILE* GetSyncFileHandle()
		{
			return m_hSynFileHandle;
		}

		BOOL LockSyncFile()
		{
			m_lockSynFile.Lock();
		}

		BOOL UnlockSyncFile()
		{
			m_lockSynFile.UnLock();
		}
	protected:
		CHAR						m_wzFilePath[128];

		CPool<CLogBufferA, 128>		m_pool;
		std::list<CLogBufferA*>		m_listLogBuffer;
		YTSvrLib::CLock				m_lockAsynFile;
		YTSvrLib::CSemaphore		m_semAsynLog;

		FILE*						m_hAsynFileHandle;
		pthread_t					m_hThread;
		FILE*						m_hSynFileHandle;
		YTSvrLib::CLock				m_lockSynFile;
		BOOL						m_bInited;
		BOOL						m_bShutdown;
		char						m_szFilePrefix[FILE_PREFIX_MAXLEN];
		char						m_szFileExt[16];
	};

	class CLogManagerW
	{
	public:
		CLogManagerW();

		virtual ~CLogManagerW();

		void Init(LPCSTR pszPrefix);

		void ShutDown();

		CLogBufferW* ApplyObj()
		{
			return m_pool.ApplyObj();
		}

		void ReclaimObj(CLogBufferW* pObj)
		{
			m_pool.ReclaimObj(pObj);
		}
	public:
		//异步LOG
		void	ThreadWriteAsynLOG();

		static void* ThreadFunction(void* arg);

		static BOOL PostBufferReady(CLogManagerW* pLogMgr, CLogBufferW* pBuffer, EM_IO_TYPE);

		BOOL PostBufferReady(CLogBufferW* pBuffer);

		FILE*	OpenAsynLogFile(const char* pstrFileName);

		void	ReOpenAsynLogFile();
	public:
		//同步LOG
		static void WriteSynLog(CLogManagerW* pLogMgr, CLogBufferW* pBuffer);

		FILE*	OpenSynLogFile();

		void	ReOpenSynLogFile();
	public:
		FILE* GetSyncFileHandle()
		{
			return m_hSynFileHandle;
		}
		BOOL LockSyncFile()
		{
			m_lockSynFile.Lock();
		}
		BOOL UnlockSyncFile()
		{
			m_lockSynFile.UnLock();
		}
	protected:
		CHAR						m_wzFilePath[128];

		CPool<CLogBufferW, 128>		m_pool;
		std::list<CLogBufferW*>		m_listLogBuffer;
		YTSvrLib::CLock				m_lockAsynFile;
		YTSvrLib::CSemaphore		m_semAsynLog;

		FILE*						m_hAsynFileHandle;
		pthread_t					m_hThread;
		FILE*						m_hSynFileHandle;
		YTSvrLib::CLock				m_lockSynFile;
		BOOL						m_bInited;
		BOOL						m_bShutdown;
		char						m_szFilePrefix[FILE_PREFIX_MAXLEN];
		char						m_szFileExt[16];
	};
#endif // LIB_WINDOWS


	// 全局进程日志.关联着LOG() LOGWARN() LOGERROR() 
	class CProcessLogMgr : public CLogManager, public CSingle < CProcessLogMgr >
	{
	public:
		CProcessLogMgr()
		{
			strncpy_s(m_wzFilePath, LOG_FILE_ASYN, 127);
		}

		virtual ~CProcessLogMgr()
		{

		}
	};
}
