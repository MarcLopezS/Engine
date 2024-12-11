#include "ModuleTexture.h"
#include "imgui.h"
#include <iostream>

ModuleTexture::ModuleTexture() : _textureID(0), _format(GL_RGBA8) 
{
    glGenTextures(1, &_textureID);
}

ModuleTexture::~ModuleTexture() 
{
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
    glDeleteTextures(1, &_textureID);
    return true;
}

bool ModuleTexture::LoadTexture(const std::string& filePath) 
{
    DirectX::ScratchImage image;
    std::wstring wFilePath = std::wstring(filePath.begin(), filePath.end());


    if (SUCCEEDED(DirectX::LoadFromDDSFile(wFilePath.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image))) {
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
        if (FAILED(DirectX::GenerateMipMaps(*image.GetImage(0, 0, 0), DirectX::TEX_FILTER_DEFAULT, 0, mipImage))) {
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
        if (i == 0)
        {
            _width = mip->width;
            _height = mip->height;
        }
        glTexImage2D(GL_TEXTURE_2D, i, internalFormat, mip->width, mip->height, 0, format, type, mip->pixels);
    }

    //Configuring mipmap levels
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, metadata.mipLevels - 1);

    _format = internalFormat;

    return true;
}

void ModuleTexture::Bind(GLuint unit) const 
{
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

void ModuleTexture::ShowTextureMenu() {
    ImGui::Begin("Texture Options");

    ImGui::Text("Width: %d", _width);
    ImGui::Text("Height: %d", _height);
    ImGui::Text("Format: 0x%X", _format);

    if (ImGui::CollapsingHeader("Wrap Mode")) {
        ImGui::Combo("Wrap S", &_textureOptions.currentWrapModeS, _textureOptions.wrapModeNames, IM_ARRAYSIZE(_textureOptions.wrapModeNames));
        ImGui::Combo("Wrap T", &_textureOptions.currentWrapModeT, _textureOptions.wrapModeNames, IM_ARRAYSIZE(_textureOptions.wrapModeNames));

        if (ImGui::Button("Apply Wrap Mode")) {
            SetWrapMode(_textureOptions.wrapModes[_textureOptions.currentWrapModeS], _textureOptions.wrapModes[_textureOptions.currentWrapModeT]);
        }
    }

    if (ImGui::CollapsingHeader("Filter Mode")) {
        ImGui::Combo("Mag Filter", &_textureOptions.currentMagFilter, _textureOptions.filterModeNames, IM_ARRAYSIZE(_textureOptions.filterModeNames));
        ImGui::Combo("Min Filter", &_textureOptions.currentMinFilter, _textureOptions.filterModeNames, IM_ARRAYSIZE(_textureOptions.filterModeNames));

        if (ImGui::Button("Apply Filter Mode")) {
            SetFilters(GL_LINEAR, _textureOptions.filterModes[_textureOptions.currentMinFilter]);
        }
    }

    if (ImGui::CollapsingHeader("Mipmaps")) {
        ImGui::Checkbox("Use Mipmaps", &_textureOptions.useMipmaps);

        if (ImGui::Button("Apply Mipmap Setting")) {
            if (_textureOptions.useMipmaps) {
                glBindTexture(GL_TEXTURE_2D, _textureID);
                glGenerateMipmap(GL_TEXTURE_2D);
                LOG("Generated MipMap");
            }
        }
    }

    ImGui::End();
}