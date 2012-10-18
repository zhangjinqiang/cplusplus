#ifndef JQ_THREAD_SAFE_OBJECT_H
#define JQ_THREAD_SAFE_OBJECT_H

#include <boost/thread/mutex.hpp>

namespace jq
{
	template<typename T>
	class thread_safe_object
	{
	public:
		thread_safe_object(const T& object = T()) : m_mutex(), m_object(object) {}
		thread_safe_object(const thread_safe_object& rhs) : m_mutex(), m_object(rhs.get()){}
		~thread_safe_object(){}

		thread_safe_object& operator=(const thread_safe_object& rhs) {
			set(rhs.get());
			return *this;
		}
		void set(const thread_safe_object& rhs) {
			boost::mutex::scoped_lock _(m_mutex);
			m_object = rhs.get();
		}

		T get() const {
			boost::mutex::scoped_lock _(m_mutex);
			return m_object;
		}

		operator T() const {
			return get();
		}

	private:
		mutable boost::mutex m_mutex;
		T m_object;
	};
}

#endif

