#include "Simulator.h"
#include <cuda_runtime.h>
#include <thrust/device_ptr.h>
#include <thrust/sort.h>

static vec2 *velocity;
static vec2 *acceleration;
static unsigned int *arrGridHash;
static unsigned int *arrGridParticle;
static unsigned int *gridFirstParticle;

__global__ void initPositions(vec2 *pos)
{
    int index = blockDim.x * blockIdx.x + threadIdx.x;
    if (index >= particleCount)
        return;
    int i = index / sqrtCount;
    int j = index - i * sqrtCount;
    pos[index] = {boxSideLen / sqrtCount * j + boxOrigin, boxSideLen / sqrtCount * i + boxOrigin};
}

__global__ void integrateOneParticle(vec2 *newPos, vec2 *oldPos, vec2 *v, vec2 *a)
{
    int index = blockDim.x * blockIdx.x + threadIdx.x;
    if (index >= particleCount)
        return;
    vec2 accel = a[index];
    accel.y -= 9.8;
    v[index] = v[index] + accel * dt;
    newPos[index] = oldPos[index] + v[index] * dt;

    float rdm = (index % 100) / 100.f * radius + radius;
    if (newPos[index].x > 1.0f)
        newPos[index].x = 1.0f - rdm;
    if (newPos[index].x < -1.0f)
        newPos[index].x = -1.0f + rdm;
    if (newPos[index].y > 1.0f)
        newPos[index].y = 1.0f - rdm;
    if (newPos[index].y < -1.0f)
        newPos[index].y = -1.0f + rdm;
}

__global__ void calcGridHash(unsigned int *gridHash, unsigned int *particleIndex, vec2 *newPos)
{
    int index = blockDim.x * blockIdx.x + threadIdx.x;
    if (index >= particleCount)
        return;
    int2 gridPos;
    gridPos.x = floor((newPos[index].x - boxOrigin) / gridUnitLen);
    gridPos.y = floor((newPos[index].y - boxOrigin) / gridUnitLen);
    gridPos.x = gridPos.x & (gridSideCount - 1);
    gridPos.y = gridPos.y & (gridSideCount - 1);
    unsigned int gh = gridPos.y * gridSideCount + gridPos.x;
    gridHash[index] = gh;
    particleIndex[index] = index;
}

__global__ void findStart(unsigned int *gridStart, unsigned int *gridHash, unsigned int *gridParticle)
{
    int index = blockDim.x * blockIdx.x + threadIdx.x;
    if (index >= particleCount)
        return;
    if (gridHash[index] != gridHash[index - 1])
        gridStart[gridHash[index]] = index;
    else if (index == 0)
        gridStart[gridHash[0]] = 0;
}

__global__ void doCollision(unsigned int *gridStart, unsigned int *gridHash, unsigned int *gridParticle, vec2 *p,
                            vec2 *v, vec2 *a)
{
    unsigned int index = blockDim.x * blockIdx.x + threadIdx.x;
    if (index >= particleCount)
        return;

    int2 gridPos;
    gridPos.x = floor((p[index].x - boxOrigin) / gridUnitLen);
    gridPos.y = floor((p[index].y - boxOrigin) / gridUnitLen);
    a[index] = {0.0f, 0.0f};
    for (int xOff = -1; xOff <= 1; xOff++)
        for (int yOff = -1; yOff < 1; yOff++)
        {
            int x2 = gridPos.x + xOff;
            int y2 = gridPos.y + yOff;
            x2 = x2 & (gridSideCount - 1);
            y2 = y2 & (gridSideCount - 1);
            unsigned int gh = gridPos.y * gridSideCount + gridPos.x;
            int j = gridStart[gh];
            for (; j < particleCount && gridHash[j] == gh; j++)
            {
                unsigned int jj = gridParticle[j];
                if (jj == index)
                    continue;
                auto force = collidePair(p[index], p[jj], v[index], v[jj], radius, radius, 0.4f);
                a[index] += force / mass;
            }
        }
}

void setup()
{
    cudaMallocManaged(&velocity, particleCount * sizeof(vec2));
    cudaMallocManaged(&acceleration, particleCount * sizeof(vec2));
    cudaMallocManaged(&arrGridHash, particleCount * sizeof(unsigned int));
    cudaMallocManaged(&arrGridParticle, particleCount * sizeof(unsigned int));
    cudaMallocManaged(&gridFirstParticle, gridSideCount * gridSideCount * sizeof(unsigned int));
}

void teardown()
{
    cudaFree(velocity);
    cudaFree(acceleration);
    cudaFree(arrGridHash);
    cudaFree(arrGridParticle);
    cudaFree(gridFirstParticle);
}

void simulationStep(vec2 *devNewPos, vec2 *devOldPos)
{
    int blockSz = 1024;
    int blkCnt = (particleCount + blockSz - 1) / blockSz;
    thrust::device_ptr<unsigned int> devGridHash = thrust::device_pointer_cast(arrGridHash);
    thrust::device_ptr<unsigned int> devGridParticle = thrust::device_pointer_cast(arrGridParticle);
    if (!devOldPos)
    {
        setup();
        initPositions<<<blkCnt, blockSz>>>(devNewPos);
        // cudaMemset(devNewPos, 0, particleCount * sizeof(vec2));
    }
    else
    {
        integrateOneParticle<<<blkCnt, blockSz>>>(devNewPos, devOldPos, velocity, acceleration);
        calcGridHash<<<blkCnt, blockSz>>>(arrGridHash, arrGridParticle, devNewPos);
        thrust::sort_by_key(devGridHash, devGridHash + particleCount, devGridParticle);
        findStart<<<blkCnt, blockSz>>>(gridFirstParticle, arrGridHash, arrGridParticle);
        doCollision<<<blkCnt, blockSz>>>(gridFirstParticle, arrGridHash, arrGridParticle, devNewPos, velocity,
                                         acceleration);
    }
}
