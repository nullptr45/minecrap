#pragma once

#include <glm/glm.hpp>

class Camera {
public:
    Camera() = default;
    Camera(float fov_deg, float aspect, float near_z, float far_z);

    const glm::vec3& forward();
    const glm::vec3& right();
    const glm::vec3& up();

    void set_position(const glm::vec3& pos);
    void set_rotation(float yaw, float pitch);
    void add_yaw(float delta);
    void add_pitch(float delta);

    void set_aspect(float aspect);
    void set_fov(float fov_deg);

    const glm::vec3& position() const { return position_; }

    float yaw() const { return yaw_; }
    float pitch() const { return pitch_; }

    const glm::mat4& view();
    const glm::mat4& proj();

private:
    void update_vectors();

private:
    glm::vec3 position_{0.f};

    float pitch_ = 0.f;
    float yaw_ = -90.f;

    float fov_ = 70.f;
    float aspect_ = 1280.f / 720.f;
    float near_ = 0.1f;
    float far_ = 1000.f;

    glm::vec3 forward_{0.f, 0.f, -1.f};
    glm::vec3 right_{1.f, 0.f, 0.f};
    glm::vec3 up_{0.f, 1.f, 0.f};

    glm::mat4 view_{1.f};
    glm::mat4 proj_{1.f};

    // dirty flags
    bool vectors_dirty_ = true;
    bool view_dirty_ = true;
    bool proj_dirty_ = true;
};
