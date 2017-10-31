#include <stdio.h>
#include <math.h>
#include <map>
#include <string>

#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include <stb_image.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "model.h"

void process_input(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double x_pos, double y_pos);
void scroll_callback(GLFWwindow* window, double x_offset, double y_offset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
unsigned int load_texture(const char *path);
unsigned int load_cubemap(std::vector<std::string> faces);

const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;

float current_time = 0.0f;	// Total Time since start of program
float delta_time = 0.0f;	// Time between current frame and last frame
float last_frame = 0.0f;	// Time of last frame

///////CAMERA CODE/////////////////

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float last_x = SCREEN_WIDTH / 2.0f;
float last_y = SCREEN_HEIGHT / 2.0f;
bool first_mouse = true;

int effect = 0;


int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		printf("Failed to create GLFW window\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// window callback set up
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("Failed to initialize GLAD\n");
		return -1;
	}

	stbi_set_flip_vertically_on_load(true);

	Shader simple_shader("shaders/simple.vs", "shaders/simple.fs");
	Shader post_processing_shader("shaders/simple.vs", "shaders/kernel.fs");
	Shader skybox_shader("shaders/skybox.vs", "shaders/skybox.fs");
	Shader refraction_shader("shaders/reflection.vs", "shaders/refraction.fs");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float cube_vertices_ccw_winding_order[] = {
		// Back face
		-0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f,  0.0f, 0.0f, // Bottom-left
		 0.5f,  0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		 0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
		 0.5f,  0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		-0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		-0.5f,  0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
		// Front face
		-0.5f, -0.5f,  0.5f, 0.0f,  0.0f, 1.0f,  0.0f, 0.0f, // bottom-left
		 0.5f, -0.5f,  0.5f, 0.0f,  0.0f, 1.0f, 1.0f, 0.0f, // bottom-right
		 0.5f,  0.5f,  0.5f, 0.0f,  0.0f, 1.0f,  1.0f, 1.0f, // top-right
		 0.5f,  0.5f,  0.5f, 0.0f,  0.0f, 1.0f,  1.0f, 1.0f, // top-right
		-0.5f,  0.5f,  0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 1.0f, // top-left
		-0.5f, -0.5f,  0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
		// Left face
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		// Right face
		 0.5f,  0.5f,  0.5f, 1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		 0.5f, -0.5f, -0.5f, 1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		 0.5f,  0.5f, -0.5f, 1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
		 0.5f, -0.5f, -0.5f, 1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		 0.5f,  0.5f,  0.5f, 1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		 0.5f, -0.5f,  0.5f, 1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
		// Bottom face
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		 0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
		 0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		 0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		-0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		// Top face
		-0.5f,  0.5f, -0.5f, 0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		 0.5f,  0.5f,  0.5f, 0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		 0.5f,  0.5f, -0.5f, 0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
		 0.5f,  0.5f,  0.5f, 0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		-0.5f,  0.5f, -0.5f, 0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		-0.5f,  0.5f,  0.5f, 0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left
	};

	// vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	float simple_quad_vertices[] = {
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
	};

	float skybox_vertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	std::vector<std::string> skybox_faces_filepaths = 
	{
		"resources/textures/skybox/right.jpg",
		"resources/textures/skybox/left.jpg",
		"resources/textures/skybox/top.jpg",
		"resources/textures/skybox/bottom.jpg",
		"resources/textures/skybox/back.jpg",
		"resources/textures/skybox/front.jpg"
	};

	unsigned int vbo, cube_vao;
	glGenVertexArrays(1, &cube_vao);
	glGenBuffers(1, &vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices_ccw_winding_order), cube_vertices_ccw_winding_order, GL_STATIC_DRAW);

	glBindVertexArray(cube_vao);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texel attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// screen quad vao
	unsigned int quad_vao, quad_vbo;

	glGenVertexArrays(1, &quad_vao);
	glGenBuffers(1, &quad_vbo);

	glBindVertexArray(quad_vao);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(simple_quad_vertices), &simple_quad_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// skybox vbo, vao, and vertices
	unsigned int skybox_vao, skybox_vbo;

	glGenVertexArrays(1, &skybox_vao);
	glGenBuffers(1, &skybox_vbo);

	glBindVertexArray(skybox_vao);
	glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// load models
	// -----------
	Model nanosuit("resources/objects/nanosuit/nanosuit.obj");

	// not sure why these are don't need to be flipped but loading them unflipped fixes this quick issue
	stbi_set_flip_vertically_on_load(false);
	unsigned int skybox_texture = load_cubemap(skybox_faces_filepaths);
	stbi_set_flip_vertically_on_load(true);

	// --------------------------------------------------------------------------
	//	framebuffer configuration -----------------------------------------------
	// --------------------------------------------------------------------------
	unsigned int framebuffer_object;
	glGenFramebuffers(1, &framebuffer_object);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object);

		// create color buffer texture
		unsigned int texture_color_buffer;
		glGenTextures(1, &texture_color_buffer);
		glBindTexture(GL_TEXTURE_2D, texture_color_buffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// attach the color buffer texture to the currently bound framebuffer object
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_color_buffer, 0);

		// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
		unsigned int renderbuffer_object;
		glGenRenderbuffers(1, &renderbuffer_object);
		glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer_object);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		// attach the renderbuffer to the currently bound framebuffer object
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer_object);
		
		// now check that the framebuffer object is complete and we can use it
		// in order to be complete it needs the following
		//	1. We have to attach at least one buffer (color, depth or stencil buffer).
		//	2. There should be at least one color attachment.
		//	3. All attachments should be complete as well(reserved memory).
		//	4. Each buffer should have the same number of samples.
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

		// unbind the framebuffer to make sure we're not accidentally rendering to the wrong framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// --------------------------------------------------------------------------
	//	end framebuffer configuration -------------------------------------------
	// --------------------------------------------------------------------------

	while (!glfwWindowShouldClose(window))
	{
		// get time for this frame
		current_time = glfwGetTime();
		delta_time = current_time - last_frame;
		last_frame = current_time;

		
		// update everything
		process_input(window);

		// bind to framebuffer and draw scene normally
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		// draw/render everything
		glClearColor(0.3f, 0.3f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glEnable(GL_CULL_FACE);

		// camera/view transformation
		glm::mat4 view = camera.get_view_matrix();

		glm::mat4 projection;
		projection = glm::perspective(glm::radians(camera.m_zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
		// note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.

		// render mirror nanosuit
		refraction_shader.use();
		glm::mat4 temp;
		temp = glm::translate(temp, glm::vec3(0.0f, -1.75f, 0.0f));
		temp = glm::scale(temp, glm::vec3(0.2f));
		refraction_shader.set_mat4("model", temp);
		refraction_shader.set_mat4("view", view);
		refraction_shader.set_mat4("projection", projection);
		refraction_shader.set_vec3("camera_pos", camera.m_position);
		//glBindVertexArray(cube_vao);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture);
		nanosuit.draw(refraction_shader, false);
		//glDrawArrays(GL_TRIANGLES, 0, 36);
		//glBindVertexArray(0);
		
		glDepthFunc(GL_LEQUAL);
		//glDepthMask(GL_FALSE);
		skybox_shader.use();
		glm::mat4 view_no_translation = glm::mat4(glm::mat3(camera.get_view_matrix()));
		skybox_shader.set_mat4("view", view_no_translation);
		skybox_shader.set_mat4("projection", projection);
		glBindVertexArray(skybox_vao);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		//glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		switch (effect)
		{
		case 0:
			simple_shader.use();
			break;
		case 1:
			post_processing_shader.use();
			break;
		}
		
		glBindVertexArray(quad_vao);
		glBindTexture(GL_TEXTURE_2D, texture_color_buffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// END OF DRAW SAWP BUFFERS
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &cube_vao);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &quad_vao);
	glDeleteBuffers(1, &quad_vbo);

	glDeleteFramebuffers(1, &framebuffer_object);

	glfwTerminate();
	return 0;
}

void process_input(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.process_keyboard(FORWARD, delta_time);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.process_keyboard(BACKWARD, delta_time);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.process_keyboard(LEFT, delta_time);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.process_keyboard(RIGHT, delta_time);
	if (glfwGetKey(window, GLFW_KEY_1))
		effect = 0;
	if (glfwGetKey(window, GLFW_KEY_2))
		effect = 1;
}

void mouse_callback(GLFWwindow* window, double x_pos, double y_pos)
{
	if (first_mouse)
	{
		last_x = x_pos;
		last_y = y_pos;
		first_mouse = false;
	}

	float x_offset = x_pos - last_x;
	float y_offset = last_y - y_pos; // reversed since y-coordinates go from bottom to top

	last_x = x_pos;
	last_y = y_pos;

	camera.process_mouse_movement(x_offset, y_offset);
}

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset)
{
	camera.process_mouse_scroll(y_offset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int load_texture(char const * path)
{
	unsigned int texture_id;
	glGenTextures(1, &texture_id);

	int width, height, nr_components;
	unsigned char *data = stbi_load(path, &width, &height, &nr_components, 0);
	if (data)
	{
		GLenum format;
		if (nr_components == 1)
			format = GL_RED;
		else if (nr_components == 3)
			format = GL_RGB;
		else if (nr_components == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, texture_id);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return texture_id;
}

unsigned int load_cubemap(std::vector<std::string> faces)
{
	unsigned int texture_id;
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

	int width, height, nr_channels;
	for (int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nr_channels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			printf("Cubemap texture failed to load at filepath: %s\n", faces[i]);
			stbi_image_free(data);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return texture_id;
}
