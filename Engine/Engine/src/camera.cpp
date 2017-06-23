#include "camera.h"


Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) 
	: m_front(glm::vec3(0.0f, 0.0f, -1.0f)), m_movement_speed(defaults::speed), m_mouse_sensitivity(defaults::sensitivity), m_zoom(defaults::zoom)
{
	m_position = position;
	m_world_up = up;
	m_yaw = yaw;
	m_pitch = pitch;
	update_camera_vectors();
}

Camera::Camera(float pos_x, float pos_y, float pos_z, float up_x, float up_y, float up_z, float yaw, float pitch)
	: m_front(glm::vec3(0.0f, 0.0f, -1.0f)), m_movement_speed(defaults::speed), m_mouse_sensitivity(defaults::sensitivity), m_zoom(defaults::zoom)
{
	m_position = glm::vec3(pos_x, pos_y, pos_z);
	m_world_up = glm::vec3(up_x, up_y, up_z);
	m_yaw = yaw;
	m_pitch = pitch;
	update_camera_vectors();
}

void Camera::process_keyboard(Camera_Movement direction, float delta_time)
{
	float velocity = m_movement_speed * delta_time;
	if (direction == FORWARD)
		m_position += m_front * velocity;
	if (direction == BACKWARD)
		m_position -= m_front * velocity;
	if (direction == LEFT)
		m_position -= m_right * velocity;
	if (direction == RIGHT)
		m_position += m_right * velocity;
}

void Camera::process_mouse_movement(float x_offset, float y_offset, GLboolean constrain_pitch)
{
	x_offset *= m_mouse_sensitivity;
	y_offset *= m_mouse_sensitivity;

	m_yaw += x_offset;
	m_pitch += y_offset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrain_pitch)
	{
		if (m_pitch > 89.0f)
			m_pitch = 89.0f;
		if (m_pitch < -89.0f)
			m_pitch = -89.0f;
	}

	// Update Front, Right and Up Vectors using the updated Eular angles
	update_camera_vectors();
}

void Camera::process_mouse_scroll(float y_offset)
{
	if (m_zoom >= 1.0f && m_zoom <= 45.0f)
		m_zoom -= y_offset;
	if (m_zoom <= 1.0f)
		m_zoom = 1.0f;
	if (m_zoom >= 45.0f)
		m_zoom = 45.0f;
}

void Camera::update_camera_vectors()
{
	// Calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	front.y = sin(glm::radians(m_pitch));
	front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	m_front = glm::normalize(front);
	// Also re-calculate the Right and Up vector
	m_right = glm::normalize(glm::cross(m_front, m_world_up));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	m_up = glm::normalize(glm::cross(m_right, m_front));
}