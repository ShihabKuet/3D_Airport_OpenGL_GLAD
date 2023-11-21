
#ifndef PlaneHand_h
#define PlaneHand_h




#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader_m.h"

using namespace std;

class PlaneHand {
public:

    // materialistic property
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    // texture property
    float TXmin = 0.0f;
    float TXmax = 1.0f;
    float TYmin = 0.0f;
    float TYmax = 1.0f;
    unsigned int diffuseMap;
    unsigned int specularMap;

    // common property
    float shininess;

    // constructors
    PlaneHand()
    {
        setUpPlaneHandVertexDataAndConfigureVertexAttribute();
    }

    PlaneHand(glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float shiny)
    {
        this->ambient = amb;
        this->diffuse = diff;
        this->specular = spec;
        this->shininess = shiny;

        setUpPlaneHandVertexDataAndConfigureVertexAttribute();
    }

    PlaneHand(unsigned int dMap, unsigned int sMap, float shiny, float textureXmin, float textureYmin, float textureXmax, float textureYmax)
    {
        this->diffuseMap = dMap;
        this->specularMap = sMap;
        this->shininess = shiny;
        this->TXmin = textureXmin;
        this->TYmin = textureYmin;
        this->TXmax = textureXmax;
        this->TYmax = textureYmax;

        setUpPlaneHandVertexDataAndConfigureVertexAttribute();
    }

    // destructor
    ~PlaneHand()
    {
        glDeleteVertexArrays(1, &cubeVAO);
        glDeleteVertexArrays(1, &lightCubeVAO);
        glDeleteVertexArrays(1, &lightTexCubeVAO);
        glDeleteBuffers(1, &cubeVBO);
        glDeleteBuffers(1, &cubeEBO);
    }

