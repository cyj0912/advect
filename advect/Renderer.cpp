#include "Renderer.h"
#include "BufferQueue.h"

void Renderer::onAcquireBuffer(std::shared_ptr<const SimBuffer> buffer)
{
    printf("Begin renderer\n");
    printf("End renderer\n");
    bufferQueue.releaseBuffer(buffer);
}
