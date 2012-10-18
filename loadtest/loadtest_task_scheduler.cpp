#include "active_task_scheduler.h"
#include "key_exists_error.h"
#include <boost/test/unit_test.hpp>
#include <boost/date_time.hpp>

using namespace boost::unit_test;

struct incrementor
{
	incrementor(int init) : m_value(init){}
	void increase(){	++m_value;	}
	int m_value;
};

struct btask
{
	btask(int init, int maximum) : m_value(init), m_maximum(maximum)
	{
		assert(m_value < m_maximum);
	}

	bool increase()
	{
		++m_value;
		return m_value < m_maximum;
	}

	int m_value;
	int m_maximum;
};

struct cancel_self_task
{
	cancel_self_task(jq::task_scheduler& scheduler,
			int init,
			int cancel_when)
	 :  m_scheduler(scheduler),
	    m_handle(),
	    m_value(init),
	    m_cancel_when(cancel_when)
	{
		assert(m_value < m_cancel_when);
	}

	void execute() {
		++m_value;
		if (m_value == m_cancel_when) {
			m_handle.cancel();	
		}
	}
	jq::task_scheduler& m_scheduler;
	jq::task_scheduler::handle m_handle;
	int m_value;
	int m_cancel_when;
};

BOOST_AUTO_TEST_CASE( task_scheduler_madness_test )
{
	jq::task_scheduler::handle ch, bh, vh; 
	{
		jq::active_task_scheduler scheduler(3);
		const std::string vtask_name = "void";
		const double vfrequency = 153.0;
		const int vmaximum = 13573647;
		incrementor vtask(0);

		const std::string btask_name = "bool";
		const double bfrequency = vfrequency + 13;	
		const int bmaximum = 23345690;
		btask b(0, bmaximum);

		const std::string ctask_name = "suisider";
		const double cfrequency = vfrequency + 2.0;
		const int cancel_when = 378635238;
		cancel_self_task c(scheduler, 0, cancel_when);

		BOOST_CHECK(!ch.valid());
		BOOST_CHECK(!bh.valid());
		BOOST_CHECK(!vh.valid());

		for(unsigned int i = 0; i < 16688; ++i) {
			if (i % 100 == 0) std::cout << __FUNCTION__ << ": no." << i << std::endl;
			if (!ch.valid()) {
				ch = scheduler.schedule<void>(ctask_name, cfrequency, boost::bind(&cancel_self_task::execute, &c));
				BOOST_CHECK( ch.valid() );
				c.m_handle = ch;
			}
			else if (i % 357 == 13) {
				ch.cancel();
				BOOST_CHECK( !ch.valid() );
			}
			if (!bh.valid()) {
				bh = scheduler.schedule<bool>(btask_name, bfrequency, boost::bind(&btask::increase, &b), bmaximum);
				BOOST_CHECK( bh.valid() );
			}
			else if (i % 234 == 17) {
				bh.cancel();
				BOOST_CHECK( !bh.valid() );
			}
			if (!vh.valid()) {
				vh = scheduler.schedule<void>(vtask_name, vfrequency, boost::bind(&incrementor::increase, &vtask), vmaximum);
				BOOST_CHECK( vh.valid() );
			}
			else if (i % 11 == 9) {
				vh.cancel();
				BOOST_CHECK( !vh.valid() );
			}
			boost::this_thread::sleep(boost::posix_time::milliseconds(1));
		}
	}
	BOOST_CHECK(!bh.valid());
	BOOST_CHECK(!vh.valid());
}

