#include <stdio.h>
#include <math.h>
#include <map>

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

//////////////LIGHTING CODE//////////////
glm::vec3 light_position(1.2f, 1.0f, 2.0f);


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

	//window callback set up
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

	Shader lighting_shader("shaders/standard.vs", "shaders/standard.fs");

	Shader lamp_shader("shaders/lamp.vs", "shaders/lamp.fs");

	Shader blending_shader("shaders/standard.vs", "shaders/blending.fs");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float cube_vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};

	float plane_vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f
	};

	// world space positions of our cubes
	glm::vec3 cube_positions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	glm::vec3 point_light_positions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	glm::vec3 windows[] = {
		glm::vec3(-1.5f, 0.0f, -0.48f),
		glm::vec3(1.5f, 0.0f, 0.51f),
		glm::vec3(0.0f, 0.0f, 0.7f),
		glm::vec3(-0.3f, 0.0f, -2.3f),
		glm::vec3(0.5f, 0.0f, -0.6f)
	};

	unsigned int vbo, cube_vao;
	glGenVertexArrays(1, &cube_vao);
	glGenBuffers(1, &vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

	glBindVertexArray(cube_vao);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//texel attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	unsigned int light_vao;

	glGenVertexArrays(1, &light_vao);
	glBindVertexArray(light_vao);

	// we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	unsigned int plane_vao, plane_vbo;

	glGenVertexArrays(1, &plane_vao);
	glGenBuffers(1, &plane_vbo);
	
	glBindVertexArray(plane_vao);
	glBindBuffer(GL_ARRAY_BUFFER, plane_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(plane_vertices), &plane_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//texel attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	
	// load models
	// -----------
	Model nanosuit("resources/objects/nanosuit/nanosuit.obj");

	unsigned int transparent_grass_texture = load_texture("resources/textures/grass.png");
	unsigned int transparent_window_texture = load_texture("resources/textures/blending_transparent_window.png");

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	while (!glfwWindowShouldClose(window))
	{
		//get time for this frame
		current_time = glfwGetTime();
		delta_time = current_time - last_frame;
		last_frame = current_time;

		process_input(window);
		//update everything

		// sort the transparent windows before rendering
		// This doesn't handle any transformations of the semi-transparent objects
		// If you want to do this try looking into order independent transparency
		std::map<float, glm::vec3> sorted;
		for (unsigned int i = 0; i < 5; i++)
		{
			float distance = glm::length(camera.m_position - windows[i]);
			sorted[distance] = windows[i];
		}

		//draw/render everything
		glClearColor(0.8f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// camera/view transformation
		glm::mat4 view = camera.get_view_matrix();

		glm::mat4 projection;
		projection = glm::perspective(glm::radians(camera.m_zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
		// note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.

		// set uniforms
		blending_shader.use();
		blending_shader.set_mat4("view", view);
		blending_shader.set_mat4("projection", projection);

		lamp_shader.use();
		lamp_shader.set_mat4("projection", projection);
		lamp_shader.set_mat4("view", view);

		// render boxes
		glBindVertexArray(cube_vao);

		glBindVertexArray(light_vao);
		for (int i = 0; i < 4; i++)
		{
			glm::mat4 model;
			model = glm::mat4();
			model = glm::translate(model, point_light_positions[i]);
			model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
			lamp_shader.set_mat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// activate shader
		lighting_shader.use();
		lighting_shader.set_mat4("view", view);
		lighting_shader.set_mat4("projection", projection);

		/*
		Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
		the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
		by defining light types as classes and set their values in there, or by using a more efficient uniform approach
		by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
		*/
		// directional light
		lighting_shader.set_vec3("directional_light.direction", -0.2f, -1.0f, -0.3f);
		lighting_shader.set_vec3("directional_light.ambient", 0.05f, 0.05f, 0.05f);
		lighting_shader.set_vec3("directional_light.diffuse", 0.4f, 0.4f, 0.4f);
		lighting_shader.set_vec3("directional_light.specular", 0.5f, 0.5f, 0.5f);
		// point light 1
		lighting_shader.set_vec3("point_lights[0].position", glm::vec3(view * glm::vec4(point_light_positions[0], 1.0f)));
		lighting_shader.set_vec3("point_lights[0].ambient", 0.05f, 0.05f, 0.05f);
		lighting_shader.set_vec3("point_lights[0].diffuse", 0.8f, 0.8f, 0.8f);
		lighting_shader.set_vec3("point_lights[0].specular", 1.0f, 1.0f, 1.0f);
		lighting_shader.set_float("point_lights[0].attenuation_constant", 1.0f);
		lighting_shader.set_float("point_lights[0].attenuation_linear", 0.09);
		lighting_shader.set_float("point_lights[0].attenuation_quadratic", 0.032);
		// point light 2
		lighting_shader.set_vec3("point_lights[1].position", glm::vec3(view * glm::vec4(point_light_positions[1], 1.0f)));
		lighting_shader.set_vec3("point_lights[1].ambient", 0.05f, 0.05f, 0.05f);
		lighting_shader.set_vec3("point_lights[1].diffuse", 0.8f, 0.8f, 0.8f);
		lighting_shader.set_vec3("point_lights[1].specular", 1.0f, 1.0f, 1.0f);
		lighting_shader.set_float("point_lights[1].attenuation_constant", 1.0f);
		lighting_shader.set_float("point_lights[1].attenuation_linear", 0.09);
		lighting_shader.set_float("point_lights[1].attenuation_quadratic", 0.032);
		// point light 3
		lighting_shader.set_vec3("point_lights[2].position", glm::vec3(view * glm::vec4(point_light_positions[2], 1.0f)));
		lighting_shader.set_vec3("point_lights[2].ambient", 0.05f, 0.05f, 0.05f);
		lighting_shader.set_vec3("point_lights[2].diffuse", 0.8f, 0.8f, 0.8f);
		lighting_shader.set_vec3("point_lights[2].specular", 1.0f, 1.0f, 1.0f);
		lighting_shader.set_float("point_lights[2].attenuation_constant", 1.0f);
		lighting_shader.set_float("point_lights[2].attenuation_linear", 0.09);
		lighting_shader.set_float("point_lights[2].attenuation_quadratic", 0.032);
		// point light 4
		lighting_shader.set_vec3("point_lights[3].position", glm::vec3(view * glm::vec4(point_light_positions[3], 1.0f)));
		lighting_shader.set_vec3("point_lights[3].ambient", 0.05f, 0.05f, 0.05f);
		lighting_shader.set_vec3("point_lights[3].diffuse", 0.8f, 0.8f, 0.8f);
		lighting_shader.set_vec3("point_lights[3].specular", 1.0f, 1.0f, 1.0f);
		lighting_shader.set_float("point_lights[3].attenuation_constant", 1.0f);
		lighting_shader.set_float("point_lights[3].attenuation_linear", 0.09);
		lighting_shader.set_float("point_lights[3].attenuation_quadratic", 0.032);
		// spot_light
		lighting_shader.set_vec3("spot_light.position", glm::vec3(view * glm::vec4(camera.m_position, 1.0f)));
		lighting_shader.set_vec3("spot_light.direction", glm::vec3(view * glm::vec4(camera.m_front, 0.0f)));
		lighting_shader.set_vec3("spot_light.ambient", 0.0f, 0.0f, 0.0f);
		lighting_shader.set_vec3("spot_light.diffuse", 1.0f, 1.0f, 1.0f);
		lighting_shader.set_vec3("spot_light.specular", 1.0f, 1.0f, 1.0f);
		lighting_shader.set_float("spot_light.attenuation_constant", 1.0f);
		lighting_shader.set_float("spot_light.attenuation_linear", 0.09);
		lighting_shader.set_float("spot_light.attenuation_quadratic", 0.032);
		lighting_shader.set_float("spot_light.cut_off", glm::cos(glm::radians(12.5f)));
		lighting_shader.set_float("spot_light.outer_cut_off", glm::cos(glm::radians(15.0f)));

		lighting_shader.set_vec3("material.specular", 0.5f, 0.5f, 0.5f);
		lighting_shader.set_float("material.shininess", 64.0f);

		
		// render the loaded model
		glm::mat4 model;
		glm::mat3 normal_matrix;
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.2f));	// it's a bit too big for our scene, so scale it down
		lighting_shader.set_mat4("model", model);

		normal_matrix = glm::mat3(glm::transpose(glm::inverse(view * model)));
		lighting_shader.set_mat3("normal_matrix", normal_matrix);

		nanosuit.draw(lighting_shader);
		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		blending_shader.use();
		glBindVertexArray(plane_vao);
		int j = 0;

		for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
		{
			model = glm::mat4();
			model = glm::translate(model, it->second);
			blending_shader.set_mat4("model", model);

			normal_matrix = glm::mat3(glm::transpose(glm::inverse(view * model)));
			blending_shader.set_mat3("normal_matrix", normal_matrix);

			glBindTexture(GL_TEXTURE_2D, transparent_window_texture);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		glDisable(GL_BLEND);

		//END OF DRAW SAWP BUFFERS
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &light_vao);
	glDeleteVertexArrays(1, &cube_vao);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &plane_vao);
	glDeleteBuffers(1, &plane_vbo);

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
