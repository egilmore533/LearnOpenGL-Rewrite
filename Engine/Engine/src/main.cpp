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

//	Forward Declarations ------------------------------------------------------------------
void process_input(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double x_pos, double y_pos);
void scroll_callback(GLFWwindow* window, double x_offset, double y_offset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
unsigned int load_texture(char const * path, bool gamma_correction);
unsigned int load_cubemap(std::vector<std::string> faces);
void render_scene(Shader shader, unsigned int quad_vao, unsigned int cube_vao);

//	Settings ------------------------------------------------------------------
const unsigned int screen_width = 1280;
const unsigned int screen_height = 720;
const unsigned int samples = 4;

//	Timing ------------------------------------------------------------------
float current_time = 0.0f;	// Total Time since start of program
float delta_time = 0.0f;	// Time between current frame and last frame
float last_frame = 0.0f;	// Time of last frame

//	Camera ------------------------------------------------------------------
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float last_x = screen_width / 2.0f;
float last_y = screen_height / 2.0f;
bool first_mouse = true;

//	Post-processing ---------------------------------------------------------
int effect = 0;


int main()
{
	// --------------------------------------------------------------------------
	//	window configuration ----------------------------------------------------
	// --------------------------------------------------------------------------

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, samples);

	GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, "LearnOpenGL", NULL, NULL);
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

	// camera/view transformation
	glm::mat4 view;
	glm::mat4 projection;

	// --------------------------------------------------------------------------
	//	Shader Programs ---------------------------------------------------------
	// --------------------------------------------------------------------------

	Shader simple_shader("shaders/simple.vs", "shaders/simple.fs");
	Shader post_processing_shader("shaders/simple.vs", "shaders/kernel.fs");
	Shader skybox_shader("shaders/skybox.vs", "shaders/skybox.fs");
	Shader lamp_shader("shaders/lamp.vs", "shaders/lamp.fs");

	// shadows (depth map debugging)
	Shader simple_depth_shader("shaders/shadow_mapping_depth.vs", "shaders/shadow_mapping_depth.fs");
	Shader debug_depth_quad("shaders/debug_depth_quad.vs", "shaders/debug_depth_quad.fs");
	Shader shadow_mapping_shader("shaders/shadow_mapping.vs", "shaders/shadow_mapping.fs");

	// --------------------------------------------------------------------------
	//	vertex data -------------------------------------------------------------
	// --------------------------------------------------------------------------

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

	std::vector<std::string> skybox_faces_filepaths = {
		"resources/textures/skybox/right.jpg",
		"resources/textures/skybox/left.jpg",
		"resources/textures/skybox/top.jpg",
		"resources/textures/skybox/bottom.jpg",
		"resources/textures/skybox/back.jpg",
		"resources/textures/skybox/front.jpg"
	};

	// depth mapping
	float plane_vertices[] = {
		// positions            // normals         // texcoords
		25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
		-25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
		-25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

		25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
		-25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
		25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
	};

	// cubes
	float cube_vertices[] = {
		// positions        // normals            // texels
		// back face
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
		1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
		// front face
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
		1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
		1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
		// left face
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		// right face
		1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
		1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
		// bottom face
		-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
		1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		// top face
		-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
		1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
	};

	// lighting info
	glm::vec3 light_pos(-2.0f, 4.0f, -1.0f);

	// --------------------------------------------------------------------------
	//	vertex array buffer configurations --------------------------------------
	// --------------------------------------------------------------------------

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

	// depth debug quad vao
	unsigned int debug_quad_vao, debug_quad_vbo;

	glGenVertexArrays(1, &debug_quad_vao);
	glGenBuffers(1, &debug_quad_vbo);

	glBindVertexArray(debug_quad_vao);
	glBindBuffer(GL_ARRAY_BUFFER, debug_quad_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(plane_vertices), &plane_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// cube vao
	unsigned int cube_vao, cube_vbo;

	glGenVertexArrays(1, &cube_vao);
	glGenBuffers(1, &cube_vbo);

	glBindVertexArray(cube_vao);
	glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), &cube_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	// --------------------------------------------------------------------------
	//	Uniform Buffer Objects Configuration -----------------------------------------------
	// --------------------------------------------------------------------------
	
	// first set the uniform block of the vertex shaders equal to the binding point (0)
	unsigned int uniform_block_index_skybox = glGetUniformBlockIndex(skybox_shader.m_program_id, "matrices");
	unsigned int uniform_block_index_shadows = glGetUniformBlockIndex(shadow_mapping_shader.m_program_id, "matrices");
	unsigned int uniform_block_index_lamp = glGetUniformBlockIndex(lamp_shader.m_program_id, "matrices");

	glUniformBlockBinding(skybox_shader.m_program_id, uniform_block_index_skybox, 0);
	glUniformBlockBinding(shadow_mapping_shader.m_program_id, uniform_block_index_shadows, 0);
	glUniformBlockBinding(lamp_shader.m_program_id, uniform_block_index_lamp, 0);
	
	// next create the actual uniform buffer object and bind the buffer to the binding point (0)
	unsigned int ubo_matrices;
	glGenBuffers(1, &ubo_matrices);

	glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo_matrices, 0, 2 * sizeof(glm::mat4));

	// now fill the buffer using glBufferSubData 
	// which lets you insert/update certain parts of a buffer but the buffer needs to have enough allocated 
	// memory which we have already done in the previous step
	
	// this is an example of how it works positioned next to the code that created the ubo so I can find it easily
	// I still need to do this in each game loop
	projection = glm::perspective(glm::radians(camera.m_zoom), (float)screen_width / (float)screen_height, 0.1f, 100.0f);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	view = camera.get_view_matrix();
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// --------------------------------------------------------------------------
	//	load models -------------------------------------------------------------
	// --------------------------------------------------------------------------


	// --------------------------------------------------------------------------
	//	load textures -----------------------------------------------------------
	// --------------------------------------------------------------------------

	// not sure why these are don't need to be flipped but loading them unflipped fixes this quick issue
	stbi_set_flip_vertically_on_load(false);
	unsigned int skybox_texture = load_cubemap(skybox_faces_filepaths);
	stbi_set_flip_vertically_on_load(true);
	unsigned int wood_texture = load_texture("resources/textures/wood.png", false);
	unsigned int wood_texture_gamma_corrected = load_texture("resources/textures/wood.png", true);

	// --------------------------------------------------------------------------
	//	framebuffer configuration -----------------------------------------------
	// --------------------------------------------------------------------------

	// depth map fbo
	const unsigned int shadow_width = 1024, shadow_height = 1024;
	unsigned int depth_map_fbo;
	glGenFramebuffers(1, &depth_map_fbo);

	// create depth texture
	unsigned int depth_map;
	glGenTextures(1, &depth_map);
	glBindTexture(GL_TEXTURE_2D, depth_map);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_width, shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float border_color[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

	//attach the depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// configure MSAA frambuffer
	unsigned int framebuffer_object;
	glGenFramebuffers(1, &framebuffer_object);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object);

		// create color buffer texture
		unsigned int texture_color_buffer_multisampled;
		glGenTextures(1, &texture_color_buffer_multisampled);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture_color_buffer_multisampled);
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, screen_width, screen_height, GL_TRUE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
		// attach the color buffer texture to the currently bound framebuffer object
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texture_color_buffer_multisampled, 0);

		// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
		unsigned int renderbuffer_object;
		glGenRenderbuffers(1, &renderbuffer_object);
		glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer_object);
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, screen_width, screen_height);
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

	//configure second post-processing framebuffer
	unsigned int intermediate_framebuffer_object;
	glGenFramebuffers(1, &intermediate_framebuffer_object);
	glBindFramebuffer(GL_FRAMEBUFFER, intermediate_framebuffer_object);

		//create a color attachment texture
		unsigned int screen_texture;
		glGenTextures(1, &screen_texture);
		glBindTexture(GL_TEXTURE_2D, screen_texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screen_width, screen_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screen_texture, 0); // we only need a color buffer here

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glEnable(GL_MULTISAMPLE);

	simple_shader.use();
	simple_shader.set_int("screen_texture", 0);

	post_processing_shader.use();
	post_processing_shader.set_int("screen_texture", 0);

	debug_depth_quad.use();
	debug_depth_quad.set_int("depth_map", 0);

	shadow_mapping_shader.use();
	shadow_mapping_shader.set_int("diffuse_texture", 0);
	shadow_mapping_shader.set_int("shadow_map", 1);

	glEnable(GL_DEPTH_TEST);

	// --------------------------------------------------------------------------
	//	Main Loop ---------------------------------------------------------------
	// --------------------------------------------------------------------------

	while (!glfwWindowShouldClose(window))
	{
		// get time for this frame
		current_time = glfwGetTime();
		delta_time = current_time - last_frame;
		last_frame = current_time;
		
		// update everything
		process_input(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 1. render depth of scene to texture (from light's perspective )
		glViewport(0, 0, shadow_width, shadow_height);
		glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
			
			glEnable(GL_DEPTH_TEST);

			glClear(GL_DEPTH_BUFFER_BIT);
			glm::mat4 light_projection, light_view, light_space_matrix;
			float near_plane = 1.0f, far_plane = 7.5f;
			light_projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
			light_view = glm::lookAt(light_pos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0));
			light_space_matrix = light_projection * light_view;

			simple_depth_shader.use();
			simple_depth_shader.set_mat4("light_space_matrix", light_space_matrix);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, wood_texture);

			render_scene(simple_depth_shader, debug_quad_vao, cube_vao);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// reset viewport 
		glViewport(0, 0, screen_width, screen_height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// bind to framebuffer and draw scene using the generated depth/shadow map
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object);

		// --------------------------------------------------------------------------
		//	Draw "Scene" to framebuffer ---------------------------------------------
		// --------------------------------------------------------------------------

			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);

			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			// update the projection and view matrices inside the uniform block
			projection = glm::perspective(glm::radians(camera.m_zoom), (float)screen_width / (float)screen_height, 0.1f, 1000.0f);
			glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
			glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			view = camera.get_view_matrix();
			glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
			glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			shadow_mapping_shader.use();
			shadow_mapping_shader.set_vec3("view_position", camera.m_position);
			shadow_mapping_shader.set_vec3("light_position", light_pos);
			shadow_mapping_shader.set_mat4("light_space_matrix", light_space_matrix);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, wood_texture);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, depth_map);

			render_scene(shadow_mapping_shader, debug_quad_vao, cube_vao);

			lamp_shader.use();
			glm::mat4 model = glm::mat4();
			model = glm::translate(model, light_pos);
			model = glm::scale(model, glm::vec3(0.25f));
			lamp_shader.set_mat4("model", model);
			glBindVertexArray(cube_vao);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
			
			/*
			glDepthFunc(GL_LEQUAL);
			skybox_shader.use();
			glm::mat4 view_no_translation = glm::mat4(glm::mat3(camera.get_view_matrix()));
			skybox_shader.set_mat4("view_no_translation", view_no_translation);
			glBindVertexArray(skybox_vao);
			glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glDepthFunc(GL_LESS);
			*/
			

		// --------------------------------------------------------------------------
		//	Finished Drawing "Scene" ------------------------------------------------
		// --------------------------------------------------------------------------

		// after drawing scene blit multisampled buffers to normal colorbuffer of intermediate fbo
		glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer_object);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediate_framebuffer_object);
		glBlitFramebuffer(0, 0, screen_width, screen_height, 0, 0, screen_width, screen_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		// next render qaud with the scene's visuals as it's texture image
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

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
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, screen_texture);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// END OF DRAW SAWP BUFFERS
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
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
unsigned int load_texture(char const * path, bool gamma_correction)
{
	unsigned int texture_id;
	glGenTextures(1, &texture_id);

	int width, height, nr_components;
	unsigned char *data = stbi_load(path, &width, &height, &nr_components, 0);
	if (data)
	{
		GLenum internal_format, data_format;
		if (nr_components == 1)
			internal_format = data_format = GL_RED;
		else if (nr_components == 3)
		{
			internal_format = (gamma_correction ? GL_SRGB : GL_RGB);
			data_format = GL_RGB;
		}
		else if (nr_components == 4)
		{
			internal_format = (gamma_correction ? GL_SRGB_ALPHA : GL_RGBA);
			data_format = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, texture_id);
		glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, data_format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, data_format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, data_format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
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

void render_scene(Shader shader, unsigned int quad_vao, unsigned int cube_vao)
{
	// floor
	glm::mat4 model;
	shader.set_mat4("model", model);
	glBindVertexArray(quad_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// cubes
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0f));
	model = glm::scale(model, glm::vec3((sin(current_time) + 1.0) / 2.0));
	shader.set_mat4("model", model);
	glBindVertexArray(cube_vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
	model = glm::scale(model, glm::vec3(0.5f));
	shader.set_mat4("model", model);
	glBindVertexArray(cube_vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
	model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f)));
	model = glm::scale(model, glm::vec3(0.25f));
	shader.set_mat4("model", model);
	glBindVertexArray(cube_vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}
