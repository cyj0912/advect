/*
 * Copyright (c) 2020 Toby Chen All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#pragma once
#include "App.h"
#include "Simulator.h"

// More like a buffer pump because the semantics are a little different from Andoird's
class BufferQueue
{
public:
    BufferQueue(App &aApp) : app(aApp) {}

    void kickoff(std::function<void(SimBuffer&)> fnInitBuffer, int bufferCount = 2);

    // Mutable side = producer
    void dequeueBufferCallback(std::function<void(std::shared_ptr<SimBuffer>)> callback);
    void queueBuffer(std::shared_ptr<SimBuffer> buffer);

    // Immutable side = consumer
    void acquireBufferCallback(std::function<void(std::shared_ptr<const SimBuffer>)> callback);
    void releaseBuffer(std::shared_ptr<const SimBuffer> buffer);

private:
    App &app;
    ExclusionGroup producerGrp, consumerGrp;
    std::function<void(std::shared_ptr<SimBuffer>)> dqcb;
    std::function<void(std::shared_ptr<SimBuffer>)> aqcb;
};
