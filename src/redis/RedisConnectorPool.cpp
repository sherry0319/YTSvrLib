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

	void CRedisConnectorPool::AddRedisConnect(const char* strIP, int nPort, const char* strAuth)
	{
		CRedisConnector* pNewRedisConnect = new CRedisConnector;

		pNewRedisConnect->SetConnectInfo(strIP, nPort, strAuth);

		m_vctRedisPool.push_back(pNewRedisConnect);
	}

	void CRedisConnectorPool::StartRedisConnect(REDIS_HASH_FUNCTION pFunc)
	{
		if (pFunc)
		{
			m_pHashFunction = pFunc;
		}

		for (size_t i = 0; i < m_vctRedisPool.size(); ++i)
		{
			if (m_vctRedisPool[i])
			{
				m_vctRedisPool[i]->SetID((int) i);
				m_vctRedisPool[i]->StartRedisConnect();
			}
		}
	}

	cpp_redis::client& CRedisConnectorPool::GetRedisConnector(int nID)
	{
		return m_vctRedisPool[nID]->GetRedisCore();
	}

	cpp_redis::client& CRedisConnectorPool::GetRedisConnectorByKey(int nParam) {
		int nID = m_pHashFunction(nParam);

		return GetRedisConnector(nID);
	}

#endif // YTSVRLIB_WITH_REDIS

}