#pragma once
#include <glm/glm.hpp>

class Camera
{
public:
    enum class EDir
    {
        Up,
        Down,
        Left,
        Right
    };

    Camera(const glm::vec3 &pivot, const glm::vec3 &position) : Pivot(pivot), Position(position) {}

    static glm::mat4 MakeProjection(float fovY, float aspect, float n, float f);
    [[nodiscard]] glm::mat4 MakeViewMatrix() const;
    [[nodiscard]] const glm::mat4 &GetViewMatrix() const;
    void Orbit(EDir dir, float angle);
    void Turn(EDir dir, float angle);
    void Move(glm::vec3 deltaPos);

    [[nodiscard]] const glm::vec3 &GetPosition() const { return Position; }
    void TestProjection() const;

private:
    glm::vec3 Pivot, Position;
    glm::vec3 Up{0.0f, 1.0f, 0.0f};
    mutable bool bViewMatrixDirty = true;
    mutable glm::mat4 CachedViewMatrix;
};
