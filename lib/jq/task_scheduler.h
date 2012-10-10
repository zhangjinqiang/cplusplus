#ifndef JQ_TASK_SCHEDULER_H
#define JQ_TASK_SCHEDULER_H

#include <boost/asio.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/signals2.hpp>
#include <boost/bimap.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/function.hpp>
#include <map>

namespace jq
{
	class task_scheduler
	{
	public:
		class handle
		{
		public:
			handle();
			handle(const handle&);
			handle& operator=(const handle& rhs);

			bool valid() const;
			void wait();
			void cancel();

			friend class task_scheduler;

		private:
			typedef boost::signals2::signal<void()> sigtype;
			handle(task_scheduler&,
				unsigned int id,
				const boost::shared_ptr<sigtype>&);
			void notified();
			void error_ocurred(const std::string&){}

			task_scheduler *m_scheduler;
			bool m_valid;
			boost::weak_ptr<sigtype> m_signal;
			unsigned int m_id;
			boost::shared_ptr<boost::interprocess::interprocess_semaphore> m_semaphore;
			boost::signals2::scoped_connection m_connection;
		};

		task_scheduler(boost::asio::io_service& io);
		virtual ~task_scheduler();

		template<typename T>
		handle schedule(const std::string& name,
				double frequency,
				const boost::function<T()>& task,
				unsigned int maximum_times = 0)
		{
			return schedule(name, frequency, task, maximum_times);
		}

		typedef boost::function<void()> void_task;
		typedef boost::function<bool()> bool_task;
		handle schedule(const std::string& name,
		  	        double frequency,
			        const void_task& task,
			        unsigned int maximum_times = 0);
		handle schedule(const std::string& name,
			        double frequency,
			        const bool_task& task,
			        unsigned int maximum_times = 0);

		bool exists(const std::string& name) const;

		//! this function gurantees after the return, the task won't execute any more
		void cancel(const std::string& name);

		friend class handler;

	private:
		struct task_info
		{
			task_info(boost::asio::io_service& io,
				  const bool_task& task,
				  double frequency,
				  unsigned long maximum_running_times);
			~task_info();

			bool_task functor;
			unsigned long interval;//! in milliseconds
			boost::shared_ptr<boost::signals2::signal<void()> > signal;
			boost::shared_ptr<handle> h;
			boost::asio::deadline_timer timer;
			unsigned long maximum_times_to_run;
			unsigned long executed_times;
		};

		void register_next_run(task_info&, unsigned long to_wait);
		void execute(const boost::system::error_code&, task_info&);
		void cancel(unsigned int id);
		boost::shared_ptr<handle> cancel_impl(unsigned int id);
		void task_finished(unsigned int id);
		void remove_record(unsigned int id);

		mutable boost::mutex m_mutex;
		boost::asio::io_service& m_io;
		unsigned int m_latestid;
		typedef std::map<unsigned int, boost::shared_ptr<task_info> > task_map;
		task_map m_tasks;
		typedef boost::bimap<unsigned int, std::string> id_name_bm_type;
		id_name_bm_type m_id_names;
	};
}

#endif

