#include "Profiler.h"
#include <imgui.h>

const std::chrono::high_resolution_clock::time_point Profiler::start = std::chrono::high_resolution_clock::now();

ProfilerThread::ProfilerThread(const std::string &name) : name(name) { ringBuffer.resize(1024); }

void ProfilerThread::beginEvent(const std::string &sym, ProfilerColor color)
{
    ringBuffer[ringNext] = {sym, getNanoSeconds(), 0, (uint32_t)color};
    ringNext += 1;
    ringNext %= ringBuffer.size();
    if (ringNext == 0)
        fullCycle = true;
}

void ProfilerThread::endEvent(const std::string &sym)
{
    ringBuffer[ringNext] = {sym, getNanoSeconds(), 1, 0};
    ringNext += 1;
    ringNext %= ringBuffer.size();
    if (ringNext == 0)
        fullCycle = true;
}

uint64_t ProfilerThread::getNanoSeconds()
{
    using namespace std::chrono;
    // Get the duration since epoch and convert it into nanoseconds
    auto end = high_resolution_clock::now();
    auto nano = nanoseconds(end - Profiler::start);
    uint64_t ns = nano.count();
    return ns;
}

std::shared_ptr<ProfilerThread> Profiler::makeThreadData(const std::string &name)
{
    std::unique_lock<std::mutex> lk(dataMutex);
    auto data = std::make_shared<ProfilerThread>(name);
    threads.push_back(data);
    return data;
}

void Profiler::drawImGui()
{
    std::unique_lock<std::mutex> lk(dataMutex);

    if (ImGui::Begin("Task Profiler", &showImGui))
    {
        ImGui::DragInt("Range MS", &rangeMs);

        // Time scale: 1 ms = 1000 us = 1e6 ns
        // 50 ms = 5e7 ns
        uint64_t intervalMs = rangeMs;
        uint64_t rightNs = ProfilerThread::getNanoSeconds();
        if (intervalMs * 1000000 > rightNs)
            intervalMs = rightNs / 1000000;
        uint64_t leftNs = rightNs - intervalMs * 1000000;

        if (ImGui::BeginChild(ImGui::GetID(this), ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysAutoResize))
        {
            float offset = 100.f;

            ImDrawList *drawList = ImGui::GetWindowDrawList();
            ImGui::AlignTextToFramePadding();
            ImGui::Text("");
            ImGui::SameLine(offset);

            // Get window width to calculate right area width
            float rightWidth = ImGui::GetContentRegionAvailWidth();
            float pxPerMs = rightWidth / intervalMs;

            // Draw ticks every 10ms
            int64_t tickMs = rightNs / 1000000;
            int64_t endMs = tickMs - intervalMs;
            for (; tickMs >= endMs; tickMs -= 100)
            {
                auto nsFromLeft = tickMs * 1000000 - leftNs;
                float msFromLeft = nsFromLeft / 1000000.f;
                float pxFromLeft = msFromLeft * pxPerMs;
                auto p0 = ImGui::GetCursorScreenPos();
                p0.x += pxFromLeft;
                auto p1 = p0;
                p1.y += ImGui::GetFrameHeight();
                drawList->AddLine(p0, p1, ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)), 1.f);
            }
            ImGui::NewLine();

            for (const auto &threadDataPtr : threads)
            {
                const auto &thread = *threadDataPtr;
                ImGui::AlignTextToFramePadding();
                ImGui::Text(thread.name.c_str());
                // Loop over events
                float begin = 0.0f;
                for (size_t i = 0; i < thread.ringNext; i++)
                {
                    auto eventNs = thread.ringBuffer[i].timeStamp;
                    if (eventNs > leftNs && eventNs < rightNs)
                    {
                        auto nsFromLeft = eventNs - leftNs;
                        float msFromLeft = nsFromLeft / 1000000.f;
                        float pxFromLeft = msFromLeft * pxPerMs;
                        if (thread.ringBuffer[i].flags)
                        {
                            ImGui::SameLine(offset + begin);
                            ImGui::Button("...", ImVec2(pxFromLeft - begin, 0));
                        }
                        else
                        {
                            begin = pxFromLeft;
                        }
                    }
                }
            }
        }
        ImGui::EndChild();
    }
    ImGui::End();
}
