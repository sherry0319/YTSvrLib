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
#include "RedisConnector.h"

namespace YTSvrLib
{
#ifdef YTSVRLIB_WITH_REDIS

	void CRedisConnector::SetConnectInfo(const char* strIP, int nPort, const char* strAuth)
	{
		if (m_bInited == FALSE)
		{
			m_bInited = TRUE;
			strncpy_s(m_szIP, strIP, 63);
			strncpy_s(m_szAuthPassword, strAuth, 127);
			m_nPort = nPort;
		}
	}

	void CRedisConnector::StartRedisConnect()
	{
		m_Core.connect("127.0.0.1", 6379,
			[this](const std::string&, std::size_t, cpp_redis::connect_state status) {
			switch (status)
			{
			case cpp_redis::connect_state::dropped: {
				LOG("OnAysncRedis=%s:%d Disconnected=%d", m_szIP, m_nPort, status);
				m_bConnected = FALSE;
			}break;
			case cpp_redis::connect_state::start:
				break;
			case cpp_redis::connect_state::sleeping:
				break;
			case cpp_redis::connect_state::ok: {
				LOG("OnAysncRedis=%s:%d Connected=%d", m_szIP, m_nPort, status);

				std::string autoKey(m_szAuthPassword);
				if (!autoKey.empty())
				{
					m_Core.auth(autoKey, [this](cpp_redis::reply& r) {
						if (r.get_type() == cpp_redis::reply::type::simple_string && r.as_string() == "OK")
						{
							m_bConnected = TRUE;
						}
					});
				}
				else {
					m_bConnected = TRUE;
				}
			}break;
			case cpp_redis::connect_state::failed:
				break;
			case cpp_redis::connect_state::lookup_failed:
				break;
			case cpp_redis::connect_state::stopped:
				break;
			default:
				break;
			}
		},3000,10,1000);
	}

#endif // YTSVRLIB_WITH_REDIS

}