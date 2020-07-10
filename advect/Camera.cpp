#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// l, r, b, t are defined in terms of the near plane
static glm::mat4 MakeProjectionInternal(float l, float r, float b, float t, float n, float f)
{
    glm::mat4 m;
    m[0][0] = 2 * n / (r - l);
    m[1][0] = 0;
    m[2][0] = (r + l) / (r - l);
    m[3][0] = 0;
    m[0][1] = 0;
    m[1][1] = 2 * n / (t - b);
    m[2][1] = (t + b) / (t - b);
    m[3][1] = 0;
    m[0][2] = 0;
    m[1][2] = 0;
    m[2][2] = -(f + n) / (f - n);
    m[3][2] = -2 * f * n / (f - n);
    m[0][3] = 0;
    m[1][3] = 0;
    m[2][3] = -1.0f;
    m[3][3] = 0;
    return m;
}

glm::mat4 Camera::MakeProjection(float fovY, float aspect, float n, float f)
{
    float t = tanf(fovY / 2.0f) * n;
    float b = -t;
    return MakeProjectionInternal(b * aspect, t * aspect, b, t, n, f);
}

glm::mat4 Camera::MakeViewMatrix() const
{
    auto translate = glm::translate(glm::identity<glm::mat4>(), -Position);
    // Make an orthonormal basis where -z points at the pivot, then transpose it
    auto z = glm::normalize(Position - Pivot);
    auto x = glm::normalize(glm::cross(Up, z));
    auto y = glm::cross(z, x);
    auto rotT = glm::mat3{x, y, z};
    auto rot = glm::mat4{glm::transpose(rotT)};
    return rot * translate;
}

const glm::mat4 &Camera::GetViewMatrix() const
{
    if (bViewMatrixDirty)
    {
        CachedViewMatrix = MakeViewMatrix();
        bViewMatrixDirty = false;
    }
    return CachedViewMatrix;
}

void Camera::Orbit(Camera::EDir dir, float angle)
{
    bViewMatrixDirty = true;
    auto arrow = Position - Pivot;
    glm::vec3 axis;
    if (dir == EDir::Left || dir == EDir::Up)
        angle = -angle;
    if (dir == EDir::Left || dir == EDir::Right)
        axis = Up;
    else
    {
        axis = glm::normalize(glm::cross(Up, arrow));
        // Prevent from going over +-90 degrees
        auto a = acosf(glm::dot(glm::normalize(Up), glm::normalize(arrow)));
        a += angle;
        if (a <= 0.0f || a >= M_PI)
            return;
    }
    auto mat = glm::rotate(glm::identity<glm::mat4>(), angle, axis);
    auto newArrow = mat * glm::vec4(arrow, 1.0f);
    Position = Pivot + glm::vec3(newArrow);
}

void Camera::Turn(Camera::EDir dir, float angle)
{
    bViewMatrixDirty = true;
    // The exact same math as in Orbit, but the arrow is reversed
}

void Camera::Move(glm::vec3 deltaPos)
{
    bViewMatrixDirty = true;
    Position += deltaPos;
    Pivot += deltaPos;
}

void Camera::TestProjection() const
{
    auto projMat = MakeProjection(90.f * M_PI / 180.f, 1280.f / 720.f, 0.1f, 100.f);
    auto invProj = glm::inverse(projMat);
    for (int z = -1; z <= 1; z += 1)
        for (int y = -1; y <= 1; y += 2)
            for (int x = -1; x <= 1; x += 2)
            {
                auto ndc = glm::vec4(x, y, z, 1.0f);
                auto viewCoord = invProj * ndc;
                viewCoord /= viewCoord.w;
                printf("(%.3f %.3f %.3f) -> (%d %d %d)\n", viewCoord.x, viewCoord.y, viewCoord.z, x, y, z);
            }
}
