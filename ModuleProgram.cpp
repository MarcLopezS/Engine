#include "ModuleProgram.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "glew-2.1.0/include/GL/glew.h"

ModuleProgram::ModuleProgram(const std::string& vs, const std::string& fs)
	:_vertexSource(vs), _fragmentSource(fs), _programID(0)
{
}

ModuleProgram::~ModuleProgram()
{
	CleanUp();
}

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
	if (_programID != 0)
	{
		glDeleteProgram(_programID);
		_programID = 0;
	}
	return true;
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

		_freea(message);

		glDeleteShader(shader_id);
		return 0;
	}

	return shader_id;
}

unsigned int ModuleProgram::CreateProgram(const std::string& vertexShaderCont, const std::string& fragmentShaderCont)
{
	_programID = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShaderCont);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderCont);

	if (vs == 0 || fs == 0)
	{
		glDeleteProgram(_programID);
		_programID = 0;
		return 0;
	}

	glAttachShader(_programID, vs);
	glAttachShader(_programID, fs);
	glLinkProgram(_programID);
	glValidateProgram(_programID);

	int res;

	glGetProgramiv(_programID, GL_LINK_STATUS, &res);

	if (res == GL_FALSE)
	{
		int len = 0;
		glGetProgramiv(_programID, GL_INFO_LOG_LENGTH, &len);
		if (len > 0)
		{
			int written = 0;
			char* info = (char*)malloc(len);
			glGetProgramInfoLog(_programID, len, &written, info);
			LOG("Program Log Info: %s", info);
			free(info);
		}
	}

	glDeleteShader(vs);
	glDeleteShader(fs);

	return _programID;
}

