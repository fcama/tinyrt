//
// Created by dante on 24/03/22.
//

#include "shader_program.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <glad/glad.h>

ShaderProgram::ShaderProgram(const std::string &vertex_path, const std::string &fragment_path) {
// 1. retrieve the vertex/fragment source code from filePath
	std::string vertex_code;
	std::string fragment_code;
	std::ifstream v_shader_file;
	std::ifstream f_shader_file;
	// ensure ifstream objects can throw exceptions:
	v_shader_file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
	f_shader_file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files
		v_shader_file.open(vertex_path);
		f_shader_file.open(fragment_path);
		std::stringstream v_shader_stream, f_shader_stream;
		// read file's buffer contents into streams
		v_shader_stream << v_shader_file.rdbuf();
		f_shader_stream << f_shader_file.rdbuf();
		// close file handlers
		v_shader_file.close();
		f_shader_file.close();
		// convert stream into string
		vertex_code   = v_shader_stream.str();
		fragment_code = f_shader_stream.str();
	}
	catch (std::ifstream::failure& e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
	}
	const char* v_shader_code = vertex_code.c_str();
	const char * f_shader_code = fragment_code.c_str();
	// 2. compile shaders
	unsigned int vertex, fragment;
	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &v_shader_code, nullptr);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");
	// fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &f_shader_code, nullptr);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");
	// shader Program
	id_ = glCreateProgram();
	glAttachShader(id_, vertex);
	glAttachShader(id_, fragment);
	glLinkProgram(id_);
	checkCompileErrors(id_, "PROGRAM");
	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void ShaderProgram::use() {
	glUseProgram(id_);
}
void ShaderProgram::setBool(const std::string &name, bool value) const {
	glUniform1i(glGetUniformLocation(id_, name.c_str()), (int)value);
}

void ShaderProgram::setInt(const std::string &name, int value) const {
	glUniform1i(glGetUniformLocation(id_, name.c_str()), value);
}
void ShaderProgram::setFloat(const std::string &name, float value) const {
	glUniform1f(glGetUniformLocation(id_, name.c_str()), value);
}

// private
void ShaderProgram::checkCompileErrors(unsigned int shader, const std::string &type) {
	int success;
	char info_log[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, nullptr, info_log);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << info_log << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, nullptr, info_log);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << info_log << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}