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

BOOST_AUTO_TEST_CASE( task_scheduler_void_task_test )
{
	const double frequency = 13.0;
	const unsigned int maximum_times = 23;
	const double expected_duration = (1000.0 / frequency) * maximum_times;

	incrementor inc(0);
	jq::active_task_scheduler scheduler(1);
	boost::posix_time::ptime start = boost::posix_time::microsec_clock::universal_time();
	jq::task_scheduler::handle h = scheduler.schedule<void>("test",
							frequency,
							boost::bind(&incrementor::increase, &inc),
							maximum_times);
	BOOST_CHECK_THROW( scheduler.schedule<void>("test", frequency, boost::bind(&incrementor::increase, &inc)), jq::key_exists_error );
	h.wait();
	boost::posix_time::ptime end = boost::posix_time::microsec_clock::universal_time();
	boost::posix_time::time_duration elapsed = end - start;
	double actual_duration = elapsed.total_milliseconds();
	const double tolerance = 0.2;
	BOOST_CHECK( inc.m_value == maximum_times );
	BOOST_CHECK( (actual_duration - expected_duration) / expected_duration < tolerance );
}

struct task
{
	task(int init, int maximum) : m_value(init), m_maximum(maximum)
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

BOOST_AUTO_TEST_CASE( task_scheduler_boolean_task_test )
{

	const int init = 0;
	const int maximum = 29;
	task t(init, maximum);
	jq::active_task_scheduler scheduler(1);
	const double frequency = 17.0;
	const double expected_duration = (maximum - init) * 1000.0 / frequency;
	boost::posix_time::ptime start = boost::posix_time::microsec_clock::universal_time();
	jq::task_scheduler::handle h = scheduler.schedule<bool>("bool",
							frequency,
							boost::bind(&task::increase, &t));
	BOOST_CHECK_THROW( scheduler.schedule<bool>("bool", frequency, boost::bind(&task::increase, &t)), jq::key_exists_error );
	h.wait();
	boost::posix_time::ptime end = boost::posix_time::microsec_clock::universal_time();
	boost::posix_time::time_duration elapsed = end - start;
	double actual_duration = elapsed.total_milliseconds();
	const double tolerance = 0.2;
	BOOST_CHECK( t.m_value == maximum );
	BOOST_CHECK( std::abs(actual_duration - expected_duration) / expected_duration < tolerance );
}

BOOST_AUTO_TEST_CASE( task_scheduler_multi_task_test )
{
	const int init0 = 0;
	const int init1 = 123456;
	const int maximum0 = 17;
	const int maximum1 = 37;
	const double frequency0 = 11.0;
	const double frequency1 = 29.0;
	const long maximum_duration = std::max(maximum0 / frequency0, maximum1 / frequency1) * 1000.0;
	incrementor inc0(init0);
	incrementor inc1(init1);
	jq::active_task_scheduler scheduler;
	boost::posix_time::ptime start = boost::posix_time::microsec_clock::universal_time();
	
	jq::task_scheduler::handle h0 = scheduler.schedule<void>("task0", frequency0, boost::bind(&incrementor::increase, &inc0), maximum0);
	jq::task_scheduler::handle h1 = scheduler.schedule<void>("task1", frequency1, boost::bind(&incrementor::increase, &inc1), maximum1);
	h0.wait();
	h1.wait();
	boost::posix_time::ptime end = boost::posix_time::microsec_clock::universal_time();
	boost::posix_time::time_duration elapsed = end - start;
	double actual_duration = elapsed.total_milliseconds();
	const double tolerance = 0.2;
	BOOST_CHECK( inc0.m_value == init0 + maximum0 );
	BOOST_CHECK( inc1.m_value == init1 + maximum1 );
	BOOST_CHECK( (actual_duration - maximum_duration) / maximum_duration < tolerance );
}

BOOST_AUTO_TEST_CASE( task_scheduler_cancel_by_handle_test )
{
	const double frequency = 11.0;
	const int init = 0;
	incrementor inc0(init);
	jq::active_task_scheduler scheduler;
	jq::task_scheduler::handle h = scheduler.schedule<void>("t", frequency, boost::bind(&incrementor::increase, &inc0));
	boost::this_thread::sleep(boost::posix_time::milliseconds(1000.0 * 3 / frequency));
	h.cancel();
	BOOST_CHECK( inc0.m_value != init );//the task has been running
	const int v_after_cancel = inc0.m_value;
	boost::this_thread::sleep(boost::posix_time::milliseconds(1000.0 * 3 / frequency));
	BOOST_CHECK( inc0.m_value == v_after_cancel );//the task hasn't been running since cancelled

	//handle can be reused
	h = scheduler.schedule<void>("t", frequency, boost::bind(&incrementor::increase, &inc0));
	incrementor inc1(init);
	jq::task_scheduler::handle h1 = scheduler.schedule<void>("t1", frequency, boost::bind(&incrementor::increase, &inc1));
	boost::this_thread::sleep(boost::posix_time::milliseconds(1000.0 * 3 / frequency));
	h1.cancel();
	BOOST_CHECK( inc0.m_value > v_after_cancel);//t has been running since rescheduled
	BOOST_CHECK( inc1.m_value > init );//t1 has been running
	const int v0_after_cancel = inc0.m_value;
	const int v1_after_cancel = inc1.m_value;
	boost::this_thread::sleep(boost::posix_time::milliseconds(1000.0 * 3 / frequency));
	BOOST_CHECK( inc1.m_value == v1_after_cancel );//t1 hasn't been running since cancelled
	BOOST_CHECK( inc0.m_value > v0_after_cancel );//t has been running after t1 is cancelled, meaning cancelling t1 doesn't affect t
	h.cancel();
	const int v0_after_cancel_again = inc0.m_value;
	boost::this_thread::sleep(boost::posix_time::milliseconds(1000.0 * 3 / frequency));
	BOOST_CHECK( inc0.m_value == v0_after_cancel_again );//t hasn't been running since cancelled again
}

struct cancel_self_task
{
	cancel_self_task(jq::task_scheduler& scheduler,
			const std::string& task_name,
			int init,
			int cancel_when)
	 :  m_scheduler(scheduler),
	    m_task_name(task_name),
	    m_value(init),
	    m_cancel_when(cancel_when)
	{
		assert(m_value < m_cancel_when);
	}

	void execute(){
		++m_value;
		if (m_value == m_cancel_when) {
			m_scheduler.cancel(m_task_name);	
		}
	}
	jq::task_scheduler& m_scheduler;
	std::string m_task_name;
	int m_value;
	int m_cancel_when;
};

BOOST_AUTO_TEST_CASE( task_scheduler_recursive_cancel_test )
{

	jq::active_task_scheduler scheduler(1);
	const std::string task_name = "hello";
	const int cancel_when = 15;
	const double frequency = 13.0;
	{
		//we use only 1 scheduler to make sure, the task object itself can cancel the task inside task function
		cancel_self_task t(scheduler, task_name, 0, cancel_when);
		jq::task_scheduler::handle h = scheduler.schedule<void>(task_name, 13.0, boost::bind(&cancel_self_task::execute, &t));
		h.wait();
		BOOST_CHECK( t.m_value == cancel_when );
	}
	//the following test is for making sure the task is deleted from scheduler
	const long double_interval = 2.0 * 1000.0 / frequency;
	boost::this_thread::sleep(boost::posix_time::milliseconds(double_interval));
	BOOST_CHECK( !scheduler.exists(task_name) );
}

