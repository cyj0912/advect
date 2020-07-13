// Copyright (c) 2020 Toby Chen All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "Simulator.h"
#include "BufferQueue.h"
#include <algorithm>
#include <cstring>

Simulator::Simulator(BufferQueue &bufferQ) : bufferQueue(bufferQ)
{
}

Simulator::~Simulator()
{
}

void Simulator::onDequeueBuffer(std::shared_ptr<SimBuffer> newBuffer)
{
    // This would have been nicer:
    // while (true)
    // {
    //     await BufferQueue->dequeueBufferAsync()
    //     doSimulation()
    //     BufferQueue->queueBuffer()
    // }
    // Of course, C++ does not support await so this is implemented as a callback

    printf("Begin simulation frame %lld\n", currentFrame);
    if (currentFrame == 0)
    {
        gridList.resize(particleCount);
        for (int i = 0; i < sqrtCount; i++)
            for (int j = 0; j < sqrtCount; j++)
                newBuffer->position[i * sqrtCount + j] = {boxSideLen / sqrtCount * j + boxOrigin,
                                                          boxSideLen / sqrtCount * i + boxOrigin};
        memset(velocity, 0, sizeof(velocity));
    }
    else
    {
        for (int i = 0; i < particleCount; i++)
        {
            acceleration[i] += vec2{0.0f, -9.8f};
            vec2 &newPos = newBuffer->position[i];
            newPos = oldBuffer->position[i] + velocity[i] * dt + 0.5f * acceleration[1] * dt * dt;
            velocity[i] = velocity[i] + acceleration[1] * dt;

            // Check outside boundary
            if (newPos.x > 1.0f)
                newPos.x = 1.0f;
            if (newPos.x < -1.0f)
                newPos.x = -1.0f;
            if (newPos.y > 1.0f)
                newPos.y = 1.0f;
            if (newPos.y < -1.0f)
                newPos.y = -1.0f;
        }

        // Collision, where N^2 algorithm is just infeasible
        // First, classify particle into uniform grid
        int gridListNext = 0;
        for (int i = 0; i < particleCount; i++)
        {
#define EPS 0.00001f
            vec2 pos = newBuffer->position[i];
            int xIdx = (int)((pos.x - boxOrigin - EPS) / gridUnitLen);
            int yIdx = (int)((pos.y - boxOrigin - EPS) / gridUnitLen);
            int gridIdx = xIdx + gridSideCount * yIdx + gridSideCount * gridSideCount * 0;
            assert(gridIdx < gridSideCount * gridSideCount);
            gridList[gridListNext] = std::make_pair(gridIdx, i);
            gridListNext += 1;
        }
        std::sort(gridList.begin(), gridList.end());
        cellStart.assign(gridSideCount * gridSideCount, 0);
        cellStart[gridList[0].first] = 0;
        for (int i = 1; i < particleCount; i++)
            if (gridList[i].first != gridList[i - 1].first)
                cellStart[gridList[i].first] = i;

        float perfAvg = 0.0f;
        float colAvg = 0.0f;
        for (int i = 0; i < particleCount; i++)
        {
            acceleration[i] = vec2{0.0f, 0.0f};
            int perfCount = 0;
            int colCount = 0;
            vec2 pos = newBuffer->position[i];
            int xIdx = (int)((pos.x - boxOrigin - EPS) / gridUnitLen);
            int yIdx = (int)((pos.y - boxOrigin - EPS) / gridUnitLen);
            for (int xOff = -1; xOff <= 1; xOff++)
                for (int yOff = -1; yOff < 1; yOff++)
                {
                    uint32_t x2 = xIdx + xOff;
                    uint32_t y2 = yIdx + yOff;
                    if (x2 < 0 || x2 >= gridSideCount)
                        continue;
                    if (y2 < 0 || y2 >= gridSideCount)
                        continue;
                    int neiIdx = x2 + gridSideCount * y2 + gridSideCount * gridSideCount * 0;
                    for (int j = cellStart[neiIdx]; j < particleCount && gridList[j].first == neiIdx; j++)
                    {
                        perfCount += 1;
                        int jj = gridList[j].second;
                        if (jj == i)
                            continue;
                        vec2 atMe = pos - newBuffer->position[jj];
                        float dist = atMe.length();
                        if (dist == 0.0f)
                            continue;
                        if (dist < radius * 2.0f)
                        {
                            colCount++;
                            acceleration[i] += kCollision * atMe / dist / dist;
                        }
                    }
                }
            // Apply damping
            acceleration[i] -= (velocity[i] + acceleration[i] * dt) * kDamp;
            perfAvg += perfCount / (float)particleCount;
            colAvg += colCount / (float)particleCount;
        }
        printf("%f %f\n", perfAvg, colAvg);
    }
    oldBuffer = newBuffer;
    printf("End simulation frame %lld\n", currentFrame);
    currentFrame += 1;
    bufferQueue.queueBuffer(newBuffer);
}
