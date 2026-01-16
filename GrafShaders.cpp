#define GLEW_DLL
#define GLFW_DLL

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "GrafShaders.h"

Shader::Shader() {

};

std::string Shader::readShFile(const char* path) {
	std::ifstream file(path, std::ios::in);
	if (!file.is_open()) {
		std::cerr << "Ошибка открытия файла шейдера: " << path << std::endl;
		return "";
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
};

int Shader::load(const char* vert_sh_path, const char* frag_sh_path) {
	std::string vert_shader_str = readShFile(vert_sh_path);
	std::string frag_shader_str = readShFile(frag_sh_path);

	const char* vert_shader = vert_shader_str.c_str();
	const char* frag_shader = frag_shader_str.c_str();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vert_shader, NULL);
	glCompileShader(vs);

	// Проверка компиляции вершинного шейдера
	GLint success;
	GLchar infoLog[1024];
	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vs, 1024, NULL, infoLog);
		std::cerr << "Ошибка компиляции вершинного шейдера:\n" << infoLog << std::endl;
		return 0;
	}

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &frag_shader, NULL);
	glCompileShader(fs);

	// Проверка компиляции фрагментного шейдера
	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fs, 1024, NULL, infoLog);
		std::cerr << "Ошибка компиляции фрагментного шейдера:\n" << infoLog << std::endl;
		glDeleteShader(vs);
		return 0;
	}

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, fs);
	glAttachShader(shaderProgram, vs);
	glLinkProgram(shaderProgram);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return 1;
};

void Shader::use() {
	glUseProgram(shaderProgram);
};

// Валидация входящего значения цвета и альфы для float (0.0-1.0)
void Shader::validate(float& _v, float _min = 0.0f, float _max = 1.0f) {
	if (_v < _min) { _v = _min; return; }
	if (_v > _max) { _v = _max; return; }
};
// Валидация входящего значения цвета и альфы для целочисленных (0-255)
void Shader::validate(int& _v, int _min = 0, int _max = 255) {
	if (_v < _min) { _v = _min; return; }
	if (_v > _max) { _v = _max; return; }
};

// Перевод целочисленного значения цвета в float (0.0-1.0)
float Shader::intToFloat(int& _v) {
	float _nv = _v / 255.0f;
	validate(_nv);
	return _nv;
};

// Color name, RGB = 0.0-1.0, alpha = 1.0
void Shader::glUniform(const char* cl_name, float r, float g, float b) {
	validate(r);
	validate(g);
	validate(b);
	glUniform4f(glGetUniformLocation(shaderProgram, cl_name), r, g, b, 1.0f);
};

// Color name, RGB = 0.0-1.0, alpha = 0.0-1.0
void Shader::glUniform(const char* cl_name, float r, float g, float b, float a) {
	validate(r);
	validate(g);
	validate(b);
	validate(a);
	glUniform4f(glGetUniformLocation(shaderProgram, cl_name), r, g, b, a);
};

// Color name, RGB = 0-255, alpha = 1.0
void Shader::glUniform(const char* cl_name, int r, int g, int b) {
	validate(r);
	validate(g);
	validate(b);
	glUniform4f(glGetUniformLocation(shaderProgram, cl_name), intToFloat(r), intToFloat(g), intToFloat(b), 1.0f);
};

// Color name, RGB = 0-255, alpha = 0.0-1.0
void Shader::glUniform(const char* cl_name, int r, int g, int b, float a) {
	validate(r);
	validate(g);
	validate(b);
	validate(a);
	glUniform4f(glGetUniformLocation(shaderProgram, cl_name), intToFloat(r), intToFloat(g), intToFloat(b), a);
};