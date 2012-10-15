#include "task_scheduler.h"
#include "key_exists_error.h"
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>

namespace jq {

task_scheduler::handle::handle()
 :  m_mutex(),
    m_scheduler(0),
    m_valid(false),
    m_signal(),
    m_id(0),
    m_semaphore(),
    m_connection()
{
}

task_scheduler::handle::handle(const handle& rhs)
 :  m_scheduler(rhs.m_scheduler),
    m_valid(rhs.m_valid),
    m_signal(rhs.m_signal),
    m_id(rhs.m_id),
    m_semaphore(),
    m_connection()
{
	establish(rhs);
}
	
task_scheduler::handle& task_scheduler::handle::operator=(const task_scheduler::handle& rhs)
{
	m_scheduler = rhs.m_scheduler;
	m_id = rhs.m_id;
	m_signal = rhs.m_signal;

	establish(rhs);

	return *this;
}

bool task_scheduler::handle::valid() const
{
	return m_valid;
}

void task_scheduler::handle::wait()
{
	if (m_valid) {
		assert( m_semaphore );
		m_semaphore->wait();
		boost::mutex::scoped_lock _(m_mutex);
		clear();
	}
}

void task_scheduler::handle::cancel()
{
	bool to_wait = false;
	if (m_valid) {
		boost::mutex::scoped_lock _(m_mutex);
		if(m_valid && m_scheduler != 0 && m_id !=0) {
			to_wait = !m_scheduler->cancel(m_id);
		}
	}

	if(to_wait) {
		wait();
	}
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
	assert(m_scheduler != 0);
	assert(id != 0);
	assert(sig);
	m_connection = sig->connect(boost::bind(&handle::notified, this));
}

void task_scheduler::handle::establish(const handle& rhs)
{
	if (rhs.m_valid) {
		if (boost::shared_ptr<sigtype> s = rhs.m_signal.lock()) {
    			m_semaphore.reset(new boost::interprocess::interprocess_semaphore(0));
			m_connection = s->connect(boost::bind(&handle::notified, this));
			if (!rhs.m_connection.connected()) {
				m_connection.disconnect();
				clear();
			}
			else {
				m_valid = true;
			}
		}
		else {
			clear();
		}
	}
	else {
		clear();
	}
}

void task_scheduler::handle::notified()
{
	m_connection.disconnect();
	assert(m_semaphore);
	m_semaphore->post();
}

void task_scheduler::handle::clear()
{
	m_valid = false;
	m_scheduler = 0;
	m_id = 0;
}

//======================= the following is for task_scheduler ==========================//

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
	ti->cancel_connection = ti->signal->connect(boost::bind(&task_scheduler::cancel_from_task_natural_exit, this, m_latestid));

	assert(m_id_names.left.find(m_latestid) == m_id_names.left.end());
	assert(m_tasks.find(m_latestid) == m_tasks.end());
	m_id_names.insert(id_name_bm_type::value_type(m_latestid, name));
	m_tasks[m_latestid] = ti;

	signal_holder sig(new jq::call_on_release<void_task>(boost::bind(static_cast<void (sig_type::*)()>(&sig_type::operator()), ti->signal.get())));
	register_next_run(ti, ti->interval, sig);

	return *(ti->h);
}

bool task_scheduler::exists(const std::string& name) const
{
	boost::mutex::scoped_lock _(m_mutex);
	return m_id_names.right.find(name) != m_id_names.right.end();
}

void task_scheduler::register_next_run(const boost::shared_ptr<task_info>& ti, unsigned long to_wait, const signal_holder& sig)
{
	ti->timer.expires_from_now(boost::posix_time::milliseconds(to_wait));
	ti->timer.async_wait(boost::bind(&task_scheduler::execute, this, _1, ti, sig));
}

void task_scheduler::execute(const boost::system::error_code& error, const boost::shared_ptr<task_info>& ti, signal_holder sig)
{
	if (!error && !ti->cancelled)
	{
		try {
			boost::posix_time::ptime start = boost::posix_time::microsec_clock::local_time();
			ti->thread_id = boost::this_thread::get_id();
			assert( !ti->executing );
			ti->executing = true;
			bool to_continue = ti->functor();
			ti->executed_times += 1;
			if (!ti->executing) {
				//cancelled in the task recursively
			}
			else {
				ti->executing = false;

				to_continue = to_continue && (ti->maximum_times_to_run == 0 || ti->executed_times < ti->maximum_times_to_run);
				if (to_continue) {
					boost::posix_time::ptime end = boost::posix_time::microsec_clock::local_time();
					unsigned long consumed = (end - start).total_milliseconds();
					unsigned long next_interval = consumed > ti->interval ? 1 : ti->interval - consumed;
					register_next_run(ti, next_interval, sig);
				}
			}
		}
		catch (const std::exception& e) {
			ti->executing = false;
			ti->h->error_ocurred(e.what());
		}
	}
}

bool task_scheduler::cancel(unsigned int id)
{
	bool recursively = false;
	boost::shared_ptr<handle> h;
	{
		boost::mutex::scoped_lock _(m_mutex);
		task_map::iterator i = m_tasks.find(id);
		assert(i != m_tasks.end());
		assert(i->second);
		task_info& task = *(i->second);
		assert(task.cancel_connection.connected());
		task.cancel_connection.disconnect();
		if (task.executing && (task.thread_id == boost::this_thread::get_id())) {
			//recursively cancelling
			task.executing = false;
			recursively = true;
		}
		else {
			task.cancel();
			h = task.h;
		}
		remove_record(id);
	}

	if (h) {
		h->wait();
		assert( !recursively  );
	}

	return recursively;
}

void task_scheduler::cancel_from_task_natural_exit(unsigned int id)
{
	boost::mutex::scoped_lock _(m_mutex);
	task_map::iterator i = m_tasks.find(id);
	if (i != m_tasks.end()) {
		assert(i->second);
		task_info& task = *(i->second);
		assert(task.cancel_connection.connected());
		task.cancel_connection.disconnect();
		assert(!task.executing);
		remove_record(id);
	}
}

void task_scheduler::remove_record(unsigned int id)
{
	assert(m_id_names.left.find(id) != m_id_names.left.end());
	assert(m_tasks.find(id) != m_tasks.end());
	m_id_names.left.erase(id);	
	m_tasks.erase(id);
}

task_scheduler::task_info::task_info(boost::asio::io_service& io,
				     const bool_task& task,
				     double frequency,
				     unsigned long maximum_running_times)
 :  executing(false),
    thread_id(),
    functor(task),
    interval(1000.0 / frequency),
    signal(new boost::signals2::signal<void()>()),
    h(),
    timer(io),
    maximum_times_to_run(maximum_running_times),
    executed_times(0),
    cancelled(false),
    cancel_connection()
{
}

task_scheduler::task_info::~task_info()
{
}

void task_scheduler::task_info::cancel()
{
	cancelled = true;
	timer.cancel();
}

}

