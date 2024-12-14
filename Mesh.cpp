#include "Mesh.h"
#include "Globals.h"
#include "Math/float3.h"
#include "Math/float2.h"
#include "tiny_gltf.h"
#include "SDL.h"
#include "glew-2.1.0/include/GL/glew.h"



Mesh::Mesh() : _vbo(0),_vbo_UV(0), _ebo(0), _vao(0), _indexCount(0)
{
}

Mesh::~Mesh()
{
}

bool Mesh::LoadMesh(const tinygltf::Model& model, const tinygltf::Mesh& mesh, const tinygltf::Primitive& primitive)
{
    LOG("Loading Mesh...");

    _materialIndex = primitive.material;

    if (!LoadAttribute(model, primitive, "POSITION", _vbo, sizeof(float3)))
    {
        LOG("Error: Failed to load POSITION attribute");
        return false;
    }

    if (!LoadAttribute(model, primitive, "TEXCOORD_0", _vbo_UV, sizeof(float2)))
    {
        LOG("Error: Failed to load TEXCOORD_0 attribute");
        return false;
    }

    if (!LoadEBO(model, primitive))
    {
        LOG("Error: Failed to load EBO");
        return false;
    }

    CreateVAO();

    return true;
}

bool Mesh::LoadAttribute(const tinygltf::Model& model, const tinygltf::Primitive& primitive,
    const std::string& attributeName, GLuint& vbo, size_t elementSize)
{
    const auto& itAttr = primitive.attributes.find(attributeName);
    if (itAttr == primitive.attributes.end())
    {
        LOG("Error: %s attribute not found", attributeName.c_str());
        return false;
    }

    const tinygltf::Accessor& accessor = model.accessors[itAttr->second];

    if (elementSize == sizeof(float3))
    {
        SDL_assert(accessor.type == TINYGLTF_TYPE_VEC3);
    }
    else if (elementSize == sizeof(float2))
    {
        SDL_assert(accessor.type == TINYGLTF_TYPE_VEC2);
    }

    SDL_assert(accessor.componentType == GL_FLOAT);

    const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
    const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
    const unsigned char* bufferData = &(buffer.data[accessor.byteOffset + bufferView.byteOffset]);

    //Readjust Stride
    size_t stride = bufferView.byteStride == 0 ? elementSize : bufferView.byteStride;
    LOG("%s Stride: %zu", attributeName.c_str(), stride);

    // Create VBO
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, elementSize * accessor.count, nullptr, GL_STATIC_DRAW);

    //Buffer Mapping
    if (elementSize == sizeof(float3))
    {
        float3* ptr = reinterpret_cast<float3*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
        if (ptr)
        {
            for (size_t i = 0; i < accessor.count; ++i)
            {
                ptr[i] = *reinterpret_cast<const float3*>(bufferData);
                bufferData += stride;
            }
            glUnmapBuffer(GL_ARRAY_BUFFER);
        }
        else
        {
            LOG("Error: Failed to map buffer for %s", attributeName.c_str());
            return false;
        }
    }
    else if (elementSize == sizeof(float2))
    {
        float2* ptr = reinterpret_cast<float2*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
        if (ptr)
        {
            for (size_t i = 0; i < accessor.count; ++i)
            {
                ptr[i] = *reinterpret_cast<const float2*>(bufferData);
                bufferData += stride;
            }
            glUnmapBuffer(GL_ARRAY_BUFFER);
        }
        else
        {
            LOG("Error: Failed to map buffer for %s", attributeName.c_str());
            return false;
        }
    }

    return true;
}

bool Mesh::LoadEBO(const tinygltf::Model& model, const tinygltf::Primitive& primitive) 
{
    if (primitive.indices >= 0) 
    {
        const tinygltf::Accessor& indAcc = model.accessors[primitive.indices];
        const tinygltf::BufferView& indView = model.bufferViews[indAcc.bufferView];
        const tinygltf::Buffer& indBuffer = model.buffers[indView.buffer];
        const unsigned char* buffer = &(indBuffer.data[indAcc.byteOffset + indView.byteOffset]);

        //Create EBO
        glGenBuffers(1, &_ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indAcc.count, nullptr, GL_STATIC_DRAW);

        //Buffer Mapping
        unsigned int* ptr = reinterpret_cast<unsigned int*>(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));
        
        if (!ptr)
        {
            LOG("Error: Failed to map EBO buffer");
            return false;
        }

        auto copyIndices = [&](auto bufferInd) 
            {
                for (size_t i = 0; i < indAcc.count; ++i) {
                    LOG("Index %zu: %u", i, reinterpret_cast<const uint16_t*>(buffer)[i]);
                    ptr[i] = bufferInd[i];
                }   
            };

        switch (indAcc.componentType) 
        {
            case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
                copyIndices(reinterpret_cast<const uint32_t*>(buffer));
                break;
            case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
                copyIndices(reinterpret_cast<const uint16_t*>(buffer));
                break;
            case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
                copyIndices(reinterpret_cast<const uint8_t*>(buffer));
                break;
            default:
                LOG("Error: Unsupported index component type");
                glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
                return false;
        }

        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
        _indexCount = indAcc.count;
        
    }

    return true;
}

void Mesh::CreateVAO()
{
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo_UV);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);

    glBindVertexArray(0);
}

void Mesh::Draw(unsigned int program, const std::vector<unsigned int>& textures)
{
    glUseProgram(program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[_materialIndex]);

    GLint textureLoc = glGetUniformLocation(program, "mytexture");

    if (textureLoc == -1)
        LOG("Error: Uniform 'mytexture' not found in shader program");


    glUniform1i(textureLoc, 0);

    glBindVertexArray(_vao);
    glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Mesh::Destroy()
{
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers(1, &_vbo);
    glDeleteBuffers(1, &_vbo_UV);
    glDeleteBuffers(1, &_ebo);

    _vao = 0;
    _vbo = 0;
    _vbo_UV = 0;
    _ebo = 0;
}



