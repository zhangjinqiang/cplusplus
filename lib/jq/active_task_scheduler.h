#ifndef JQ_ACTIVE_TASK_SCHEDULER_H
#define JQ_ACTIVE_TASK_SCHEDULER_H

#include "task_scheduler.h"
#include "io_service_pool.h"

namespace jq
{
	class active_task_scheduler :	public io_service_pool,
					public task_scheduler
	{
	public:
		active_task_scheduler(unsigned int poolsize = 1);
		virtual ~active_task_scheduler();
	};
}

#endif

