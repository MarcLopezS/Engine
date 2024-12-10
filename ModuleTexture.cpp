#include "ModuleTexture.h"
#include <iostream>

ModuleTexture::ModuleTexture() : _textureID(0), _width(0), _height(0), _format(GL_RGBA8) 
{
    glGenTextures(1, &_textureID);
}

ModuleTexture::~ModuleTexture() 
{
    glDeleteTextures(1, &_textureID);
}

bool ModuleTexture::Init()
{
    return true;
}

update_status ModuleTexture::Update()
{
    return UPDATE_CONTINUE;
}


bool ModuleTexture::CleanUp()
{
    return true;
}

bool ModuleTexture::LoadTexture(const std::string& filePath) 
{
    DirectX::ScratchImage image;
    HRESULT hr = DirectX::LoadFromDDSFile(std::wstring(filePath.begin(), filePath.end()).c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);

    if (FAILED(hr)) {
        hr = DirectX::LoadFromTGAFile(std::wstring(filePath.begin(), filePath.end()).c_str(), nullptr, image);
        if (FAILED(hr)) {
            hr = DirectX::LoadFromWICFile(std::wstring(filePath.begin(), filePath.end()).c_str(), DirectX::WIC_FLAGS_NONE, nullptr, image);
            if (FAILED(hr)) {
                LOG("Failure in loading texture")
                return false; 
            }
        }
    }

    const DirectX::TexMetadata& metadata = image.GetMetadata();
    const DirectX::Image* img = image.GetImage(0, 0, 0);

    _width = static_cast<int>(metadata.width);
    _height = static_cast<int>(metadata.height);

    glGenTextures(1, &_textureID);
    glBindTexture(GL_TEXTURE_2D, _textureID);

    GLenum internalFormat, format, type;
    
    switch (metadata.format) {
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
        internalFormat = GL_RGBA8;
        format = GL_RGBA;
        type = GL_UNSIGNED_BYTE;
        break;
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8A8_UNORM:
        internalFormat = GL_RGBA8;
        format = GL_BGRA;
        type = GL_UNSIGNED_BYTE;
        break;
    case DXGI_FORMAT_B5G6R5_UNORM:
        internalFormat = GL_RGB8;
        format = GL_BGR;
        type = GL_UNSIGNED_BYTE;
        break;
    default:
        assert(false && "Unsupported format");
        return false;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, _width, _height, 0, format, type, img->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    return true;
}

void ModuleTexture::Bind(GLuint unit) const 
{
    LOG("Binding Texture...");
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, _textureID);
}

void ModuleTexture::SetFilters(GLint magFilter, GLint minFilter) 
{
    LOG("Setting Texture Filters");
    glBindTexture(GL_TEXTURE_2D, _textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
}

void ModuleTexture::SetWrapMode(GLint wrapS, GLint wrapT) 
{
    LOG("Setting Wrap Mode");
    glBindTexture(GL_TEXTURE_2D, _textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
}