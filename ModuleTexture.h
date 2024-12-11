#pragma once
#include "Module.h"
#include "glew-2.1.0/include/GL/glew.h"
#include ".\DirectXTex\DirectXTex.h"
#include <string>


class ModuleTexture :
    public Module
{
public: 
    ModuleTexture();
    ~ModuleTexture();

    bool Init() override;
    update_status Update() override;
    bool CleanUp() override;

    bool LoadTexture(const std::string& filePath);
    void Bind(GLuint unit = 0) const;
    void SetFilters(GLint magFilter, GLint minFilter);
    void SetWrapMode(GLint wrapS, GLint wrapT);

    GLenum GetFormat() const { return _format; }

private:
    GLuint _textureID;
    GLenum _format;
};

