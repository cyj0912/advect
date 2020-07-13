// Copyright (c) 2020 Toby Chen All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "App.h"
#include <iostream>

Profiler globalProfiler;

int App::run()
{
    if (!setup())
        return -1;
    // So conceptually, we are gonna have 1 or 2 worker threads. Two tasks, simulator and renderer, each has an input
    // queue associated. Both queues combined is call a BufferQueue
    std::vector<std::thread> workers;
    for (int i = 1; i < 1; i++)
    {
        workers.push_back(std::thread([this, i]() { workerThreadLoop(i); }));
    }
    workerThreadLoop(0);
    for (auto &thread : workers)
        thread.join();
    size_t outstanding = 0;
    outstanding += readyQueue.size();
    outstanding += exclusiveQueue.size();
    printf("Exiting with %llu outstanding jobs\n", outstanding);
    return 0;
}

void App::workerThreadLoop(int workerId)
{
    using namespace std::chrono_literals;
    std::string idStr = std::to_string(workerId);
    auto profThread = globalProfiler.makeThreadData("worker" + idStr);
    while (true)
    {
        std::unique_lock<std::mutex> lk(workQueueMutex);
        std::shared_ptr<IWorkItem> workItem;
        std::string taskSym = "w";
        do
        {
            if (quitting)
                return;
            for (auto iter = exclusiveQueue.begin(); iter != exclusiveQueue.end(); ++iter)
            {
                auto &workPtr = *iter;
                if (workPtr->getGroup().atomicCounter == 0)
                {
                    workPtr->getGroup().atomicCounter += 1;
                    readyQueue.emplace_front(std::move(workPtr));
                    exclusiveQueue.erase(iter);
                    break;
                }
            }
            if (!readyQueue.empty())
            {
                workItem = std::move(readyQueue.front());
                readyQueue.pop_front();
                break;
            }
            // Invariant: only reach here when no useful work can be done
            workQueueWaitList.wait_for(lk, 10ms);
            if (quitting)
                return;
        } while (true);
        lk.unlock();

        profThread->beginEvent(taskSym, ProfilerColor::DefaultBlue);
        workItem->doWork();
        profThread->endEvent(taskSym);
    }
}
