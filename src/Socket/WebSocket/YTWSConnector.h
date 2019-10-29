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

#include <websocketpp/config/asio.hpp>
#include <websocketpp/server.hpp>
#include "WSSendBuffer.h"
#include "YTWSServer.h"

namespace YTSvrLib
{
#ifdef LIB_WINDOWS
	using SOCKET_HANDLE = HANDLE;
#else
	using SOCKET_HANDLE = LONGLONG;
#endif // LIB_WINDOWS

	class YTSVRLIB_EXPORT IWSCONNECTOR
	{
	public:
		IWSCONNECTOR();
		virtual ~IWSCONNECTOR();

		virtual void Create(IWSSERVER* server, IWSSERVER::connection_ptr& con);

		bool IsEnable();

		void Clean();

		void SafeClose();

		void Send(const char* msg, int len);

		void OnSend();

		SOCKET_HANDLE GetSocket() const;

		std::string& GetAddrIp();

		int GetAddrPort() const;
	protected:
		CWSSendBuffer m_sendBuf;
	private:
		YTSvrLib::CLock m_sendLock;
		IWSSERVER::connection_ptr m_con;
		IWSSERVER* m_server;
		std::string m_strIP;
		int m_nPort;
	};
}