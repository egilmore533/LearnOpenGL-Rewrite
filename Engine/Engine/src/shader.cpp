#include "shader.h"


Shader::Shader(const GLchar *vertex_path, const GLchar*fragment_path, const GLchar* geometry_path)
{
	// 1. retrieve the vertex/fragment source code from filePath
	std::string vertex_code;
	std::string fragment_code;
	std::string geometry_code;
	std::ifstream v_shader_file;
	std::ifstream f_shader_file;
	std::ifstream g_shader_file;
	// ensure ifstream objects can throw exceptions:
	v_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	f_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	g_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	bool has_geometry = geometry_path != "";
	try
	{
		// open files
		v_shader_file.open(vertex_path);
		f_shader_file.open(fragment_path);
		if (has_geometry)
			g_shader_file.open(geometry_path);

		std::stringstream v_shader_stream, f_shader_stream, g_shader_stream;
		// read file's buffer contents into streams
		v_shader_stream << v_shader_file.rdbuf();
		f_shader_stream << f_shader_file.rdbuf();
		if(has_geometry)
			g_shader_stream << g_shader_file.rdbuf();

		// close file handlers
		v_shader_file.close();
		f_shader_file.close();
		if (has_geometry)
			g_shader_file.close();
		// convert stream into string
		vertex_code = v_shader_stream.str();
		fragment_code = f_shader_stream.str();
		if (has_geometry)
			geometry_code = g_shader_stream.str();
	}
	catch (std::ifstream::failure e)
	{
		printf("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ\n");
	}
	const char* v_shader_code = vertex_code.c_str();
	const char* f_shader_code = fragment_code.c_str();
	const char* g_shader_code;
	if(has_geometry)
		g_shader_code = geometry_code.c_str();

	// 2. compile shaders
	unsigned int vertex, fragment, geometry;

	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &v_shader_code, NULL);
	glCompileShader(vertex);
	check_compile_error(vertex, VERTEX);

	// fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &f_shader_code, NULL);
	glCompileShader(fragment);
	check_compile_error(fragment, FRAGMENT);

	// geometry shader
	if (has_geometry)
	{
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &g_shader_code, NULL);
		glCompileShader(geometry);
		check_compile_error(geometry, GEOMETRY);
	}
	
	// shader Program
	this->m_program_id = glCreateProgram();
	glAttachShader(this->m_program_id, vertex);
	glAttachShader(this->m_program_id, fragment);
	if (has_geometry)
		glAttachShader(this->m_program_id, geometry);
	glLinkProgram(this->m_program_id);
	check_compile_error(this->m_program_id, PROGRAM);

	// delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::use()
{
	glUseProgram(m_program_id);
}

void Shader::set_bool(const std::string &name, bool value) const
{
	glUniform1i(glGetUniformLocation(m_program_id, name.c_str()), (int)value);
}
void Shader::set_int(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(m_program_id, name.c_str()), value);
}
void Shader::set_float(const std::string &name, float value) const
{
	glUniform1f(glGetUniformLocation(m_program_id, name.c_str()), value);
}

void Shader::set_vec2(const std::string &name, const glm::vec2 &value) const
{
	glUniform2fv(glGetUniformLocation(m_program_id, name.c_str()), 1, &value[0]);
}

void Shader::set_vec2(const std::string &name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(m_program_id, name.c_str()), x, y);
}

void Shader::set_vec3(const std::string &name, const glm::vec3 &value) const
{
	glUniform3fv(glGetUniformLocation(m_program_id, name.c_str()), 1, &value[0]);
}

void Shader::set_vec3(const std::string &name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(m_program_id, name.c_str()), x, y, z);
}

void Shader::set_vec4(const std::string &name, const glm::vec4 &value) const
{
	glUniform4fv(glGetUniformLocation(m_program_id, name.c_str()), 1, &value[0]);
}

void Shader::set_vec4(const std::string &name, float x, float y, float z, float w) const
{
	glUniform4f(glGetUniformLocation(m_program_id, name.c_str()), x, y, z, w);
}

void Shader::set_mat2(const std::string &name, const glm::mat2 &mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(m_program_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::set_mat3(const std::string &name, const glm::mat3 &mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(m_program_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::set_mat4(const std::string &name, const glm::mat4 &mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(m_program_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}


void Shader::check_compile_error(unsigned int shader, Shader_Type type)
{
	int success;
	char info_log[1024];

	if (type != PROGRAM)
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, info_log);
			printf("ERROR::SHADER_COMPILATION_ERROR of type: %d\n%s\n-- --------------------------------------------------- --\n", type, info_log);
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, info_log);
			printf("ERROR::PROGRAM_LINKING_ERROR of type: %d\n%s\n-- --------------------------------------------------- --\n", type, info_log);
		}
	}

	
}
