#pragma once

#ifndef __MESSAGE_MAPPING_H_
#define __MESSAGE_MAPPING_H_

namespace YTSvrLib
{
	typedef std::function<void(YTSvrLib::ITCPBASE*, const char*, int)> CLTPSRFUNC;

	class YTSVRLIB_EXPORT ITCPMSGHANDLER {
	protected:
		std::unordered_map<int, CLTPSRFUNC> m_CltMsgProcs;
	protected:
		ITCPMSGHANDLER()
		{
			m_CltMsgProcs.clear();
		}
	protected:
		virtual void Register(WORD wType, CLTPSRFUNC pFunc)
		{
			m_CltMsgProcs[wType] = pFunc;
		}
	};
}

#endif
