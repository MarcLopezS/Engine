#include "ModuleProgram.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "glew-2.1.0/include/GL/glew.h"

ModuleProgram::ModuleProgram(const std::string& vs, const std::string& fs)
{
	_vertexSource = vs;
	_fragmentSource = fs;
}

ModuleProgram::~ModuleProgram()
{}

bool ModuleProgram::Init()
{
	return false;
}

update_status ModuleProgram::Update() 
{
	return UPDATE_CONTINUE;
}

bool ModuleProgram::CleanUp()
{
	return false;
}

std::string ModuleProgram::readShaderSource(const std::string& filePath)
{
	std::ifstream file(filePath);

	if (!file.is_open())
	{
		std::cerr << "Cannot open file: " << filePath << std::endl;
		return "";
	}

	std::stringstream buffer;
	buffer << file.rdbuf();

	return buffer.str();
}

unsigned int ModuleProgram::CompileShader(unsigned int type, const std::string& source)
{
	unsigned int shader_id = glCreateShader(type);
	const char* src = source.c_str();

	glShaderSource(shader_id, 1, &src, nullptr);
	glCompileShader(shader_id);

	int result;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);

	if (result == GL_FALSE)
	{
		int length;

		glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);

		char* message = (char*)_malloca(length * sizeof(char));

		glGetShaderInfoLog(shader_id, length, &length, message);

		std::cout << "Failed to compile" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
		std::cout << message << std::endl;

		glDeleteShader(shader_id);
		return 0;
	}

	return shader_id;
}

unsigned int ModuleProgram::CreateProgram(const std::string& vertexShaderCont, const std::string& fragmentShaderCont)
{
	unsigned int program_id = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShaderCont);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderCont);

	glAttachShader(program_id, vs);
	glAttachShader(program_id, fs);
	glLinkProgram(program_id);
	glValidateProgram(program_id);

	int res;

	glGetProgramiv(program_id, GL_LINK_STATUS, &res);

	if (res == GL_FALSE)
	{
		int len = 0;
		glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &len);
		if (len > 0)
		{
			int written = 0;
			char* info = (char*)malloc(len);
			glGetProgramInfoLog(program_id, len, &written, info);
			LOG("Program Log Info: %s", info);
			free(info);
		}
	}

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program_id;
}

