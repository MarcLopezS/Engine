#pragma once
#include "Module.h"
#include <string>


class ModuleProgram :
    public Module
{
public:
    ModuleProgram(const std::string& vs, const std::string& fs);
    ~ModuleProgram();

    bool Init();
    update_status Update();
    bool CleanUp();

    std::string readShaderSource(const std::string& filePath);

    unsigned int CompileShader(unsigned int type, const std::string& source);
    unsigned int CreateProgram(const std::string& vertexShader, const std::string& fragmentShader);

    const std::string GetVertexSource() { return vertexSource; }
    const std::string GetFragmentSource() { return fragmentSource; }


private:
    std::string vertexSource;
    std::string fragmentSource;

};

