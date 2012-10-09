#include "io_service_pool.h"
#include <boost/bind.hpp>

namespace jq {

io_service_pool::io_service_pool(unsigned int size)
 :  m_io(),
    m_iowork(m_io),
    m_poolsize(size),
    m_pool()
{
	assert(size != 0 && "who likes an empty pool?");
}

io_service_pool::~io_service_pool()
{
}

boost::asio::io_service& io_service_pool::get_io_service()
{
	return m_io;
}

void io_service_pool::start()
{
	for(unsigned int i = 0; i < m_poolsize; ++i) {
		m_pool.push_back(boost::shared_ptr<boost::thread>(
			new boost::thread(boost::bind(&boost::asio::io_service::run, &m_io))));
	}
}

void io_service_pool::stop()
{
	m_io.stop();
	while(!m_pool.empty()) {
		m_pool.front()->join();
		m_pool.pop_front();
	}
	m_io.reset();
}

}

