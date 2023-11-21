

#ifndef hexagon_h
#define hexagon_h

#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader_m.h"

using namespace std;

class Hexagon {
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
    Hexagon()
    {
        setUpHexagonVertexDataAndConfigureVertexAttribute();
    }

    Hexagon(glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float shiny)
    {
        this->ambient = amb;
        this->diffuse = diff;
        this->specular = spec;
        this->shininess = shiny;

        setUpHexagonVertexDataAndConfigureVertexAttribute();
    }

    Hexagon(unsigned int dMap, unsigned int sMap, float shiny, float textureXmin, float textureYmin, float textureXmax, float textureYmax)
    {
        this->diffuseMap = dMap;
        this->specularMap = sMap;
        this->shininess = shiny;
        this->TXmin = textureXmin;
        this->TYmin = textureYmin;
        this->TXmax = textureXmax;
        this->TYmax = textureYmax;

        setUpHexagonVertexDataAndConfigureVertexAttribute();
    }

    // destructor
    ~Hexagon()
    {
        glDeleteVertexArrays(1, &hexagonVAO);
        glDeleteVertexArrays(1, &lightCubeVAO);
        glDeleteVertexArrays(1, &lightTexCubeVAO);
        glDeleteBuffers(1, &hexagonVBO);
        glDeleteBuffers(1, &hexagonEBO);
    }

