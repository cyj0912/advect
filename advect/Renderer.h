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
    }

    Renderer(const Renderer &) = delete;
    Renderer &operator=(const Renderer &) = delete;

    ~Renderer() { renderThread.join(); }

    void onAcquireBuffer(std::shared_ptr<const SimBuffer> buffer);

private:
    int renderThreadLoop();

    App &app;
    BufferQueue &bufferQueue;
    std::thread renderThread;
};
