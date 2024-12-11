#include "ModuleTexture.h"
#include <iostream>

ModuleTexture::ModuleTexture() : _textureID(0), _format(GL_RGBA8) 
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
    std::wstring wFilePath = std::wstring(filePath.begin(), filePath.end());

    HRESULT hr = DirectX::LoadFromDDSFile(wFilePath.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);

    if (SUCCEEDED(hr)) {
        LOG("Loaded texture as DDS: %s", filePath.c_str());
    }
    else if (SUCCEEDED(DirectX::LoadFromTGAFile(wFilePath.c_str(), nullptr, image))) {
        LOG("Loaded texture as TGA: %s", filePath.c_str());
    }
    else if (SUCCEEDED(DirectX::LoadFromWICFile(wFilePath.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, image))) {
        LOG("Loaded texture as WIC: %s", filePath.c_str());
    }
    else if (FAILED(hr)) {
        LOG("Failure in loading texture")
        return false; 
    }    

    //If image does not contain mipmap, it is generated
    if (image.GetMetadata().mipLevels <= 1) {
        DirectX::ScratchImage mipImage;
        hr = DirectX::GenerateMipMaps(*image.GetImage(0, 0, 0), DirectX::TEX_FILTER_DEFAULT, 0, mipImage);
        if (FAILED(hr)) {
            LOG("Failed to generate mipmaps: %s", filePath);
            return false;
        }
        image = std::move(mipImage);
    }

    const DirectX::TexMetadata& metadata = image.GetMetadata();

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

    for (size_t i = 0; i < metadata.mipLevels; ++i)
    {
        const DirectX::Image* mip = image.GetImage(i, 0, 0);
        glTexImage2D(GL_TEXTURE_2D, i, internalFormat, mip->width, mip->height, 0, format, type, mip->pixels);
    }

    //Configuring mipmap levels
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, metadata.mipLevels - 1);

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