/*
 * Copyright (c) 2020 Toby Chen All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#pragma once
#include <memory>
#include <vector>

struct vec2
{
    float x;
    float y;

    vec2 &operator+=(const vec2 &other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    vec2 &operator-=(const vec2 &other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    vec2 &operator*=(float other)
    {
        x *= other;
        y *= other;
        return *this;
    }

    vec2 &operator/=(float other)
    {
        x /= other;
        y /= other;
        return *this;
    }

    float length() { return sqrtf(x * x + y * y); }
};

inline vec2 operator+(const vec2 &lhs, const vec2 &rhs)
{
    vec2 result = lhs;
    return result += rhs;
}

inline vec2 operator-(const vec2 &lhs, const vec2 &rhs)
{
    vec2 result = lhs;
    return result -= rhs;
}

inline vec2 operator*(const vec2 &lhs, float rhs)
{
    vec2 result = lhs;
    return result *= rhs;
}

inline vec2 operator*(float rhs, const vec2 &lhs)
{
    vec2 result = lhs;
    return result *= rhs;
}

inline vec2 operator/(const vec2 &lhs, float rhs)
{
    vec2 result = lhs;
    return result /= rhs;
}

inline vec2 operator/(float rhs, const vec2 &lhs)
{
    vec2 result = lhs;
    return result /= rhs;
}

constexpr size_t particleCount = 10000;
constexpr size_t sqrtCount = 100;

struct SimBuffer
{
    vec2 position[particleCount];
};

class BufferQueue;

class Simulator
{
public:
    Simulator(BufferQueue &bufferQ) : bufferQueue(bufferQ) {}

    void onDequeueBuffer(std::shared_ptr<SimBuffer> newBuffer);

private:
    BufferQueue &bufferQueue;
    uint64_t currentFrame = 0;
    float dt = 0.001f;
    std::shared_ptr<SimBuffer> oldBuffer;
    vec2 velocity[particleCount];
    vec2 acceleration[particleCount];
    float mass = 1.0f;

    float boxOrigin = -1.0f;
    float boxSideLen = 2.0f;
    uint32_t gridSideCount = 128;
    float gridUnitLen = boxSideLen / gridSideCount;
    float radius = 1.0f / 256.f;
    float kCollision = 10.f;
    float kDamp = 20.f;

    std::vector<std::pair<uint32_t, uint32_t>> gridList;
    std::vector<int> cellStart;
};
