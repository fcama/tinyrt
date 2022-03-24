//
// Created by dante on 24/03/22.
//

#ifndef TINYRT_SRC_SHADER_PROGRAM_H_
#define TINYRT_SRC_SHADER_PROGRAM_H_

#include <string>

class ShaderProgram {
 public:
	ShaderProgram(const std::string &vertex_path, const std::string &fragment_path);
	void Use();
	void SetBool(const std::string &name, bool value) const;
	void SetInt(const std::string &name, int value) const;
	void SetFloat(const std::string &name, float value) const;

  	unsigned int id_;

 private:
	void CheckCompileErrors(unsigned int shader, const std::string &type);
};

#endif //TINYRT_SRC_SHADER_PROGRAM_H_