    void drawPlaneHandWithTexture(Shader& lightingShaderWithTexture, glm::mat4 model = glm::mat4(1.0f))
    {
        lightingShaderWithTexture.use();

        lightingShaderWithTexture.setInt("material.diffuse", 0);
        lightingShaderWithTexture.setInt("material.specular", 1);
        lightingShaderWithTexture.setFloat("material.shininess", this->shininess);


        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->diffuseMap);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, this->specularMap);

        lightingShaderWithTexture.setMat4("model", model);

        glBindVertexArray(lightTexCubeVAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }

    void drawPlaneHandWithMaterialisticProperty(Shader& lightingShader, glm::mat4 model = glm::mat4(1.0f))
    {
        lightingShader.use();

        lightingShader.setVec3("material.ambient", this->ambient);
        lightingShader.setVec3("material.diffuse", this->diffuse);
        lightingShader.setVec3("material.specular", this->specular);
        lightingShader.setFloat("material.shininess", this->shininess);

        lightingShader.setMat4("model", model);

        glBindVertexArray(lightCubeVAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }

    void drawPlaneHand(Shader& shader, glm::mat4 model = glm::mat4(1.0f), float r = 1.0f, float g = 1.0f, float b = 1.0f)
    {
        shader.use();

        shader.setVec3("color", glm::vec3(r, g, b));
        shader.setMat4("model", model);

        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }

    void setMaterialisticProperty(glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float shiny)
    {
        this->ambient = amb;
        this->diffuse = diff;
        this->specular = spec;
        this->shininess = shiny;
    }

    void setTextureProperty(unsigned int dMap, unsigned int sMap, float shiny)
    {
        this->diffuseMap = dMap;
        this->specularMap = sMap;
        this->shininess = shiny;
    }

private:
    unsigned int cubeVAO;
    unsigned int lightCubeVAO;
    unsigned int lightTexCubeVAO;
    unsigned int cubeVBO;
    unsigned int cubeEBO;

    void setUpPlaneHandVertexDataAndConfigureVertexAttribute()
    {
        // set up vertex data (and buffer(s)) and configure vertex attributes
        // ------------------------------------------------------------------

        float phand_vertices[] = {
            // Position         // Normal
            0.0f, 0.0f, 0.0f,   0.0f, -1.0f, 0.0f,   TXmin, TYmin,         // Bottom
            3.5f, 0.0f, 2.2f,   0.0f, -1.0f, 0.0f,   TXmax, TYmin,
            3.5f, 0.0f, 2.8f,   0.0f, -1.0f, 0.0f,   TXmax, TYmax,
            0.0f, 0.0f, 1.4f,   0.0f, -1.0f, 0.0f,   TXmin, TYmax,

            0.0f, 0.3f, 0.0f,   0.0f, 1.0f, 0.0f,   TXmin, TYmin,         // Top
            3.5f, 0.3f, 2.2f,   0.0f, 1.0f, 0.0f,   TXmax, TYmin,
            3.5f, 0.3f, 2.8f,   0.0f, 1.0f, 0.0f,   TXmax, TYmax,
            0.0f, 0.3f, 1.4f,   0.0f, 1.0f, 0.0f,   TXmin, TYmax,

            0.0f, 0.0f, 0.0f,   -1.0f, 0.0f, 0.0f,   TXmin, TYmax,         // Left
            0.0f, 0.0f, 1.4f,   -1.0f, 0.0f, 0.0f,   TXmin, TYmax,
            0.0f, 0.3f, 1.4f,   -1.0f, 0.0f, 0.0f,   TXmin, TYmax,
            0.0f, 0.3f, 0.0f,   -1.0f, 0.0f, 0.0f,   TXmin, TYmax,

            3.5f, 0.0f, 2.2f,   1.0f, 0.0f, 0.0f,   TXmin, TYmax,         // Right
            3.5f, 0.0f, 2.8f,   1.0f, 0.0f, 0.0f,   TXmin, TYmax,
            3.5f, 0.3f, 2.8f,   1.0f, 0.0f, 0.0f,   TXmin, TYmax,
            3.5f, 0.3f, 2.2f,   1.0f, 0.0f, 0.0f,   TXmin, TYmax,

            0.0f, 0.0f, 0.0f,   0.53217f, 0.0f, -0.84664f,   TXmin, TYmax,         // Front
            3.5f, 0.0f, 2.2f,   0.53217f, 0.0f, -0.84664f,   TXmin, TYmax,
            3.5f, 0.3f, 2.2f,   0.53217f, 0.0f, -0.84664f,   TXmin, TYmax,
            0.0f, 0.3f, 0.0f,   0.53217f, 0.0f, -0.84664f,   TXmin, TYmax,

            0.0f, 0.0f, 1.4f,   -0.37139f, 0.0f, 0.92848f,   TXmin, TYmax,         // BCK
            3.5f, 0.0f, 2.8f,   -0.37139f, 0.0f, 0.92848f,   TXmin, TYmax,
            3.5f, 0.3f, 2.8f,   -0.37139f, 0.0f, 0.92848f,   TXmin, TYmax,
            0.0f, 0.3f, 1.4f,   -0.37139f, 0.0f, 0.92848f,   TXmin, TYmax
        };



        unsigned int phand_indices[] = {
            0, 1, 2,
            0, 2, 3,

            4, 5, 6,
            4, 6, 7,

            8, 9, 10,
            8, 10, 11,

            12, 13, 14,
            12, 14, 15,

            16, 17, 18,
            16, 18, 19,

            20, 21, 22,
            20, 22, 23
        };

        glGenVertexArrays(1, &cubeVAO);
        glGenVertexArrays(1, &lightCubeVAO);
        glGenVertexArrays(1, &lightTexCubeVAO);
        glGenBuffers(1, &cubeVBO);
        glGenBuffers(1, &cubeEBO);


        glBindVertexArray(lightTexCubeVAO);

        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(phand_vertices), phand_vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(phand_indices), phand_indices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // vertex normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)12);
        glEnableVertexAttribArray(1);

        // texture coordinate attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)24);
        glEnableVertexAttribArray(2);


        glBindVertexArray(lightCubeVAO);

        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)12);
        glEnableVertexAttribArray(1);


        glBindVertexArray(cubeVAO);

        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }

};


#endif /* cube_h */




