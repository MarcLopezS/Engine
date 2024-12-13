#include "Mesh.h"
#include "Globals.h"
#include "Math/float3.h"
#include "tiny_gltf.h"
#include "SDL.h"
#include "glew-2.1.0/include/GL/glew.h"



Mesh::Mesh() : _vbo(0), _ebo(0), _indexCount(0)
{
}

Mesh::~Mesh()
{
}

bool Mesh::LoadMesh(const tinygltf::Model& model, const tinygltf::Mesh& mesh, const tinygltf::Primitive& primitive)
{
    const auto& itPos = primitive.attributes.find("POSITION");
    
    if (itPos != primitive.attributes.end()) 
    {
        const tinygltf::Accessor& posAcc = model.accessors[itPos->second];
        SDL_assert(posAcc.type == TINYGLTF_TYPE_VEC3);
        SDL_assert(posAcc.componentType == GL_FLOAT);

        const tinygltf::BufferView& posView = model.bufferViews[posAcc.bufferView];
        const tinygltf::Buffer& posBuffer = model.buffers[posView.buffer];
        const unsigned char* bufferPos = &(posBuffer.data[posAcc.byteOffset + posView.byteOffset]);

        // Create VBO
        glGenBuffers(1, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * posAcc.count, nullptr, GL_STATIC_DRAW);
        
        //Buffer Mapping
        float3* ptr = reinterpret_cast<float3*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));

        if (ptr)
        {
            for (size_t i = 0; i < posAcc.count; ++i)
            {
                ptr[i] = *reinterpret_cast<const float3*>(bufferPos);
                bufferPos += posView.byteStride;
            }
            glUnmapBuffer(GL_ARRAY_BUFFER);
        }
        else {
            LOG("Error: Failed to map buffer");
            return false;
        }
    }
    else {
        LOG("Error: POSITION attribute not found");
        return false;
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

void Mesh::Draw() const
{
    //TODO: Pending
}

void Mesh::Destroy()
{
    //TODO: Pending
}



