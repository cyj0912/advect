// Copyright (c) 2020 Toby Chen All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "Renderer.h"
#include "BufferQueue.h"

void Renderer::onAcquireBuffer(std::shared_ptr<const SimBuffer> buffer)
{
    printf("Begin renderer\n");
    printf("End renderer\n");
    bufferQueue.releaseBuffer(buffer);
}
