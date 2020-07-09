#include "App.h"
#include "BufferQueue.h"
#include "Renderer.h"
#include "Simulator.h"

class AdvectApp : public App
{
    void test()
    {
        using namespace std::chrono_literals;
        auto grp = std::make_shared<ExclusionGroup>();
        auto grp2 = std::make_shared<ExclusionGroup>();
        schedule([]() { printf("Hello world\n"); });
        scheduleExclusive(
            []() {
                printf("Task A\n");
                std::this_thread::sleep_for(1s);
            },
            grp);
        scheduleExclusive(
            []() {
                printf("Task B\n");
                std::this_thread::sleep_for(1s);
            },
            grp);
        scheduleExclusive(
            []() {
                printf("Task C\n");
                std::this_thread::sleep_for(1s);
            },
            grp);
        scheduleExclusive(
            []() {
                printf("Task D\n");
                std::this_thread::sleep_for(1s);
            },
            grp);
        scheduleExclusive(
            []() {
                printf("Task E\n");
                std::this_thread::sleep_for(1s);
            },
            grp);
        scheduleExclusive(
            []() {
                printf("Task 1\n");
                std::this_thread::sleep_for(1s);
            },
            grp2);
        scheduleExclusive(
            []() {
                printf("Task 2\n");
                std::this_thread::sleep_for(1s);
            },
            grp2);
    }

    bool setup() override
    {
        bufferQueue.dequeueBufferCallback([this](auto buffer) { simulator.onDequeueBuffer(buffer); });
        bufferQueue.acquireBufferCallback([this](auto buffer) { renderer.onAcquireBuffer(buffer); });
        bufferQueue.kickoff(2);
        return true;
    }

    BufferQueue bufferQueue{*this};
    Simulator simulator{bufferQueue};
    Renderer renderer{*this, bufferQueue};
};

int main()
{
    auto app = std::make_unique<AdvectApp>();
    return app->run();
}
