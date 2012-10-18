#ifndef JQ_CALL_ON_RELEASE_H
#define JQ_CALL_ON_RELEASE_H

namespace jq
{
	template<typename T>
	struct call_on_release
	{
		call_on_release(const T& t) : m_call(t) {
		}
		~call_on_release() {
			m_call();
		}
		T m_call;
	};
}

#endif

