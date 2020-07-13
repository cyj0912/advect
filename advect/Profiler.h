#pragma once
#include <chrono>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

enum class ProfilerColor
{
    DefaultBlue = 0,
    Blue,
    Green,
    Red
};

class ProfilerThread
{
public:
    ProfilerThread(const std::string &name);

    ProfilerThread(const ProfilerThread &) = delete;
    ProfilerThread &operator=(const ProfilerThread &) = delete;

    void beginEvent(const std::string &sym, ProfilerColor color);
    void endEvent(const std::string &sym);

private:
    friend class Profiler;

    struct EventData
    {
        // TODO: use AVL tree for name storage
        std::string sym;
        uint64_t timeStamp;
        uint32_t flags : 1; // bit 0 == 1 ? close : open
        uint32_t color : 31;
    };

    // Get the nanoseconds since program began
    static uint64_t getNanoSeconds();

    std::string name;
    // We assume a single thread writes this so we don't lock it
    std::vector<EventData> ringBuffer;
    size_t ringNext = 0;
    bool fullCycle = false;
};

class Profiler
{
public:
    std::shared_ptr<ProfilerThread> makeThreadData(const std::string &name);

    void drawImGui();

    static const std::chrono::high_resolution_clock::time_point start;

private:
    std::mutex dataMutex;
    std::vector<std::shared_ptr<ProfilerThread>> threads;
    bool showImGui = true;
    int rangeMs = 1000;
    uint64_t lastRightNs = 0;
    float refreshEvery = 1.0f;
};
