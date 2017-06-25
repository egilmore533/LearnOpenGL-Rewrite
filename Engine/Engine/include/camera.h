#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <glad\glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>


/**
* @enum		Camera_Movement
* @brief	Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
*/
enum Camera_Movement
{
	FORWARD = 0,
	BACKWARD,
	LEFT,
	RIGHT
};

/**
* @namespace	defaults
* @brief		Default camera values
*/
namespace defaults
{
	const float yaw =			-90.0f;
	const float pitch =			0.0f;
	const float speed =			2.5f;
	const float sensitivity =	0.1f;
	const float zoom =			45.0f;
}

/**
* @class	Camera
* @brief	An abstract (FPS) camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
*/
class Camera
{
public:

	//Attributes
	glm::vec3 m_position;	/**< position of the camera in world space */
	glm::vec3 m_front;		/**< direction vector for front of camera */
	glm::vec3 m_up;			/**< direction vector for top of camera */
	glm::vec3 m_right;		/**< direction vector for the right of camera */
	glm::vec3 m_world_up;	/**< direction for up at the start, assumed to be the up direction for the world */

	// Eular Angles
	float m_yaw;			/**< camera's yaw rotation in degrees */
	float m_pitch;			/**< camera's pitch rotation in degrees */

	// Camera options
	float m_movement_speed;		/**< camera's movement speed */
	float m_mouse_sensitivity;	/**< the amount that mouse movement will change the camera's rotation */
	float m_zoom;				/**< the camera's field of view */

	/**
	 * @brief	A constructor for the camera that will position and rotate the camera as defined by the vector input parameters or the default values
	 * @param	position	(Optional) The position.
	 * @param	up			(Optional) The up.
	 * @param	yaw			(Optional) The yaw.
	 * @param	pitch   	(Optional) The pitch.
	 */
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = defaults::yaw, float pitch = defaults::pitch);

	/**
	 * @brief	A constructor for the camera that will position and rotate the camera as defined by the float input parameters
	 * @param	pos_x	The position x coordinate.
	 * @param	pos_y	The position y coordinate.
	 * @param	pos_z	The position z coordinate.
	 * @param	up_x 	The up x coordinate.
	 * @param	up_y 	The up y coordinate.
	 * @param	up_z 	The up z coordinate.
	 * @param	yaw  	The yaw.
	 * @param	pitch	The pitch.
	 */
	Camera(float pos_x, float pos_y, float pos_z, float up_x, float up_y, float up_z, float yaw, float pitch);

	/**
	* @brief	quick getter for the view matrix from this camera using glm::lookAt
	* @return	a view matrix made from glm::lookAt
	*/
	glm::mat4 get_view_matrix()	{	return glm::lookAt(m_position, m_position + m_front, m_up);	}

	/**
	* @brief	handles the keyboard input callback
	*			moves the camera through the scene based on its foward, up, and right vectors to calcultae a new position
	* @param	direction	the direction the input was in
	* @param	delta_time	hte time difference since the last frame
	*/
	void process_keyboard(Camera_Movement direction, float delta_time);

	/**
	* @brief	handles the mouse input callback
	*			rotates the camera based on its current rotation and the input floats
	*			constrains by default to prevent gimbal lock
	* @param	x_offset			the difference of the cursors horizontal location from the center of the screen
	* @param	y_offset			the difference of the cursors vertical location from the center of the screen
	* @param	constrain_pitch		option to prevent the camera from rotation vertically forever, defaulted to on
	*/
	void process_mouse_movement(float x_offset, float y_offset, GLboolean constrain_pitch = true);

	/**
	* @brief	handles the mouse scroll wheel callback
	*			updates the zoom (field of view) of the camera
	* @param	y_offset		the difference in zoom to be applied
	*/
	void process_mouse_scroll(float y_offset);
	

private:

	/**
	* @brief	updates the front, up, and right direction vectors for use
	*			used after anything is updated with the camera
	*/
	void update_camera_vectors();
};


#endif
