//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#include "util/priority.h"

#include "util/logging.h"

#include <errno.h>
#include <pthread.h>
#include <sys/resource.h>

#include <algorithm>
#include <cstring>
#include <map>

namespace beewatch
{

    //================================================================
    PriorityGuard::PriorityGuard(Priority lvl)
    {
        _prevLvl = getPriority();
        setPriority(lvl);
    }

    PriorityGuard::~PriorityGuard()
    {
        setPriority(_prevLvl);
    }

    
    //================================================================
    const std::map<Priority, int> lvlToNice =
    {
        std::make_pair(Priority::Normal, 0),
        std::make_pair(Priority::RealTime, -20),
        std::make_pair(Priority::Background, 19)
    };

    const std::map<Priority, int> lvlToSchedPolicy =
    {
        std::make_pair(Priority::Normal, SCHED_OTHER),
        std::make_pair(Priority::RealTime, SCHED_FIFO),
        std::make_pair(Priority::Background, SCHED_OTHER)
    };

    const std::map<Priority, int> lvlToSchedPriority =
    {
        std::make_pair(Priority::Normal, 0),
        std::make_pair(Priority::RealTime, sched_get_priority_max(SCHED_FIFO)),
        std::make_pair(Priority::Background, sched_get_priority_min(SCHED_OTHER)),
    };

    void PriorityGuard::setPriority(Priority lvl)
    {
        int nice = lvlToNice.at(lvl);
        int schedPolicy = lvlToSchedPolicy.at(lvl);

        struct sched_param schedParams;
        schedParams.sched_priority = lvlToSchedPolicy.at(lvl);

        if ( setpriority(PRIO_PROCESS, 0, nice) < 0 ||
             pthread_setschedparam(pthread_self(), schedPolicy, &schedParams) < 0 )
        {
            logger.print(Logger::Error,
                         std::string("Unable to set requested priority: ") + strerror(errno));
        }
    }

    Priority PriorityGuard::getPriority()
    {
        int nice = ::getpriority(PRIO_PROCESS, 0);

        auto itProcPriority = std::find_if( lvlToNice.begin(), lvlToNice.end(),
                                            [nice](std::pair<Priority, int> p) { return p.second == nice; } );

        return itProcPriority->first;
    }

} // namespace beewatch
