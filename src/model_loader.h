#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "tiny_gltf.h"

// Struktura jak w main.cpp
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

struct Object {
    GLuint VAO, VBO, EBO;
    GLuint texture;
    GLuint vertexCount;
    glm::mat4 model;
};

// Minimalny loader GLTF
Object loadModelFromGLTF(const std::string& path) {
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err, warn;

    bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, path);
    if (!warn.empty()) std::cout << "GLTF Warning: " << warn << std::endl;
    if (!err.empty()) std::cerr << "GLTF Error: " << err << std::endl;
    if (!ret) throw std::runtime_error("Failed to load glTF");

    const auto& mesh = model.meshes[0];
    const auto& primitive = mesh.primitives[0];

    const float* positions = reinterpret_cast<const float*>(
        &model.buffers[model.bufferViews[model.accessors[primitive.attributes.find("POSITION")->second].bufferView].buffer].data[0]
    );

    const float* normals = reinterpret_cast<const float*>(
        &model.buffers[model.bufferViews[model.accessors[primitive.attributes.find("NORMAL")->second].bufferView].buffer].data[0]
    );

    const float* texcoords = reinterpret_cast<const float*>(
        &model.buffers[model.bufferViews[model.accessors[primitive.attributes.find("TEXCOORD_0")->second].bufferView].buffer].data[0]
    );

    const unsigned short* indices = reinterpret_cast<const unsigned short*>(
        &model.buffers[model.bufferViews[primitive.indices].buffer].data[0]
    );

    size_t vertexCount = model.accessors[primitive.indices].count;
    std::vector<Vertex> vertices;

    for (size_t i = 0; i < vertexCount; ++i) {
        Vertex v;
        v.position = glm::vec3(positions[i * 3 + 0], positions[i * 3 + 1], positions[i * 3 + 2]);
        v.normal   = glm::vec3(normals[i * 3 + 0], normals[i * 3 + 1], normals[i * 3 + 2]);
        v.texCoord = glm::vec2(texcoords[i * 2 + 0], texcoords[i * 2 + 1]);
        vertices.push_back(v);
    }

    std::vector<unsigned short> indexData(indices, indices + vertexCount);

    Object obj;
    obj.vertexCount = indexData.size();

    glGenVertexArrays(1, &obj.VAO);
    glGenBuffers(1, &obj.VBO);
    glGenBuffers(1, &obj.EBO);

    glBindVertexArray(obj.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, obj.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexData.size() * sizeof(unsigned short), indexData.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, texCoord)));
    glEnableVertexAttribArray(2);

    obj.model = glm::mat4(1.0f);
    obj.texture = 0; // TODO: Załaduj teksturę jeśli chcesz

    return obj;
}
