#include "BufferQueue.h"

void BufferQueue::kickoff(int bufferCount)
{
    for (int i = 0; i < bufferCount; i++)
    {
        auto buffer = std::make_shared<SimBuffer>();
        releaseBuffer(buffer);
    }
}

void BufferQueue::dequeueBufferCallback(std::function<void(std::shared_ptr<SimBuffer>)> callback)
{
    dqcb = std::move(callback);
}

void BufferQueue::queueBuffer(std::shared_ptr<SimBuffer> buffer)
{
    app.scheduleExclusive([this, buffer]() { aqcb(buffer); }, consumerGrp);
}

void BufferQueue::acquireBufferCallback(std::function<void(std::shared_ptr<const SimBuffer>)> callback)
{
    aqcb = std::move(callback);
}

void BufferQueue::releaseBuffer(std::shared_ptr<const SimBuffer> buffer)
{
    app.scheduleExclusive([this, buffer]() { dqcb(std::const_pointer_cast<SimBuffer>(buffer)); }, producerGrp);
}
