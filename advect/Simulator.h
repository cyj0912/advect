/*
 * Copyright (c) 2020 Toby Chen All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#pragma once
#include <memory>
#include <vector>
#include <cmath>

#ifndef __CUDACC__
#define __host__
#define __device__
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
constexpr float radius = 1.0f / 256.f;
constexpr float kCollision = 10.f;
constexpr float kDamp = 20.f;

struct SimBuffer
{
    vec2 position[particleCount];
    vec2* cuPosition;
};

class BufferQueue;

class Simulator
{
public:
    Simulator(BufferQueue &bufferQ);

    Simulator(const Simulator&) = delete;
    Simulator& operator=(const Simulator&) = delete;

    ~Simulator();

    void onDequeueBuffer(std::shared_ptr<SimBuffer> newBuffer);

private:
    BufferQueue &bufferQueue;
    uint64_t currentFrame = 0;
    // Shared state
    std::shared_ptr<SimBuffer> oldBuffer;
    // State
    vec2 velocity[particleCount];
    vec2 acceleration[particleCount];

    // Temporary state
    std::vector<std::pair<uint32_t, uint32_t>> gridList;
    std::vector<int> cellStart;
};

