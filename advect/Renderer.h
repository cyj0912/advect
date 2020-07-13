/*
 * Copyright (c) 2020 Toby Chen All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#pragma once
#include "Simulator.h"
#include <thread>

class App;

class Renderer
{
public:
    Renderer(App &app, BufferQueue &bufferQ) : app(app), bufferQueue(bufferQ)
    {
        renderThread = std::thread(&Renderer::renderThreadLoop, this);
        while (!spinOnMe)
            ;
    }

    Renderer(const Renderer &) = delete;
    Renderer &operator=(const Renderer &) = delete;

    ~Renderer() { renderThread.join(); }

    void initBuffer(SimBuffer &buffer);
    void startRenderer() { spinner2 = true; }

    void onAcquireBuffer(std::shared_ptr<const SimBuffer> buffer);

private:
    int renderThreadLoop();

    App &app;
    BufferQueue &bufferQueue;
    std::thread renderThread;
    bool spinOnMe = false;
    bool spinner2 = false;
};
