#include "camera.h"

#include <algorithm>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

Camera::Camera(float fov_deg, float aspect, float near_z, float far_z) :
    fov_(fov_deg), aspect_(aspect), near_(near_z), far_(far_z)
{}

void Camera::set_position(const glm::vec3& pos)
{
    position_ = pos;
    view_dirty_ = true;
}

void Camera::set_rotation(float yaw, float pitch)
{
    yaw_ = yaw;
    pitch_ = std::clamp(pitch, -89.f, 89.f);
    vectors_dirty_ = true;
    view_dirty_ = true;
}

void Camera::add_yaw(float delta)
{
    yaw_ += delta;
    vectors_dirty_ = true;
    view_dirty_ = true;
}

void Camera::add_pitch(float delta)
{
    pitch_ = std::clamp(pitch_ + delta, -89.f, 89.f);
    vectors_dirty_ = true;
    view_dirty_ = true;
}

void Camera::set_aspect(float aspect)
{
    aspect_ = aspect;
    proj_dirty_ = true;
}

void Camera::set_fov(float fov_deg)
{
    fov_ = fov_deg;
    proj_dirty_ = true;
}

void Camera::update_vectors()
{
    glm::vec3 f;
    f.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    f.y = sin(glm::radians(pitch_));
    f.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    forward_ = glm::normalize(f);
    right_ = glm::normalize(glm::cross(forward_, {0.f, 1.f, 0.f}));
    up_ = glm::normalize(glm::cross(right_, forward_));
    vectors_dirty_ = false;
}

const glm::vec3& Camera::forward()
{
    if (vectors_dirty_) update_vectors();
    return forward_;
}

const glm::vec3& Camera::right()
{
    if (vectors_dirty_) update_vectors();
    return right_;
}

const glm::vec3& Camera::up()
{
    if (vectors_dirty_) update_vectors();
    return up_;
}


const glm::mat4& Camera::view()
{
    if (view_dirty_) {
        if (vectors_dirty_) update_vectors();

        view_ = glm::lookAt(position_, position_ + forward_, up_);
        view_dirty_ = false;
    }
    return view_;
}

const glm::mat4& Camera::proj()
{
    if (proj_dirty_) {
        proj_ = glm::perspective(glm::radians(fov_), aspect_, near_, far_);
        proj_dirty_ = false;
    }
    return proj_;
}
