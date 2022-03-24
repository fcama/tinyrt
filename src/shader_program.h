//
// Created by dante on 24/03/22.
//

#ifndef TINYRT_SRC_SHADER_PROGRAM_H_
#define TINYRT_SRC_SHADER_PROGRAM_H_

#include <string>

class ShaderProgram {
 public:
	ShaderProgram(const std::string &vertex_path, const std::string &fragment_path);
	void use();
	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;

  	unsigned int id_;

 private:
	void checkCompileErrors(unsigned int shader, const std::string &type);
};

#endif //TINYRT_SRC_SHADER_PROGRAM_H_
