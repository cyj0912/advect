/*
 * Copyright (c) 2020 Toby Chen All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#pragma once
#include <cmath>
#include <memory>
#include <mutex>
#include <vector>

#ifndef __CUDACC__
#define __host__
#define __device__
typedef struct cudaGraphicsResource *cudaGraphicsResource_t;
#endif

struct vec2
{
    float x;
    float y;

    __host__ __device__ vec2 &operator+=(const vec2 &other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    __host__ __device__ vec2 &operator-=(const vec2 &other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    __host__ __device__ vec2 &operator*=(float other)
    {
        x *= other;
        y *= other;
        return *this;
    }

    __host__ __device__ vec2 &operator/=(float other)
    {
        x /= other;
        y /= other;
        return *this;
    }

    __host__ __device__ float length() { return sqrtf(x * x + y * y); }

    __host__ __device__ float dot(const vec2 &other) { return x * other.x + y * other.y; }
};

__host__ __device__ inline vec2 operator+(const vec2 &lhs, const vec2 &rhs)
{
    vec2 result = lhs;
    return result += rhs;
}

__host__ __device__ inline vec2 operator-(const vec2 &lhs, const vec2 &rhs)
{
    vec2 result = lhs;
    return result -= rhs;
}

__host__ __device__ inline vec2 operator*(const vec2 &lhs, float rhs)
{
    vec2 result = lhs;
    return result *= rhs;
}

__host__ __device__ inline vec2 operator*(float rhs, const vec2 &lhs)
{
    vec2 result = lhs;
    return result *= rhs;
}

__host__ __device__ inline vec2 operator/(const vec2 &lhs, float rhs)
{
    vec2 result = lhs;
    return result /= rhs;
}

__host__ __device__ inline vec2 operator/(float rhs, const vec2 &lhs)
{
    vec2 result = lhs;
    return result /= rhs;
}

constexpr size_t particleCount = 10000;
constexpr size_t sqrtCount = 100;

constexpr float dt = 0.001f;
constexpr float mass = 1.0f;
constexpr float boxOrigin = -1.0f;
constexpr float boxSideLen = 2.0f;
constexpr uint32_t gridSideCount = 128;
constexpr float gridUnitLen = boxSideLen / gridSideCount;
constexpr float radius = 1.0f / 256.f * 5.0f;
constexpr float kCollision = 10.f;
constexpr float kDamp = 20.f;

extern std::mutex dxgiMutex;

constexpr float kSpring = 500.0f;
constexpr float kDamping = 10.0f;
constexpr float kShear = 1.0f;

__host__ __device__ inline vec2 collidePair(vec2 posA, vec2 posB, vec2 velA, vec2 velB, float radiusA, float radiusB,
                                            float attraction)
{
    vec2 relPos = posB - posA;
    float dist = relPos.length();
    float collideDist = radiusA + radiusB;
    vec2 force = {0.0f, 0.0f};
    if (dist == 0.0f)
        return force;
    if (dist < collideDist)
    {
        vec2 norm = relPos / dist;
        vec2 relVel = velB - velA;
        vec2 tanVel = relVel - (relVel.dot(norm) * norm);
        force = kSpring * -(collideDist - dist) * norm;
        force += kDamping * relVel;
        force += kShear * tanVel;
        force += attraction * relPos;
    }
    return force;
}

struct SimBuffer
{
    vec2 position[particleCount];
    // TODO: release this
    void *d3dBuffer;
    cudaGraphicsResource_t cuResource;
};

void simulationStep(vec2 *devNewPos, vec2 *devOldPos);

class BufferQueue;

class Simulator
{
public:
    Simulator(BufferQueue &bufferQ);

    Simulator(const Simulator &) = delete;
    Simulator &operator=(const Simulator &) = delete;

    ~Simulator();

    void onDequeueBuffer(std::shared_ptr<SimBuffer> newBuffer);

private:
    BufferQueue &bufferQueue;
    uint64_t currentFrame = 0;
    // Shared state
    std::shared_ptr<SimBuffer> oldBuffer;
    cudaGraphicsResource_t oldCuResource = nullptr;
    // State
    vec2 velocity[particleCount]{0};
    vec2 acceleration[particleCount]{0};

    // Temporary state
    std::vector<std::pair<uint32_t, uint32_t>> gridList;
    std::vector<int> cellStart;
};
