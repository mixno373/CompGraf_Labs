#pragma once

#define GLEW_DLL
#define GLFW_DLL

#include "GL/glew.h"
#include "GLFW/glfw3.h"

class Shader {
private:
	// Считать шейдер из файла
	std::string readShFile(const char* path);

	// Валидация входящего значения цвета и альфы для float (0.0-1.0)
	void validate(float& _v, float _min, float _max);
	// Валидация входящего значения цвета и альфы для целочисленных (0-255)
	void validate(int& _v, int _min, int _max);
	// Перевод целочисленного значения цвета в float (0.0-1.0).
	// Использовать только после валидации значения
	float intToFloat(int& _v);

public:
	GLuint shaderProgram = 0;

	Shader();

	// Принимает путь до файлов шейдеров, возвращает статус загрузки 0/1
	int load(const char* vert_sh_path, const char* frag_sh_path);

	// Подгрузка программы
	void use();

	// Color name, RGB = 0.0-1.0, alpha = 1.0
	void glUniform(const char* cl_name, float r, float g, float b);
	// Color name, RGB = 0.0-1.0, alpha = 0.0-1.0
	void glUniform(const char* cl_name, float r, float g, float b, float a);
	// Color name, RGB = 0-255, alpha = 1.0
	void glUniform(const char* cl_name, int r, int g, int b);
	// Color name, RGB = 0-255, alpha = 0.0-1.0
	void glUniform(const char* cl_name, int r, int g, int b, float a);
};