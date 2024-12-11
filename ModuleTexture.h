#pragma once
#include "Module.h"
#include "glew-2.1.0/include/GL/glew.h"
#include ".\DirectXTex\DirectXTex.h"
#include <string>


struct TextureOptions {
    static constexpr GLint wrapModes[4] = { GL_CLAMP_TO_BORDER, GL_CLAMP, GL_REPEAT, GL_MIRRORED_REPEAT };
    static constexpr const char* wrapModeNames[4] = { "CLAMP_TO_BORDER","GL_CLAMP","REPEAT", "MIRRORED_REPEAT" };

    static constexpr GLint filterModes[4] = { GL_NEAREST_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR};
    static constexpr const char* filterModeNames[4] = { "NEAREST_MIPMAP_NEAREST", "LINEAR_MIPMAP_NEAREST", "NEAREST_MIPMAP_LINEAR", "LINEAR_MIPMAP_LINEAR"};

    int currentWrapModeS = 2;
    int currentWrapModeT = 2;
    int currentMagFilter = 3;
    int currentMinFilter = 3;
    bool useMipmaps = true;
};

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

    void ShowTextureMenu();

    GLenum GetFormat() const { return _format; }
    int GetWidth() const { return _width; }
    int GetHeight() const { return _height; }

private:
    GLuint _textureID;
    GLenum _format;

    int _width = 0;
    int _height = 0;

    TextureOptions _textureOptions;
};

