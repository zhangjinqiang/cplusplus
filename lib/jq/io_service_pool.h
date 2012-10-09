#ifndef JQ_IO_SERVICE_POOL_H
#define JQ_IO_SERVICE_POOL_H

#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <list>

namespace jq
{
	class io_service_pool
	{
	public:
		io_service_pool(unsigned int size = 1);
		virtual ~io_service_pool();

		boost::asio::io_service& get_io_service();

		void start();

		void stop();

	private:
		boost::asio::io_service m_io;
		boost::asio::io_service::work m_iowork;
		unsigned int m_poolsize;
		std::list<boost::shared_ptr<boost::thread> > m_pool;
	};
}

#endif

