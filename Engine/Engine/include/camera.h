#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <glad\glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

enum Camera_Movement
{
	FORWARD = 0,
	BACKWARD,
	LEFT,
	RIGHT
};

//Default camera values
namespace defaults
{
	const float yaw =			-90.0f;
	const float pitch =			0.0f;
	const float speed =			2.5f;
	const float sensitivity =	0.1f;
	const float zoom =			45.0f;
}

class Camera
{
public:
	glm::vec3 m_position;
	glm::vec3 m_front;
	glm::vec3 m_up;
	glm::vec3 m_right;
	glm::vec3 m_world_up;

	float m_yaw;
	float m_pitch;

	float m_movement_speed;
	float m_mouse_sensitivity;
	float m_zoom;

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = defaults::yaw, float pitch = defaults::pitch);

	Camera(float pos_x, float pos_y, float pos_z, float up_x, float up_y, float up_z, float yaw, float pitch);

	glm::mat4 get_view_matrix()	{	return glm::lookAt(m_position, m_position + m_front, m_up);	}

	void process_keyboard(Camera_Movement direction, float delta_time);

	void process_mouse_movement(float x_offset, float y_offset, GLboolean constrain_pitch = true);

	void process_mouse_scroll(float y_offset);
	

private:
	void update_camera_vectors();
};


#endif
