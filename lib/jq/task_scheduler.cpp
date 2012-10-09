#include "task_scheduler.h"
#include "key_exists_error.h"
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>

namespace jq {

task_scheduler::handle::handle()
 :  m_scheduler(0),
    m_valid(false),
    m_signal(),
    m_id(0),
    m_semaphore(),
    m_connection()
{
}

task_scheduler::handle::handle(const handle& rhs)
 :  m_scheduler(rhs.m_scheduler),
    m_id(rhs.m_id),
    m_signal(rhs.m_signal),
    m_valid(rhs.m_valid),
    m_semaphore(),
    m_connection()
{
	if(m_valid) {
		if(boost::shared_ptr<sigtype> s = m_signal.lock()) {
			m_connection = s->connect(boost::bind(&handle::notified, this));
    			m_semaphore.reset(new boost::interprocess::interprocess_semaphore(0));
		}
	}
}
			
task_scheduler::handle& task_scheduler::handle::operator=(const task_scheduler::handle& rhs)
{
	m_id = rhs.m_id;
	m_valid = rhs.m_valid;
	m_signal = rhs.m_signal;
	if(boost::shared_ptr<sigtype> s = m_signal.lock()) {
		if (m_valid) {
			m_connection = s->connect(boost::bind(&handle::notified, this));
    			m_semaphore.reset(new boost::interprocess::interprocess_semaphore(0));
		}
	}
	else {
		m_valid = false;
	}

	if(!m_valid) {
		m_semaphore.reset();
	}

	return *this;
}

void task_scheduler::handle::wait()
{
	if (m_valid && m_semaphore) {
		m_semaphore->wait();
	}
}

void task_scheduler::handle::cancel()
{
	if (m_valid) {
		assert(m_id != 0);
		m_scheduler->cancel(m_id);
	}

	wait();
}

task_scheduler::handle::handle (task_scheduler& scheduler,
				unsigned int id,
				const boost::shared_ptr<sigtype>& sig)
 :  m_scheduler(&scheduler),
    m_id(id),
    m_valid(true),
    m_signal(sig),
    m_semaphore(new boost::interprocess::interprocess_semaphore(0)),
    m_connection()
{
	assert(id != 0);
	assert(sig);
	m_connection = sig->connect(boost::bind(&handle::notified, this));
}

void task_scheduler::handle::notified()
{
	m_semaphore->post();
}

task_scheduler::task_scheduler(boost::asio::io_service& io)
 :  m_mutex(),
    m_io(io),
    m_latestid(0),
    m_tasks(),
    m_id_names()
{
}

task_scheduler::~task_scheduler()
{
}

task_scheduler::handle task_scheduler::schedule(const std::string& name,
 			        		double frequency,
			        		const void_task& task,
			        		unsigned int maximum_times)
{
	bool_task t = (boost::lambda::bind(task), true);

	return schedule(name, frequency, t, maximum_times);
}
		
task_scheduler::handle task_scheduler::schedule(const std::string& name,
						double frequency,
						const bool_task& task,
						unsigned int maximum_times)
{
	assert(!name.empty() && "please give a non-empty name");
	assert(frequency > 0.0 && "please specify a positive frequency");
	boost::mutex::scoped_lock _(m_mutex);
	id_name_bm_type::right_const_iterator ni = m_id_names.right.find(name);
	if (ni != m_id_names.right.end()) {
		throw jq::key_exists_error("task_schedule already has the task", name);
	}
	
	boost::shared_ptr<task_info> ti(new task_info(m_io, task, frequency, maximum_times));
	ti->h.reset(new handle(*this, ++m_latestid, ti->signal));

	assert(m_id_names.left.find(m_latestid) == m_id_names.left.end());
	assert(m_tasks.find(m_latestid) == m_tasks.end());
	m_id_names.insert(id_name_bm_type::value_type(m_latestid, name));
	m_tasks[m_latestid] = ti;

	register_next_run(*ti, ti->interval);

	return *(ti->h);
}

bool task_scheduler::exists(const std::string& name) const
{
	boost::mutex::scoped_lock _(m_mutex);
	return m_id_names.right.find(name) != m_id_names.right.end();
}

void task_scheduler::register_next_run(task_info& ti, unsigned long to_wait)
{
	ti.timer.expires_from_now(boost::posix_time::milliseconds(to_wait));
	ti.timer.async_wait(boost::bind(&task_scheduler::execute, this, _1, boost::ref(ti)));
}

void task_scheduler::execute(const boost::system::error_code& error, task_info& ti)
{
	bool task_alive = false;
	if(!error)
	{
		try {
			boost::posix_time::ptime start = boost::posix_time::microsec_clock::local_time();
			bool to_continue = ti.functor();
			ti.executed_times += 1;
			if(to_continue) {
				to_continue = ti.maximum_times_to_run == 0 || ti.executed_times < ti.maximum_times_to_run;
			}
			if(to_continue) {
				boost::posix_time::ptime end = boost::posix_time::microsec_clock::local_time();
				unsigned long consumed = (end - start).total_milliseconds();
				unsigned long next_interval = consumed > ti.interval ? 1 : ti.interval - consumed;
				register_next_run(ti, next_interval);
				task_alive = true;
			}
		}
		catch(const std::exception& e) {
			ti.h->error_ocurred(e.what());
		}
	}
		
	if (!task_alive) {
		task_finished(ti.h->m_id);
	}
}

void task_scheduler::cancel(const std::string& name)
{
	boost::shared_ptr<handle> h;
	{
		boost::mutex::scoped_lock _(m_mutex);
		id_name_bm_type::right_const_iterator ni = m_id_names.right.find(name);
		if (ni != m_id_names.right.end()) {
			unsigned int id = ni->second;
			h = cancel_impl(id);	
		}
	}
	h->wait();
}

void task_scheduler::cancel(unsigned int id)
{
	boost::shared_ptr<handle> h;
	{
		boost::mutex::scoped_lock _(m_mutex);
		id_name_bm_type::left_const_iterator ii = m_id_names.left.find(id);
		if (ii != m_id_names.left.end()) {
			h = cancel_impl(id);
		}
	}
	h->wait();
}

boost::shared_ptr<task_scheduler::handle> task_scheduler::cancel_impl(unsigned int id)
{
	task_map::iterator i = m_tasks.find(id);
	assert(i != m_tasks.end());
	i->second->timer.cancel();
	boost::shared_ptr<handle> h = i->second->h;
	remove_record(id);
	return h;
}

//!this is called when task is dead naturally
void task_scheduler::task_finished(unsigned int id)
{
	boost::mutex::scoped_lock _(m_mutex);
	remove_record(id);
}

void task_scheduler::remove_record(unsigned int id)
{
	m_id_names.left.erase(id);	
	m_tasks.erase(id);
}

task_scheduler::task_info::task_info(boost::asio::io_service& io,
				     const bool_task& task,
				     double frequency,
				     unsigned long maximum_running_times)
 :  functor(task),
    interval(1000.0 / frequency),
    signal(new boost::signals2::signal<void()>()),
    h(),
    timer(io),
    maximum_times_to_run(maximum_running_times),
    executed_times(0)
{
}

task_scheduler::task_info::~task_info()
{
	(*signal)();
}

}

