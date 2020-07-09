#pragma once
#include <memory>
#include <vector>

constexpr size_t particleCount = 1000000;

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
    float dt = 0.003f;
    std::shared_ptr<SimBuffer> oldBuffer;
    vec2 velocity[particleCount];
    vec2 acceleration[particleCount];
    float mass = 1.0f;

    float boxOrigin = -1.0f;
    float boxSideLen = 2.0f;
    uint32_t gridSideCount = 512;
    float gridUnitLen = boxSideLen / gridSideCount;
    float radius = 1.0f / 256.0f;
    float kCollision = 1.0f;

    std::vector<std::pair<uint32_t, uint32_t>> gridList;
    std::vector<int> cellStart;
};
