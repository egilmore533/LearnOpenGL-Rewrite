#ifndef __SHADER_H__
#define __SHADER_H__

#include <glad/glad.h>; // include glad to get all the required OpenGL headers
  
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>
  
/**
* @enum Shader_Type
* @brief Defines the different types of shader's a shader id could be
*/
enum Shader_Type
{
	VERTEX = 0,
	FRAGMENT,
	PROGRAM
};

/**
* @class Camera
* @brief An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
*/
class Shader
{
public:
    unsigned int m_program_id;	/**< the program id for the shader */
  
    /**
	* @brief constructor to read, compile, and link a shader program written in GLSL
	* @param *vertex_path		path to the vertex shader
	* @param *fragment_path		path to the fragment shader
	*/
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
    
	/**
	* @brief activate the shader
	*/
    void use();

	// uniform setters

    /**
	* @brief utility function to set a uniform bool in the shader
	* @param &name		name of the uniform
	* @param value		value to set the uniform to
	*/
    void set_bool(const std::string &name, bool value) const;  
    
	/**
	* @brief utility function to set a uniform int in the shader
	* @param &name		name of the uniform
	* @param value		value to set the uniform to
	*/
	void set_int(const std::string &name, int value) const;   

	/**
	* @brief utility function to set a uniform float in the shader
	* @param &name		name of the uniform
	* @param value		value to set the uniform to
	*/
    void set_float(const std::string &name, float value) const;

	/**
	* @brief utility function to set a uniform vec2 in the shader
	* @param &name		name of the uniform
	* @param value		value to set the uniform to
	*/
	void set_vec2(const std::string &name, const glm::vec2 &value) const;

	/**
	* @brief utility function to set a uniform vec2 in the shader
	* @param &name		name of the uniform
	* @param x			x value to set the uniform to
	* @param y			y value to set the uniform to
	*/
	void set_vec2(const std::string &name, float x, float y) const;

	/**
	* @brief utility function to set a uniform vec3 in the shader
	* @param &name		name of the uniform
	* @param value		value to set the uniform to
	*/
	void set_vec3(const std::string &name, const glm::vec3 &value) const;

	/**
	* @brief utility function to set a uniform vec3 in the shader
	* @param &name		name of the uniform
	* @param x			x value to set the uniform to
	* @param y			y value to set the uniform to
	* @param z			z value to set the uniform to
	*/
	void set_vec3(const std::string &name, float x, float y, float z) const;

	/**
	* @brief utility function to set a uniform vec4 in the shader
	* @param &name		name of the uniform
	* @param value		value to set the uniform to
	*/
	void set_vec4(const std::string &name, const glm::vec4 &value) const;

	/**
	* @brief utility function to set a uniform vec4 in the shader
	* @param &name		name of the uniform
	* @param x			x value to set the uniform to
	* @param y			y value to set the uniform to
	* @param z			z value to set the uniform to
	* @param w			w value to set the uniform to
	*/
	void set_vec4(const std::string &name, float x, float y, float z, float w) const;

	/**
	* @brief utility function to set a uniform mat2 in the shader
	* @param &name		name of the uniform
	* @param value		value to set the uniform to
	*/
	void set_mat2(const std::string &name, const glm::mat2 &mat) const;

	/**
	* @brief utility function to set a uniform mat3 in the shader
	* @param &name		name of the uniform
	* @param value		value to set the uniform to
	*/
	void set_mat3(const std::string &name, const glm::mat3 &mat) const;

	/**
	* @brief utility function to set a uniform mat4 in the shader
	* @param &name		name of the uniform
	* @param value		value to set the uniform to
	*/
	void set_mat4(const std::string &name, const glm::mat4 &mat) const;

private:

	/**
	* @brief utility function for checking shader compilation/linking errors
	* @param shader		shader to check for error
	* @param type		the type of shader
	*/
	void check_compile_error(unsigned int shader, Shader_Type type);
};
  
#endif