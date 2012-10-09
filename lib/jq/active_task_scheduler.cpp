#include "active_task_scheduler.h"

namespace jq {

active_task_scheduler::active_task_scheduler(unsigned int poolsize)
 :  io_service_pool(poolsize),
    task_scheduler(io_service_pool::get_io_service())
{
	io_service_pool::start();
}

active_task_scheduler::~active_task_scheduler()
{
	io_service_pool::stop();
}

}