    void drawHexagonWithTexture(Shader& lightingShaderWithTexture, glm::mat4 model = glm::mat4(1.0f))
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
        glDrawElements(GL_TRIANGLES, 60, GL_UNSIGNED_INT, 0);
    }

    void drawHexagonWithMaterialisticProperty(Shader& lightingShader, glm::mat4 model = glm::mat4(1.0f))
    {
        lightingShader.use();

        lightingShader.setVec3("material.ambient", this->ambient);
        lightingShader.setVec3("material.diffuse", this->diffuse);
        lightingShader.setVec3("material.specular", this->specular);
        lightingShader.setFloat("material.shininess", this->shininess);

        lightingShader.setMat4("model", model);

        glBindVertexArray(lightCubeVAO);
        glDrawElements(GL_TRIANGLES, 60, GL_UNSIGNED_INT, 0);
    }

    void drawHexagon(Shader& shader, glm::mat4 model = glm::mat4(1.0f), float r = 1.0f, float g = 1.0f, float b = 1.0f)
    {
        shader.use();

        shader.setVec3("color", glm::vec3(r, g, b));
        shader.setMat4("model", model);

        glBindVertexArray(hexagonVAO);
        glDrawElements(GL_TRIANGLES, 60, GL_UNSIGNED_INT, 0);
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
    unsigned int hexagonVAO;
    unsigned int lightCubeVAO;
    unsigned int lightTexCubeVAO;
    unsigned int hexagonVBO;
    unsigned int hexagonEBO;

    void setUpHexagonVertexDataAndConfigureVertexAttribute()
    {
        // set up vertex data (and buffer(s)) and configure vertex attributes
        // ------------------------------------------------------------------

        float hex_vertices[] = {
            0.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,           // Bottom
            1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
            2.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 2.0f,   1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 2.0f,   1.0f, 0.0f, 0.0f,
           -1.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f,

            0.0f, 2.0f, 0.0f,   1.0f, 1.0f, 1.0f,           // Top
            1.0f, 2.0f, 0.0f,   1.0f, 1.0f, 1.0f,
            2.0f, 2.0f, 1.0f,   1.0f, 1.0f, 1.0f,
            1.0f, 2.0f, 2.0f,   1.0f, 1.0f, 1.0f,
            0.0f, 2.0f, 2.0f,   1.0f, 1.0f, 1.0f,
           -1.0f, 2.0f, 1.0f,   1.0f, 1.0f, 1.0f,

            0.0f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f,			   // Front
            1.0f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f,
            1.0f, 2.0f, 0.0f,   0.0f, 1.0f, 0.0f,
            0.0f, 2.0f, 0.0f,   0.0f, 1.0f, 0.0f,

            1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 1.0f,			   // Front Right
            2.0f, 0.0f, 1.0f,   1.0f, 0.0f, 1.0f,
            2.0f, 2.0f, 1.0f,   1.0f, 0.0f, 1.0f,
            1.0f, 2.0f, 0.0f,   1.0f, 0.0f, 1.0f,

            1.0f, 0.0f, 2.0f,   1.0f, 1.0f, 0.0f,			   // Back Right
            2.0f, 0.0f, 1.0f,   1.0f, 1.0f, 0.0f,
            2.0f, 2.0f, 1.0f,   1.0f, 1.0f, 0.0f,
            1.0f, 2.0f, 2.0f,   1.0f, 1.0f, 0.0f,

            0.0f, 0.0f, 2.0f,   0.0f, 1.0f, 0.0f,			   // Back
            1.0f, 0.0f, 2.0f,   0.0f, 1.0f, 0.0f,
            1.0f, 2.0f, 2.0f,   0.0f, 1.0f, 0.0f,
            0.0f, 2.0f, 2.0f,   0.0f, 1.0f, 0.0f,

           -1.0f, 0.0f, 1.0f,   1.0f, 0.0f, 1.0f,			   // Back Left
            0.0f, 0.0f, 2.0f,   1.0f, 0.0f, 1.0f,
            0.0f, 2.0f, 2.0f,   1.0f, 0.0f, 1.0f,
           -1.0f, 2.0f, 1.0f,   1.0f, 0.0f, 1.0f,

           -1.0f, 0.0f, 1.0f,   0.0f, 0.0f, 0.0f,			   // Front Left
            0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
            0.0f, 2.0f, 0.0f,   0.0f, 0.0f, 0.0f,
           -1.0f, 2.0f, 1.0f,   0.0f, 0.0f, 0.0f
        };

        float hexa_vertices[] = {
            0.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,           // Bottom
            1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
            2.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 2.0f,   1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 2.0f,   1.0f, 0.0f, 0.0f,
           -1.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f,

            0.0f, 2.0f, 0.0f,   1.0f, 1.0f, 1.0f,           // Top
            1.0f, 2.0f, 0.0f,   1.0f, 1.0f, 1.0f,
            2.0f, 2.0f, 1.0f,   1.0f, 1.0f, 1.0f,
            1.0f, 2.0f, 2.0f,   1.0f, 1.0f, 1.0f,
            0.0f, 2.0f, 2.0f,   1.0f, 1.0f, 1.0f,
           -1.0f, 2.0f, 1.0f,   1.0f, 1.0f, 1.0f,

            0.0f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f,			   // Front
            1.0f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f,
            1.0f, 2.0f, 0.0f,   0.0f, 1.0f, 0.0f,
            0.0f, 2.0f, 0.0f,   0.0f, 1.0f, 0.0f,

            1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 1.0f,			   // Front Right
            2.0f, 0.0f, 1.0f,   1.0f, 0.0f, 1.0f,
            2.0f, 2.0f, 1.0f,   1.0f, 0.0f, 1.0f,
            1.0f, 2.0f, 0.0f,   1.0f, 0.0f, 1.0f,

            1.0f, 0.0f, 2.0f,   1.0f, 1.0f, 0.0f,			   // Back Right
            2.0f, 0.0f, 1.0f,   1.0f, 1.0f, 0.0f,
            2.0f, 2.0f, 1.0f,   1.0f, 1.0f, 0.0f,
            1.0f, 2.0f, 2.0f,   1.0f, 1.0f, 0.0f,

            0.0f, 0.0f, 2.0f,   0.0f, 1.0f, 0.0f,			   // Back
            1.0f, 0.0f, 2.0f,   0.0f, 1.0f, 0.0f,
            1.0f, 2.0f, 2.0f,   0.0f, 1.0f, 0.0f,
            0.0f, 2.0f, 2.0f,   0.0f, 1.0f, 0.0f,

           -1.0f, 0.0f, 1.0f,   1.0f, 0.0f, 1.0f,			   // Back Left
            0.0f, 0.0f, 2.0f,   1.0f, 0.0f, 1.0f,
            0.0f, 2.0f, 2.0f,   1.0f, 0.0f, 1.0f,
           -1.0f, 2.0f, 1.0f,   1.0f, 0.0f, 1.0f,

           -1.0f, 0.0f, 1.0f,   0.0f, 0.0f, 0.0f,			   // Front Left
            0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
            0.0f, 2.0f, 0.0f,   0.0f, 0.0f, 0.0f,
           -1.0f, 2.0f, 1.0f,   0.0f, 0.0f, 0.0f
        };

        unsigned int hex_indices[] = {
            0, 1, 2,
            0, 2, 3,
            0, 3, 4,
            0, 4, 5,

            6, 7, 8,
            6, 8, 9,
            6, 9, 10,
            6, 10, 11,

            12, 13, 14,
            12, 14, 15,

            16, 17, 18,
            16, 18, 19,

            20, 21, 22,
            20, 22, 23,

            24, 25, 26,
            24, 26, 27,

            28, 29, 30,
            28, 30, 31,

            32, 33, 34,
            32, 34, 35
        };

        glGenVertexArrays(1, &hexagonVAO);
        glGenVertexArrays(1, &lightCubeVAO);
        glGenVertexArrays(1, &lightTexCubeVAO);
        glGenBuffers(1, &hexagonVBO);
        glGenBuffers(1, &hexagonEBO);


        glBindVertexArray(lightTexCubeVAO);

        glBindBuffer(GL_ARRAY_BUFFER, hexagonVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(hex_vertices), hex_vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hexagonEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(hex_indices), hex_indices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); 
        glEnableVertexAttribArray(0);

        // color attribute by SHIHAB
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
        glEnableVertexAttribArray(1);

        // vertex normal attribute
       // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)12);
       // glEnableVertexAttribArray(1);

        // texture coordinate attribute
       // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)24);
       // glEnableVertexAttribArray(2);


        glBindVertexArray(lightCubeVAO);

        glBindBuffer(GL_ARRAY_BUFFER, hexagonVBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hexagonEBO);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)12);
        //glEnableVertexAttribArray(1);
     
        // color attribute by SHIHAB
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
        glEnableVertexAttribArray(1);


        glBindVertexArray(hexagonVAO);

        glBindBuffer(GL_ARRAY_BUFFER, hexagonVBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hexagonEBO);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // color attribute by SHIHAB
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
        glEnableVertexAttribArray(1);

    }

};


#endif 






