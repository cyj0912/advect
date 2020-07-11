/*
 * Copyright (c) 2020 Toby Chen All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#pragma once
#include "Profiler.h"
#include <atomic>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <thread>

extern Profiler globalProfiler;

class IWorkItem
{
public:
    virtual ~IWorkItem() = default;
    virtual void doWork() = 0;
};

class ExclusionGroup
{
    friend class App;
    std::atomic_int atomicCounter{0};
};

// An App with a simple but not naive scheduler
class App
{
public:
    int run();

    // trait Scheduler {
    template <typename T> void schedule(T &&fn)
    {
        std::unique_lock<std::mutex> lk(workQueueMutex);
        readyQueue.emplace_back(std::make_shared<LambdaWork<T>>(std::forward<T>(fn)));
        workQueueWaitList.notify_one();
    }

    template <typename T> void scheduleExclusive(T &&fn, std::shared_ptr<ExclusionGroup> group)
    {
        std::unique_lock<std::mutex> lk(workQueueMutex);
        exclusiveQueue.emplace_back(std::make_shared<LambdaExclusiveWork<T>>(std::forward<T>(fn), std::move(group)));
        workQueueWaitList.notify_one();
    }

    template <typename T> void scheduleExclusive(T &&fn, ExclusionGroup& group)
    {
        std::unique_lock<std::mutex> lk(workQueueMutex);
        exclusiveQueue.emplace_back(std::make_shared<LambdaExclusiveWorkAlt<T>>(std::forward<T>(fn), group));
        workQueueWaitList.notify_one();
    }

    // scheduleAfter(T&& fn, ???);

    void notifyQuit() { quitting = true; }
    // }

protected:
    virtual bool setup() { return true; }

private:
    template <typename T> class LambdaWork : public IWorkItem
    {
    public:
        LambdaWork(T &&arg) : fn(std::forward<T>(arg)) {}

        void doWork() { fn(); }

    private:
        T fn;
    };

    class IExclusiveWork : public IWorkItem
    {
    public:
        virtual ExclusionGroup &getGroup() const = 0;
    };

    template <typename T> class LambdaExclusiveWork : public IExclusiveWork
    {
    public:
        LambdaExclusiveWork(T &&arg, std::shared_ptr<ExclusionGroup> aGroup)
            : fn(std::forward<T>(arg)), group(std::move(aGroup))
        {
        }

        void doWork()
        {
            fn();
            assert(getGroup().atomicCounter == 1);
            getGroup().atomicCounter -= 1;
        }

        ExclusionGroup &getGroup() const override { return *group; }

    private:
        T fn;
        std::shared_ptr<ExclusionGroup> group;
    };

    template <typename T> class LambdaExclusiveWorkAlt : public IExclusiveWork
    {
    public:
        LambdaExclusiveWorkAlt(T &&arg, ExclusionGroup& aGroup)
            : fn(std::forward<T>(arg)), group(aGroup)
        {
        }

        void doWork()
        {
            fn();
            assert(getGroup().atomicCounter == 1);
            getGroup().atomicCounter -= 1;
        }

        ExclusionGroup &getGroup() const override { return group; }

    private:
        T fn;
        ExclusionGroup& group;
    };

    void workerThreadLoop(int workerId);

    std::mutex workQueueMutex;
    std::condition_variable workQueueWaitList;
    std::deque<std::shared_ptr<IWorkItem>> readyQueue;
    std::list<std::shared_ptr<IExclusiveWork>> exclusiveQueue;
    bool quitting = false;
};
