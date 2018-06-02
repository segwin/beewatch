//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#include "util/logging.h"
#include "util/priority.h"

#ifdef __linux__
#include <errno.h>
#include <pthread.h>
#include <sys/resource.h>
#elif WIN32
#include <windows.h>
#endif

#include <cstring>
#include <iostream>
#include <sstream>

namespace beewatch
{
    namespace util
    {

        //================================================================
        UTIL_API void setPriority(Priority priority)
        {
#ifdef __linux__
            int procPriority;
            int schedPolicy;
            struct sched_param schedParams;

            switch (priority)
            {
            case Priority::REALTIME:
                procPriority = -20;
                schedPolicy = SCHED_FIFO;
                schedParams.sched_priority = sched_get_priority_max(SCHED_FIFO);
                break;

            default:
            //case Priority::NORMAL:
                procPriority = 0;
                schedPolicy = SCHED_OTHER;
                schedParams.sched_priority = 0;
                break;
            }

            if (!setpriority(PRIO_PROCESS, 0, procPriority) ||
                !pthread_setschedparam(pthread_self(), schedPolicy, &schedParams))
            {
                logger.dualPrint(Logger::Error,
                                 std::string("Unable to set requested priority: ") + strerror(errno));
            }
#elif WIN32
            DWORD priorityClass;
            int threadPriority;

            switch (priority)
            {
            case Priority::REALTIME:
                priorityClass = REALTIME_PRIORITY_CLASS;
                threadPriority = THREAD_PRIORITY_TIME_CRITICAL;
                break;

            default:
            //case Priority::NORMAL:
                priorityClass = NORMAL_PRIORITY_CLASS;
                threadPriority = THREAD_PRIORITY_NORMAL;
                break;
            }

            if (!SetPriorityClass(GetCurrentProcess(), priorityClass) ||
                !SetThreadPriority(GetCurrentThread(), threadPriority))
            {
                std::ostringstream oss;

                oss << "Unable to set requested priority ("
                    << std::hex << GetLastError()
                    << ")" << std::endl;
                
                logger.dualPrint(Logger::Error, oss.str());
            }
#endif
        }

    } // namespace util
} // namespace beewatch
