#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>; // include glad to get all the required OpenGL headers
  
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
  

enum Shader_Type
{
	VERTEX = 0,
	FRAGMENT,
	PROGRAM
};

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

private:

	/**
	* @brief utility function for checking shader compilation/linking errors
	* @param shader		shader to check for error
	* @param type		the type of shader
	*/
	void check_compile_error(unsigned int shader, Shader_Type type);
};
  
#endif