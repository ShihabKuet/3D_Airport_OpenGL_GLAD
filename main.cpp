#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>


#include "shader_m.h"
#include "camera.h"
#include "point_light.h"
#include "cube.h"
#include "Model.h"
#include "BezierCurve.h"
#include "hexagon.h"
#include "HexagonAdvanced.h"
#include "PlaneHand.h"
#include "Frustum.h"



#include <iostream>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void drawGround(Cube &cube, Shader lightingShaderWithTexture, glm::mat4 combinedMatrix);
void drawRunway(Cube &cube, Shader ourShader, glm::mat4 combinedMatrix); 
void drawHelipad(Cube& cube, Shader ourShader, glm::mat4 combinedMatrix);
void drawFroad(Cube& cube, Shader ourShader, glm::mat4 combinedMatrix);
void drawHexagon(Hexagon& hexa, Shader shader, glm::mat4 combinedMatrix);
void drawHexagonAdvanced(HexagonAdvanced& hexa, Shader shader, glm::mat4 combinedMatrix);
void drawHexagonBoundary(Hexagon& hexa, Shader& uniqueColorShader);
void drawHelicopter(BezierCurve& heliBody, BezierCurve planeCone, BezierCurve& heliGlass, BezierCurve cylinder, Cube& wall, Shader& lightingShaderWithTexture, glm::mat4 combinedMatrix);
void drawHelicopterStatic(BezierCurve& heliBody, BezierCurve planeCone, BezierCurve& heliGlass, BezierCurve cylinder, Cube& wall, Shader& lightingShaderWithTexture, glm::mat4 combinedMatrix);
void heliMoveAnimation(BezierCurve& heliBody, BezierCurve planeCone, BezierCurve& heliGlass, BezierCurve cylinder, Cube& wall, Shader& lightingShaderWithTexture, glm::mat4 combinedMatrix, float& heli_r);
void drawPlane(BezierCurve& cylinder, BezierCurve& cone, BezierCurve& fanBody, BezierCurve& wheel, PlaneHand& wing, Cube& fan, Cube& planeBlackGlass, Shader& lightingShaderWithTexture, glm::mat4 combinedMatrix);
void drawPlaneStatic(BezierCurve& cylinder, BezierCurve& cone, BezierCurve& fanBody, BezierCurve& wheel, PlaneHand& wing, Cube& fan, Cube& planeBlackGlass, Shader& lightingShaderWithTexture, glm::mat4 combinedMatrix);
void drawControlTower(Cube& cube, Cube& wall, Cube& signboard_ct, Frustum& frustum, BezierCurve& dishCone, unsigned int VAO, unsigned int VAO_f, Shader ourShader, glm::mat4 comMatrix);
void ControlTowerGroundWindow(Cube& windw, unsigned int VAO, Shader ourShader, glm::mat4 comMatrix);
void airportBuilding(Cube& tilesFloor, Cube& brick, Cube& signboard, HexagonAdvanced& pillar, Shader& lightingShaderWithTexture, Shader& ourShader, glm::mat4 combinedmatrix);
void drawPlaneHouse(Cube& wall, Cube& sidewall, Cube& brick, Shader lightingShaderWithTexture, glm::mat4 combinedMatrix);
void mainGate(Cube& wall, Cube& sidewall, Cube& brick, Shader lightingShaderWithTexture, glm::mat4 combinedMatrix);
void borderWall(Cube& wall, Cube& brick, Shader lightingShaderWithTexture, glm::mat4 combinedMatrix);
void planeAnimation(Cube& demo, Shader ourShader, glm::mat4 comMatrix, float& r, float& x_pilot_eye, float& y_pilot_eye, float& z_pilot_eye);
void planeMoveAnimation(BezierCurve& cylinder, BezierCurve& cone, BezierCurve& fanBody, BezierCurve& wheel, Cube& fan, Cube& planeBlackGlass, PlaneHand& wing, Shader& lightingShaderWithTexture, float& r, float& x_pilot_eye, float& y_pilot_eye, float& z_pilot_eye);
void dayNightConversion(bool &isDay);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadCubemap(vector<std::string> faces);
unsigned int loadTransparentTexture(char const* path);
unsigned int loadTexture(char const* path, GLenum textureWrappingModeS, GLenum textureWrappingModeT, GLenum textureFilteringModeMin, GLenum textureFilteringModeMax);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(-17.0f, 2.0f, 38.0f));  // Position of the camera
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

bool planeMove = false;
bool skyPlaneMove = false;
bool heliMove = false;


float r = 0.0;
float heli_r = 0.0;
float rot = 0.0;
float fanRotate = 0.0;
float heli_fanRotate = 0.0;
float x_pilot_eye = -7.0f, y_pilot_eye = 0.0f, z_pilot_eye = -1.5f;
float x_pilot_lookAt = -4.0f, y_pilot_lookAt = 0.0f, z_pilot_lookAt = -1.5f;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

float timeDay = 0.0;
bool isDay = true;

// LIGHTING
// positions of the point lights
glm::vec3 pointLightPositions[] = {
    glm::vec3(0.0f,  40.0f,  0.0f), // Middle SkyLight
    glm::vec3(-20.8f,  20.0f,  -20.0f), // SKY light Opposite of Gate
    glm::vec3(-4.45f,  7.7f,  2.45f), // Light Top of the tower //-4.45
    glm::vec3(-20.8f, 20.0f,  20.0f), // Sky Light In front of Gate
    glm::vec3(20.0f,  20.0f,  20.0f), // Road Start Top
    glm::vec3(20.0f,  20.0f,  -20.0f),  // Opposite Road Site Top
    glm::vec3(-20.8f, 20.0f,  50.0f)  // Sky Light Top Of AP building
};


//glm::vec3(-0.5, 1, -0.5)


PointLight pointlight1(

    pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    20.0f, 20.0f, 20.0f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    1       // light number
);
PointLight pointlight2(

    pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    30.0f, 30.0f, 30.0f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    2       // light number
);
PointLight pointlight3(

    pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    1.0f, 1.0f, 1.0f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    3       // light number
);
PointLight pointlight4(

    pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    15.0f, 15.0f, 15.0f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    4       // light number
);
PointLight pointlight5(

    pointLightPositions[4].x, pointLightPositions[4].y, pointLightPositions[4].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    30.0f, 30.0f, 30.0f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    5       // light number
);
PointLight pointlight6(

    pointLightPositions[5].x, pointLightPositions[5].y, pointLightPositions[5].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    30.0f, 30.0f, 30.0f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    6       // light number
);

PointLight pointlight7(

    pointLightPositions[6].x, pointLightPositions[6].y, pointLightPositions[6].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    30.0f, 30.0f, 30.0f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    7       // light number
);

// light settings

bool dirLightOn = true;
bool pointLightOn = true;
bool spotLightOn = true;
bool ambientToggle = true;
bool diffuseToggle = true;
bool specularToggle = true;

// LIGHTING END

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "KUET AIRPORT - 1807104", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    

    // build and compile shaders
    // -------------------------
    Shader shader("6.1.cubemaps.vs", "6.1.cubemaps.fs");
    Shader skyboxShader("6.1.skybox.vs", "6.1.skybox.fs");
    Shader lightingShaderWithTexture("LightingShaderWithTexture.vs", "LightingShaderWithTexture.fs");
    Shader ourShader("vertexShader.vs", "fragmentShader.fs");
    Shader uniqueColorShader("vertexShader.vs", "fragmentShaderUniqueColor.fs");
    Shader transparentShader("vertexShaderForBlending.vs", "fragmentShaderForBlending.fs");

    //Shader modelShader("model_loading.vs", "model_loading.fs");

    // load 3D models
    // -----------
    //Model ourModel("backpack.obj");

    

    float cube_vertices[] = {
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,

        0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.0f, 0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,

        0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

        0.0f, 0.0f, 0.5f, 1.0f, 1.0f, 0.0f,
        0.0f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f,

        0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        0.5f, 0.0f, 0.5f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 0.5f, 1.0f, 0.0f, 1.0f
    };
    unsigned int cube_indices[] = {
        0, 3, 2,
        2, 1, 0,

        4, 5, 7,
        7, 6, 4,

        8, 9, 10,
        10, 11, 8,

        12, 13, 14,
        14, 15, 12,

        16, 17, 18,
        18, 19, 16,

        20, 21, 22,
        22, 23, 20
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);


    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);

    // Frustrum
    float frustum_vertices[] = {
        .15f, 0.0f, .15f, 1.0f, 0.0f, 0.0f,
        .35f, 0.0f, .15f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,

        .35f, 0.0f, .15f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        .35f, 0.0f, .35f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,

        .15f, 0.0f, .35f, 0.0f, 0.0f, 1.0f,
        .35f, 0.0f, .35f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

        .15f, 0.0f, .35f, 1.0f, 1.0f, 0.0f,
        0.0f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f,
        .15f, 0.0f, .15f, 1.0f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f,

        .15f, 0.0f, .15f, 1.0f, 0.0f, 1.0f,
        .35f, 0.0f, .15f, 1.0f, 0.0f, 1.0f,
        .35f, 0.0f, .35f, 1.0f, 0.0f, 1.0f,
        .15f, 0.0f, .35f, 1.0f, 0.0f, 1.0f
    };
    unsigned int frustum_indices[] = {
        0, 3, 2,
        2, 1, 0,

        4, 5, 7,
        7, 6, 4,

        8, 9, 10,
        10, 11, 8,

        12, 13, 14,
        14, 15, 12,

        16, 17, 18,
        18, 19, 16,

        20, 21, 22,
        22, 23, 20
    };

    unsigned int VBO_f, VAO_f, EBO_f;
    glGenVertexArrays(1, &VAO_f);
    glGenBuffers(1, &VBO_f);
    glGenBuffers(1, &EBO_f);

    glBindVertexArray(VAO_f);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_f);
    glBufferData(GL_ARRAY_BUFFER, sizeof(frustum_vertices), frustum_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_f);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(frustum_indices), frustum_indices, GL_STATIC_DRAW);


    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);

    
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };


    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


    // Transparent Window
    float transparentVertices[] = {
        // positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
        1.0f,  0.5f,  0.0f,  1.0f,  0.0f
    };

    // transparent VAO
    unsigned int transparentVAO, transparentVBO;
    glGenVertexArrays(1, &transparentVAO);
    glGenBuffers(1, &transparentVBO);
    glBindVertexArray(transparentVAO);
    glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    string Path = "blending_transparent_window.png";
    unsigned int transparentTexture = loadTransparentTexture(Path.c_str());

    // For Lighting

    float lcube_vertices[] = {
        // positions      // normals
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f,
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f,

        0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

        0.0f, 0.0f, 0.5f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,

        0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        0.5f, 0.0f, 0.5f, 0.0f, -1.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f, -1.0f, 0.0f
    };
    unsigned int lcube_indices[] = {
        0, 3, 2,
        2, 1, 0,

        4, 5, 7,
        7, 6, 4,

        8, 9, 10,
        10, 11, 8,

        12, 13, 14,
        14, 15, 12,

        16, 17, 18,
        18, 19, 16,

        20, 21, 22,
        22, 23, 20
    };

    unsigned int lcubeVAO, lcubeVBO, lcubeEBO;
    glGenVertexArrays(1, &lcubeVAO);
    glGenBuffers(1, &lcubeVBO);
    glGenBuffers(1, &lcubeEBO);

    glBindVertexArray(lcubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, lcubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lcube_vertices), lcube_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lcubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(lcube_indices), lcube_indices, GL_STATIC_DRAW);


    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // vertex normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);

    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, lcubeVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lcubeEBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Lighting Part END



    // load textures
    // -------------
    //unsigned int cubeTexture = loadTexture("image.jpg");

    vector<std::string> faces
    {
        "Daylight Box_Right.bmp",
        "Daylight Box_Left.bmp",
        "Daylight Box_Top.bmp",
        "Daylight Box_Bottom.bmp",
        "Daylight Box_Front.bmp",
        "Daylight Box_Back.bmp"
    };
    unsigned int cubemapTexture = loadCubemap(faces);

    vector<std::string> facesNight
    {
        "Daylight Box_Right_Night.jpg",
        "Daylight Box_Left_Night.jpg",
        "Daylight Box_Top_Night.jpg",
        "Daylight Box_Bottom_Night.jpg",
        "Daylight Box_Front_Night.jpg",
        "Daylight Box_Back_Night.jpg"
    };
    unsigned int cubemapNightTexture = loadCubemap(facesNight);

    GLfloat ctrlCylinderpoints[] =
    {
        -2.0f, 4.0f, 0.0f,
        -2.2f, 2.0f, 0.0f,
        -2.0f, 0.0f, 0.0f
    };
    BezierCurve cylinder = BezierCurve(ctrlCylinderpoints, 9);

    GLfloat ctrlHeliBodypoints[] =
    {
        -2.0f, 4.0f, 0.0f,
        -2.2f, 2.0f, 0.0f,
        -2.0f, 0.0f, 0.0f,
        0.001, 0.0f, 0.0f
    };


    GLfloat ctrlWheelpoints[] =
    {
        0.001f, 4.0f, 0.0f,
        -2.0f, 3.9f, 0.0f,
       -2.0f, 4.0f, 0.0f,
        -2.0f, 0.0f, 0.0f,
        -2.0f, 0.1f, 0.0f,
        0.001f, 0.0f, 0.0f
    };
    BezierCurve wheel = BezierCurve(ctrlWheelpoints, 18);

    GLfloat ctrlConepoints[] =
    {
         0.001f, 4.5f, 0.0f,
        -2.0f, 3.0f, 0.0f,
        -2.0f, 0.0f, 0.0f
    };
    BezierCurve cone = BezierCurve(ctrlConepoints, 9);

    GLfloat ctrlFanBodypoints[] =
    {
        -2.0f, 8.0f, 0.0f,
        -2.0f, 1.0f, 0.0f,
        0.001f, 0.0f, 0.0f
    };
    BezierCurve fanBody = BezierCurve(ctrlFanBodypoints, 9);

    // shader configuration
    // --------------------
    shader.use();
    shader.setInt("texture1", 0);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    // Ground
    string diffuseGroundPath = "airport_grass2.jpg";
    string specularGroundPath = "airport_grass2.jpg";
    unsigned int diffGroudnMap = loadTexture(diffuseGroundPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specGroundMap = loadTexture(specularGroundPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube grass = Cube(diffGroudnMap, specGroundMap, 32.0f, 0.0f, 0.0f, 8.0f, 13.0f);

    // Runway
    string diffuseRoadPath = "road.jpg";
    string specularRoadPath = "road.jpg";
    unsigned int diffRoadMap = loadTexture(diffuseRoadPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_NEAREST);
    unsigned int specRoadMap = loadTexture(specularRoadPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_NEAREST);
    Cube road = Cube(diffRoadMap, specRoadMap, 32.0f, 0.0f, 0.0f, 10.0f, 1.0f);

    // Outer Road
    string diffuseORoadPath = "road_texture.jpg";
    string specularORoadPath = "road_texture.jpg";
    unsigned int diffORoadMap = loadTexture(diffuseORoadPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_NEAREST);
    unsigned int specORoadMap = loadTexture(specularORoadPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_NEAREST);
    Cube outer_road = Cube(diffORoadMap, specORoadMap, 32.0f, 0.0f, 0.0f, 20.0f, 1.0f);

    // Brick 
    string diffuseBrickPath = "brick.jpg";
    string specularBrickPath = "brick.jpg";
    unsigned int diffBrickMap = loadTexture(diffuseBrickPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specBrickMap = loadTexture(specularBrickPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube brick = Cube(diffBrickMap, specBrickMap, 32.0f, 0.0f, 0.0f, 2.0f, 2.0f);
    
    // Control Tower GF
    string ctgfDiffuseMapPath = "control_tower_gf.jpg";
    string ctgfSpecularMapPath = "control_tower_gf.jpg";
    unsigned int ctgfdiffMap = loadTexture(ctgfDiffuseMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int ctgfspecMap = loadTexture(ctgfSpecularMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube ctower_gfloor = Cube(ctgfdiffMap, ctgfspecMap, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    // Control Tower SF
    string ctsfDiffuseMapPath = "control_tower_sf.jpg";
    string ctsfSpecularMapPath = "control_tower_sf.jpg";
    unsigned int ctsfdiffMap = loadTexture(ctsfDiffuseMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int ctsfspecMap = loadTexture(ctsfSpecularMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube ctower_sfloor = Cube(ctsfdiffMap, ctsfspecMap, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    // Wall
    string wallDiffuseMapPath = "wall.jpg";
    string wallSpecularMapPath = "wall.jpg";
    unsigned int walldiffMap = loadTexture(wallDiffuseMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int wallspecMap = loadTexture(wallSpecularMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube wall = Cube(walldiffMap, wallspecMap, 32.0f, 0.0f, 0.0f, 2.0f, 2.0f);

    // Airport Tiles Floor
    string tfloorDiffuseMapPath = "tiles_floor.jpg";
    string tfloorSpecularMapPath = "tiles_floor.jpg";
    unsigned int tfloordiffMap = loadTexture(tfloorDiffuseMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int tfloorspecMap = loadTexture(tfloorSpecularMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube tilesFloor = Cube(tfloordiffMap, tfloorspecMap, 32.0f, 0.0f, 0.0f, 8.0f, 4.0f);


    // Window
    string windowDiffuseMapPath = "window.jpg";
    string windowSpecularMapPath = "window.jpg";
    unsigned int WindowdiffMap = loadTexture(windowDiffuseMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int WindowspecMap = loadTexture(windowSpecularMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube windw = Cube(WindowdiffMap, WindowspecMap, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    // KUET wall
    string KUETdiffuseMapPath = "wall_kuet.jpg";
    string KUETspecularMapPath = "wall_kuet.jpg";
    unsigned int KUETdiffMap = loadTexture(KUETdiffuseMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int KUETspecMap = loadTexture(KUETspecularMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube sidewall = Cube(KUETdiffMap, KUETspecMap, 32.0f, 0.0f, 0.0f, 2.0f, 1.0f);

    // Helipad
    string HelipaddiffuseMapPath = "helipad.jpg";
    string HelipadspecularMapPath = "helipad.jpg";
    unsigned int HelidiffMap = loadTexture(HelipaddiffuseMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int HelispecMap = loadTexture(HelipadspecularMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube heli = Cube(HelidiffMap, HelispecMap, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    // Floor Road
    string FRdiffuseMapPath = "floor_road.jpg";
    string FRspecularMapPath = "floor_road.jpg";
    unsigned int FRdiffMap = loadTexture(FRdiffuseMapPath.c_str(), GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int FRspecMap = loadTexture(FRspecularMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube froad = Cube(FRdiffMap, FRspecMap, 32.0f, 0.0f, 0.0f, 8.0f, 4.0f);

    // Signboard
    string SBdiffuseMapPath = "signboard.jpg";
    string SBspecularMapPath = "signboard.jpg";
    unsigned int SBdiffMap = loadTexture(SBdiffuseMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int SBspecMap = loadTexture(SBspecularMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube signboard = Cube(SBdiffMap, SBspecMap, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    // Signboard
    string SBCTdiffuseMapPath = "signboard_ct.jpg";
    string SBCTspecularMapPath = "signboard_ct.jpg";
    unsigned int SBCTdiffMap = loadTexture(SBCTdiffuseMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int SBCTspecMap = loadTexture(SBCTspecularMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube signboard_ct = Cube(SBCTdiffMap, SBCTspecMap, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    // Wall Border
    SBdiffuseMapPath = "wall_border.jpg";
    SBspecularMapPath = "wall_border.jpg";
    SBdiffMap = loadTexture(SBdiffuseMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    SBspecMap = loadTexture(SBspecularMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube wall_border = Cube(SBdiffMap, SBspecMap, 32.0f, 0.0f, 0.0f, 3.0f, 1.0f);

    // Plane Black Glass
    SBdiffuseMapPath = "plane_black_glass.jpg";
    SBspecularMapPath = "plane_black_glass.jpg";
    SBdiffMap = loadTexture(SBdiffuseMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    SBspecMap = loadTexture(SBspecularMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube planeBlackGlass = Cube(SBdiffMap, SBspecMap, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    // Helicopter Black Glass
    BezierCurve heliGlass = BezierCurve(ctrlConepoints, 9, SBdiffMap, SBspecMap);

    //PlaneHand
    string WingdiffuseMapPath = "planewing.jpg";
    string WingspecularMapPath = "planewing.jpg";
    SBdiffMap = loadTexture(WingdiffuseMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    SBspecMap = loadTexture(WingspecularMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    PlaneHand wing = PlaneHand(SBdiffMap, SBspecMap, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    // Plane Body
    string diffuseMapPath = "plane_body.jpg";
    string specularMapPath = "plane_body.jpg";
    unsigned int planeBodydiffMap = loadTexture(diffuseMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int planeBodyspecMap = loadTexture(specularMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    BezierCurve planeBody = BezierCurve(ctrlCylinderpoints, 9, planeBodydiffMap, planeBodyspecMap);


    // Plane Body Gradient
    diffuseMapPath = "plane_body_gradient.jpg";
    specularMapPath = "plane_body_gradient.jpg";
    unsigned int planeBodyGdiffMap = loadTexture(diffuseMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int planeBodyGspecMap = loadTexture(specularMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    BezierCurve planeBodyGradient = BezierCurve(ctrlCylinderpoints, 9, planeBodyGdiffMap, planeBodyGspecMap);

    // Helicopter Body
    BezierCurve heliBody = BezierCurve(ctrlHeliBodypoints, 12, planeBodyGdiffMap, planeBodyGspecMap);
    

    // Plane Cone
    BezierCurve planeConeTex = BezierCurve(ctrlConepoints, 9, planeBodyGdiffMap, planeBodyGspecMap);

    // Plane Tail
    diffuseMapPath = "plane_tail.jpg";
    specularMapPath = "plane_tail.jpg";
    unsigned int planeTaildiffMap = loadTexture(diffuseMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int planeTailspecMap = loadTexture(specularMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    BezierCurve planeCone = BezierCurve(ctrlConepoints, 9, planeTaildiffMap, planeTailspecMap);

    // Plane Wheel
    diffuseMapPath = "plane_wheel.jpg";
    specularMapPath = "plane_wheel.jpg";
    unsigned int planeWheeldiffMap = loadTexture(diffuseMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int planeWheelspecMap = loadTexture(specularMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    BezierCurve planeWheelTex = BezierCurve(ctrlWheelpoints, 18, planeWheeldiffMap, planeWheelspecMap);

    // FanBody
    BezierCurve fanBodyCone = BezierCurve(ctrlFanBodypoints, 9, planeTaildiffMap, planeTailspecMap);

    // Front
    BezierCurve planeFront = BezierCurve(ctrlConepoints, 9, planeTaildiffMap, planeTailspecMap);

    // Dish
    diffuseMapPath = "dish_antenna.jpg";
    specularMapPath = "dish_antenna.jpg";
    unsigned int DishdiffMap = loadTexture(diffuseMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int DishspecMap = loadTexture(specularMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    BezierCurve dishCone = BezierCurve(ctrlConepoints, 9, DishdiffMap, DishspecMap);



    // Hexagon
    uniqueColorShader.use(); 
    Hexagon hexa = Hexagon();
    
    // Hex Advanced   
    HexagonAdvanced hexAd = HexagonAdvanced(walldiffMap, specBrickMap, 32.0f, 0.0f, 0.0f, 2.0f, 2.0f);

    // Pillar
    string pillarDiffuseMapPath = "pillar.jpg";
    string pillarSpecularMapPath = "pillar.jpg";
    unsigned int pillardiffMap = loadTexture(pillarDiffuseMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int pillarspecMap = loadTexture(pillarSpecularMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    HexagonAdvanced pillar = HexagonAdvanced(pillardiffMap, pillarspecMap, 32.0f, 0.0f, 0.0f, 2.0f, 2.0f);

    // Frustum
    diffuseMapPath = "frustum_texture.jpg";
    specularMapPath = "frustum_texture.jpg";
    unsigned int frustumdiffMap = loadTexture(diffuseMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int frustumspecMap = loadTexture(specularMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Frustum frust = Frustum(frustumdiffMap, frustumspecMap, 32.0f, 0.0f, 0.0f, 1.0f, 10.0f);


    // Curve Objects with Texture
    // 
   // BeizerCurve cylinder = BeizerCurve(SBdiffMap, SBspecMap, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    GLfloat ctrlCpoints[] =
    {
        -2.0f, 4.0f, 0.0f,
        -2.2f, 2.0f, 0.0f,
        -2.0f, 0.0f, 0.0f
    };
    BezierCurve cylinder2 = BezierCurve(ctrlCpoints, 9, SBdiffMap, SBdiffMap);




    // Model Loading....
    // render the loaded model
    glm::mat4 model_load = glm::mat4(1.0f);
    model_load = glm::translate(model_load, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    model_load = glm::scale(model_load, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
    //modelShader.setMat4("model", model_load);
   // ourModel.Draw(modelShader);

    


    // For window shader config
    transparentShader.use();
    transparentShader.setInt("texture1", 0);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw scene as normal
        shader.use();
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);


        // point light 1
        pointlight1.setUpPointLight(lightingShaderWithTexture);
        // point light 2
        pointlight2.setUpPointLight(lightingShaderWithTexture);
        // point light 3
        pointlight3.setUpPointLight(lightingShaderWithTexture);
        // point light 4
        pointlight4.setUpPointLight(lightingShaderWithTexture);
        // point light 5
        pointlight5.setUpPointLight(lightingShaderWithTexture);
        // point light 6
        pointlight6.setUpPointLight(lightingShaderWithTexture);
        // point light 7
        pointlight7.setUpPointLight(lightingShaderWithTexture);

        // be sure to activate shader when setting uniforms/drawing objects
        lightingShaderWithTexture.use();
        lightingShaderWithTexture.setVec3("viewPos", camera.Position);

        // pass projection matrix to shader (note that in this case it could change every frame)
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        //glm::mat4 projection = glm::ortho(-2.0f, +2.0f, -1.5f, +1.5f, 0.1f, 100.0f);
        lightingShaderWithTexture.setMat4("projection", projection);

        // camera/view transformation
        view = camera.GetViewMatrix();
        lightingShaderWithTexture.setMat4("view", view);

        //END


        // Modelling Here
        glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 translateMatrix, rotateMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix ,transformMatrix;
        glm::mat3 unitMatrix = glm::mat3(1.0f);

        translateMatrix = identityMatrix;

        lightingShaderWithTexture.use();

        // Ground Field
        drawGround(grass, lightingShaderWithTexture, identityMatrix);

        // Runway
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0325, 1.0f, 2.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.04f, 0.0f));
        model = translateMatrix * scaleMatrix;
        drawRunway(road, lightingShaderWithTexture, model); // Runway 1

        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.04f, -8.0f));
        model = translateMatrix * scaleMatrix;
        drawRunway(road, lightingShaderWithTexture, model);  // Runway 2

        transformMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 1.0f, 0.7f)) * glm::translate(identityMatrix, glm::vec3(-16.0f, 0.04f, 2.5f)) * glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        drawRunway(road, lightingShaderWithTexture, transformMatrix);  // Runway Road


        // Draw Outer Road
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.2, 1.0f, 1.25f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.5f, 0.04f, 16.35f));
        model = translateMatrix * scaleMatrix;
        drawRunway(outer_road, lightingShaderWithTexture, model); // Outer ROad

        // Draw Helipad
        drawHelipad(heli, lightingShaderWithTexture, identityMatrix);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(5.0f, 0.0f, 0.0f));
        drawHelipad(heli, lightingShaderWithTexture, translateMatrix);

        // Draw Floor Road
        drawFroad(froad, lightingShaderWithTexture, identityMatrix);

        // Control Tower
        drawControlTower(ctower_gfloor, ctower_sfloor, signboard_ct, frust, dishCone, VAO, VAO_f, lightingShaderWithTexture, identityMatrix);


        // Plane House
        drawPlaneHouse(sidewall, sidewall, brick, lightingShaderWithTexture, identityMatrix);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0f, 0.0f, 0.0f));
        drawPlaneHouse(sidewall, sidewall, brick, lightingShaderWithTexture, translateMatrix);


        // Main Gate
       // mainGate(wall, signboard, brick, lightingShaderWithTexture, identityMatrix);

        // Border Wall
        borderWall(wall_border, brick, lightingShaderWithTexture, identityMatrix);

        // Plane 1
        planeMoveAnimation(planeBody, planeCone, fanBodyCone, planeWheelTex, wall, planeBlackGlass, wing,  lightingShaderWithTexture, r, x_pilot_eye, y_pilot_eye, z_pilot_eye);


        // Plane 2
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3, 0.3f, 0.3f));
        rotateMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.6f, -0.44f, 4.0f));
        model = translateMatrix * rotateMatrix * scaleMatrix;
        drawPlaneStatic(planeBodyGradient, planeConeTex, fanBodyCone, planeWheelTex, wing, wall, planeBlackGlass, lightingShaderWithTexture, model);


        // Plane on the SKY
        if (skyPlaneMove)
        {
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3, 0.3f, 0.3f));
            rotateMatrix = glm::rotate(identityMatrix, glm::radians(rot + 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-(rot/100) + 4.6f, 28.44f, -15.0f));
            model = rotateMatrix * translateMatrix * scaleMatrix;
            drawPlane(planeBodyGradient, planeConeTex, fanBodyCone, planeWheelTex, wing, wall, planeBlackGlass, lightingShaderWithTexture, model);
            rot = rot - 0.05;
        }
        
        // Helicopter
        //drawHelicopter(heliBody, planeCone, planeBody, road, lightingShaderWithTexture, identityMatrix);
        heliMoveAnimation(heliBody, planeCone, heliGlass, planeBody, road, lightingShaderWithTexture, identityMatrix, heli_r);

        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.7, 0.7f, 0.7f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(16.5f, -0.2f, 2.0f));
        model = translateMatrix * scaleMatrix;
        drawHelicopterStatic(heliBody, planeCone, heliGlass, planeBody, road, lightingShaderWithTexture, model); 


        // Hexagon
        uniqueColorShader.use();
        uniqueColorShader.setMat4("projection", projection);
        uniqueColorShader.setMat4("view", view);
       
        drawHexagonBoundary(hexa, uniqueColorShader);

        // Airport Building
        airportBuilding(tilesFloor, brick, signboard, pillar,lightingShaderWithTexture, ourShader, identityMatrix);

        scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.2, 1.0f, 2.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.0f, 0.04f, 31.0f));
        model = translateMatrix * scaleMatrix;
        drawRunway(outer_road, lightingShaderWithTexture, model); // Outer Road



        // Draft Light
        ourShader.use();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        // we now draw as many light bulbs as we have point lights.
        glBindVertexArray(lightCubeVAO);
        for (unsigned int i = 0; i < 7; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.4f)); // Make it a smaller cube
            ourShader.setMat4("model", model);
            ourShader.setVec4("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            //glDrawArrays(GL_TRIANGLES, 0, 36);
        }


        

        

        // Transparent Window
        transparentShader.use();
        transparentShader.setMat4("projection", projection);
        transparentShader.setMat4("view", view);
        //brick.drawCubeTransparentWithTexture(transparentShader, transparentVAO, transparentTexture, identityMatrix);

        model = glm::mat4(1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.0f, 1.0f, -10.0f));
        transparentShader.setMat4("model", translateMatrix);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBindVertexArray(transparentVAO);
        glBindTexture(GL_TEXTURE_2D, transparentTexture);
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glDisable(GL_BLEND);
 


        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        if (isDay) // Just for Night Day Conversion
        {
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        else
        {
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapNightTexture);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        
        
        dayNightConversion(isDay);    
        


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------

    glDeleteVertexArrays(1, &skyboxVAO);

    glDeleteBuffers(1, &skyboxVBO);

    glfwTerminate();
    return 0;
}

void dayNightConversion(bool &isDay)
{
    cout << timeDay << " ";
    timeDay = timeDay + 0.001;
    if (timeDay > 10.0)
    {
        //pointlight1.turnOff();
        //pointlight2.turnOff();
       // pointlight3.turnOff();
        //pointlight4.turnOff();
        //pointlight5.turnOff();
        //pointlight6.turnOff();
        pointlight1.diffuse = glm::vec3(5.0f, 5.0f, 5.0f);
        pointlight2.diffuse = glm::vec3(5.0f, 5.0f, 5.0f);
        pointlight3.diffuse = glm::vec3(5.0f, 5.0f, 5.0f);
        pointlight4.diffuse = glm::vec3(5.0f, 5.0f, 5.0f);
        pointlight5.diffuse = glm::vec3(5.0f, 5.0f, 5.0f);
        pointlight6.diffuse = glm::vec3(5.0f, 5.0f, 5.0f);
        isDay = false;
    }
    if (timeDay < 10.0)
    {
        //pointlight1.turnOn();
        //pointlight2.turnOn();
        //pointlight3.turnOn();
        //pointlight4.turnOn();
        //pointlight5.turnOn();
        //pointlight6.turnOn();
        pointlight1.diffuse = glm::vec3(30.0f, 30.0f, 30.0f);
        pointlight2.diffuse = glm::vec3(30.0f, 30.0f, 30.0f);
        pointlight3.diffuse = glm::vec3(0.0f, 0.0f, 0.0f);
        pointlight4.diffuse = glm::vec3(30.0f, 30.0f, 30.0f);
        pointlight5.diffuse = glm::vec3(30.0f, 30.0f, 30.0f);
        pointlight6.diffuse = glm::vec3(30.0f, 30.0f, 30.0f);
        isDay = true;
    }
    if (timeDay > 20.0)
    {
        timeDay = 0.0;
    }
}

void drawHelicopter(BezierCurve& heliBody, BezierCurve planeCone, BezierCurve& heliGlass, BezierCurve cylinder, Cube& wall, Shader& lightingShaderWithTexture, glm::mat4 combinedMatrix)
{
    glm::mat4 scale2Matrix, translateMatrix, translateOrigin, translateOriginBack, rotateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
    glm::mat4 identityMatrix = glm::mat4(1.0f);

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3, 0.2f, 0.3f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-8.0f, 0.0f, 3.0f));
    model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    planeCone.drawBezierCurveWithTexture(lightingShaderWithTexture, model, glm::vec3(1.0f, 1.0f, 1.0f)); // Back Body

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3, 0.3f, 0.3f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-8.0f, 0.0f, 2.0f));
    model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    heliBody.drawBezierCurveWithTexture(lightingShaderWithTexture, model, glm::vec3(1.0f, 1.0f, 1.0f)); //Front Body

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.25, 0.06f, 0.2f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-8.0f, 0.2f, 2.3f));
    model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    heliGlass.drawBezierCurveWithTexture(lightingShaderWithTexture, model, glm::vec3(1.0f, 1.0f, 1.0f)); //Glass

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3, 0.3f, 2.7f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-8.1f, 0.0f, 3.7f));
    model = combinedMatrix * translateMatrix * scaleMatrix;
    wall.drawCubeWithTexture(lightingShaderWithTexture, model); // Tail

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3, 1.2f, 0.3f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-8.1f, -0.2f, 4.9f));
    model = combinedMatrix * translateMatrix * scaleMatrix;
    wall.drawCubeWithTexture(lightingShaderWithTexture, model); // Tail End

    // Left Fan Blade
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 4.0f, 0.55f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-6.95f, 1.0f, 2.60f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(heli_fanRotate + 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    translateOrigin = glm::translate(identityMatrix, glm::vec3(0.0f, -1.0f, 0.0f));
    translateOriginBack = glm::translate(identityMatrix, glm::vec3(0.0f, 1.0f, 0.0f));
    scale2Matrix = glm::scale(identityMatrix, glm::vec3(8.2f, 1.0f, 1.0f));
    model = combinedMatrix * translateMatrix  * rotateMatrix * translateOriginBack * rotateXMatrix * translateOrigin * scaleMatrix;
    wall.drawCubeWithTexture(lightingShaderWithTexture, model);  // fan x axis

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 4.0f, 0.5f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.55f, 0.5f, 1.38f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(heli_fanRotate + 90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    translateOrigin = glm::translate(identityMatrix, glm::vec3(0.0f, -1.0f, 0.0f));
    translateOriginBack = glm::translate(identityMatrix, glm::vec3(0.0f, 1.0f, 0.0f));
    scale2Matrix = glm::scale(identityMatrix, glm::vec3(1.0f, 1.0f, 8.2f));
    model = combinedMatrix * translateMatrix * translateMatrix * rotateMatrix * translateOriginBack * rotateXMatrix * translateOrigin * scaleMatrix;
    wall.drawCubeWithTexture(lightingShaderWithTexture, model);  // fan z axis

    if (heliMove)
    {
        //fanRotate = fanRotate + 20;
        heli_fanRotate = heli_fanRotate + 20;
    }

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.01f, 0.15f, 0.01f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.98f, 0.2f, 1.38f));
    model = combinedMatrix * translateMatrix * translateMatrix *  scaleMatrix;
    cylinder.drawBezierCurveWithTexture(lightingShaderWithTexture, model, glm::vec3(1.0f, 1.0f, 1.0f));  // fan connector

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 0.2f, 3.7f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-8.65f, 0.0f, 2.0f));
    model = combinedMatrix * translateMatrix * scaleMatrix;
    wall.drawCubeWithTexture(lightingShaderWithTexture, model); //LU

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 0.2f, 3.7f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-8.85f, -0.88f, 2.0f));
    model = combinedMatrix * translateMatrix * scaleMatrix;
    wall.drawCubeWithTexture(lightingShaderWithTexture, model); //LD

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 0.2f, 3.7f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-7.4f, 0.0f, 2.0f));
    model = combinedMatrix * translateMatrix * scaleMatrix;
    wall.drawCubeWithTexture(lightingShaderWithTexture, model); //RU

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 0.2f, 3.7f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-7.2f, -0.88f, 2.0f));
    model = combinedMatrix * translateMatrix * scaleMatrix;
    wall.drawCubeWithTexture(lightingShaderWithTexture, model); //RD

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 1.9f, 0.3f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-7.2f, -0.88f, 2.2f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(12.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    wall.drawCubeWithTexture(lightingShaderWithTexture, model); //RR

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 1.9f, 0.3f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-7.2f, -0.88f, 3.4f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(12.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    wall.drawCubeWithTexture(lightingShaderWithTexture, model); //RL

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 1.9f, 0.3f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-8.85f, -0.88f, 2.2f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(-12.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    wall.drawCubeWithTexture(lightingShaderWithTexture, model); //LL

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 1.9f, 0.3f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-8.85f, -0.88f, 3.4f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(-12.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    wall.drawCubeWithTexture(lightingShaderWithTexture, model); //LR


}

void drawHelicopterStatic(BezierCurve& heliBody, BezierCurve planeCone, BezierCurve& heliGlass, BezierCurve cylinder, Cube& wall, Shader& lightingShaderWithTexture, glm::mat4 combinedMatrix)
{
    glm::mat4 scale2Matrix, translateMatrix, translateOrigin, translateOriginBack, rotateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
    glm::mat4 identityMatrix = glm::mat4(1.0f);

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3, 0.2f, 0.3f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-8.0f, 0.0f, 3.0f));
    model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    planeCone.drawBezierCurveWithTexture(lightingShaderWithTexture, model, glm::vec3(1.0f, 1.0f, 1.0f));

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3, 0.3f, 0.3f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-8.0f, 0.0f, 2.0f));
    model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    heliBody.drawBezierCurveWithTexture(lightingShaderWithTexture, model, glm::vec3(1.0f, 1.0f, 1.0f));

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.25, 0.06f, 0.25f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(-70.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-8.0f, 0.2f, 2.3f));
    model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    heliGlass.drawBezierCurveWithTexture(lightingShaderWithTexture, model, glm::vec3(1.0f, 1.0f, 1.0f)); //Glass

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3, 0.3f, 2.7f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-8.1f, 0.0f, 3.7f));
    model = combinedMatrix * translateMatrix * scaleMatrix;
    wall.drawCubeWithTexture(lightingShaderWithTexture, model); // Tail

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3, 1.2f, 0.3f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-8.1f, -0.2f, 4.9f));
    model = combinedMatrix * translateMatrix * scaleMatrix;
    wall.drawCubeWithTexture(lightingShaderWithTexture, model); // Tail End

    // Left Fan Blade
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 4.0f, 0.55f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-6.95f, 1.0f, 2.60f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians( 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    translateOrigin = glm::translate(identityMatrix, glm::vec3(0.0f, -1.0f, 0.0f));
    translateOriginBack = glm::translate(identityMatrix, glm::vec3(0.0f, 1.0f, 0.0f));
    scale2Matrix = glm::scale(identityMatrix, glm::vec3(8.2f, 1.0f, 1.0f));
    model = combinedMatrix * translateMatrix * rotateMatrix * translateOriginBack * rotateXMatrix * translateOrigin * scaleMatrix;
    wall.drawCubeWithTexture(lightingShaderWithTexture, model);  // fan x axis

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 4.0f, 0.5f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.55f, 0.5f, 1.38f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians( 90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    translateOrigin = glm::translate(identityMatrix, glm::vec3(0.0f, -1.0f, 0.0f));
    translateOriginBack = glm::translate(identityMatrix, glm::vec3(0.0f, 1.0f, 0.0f));
    scale2Matrix = glm::scale(identityMatrix, glm::vec3(1.0f, 1.0f, 8.2f));
    model = combinedMatrix * translateMatrix * translateMatrix * rotateMatrix * translateOriginBack * rotateXMatrix * translateOrigin * scaleMatrix;
    wall.drawCubeWithTexture(lightingShaderWithTexture, model);  // fan z axis

    if (heliMove)
    {
        //fanRotate = fanRotate + 20;
        heli_fanRotate = heli_fanRotate + 20;
    }

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.01f, 0.15f, 0.01f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.98f, 0.2f, 1.38f));
    model = combinedMatrix * translateMatrix * translateMatrix * scaleMatrix;
    cylinder.drawBezierCurveWithTexture(lightingShaderWithTexture, model, glm::vec3(1.0f, 1.0f, 1.0f));  // fan connector

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 0.2f, 3.7f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-8.65f, 0.0f, 2.0f));
    model = combinedMatrix * translateMatrix * scaleMatrix;
    wall.drawCubeWithTexture(lightingShaderWithTexture, model); //LU

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 0.2f, 3.7f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-8.85f, -0.88f, 2.0f));
    model = combinedMatrix * translateMatrix * scaleMatrix;
    wall.drawCubeWithTexture(lightingShaderWithTexture, model); //LD

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 0.2f, 3.7f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-7.4f, 0.0f, 2.0f));
    model = combinedMatrix * translateMatrix * scaleMatrix;
    wall.drawCubeWithTexture(lightingShaderWithTexture, model); //RU

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 0.2f, 3.7f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-7.2f, -0.88f, 2.0f));
    model = combinedMatrix * translateMatrix * scaleMatrix;
    wall.drawCubeWithTexture(lightingShaderWithTexture, model); //RD

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 1.9f, 0.3f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-7.2f, -0.88f, 2.2f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(12.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    wall.drawCubeWithTexture(lightingShaderWithTexture, model); //RR

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 1.9f, 0.3f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-7.2f, -0.88f, 3.4f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(12.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    wall.drawCubeWithTexture(lightingShaderWithTexture, model); //RL

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 1.9f, 0.3f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-8.85f, -0.88f, 2.2f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(-12.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    wall.drawCubeWithTexture(lightingShaderWithTexture, model); //LL

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 1.9f, 0.3f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-8.85f, -0.88f, 3.4f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(-12.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    wall.drawCubeWithTexture(lightingShaderWithTexture, model); //LR
}

void heliMoveAnimation(BezierCurve& heliBody, BezierCurve planeCone, BezierCurve& heliGlass, BezierCurve cylinder, Cube& wall, Shader& lightingShaderWithTexture, glm::mat4 combinedMatrix, float& heli_r)
{
    if (heliMove)
    {
        glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model, model_i, extra;
        identityMatrix = glm::mat4(1.0f);
        extra = identityMatrix;
        float x_init = 0.0;
        float y_init = 0.0;

        if (heli_r >= 12.0f)
        {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(x_init, 10, - (heli_r - 12.0f)));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(-((heli_r * 2.0f)-12.0f)), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        else if (heli_r >= 10.0f)
        {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(x_init, 10.0f, 0.0f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        }

        else
        {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(x_init, heli_r + y_init, 0.0f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        }

        model = rotateYMatrix * translateMatrix;

        drawHelicopter(heliBody, planeCone, heliGlass, cylinder, wall, lightingShaderWithTexture, model);


        heli_r = heli_r + 0.005;
        cout << heli_r << " ";

    }
    else
    {
        glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
        identityMatrix = glm::mat4(1.0f);

        model = translateMatrix * scaleMatrix;

        drawHelicopter(heliBody, planeCone, heliGlass, cylinder,  wall, lightingShaderWithTexture, identityMatrix);
    }
}

void drawPlane(BezierCurve& cylinder, BezierCurve& cone, BezierCurve& fanBody, BezierCurve& wheel, PlaneHand& wing, Cube& fan, Cube& planeBlackGlass, Shader& lightingShaderWithTexture, glm::mat4 combinedMatrix)
{
    glm::mat4 translateMatrix, translateOrigin, translateOriginBack, rotateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, 1.0f, 0.5f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.5f, 0.0f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    //cylinder.drawBezierCurve(lightingShaderWithTexture, model); // Middle Body
    cylinder.drawBezierCurveWithTexture(lightingShaderWithTexture, model, glm::vec3(0.6, 0.6f, 0.6f)); // Middle Body

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, 0.5f, 0.5f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.5f, 0.0f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    cone.drawBezierCurveWithTexture(lightingShaderWithTexture, model, glm::vec3(0.6, 0.6f, 0.6f)); // Front Side

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, 1.0f, 0.5f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.0f, 0.5f, 0.0f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    cone.drawBezierCurveWithTexture(lightingShaderWithTexture, model, glm::vec3(0.6, 0.6f, 0.6f));  // Back Side

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 0.3f, 1.2f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.8f, 0.4f, 0.95f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    wing.drawPlaneHandWithTexture(lightingShaderWithTexture, model);  // Right WIng front


    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.8f, 0.5f, -0.95f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = combinedMatrix * translateMatrix * rotateYMatrix * rotateMatrix * scaleMatrix;
    wing.drawPlaneHandWithTexture(lightingShaderWithTexture, model);  // Left WIng front

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, 0.2f, 0.8f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-6.8f, 0.4f, 0.3f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    wing.drawPlaneHandWithTexture(lightingShaderWithTexture, model);  // Right WIng back

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-6.82f, 0.5f, -0.28f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = combinedMatrix * translateMatrix * rotateYMatrix * rotateMatrix * scaleMatrix;
    wing.drawPlaneHandWithTexture(lightingShaderWithTexture, model);  // Left WIng back


    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, 0.2f, 0.8f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-6.8f, 0.8f, 0.05f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = combinedMatrix * translateMatrix * rotateXMatrix * rotateMatrix * scaleMatrix;
    wing.drawPlaneHandWithTexture(lightingShaderWithTexture, model);  // Back Upper

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-5.0f, 0.04f, -2.0f));
    model = combinedMatrix * translateMatrix * rotateYMatrix* rotateMatrix * scaleMatrix;
    fanBody.drawBezierCurveWithTexture(lightingShaderWithTexture, model, glm::vec3(0.6, 0.6f, 0.6f));   // lEFT Big Fan Body


    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(-5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-5.0f, 0.02f, 2.0f));
    model = combinedMatrix * translateMatrix * rotateYMatrix * rotateMatrix * scaleMatrix;
    fanBody.drawBezierCurveWithTexture(lightingShaderWithTexture, model, glm::vec3(0.6, 0.6f, 0.6f));   // Right Big Fan Body


   // Front Wheel Left
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 0.05f, 0.1f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, -1.27f, 0.05f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(fanRotate + 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    //model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    model = combinedMatrix * translateMatrix *  rotateZMatrix * rotateMatrix * scaleMatrix;
    wheel.drawBezierCurveWithTexture(lightingShaderWithTexture, model, glm::vec3(0.6, 0.6f, 0.6f));

    // Front Wheel Right
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 0.05f, 0.1f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, -1.27f, -0.25f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(fanRotate + 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    //model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    model = combinedMatrix * translateMatrix  * rotateZMatrix * rotateMatrix * scaleMatrix;
    wheel.drawBezierCurveWithTexture(lightingShaderWithTexture, model, glm::vec3(0.6, 0.6f, 0.6f));

    //Front Wheel Piller
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.22f, 0.02f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, -1.3f, 0.0f));
    model = combinedMatrix * translateMatrix * scaleMatrix;
    cylinder.drawBezierCurve(lightingShaderWithTexture, model); 

    // Front Wheel Connector Middle Cylinder
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.04f, 0.02f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, -1.3f, -0.08f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    cylinder.drawBezierCurve(lightingShaderWithTexture, model);

    // Left Wheel Right
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.15f, 0.07f, 0.15f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.0f, -1.25f, -1.182f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    //model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    model = combinedMatrix * translateMatrix * rotateZMatrix * rotateMatrix * scaleMatrix;
    wheel.drawBezierCurve(lightingShaderWithTexture, model);

    // Left Wheel Left
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.15f, 0.07f, 0.15f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.0f, -1.25f, -1.68f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    //model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    model = combinedMatrix * translateMatrix * rotateZMatrix * rotateMatrix * scaleMatrix;
    wheel.drawBezierCurve(lightingShaderWithTexture, model);

    //Left Wheel Piller
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.03f, 0.45f, 0.03f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.0f, -1.3f, -1.3f));
    model = combinedMatrix * translateMatrix * scaleMatrix;
    cylinder.drawBezierCurve(lightingShaderWithTexture, model);

    // Left Wheel Connector Middle Cylinder
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.03f, 0.06f, 0.03f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.0f, -1.3f, -1.42f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    cylinder.drawBezierCurve(lightingShaderWithTexture, model);

    // Right Wheel Left
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.15f, 0.07f, 0.15f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.0f, -1.25f, 0.92f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    //model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    model = combinedMatrix * translateMatrix * rotateZMatrix * rotateMatrix * scaleMatrix;
    wheel.drawBezierCurve(lightingShaderWithTexture, model);

    // Right Wheel Right
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.15f, 0.07f, 0.15f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.0f, -1.25f, 1.41f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    //model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    model = combinedMatrix * translateMatrix * rotateZMatrix * rotateMatrix * scaleMatrix;
    wheel.drawBezierCurve(lightingShaderWithTexture, model);

    //Right Wheel Piller
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.03f, 0.45f, 0.03f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.0f, -1.3f, 1.3f));
    model = combinedMatrix * translateMatrix * scaleMatrix;
    cylinder.drawBezierCurve(lightingShaderWithTexture, model);

    // Right Wheel Connector Middle Cylinder
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.03f, 0.06f, 0.03f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.0f, -1.3f, 1.19f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    cylinder.drawBezierCurve(lightingShaderWithTexture, model);

    // Left Fan Blade
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 1.3f, 0.15f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.5f, -0.3f, 2.10f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(-5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(fanRotate + 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    translateOrigin = glm::translate(identityMatrix, glm::vec3(0.0f, -0.325f, 0.0f));
    translateOriginBack = glm::translate(identityMatrix, glm::vec3(0.0f, 0.325f, 0.0f));
    model = combinedMatrix * translateMatrix * rotateMatrix * translateOriginBack * rotateXMatrix * translateOrigin * scaleMatrix;
    fan.drawCubeWithTexture(lightingShaderWithTexture, model);  // 1 

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 1.3f, 0.15f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.5f, -0.27f, 2.12f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(-5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(fanRotate +  90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    translateOrigin = glm::translate(identityMatrix, glm::vec3(0.0f, -0.325f, 0.0f));
    translateOriginBack = glm::translate(identityMatrix, glm::vec3(0.0f, 0.325f, 0.0f));
    model = combinedMatrix * translateMatrix * rotateMatrix * translateOriginBack * rotateXMatrix * translateOrigin * scaleMatrix;
    fan.drawCubeWithTexture(lightingShaderWithTexture, model);  // 2

    // Right Fan Blade
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 1.3f, 0.15f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.5f, -0.3f, -2.16f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(fanRotate + 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    translateOrigin = glm::translate(identityMatrix, glm::vec3(0.0f, -0.325f, 0.0f));
    translateOriginBack = glm::translate(identityMatrix, glm::vec3(0.0f, 0.325f, 0.0f));
    model = combinedMatrix * translateMatrix * rotateMatrix * translateOriginBack * rotateXMatrix * translateOrigin * scaleMatrix;
    fan.drawCubeWithTexture(lightingShaderWithTexture, model);  // 1 P

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 1.3f, 0.15f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.5f, -0.27f, -2.14f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(fanRotate + 90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    translateOrigin = glm::translate(identityMatrix, glm::vec3(0.0f, -0.325f, 0.0f));
    translateOriginBack = glm::translate(identityMatrix, glm::vec3(0.0f, 0.325f, 0.0f));
    model = combinedMatrix * translateMatrix * rotateMatrix * translateOriginBack * rotateXMatrix * translateOrigin * scaleMatrix;
    fan.drawCubeWithTexture(lightingShaderWithTexture, model);  // 2 H

    if (planeMove)
    {
        fanRotate = fanRotate + 20;
    }
     
    // Front Glass
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 0.7f, 1.5f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.8f, 0.92f, -0.4f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(48.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = combinedMatrix * translateMatrix * rotateMatrix  * scaleMatrix;
    planeBlackGlass.drawCubeWithTexture(lightingShaderWithTexture, model); // M

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 0.6f, 1.5f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.75f, 0.92f, -0.8f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(75.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = combinedMatrix * translateMatrix * rotateYMatrix * rotateMatrix * scaleMatrix;
    planeBlackGlass.drawCubeWithTexture(lightingShaderWithTexture, model); // L


    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 0.6f, 1.5f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.45f, 0.92f, 0.63f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(-75.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = combinedMatrix * translateMatrix * rotateYMatrix * rotateMatrix * scaleMatrix;
    planeBlackGlass.drawCubeWithTexture(lightingShaderWithTexture, model); // R

}

void drawPlaneStatic(BezierCurve& cylinder, BezierCurve& cone, BezierCurve& fanBody, BezierCurve& wheel, PlaneHand& wing, Cube& fan, Cube& planeBlackGlass, Shader& lightingShaderWithTexture, glm::mat4 combinedMatrix)
{
    glm::mat4 translateMatrix, translateOrigin, translateOriginBack, rotateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, 1.0f, 0.5f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.5f, 0.0f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    //cylinder.drawBezierCurve(lightingShaderWithTexture, model); // Middle Body
    cylinder.drawBezierCurveWithTexture(lightingShaderWithTexture, model, glm::vec3(0.6, 0.6f, 0.6f)); // Middle Body

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, 0.5f, 0.5f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.5f, 0.0f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    cone.drawBezierCurveWithTexture(lightingShaderWithTexture, model, glm::vec3(0.6, 0.6f, 0.6f)); // Front Side

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, 1.0f, 0.5f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.0f, 0.5f, 0.0f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    cone.drawBezierCurveWithTexture(lightingShaderWithTexture, model, glm::vec3(0.6, 0.6f, 0.6f));  // Back Side

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 0.3f, 1.2f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.8f, 0.4f, 0.95f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    wing.drawPlaneHandWithTexture(lightingShaderWithTexture, model);  // Right WIng front


    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.8f, 0.5f, -0.95f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = combinedMatrix * translateMatrix * rotateYMatrix * rotateMatrix * scaleMatrix;
    wing.drawPlaneHandWithTexture(lightingShaderWithTexture, model);  // Left WIng front

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, 0.2f, 0.8f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-6.8f, 0.4f, 0.3f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    wing.drawPlaneHandWithTexture(lightingShaderWithTexture, model);  // Right WIng back

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-6.82f, 0.5f, -0.28f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = combinedMatrix * translateMatrix * rotateYMatrix * rotateMatrix * scaleMatrix;
    wing.drawPlaneHandWithTexture(lightingShaderWithTexture, model);  // Left WIng back


    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, 0.2f, 0.8f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-6.8f, 0.8f, 0.05f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = combinedMatrix * translateMatrix * rotateXMatrix * rotateMatrix * scaleMatrix;
    wing.drawPlaneHandWithTexture(lightingShaderWithTexture, model);  // Back Upper

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-5.0f, 0.04f, -2.0f));
    model = combinedMatrix * translateMatrix * rotateYMatrix * rotateMatrix * scaleMatrix;
    fanBody.drawBezierCurveWithTexture(lightingShaderWithTexture, model, glm::vec3(0.6, 0.6f, 0.6f));   // lEFT Big Fan Body


    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(-5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-5.0f, 0.02f, 2.0f));
    model = combinedMatrix * translateMatrix * rotateYMatrix * rotateMatrix * scaleMatrix;
    fanBody.drawBezierCurveWithTexture(lightingShaderWithTexture, model, glm::vec3(0.6, 0.6f, 0.6f));   // Right Big Fan Body


    // Front Wheel Left
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 0.05f, 0.1f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, -1.27f, 0.05f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    //model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    model = combinedMatrix * translateMatrix * rotateZMatrix * rotateMatrix * scaleMatrix;
    wheel.drawBezierCurveWithTexture(lightingShaderWithTexture, model, glm::vec3(0.6, 0.6f, 0.6f));

    // Front Wheel Right
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 0.05f, 0.1f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, -1.27f, -0.25f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    //model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    model = combinedMatrix * translateMatrix * rotateZMatrix * rotateMatrix * scaleMatrix;
    wheel.drawBezierCurveWithTexture(lightingShaderWithTexture, model, glm::vec3(0.6, 0.6f, 0.6f));

    //Front Wheel Piller
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.22f, 0.02f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, -1.3f, 0.0f));
    model = combinedMatrix * translateMatrix * scaleMatrix;
    cylinder.drawBezierCurve(lightingShaderWithTexture, model);

    // Front Wheel Connector Middle Cylinder
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.04f, 0.02f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, -1.3f, -0.08f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    cylinder.drawBezierCurve(lightingShaderWithTexture, model);

    // Left Wheel Right
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.15f, 0.07f, 0.15f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.0f, -1.25f, -1.182f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    //model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    model = combinedMatrix * translateMatrix * rotateZMatrix * rotateMatrix * scaleMatrix;
    wheel.drawBezierCurve(lightingShaderWithTexture, model);

    // Left Wheel Left
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.15f, 0.07f, 0.15f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.0f, -1.25f, -1.68f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    //model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    model = combinedMatrix * translateMatrix * rotateZMatrix * rotateMatrix * scaleMatrix;
    wheel.drawBezierCurve(lightingShaderWithTexture, model);

    //Left Wheel Piller
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.03f, 0.45f, 0.03f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.0f, -1.3f, -1.3f));
    model = combinedMatrix * translateMatrix * scaleMatrix;
    cylinder.drawBezierCurve(lightingShaderWithTexture, model);

    // Left Wheel Connector Middle Cylinder
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.03f, 0.06f, 0.03f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.0f, -1.3f, -1.42f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    cylinder.drawBezierCurve(lightingShaderWithTexture, model);

    // Right Wheel Left
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.15f, 0.07f, 0.15f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.0f, -1.25f, 0.92f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    //model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    model = combinedMatrix * translateMatrix * rotateZMatrix * rotateMatrix * scaleMatrix;
    wheel.drawBezierCurve(lightingShaderWithTexture, model);

    // Right Wheel Right
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.15f, 0.07f, 0.15f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.0f, -1.25f, 1.41f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    //model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    model = combinedMatrix * translateMatrix * rotateZMatrix * rotateMatrix * scaleMatrix;
    wheel.drawBezierCurve(lightingShaderWithTexture, model);

    //Right Wheel Piller
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.03f, 0.45f, 0.03f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.0f, -1.3f, 1.3f));
    model = combinedMatrix * translateMatrix * scaleMatrix;
    cylinder.drawBezierCurve(lightingShaderWithTexture, model);

    // Right Wheel Connector Middle Cylinder
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.03f, 0.06f, 0.03f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.0f, -1.3f, 1.19f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    cylinder.drawBezierCurve(lightingShaderWithTexture, model);

    // Left Fan Blade
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 1.3f, 0.15f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.5f, -0.3f, 2.10f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(-5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    translateOrigin = glm::translate(identityMatrix, glm::vec3(0.0f, -0.325f, 0.0f));
    translateOriginBack = glm::translate(identityMatrix, glm::vec3(0.0f, 0.325f, 0.0f));
    model = combinedMatrix * translateMatrix * rotateMatrix * translateOriginBack * rotateXMatrix * translateOrigin * scaleMatrix;
    fan.drawCubeWithTexture(lightingShaderWithTexture, model);  // 1 

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 1.3f, 0.15f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.5f, -0.27f, 2.12f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(-5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    translateOrigin = glm::translate(identityMatrix, glm::vec3(0.0f, -0.325f, 0.0f));
    translateOriginBack = glm::translate(identityMatrix, glm::vec3(0.0f, 0.325f, 0.0f));
    model = combinedMatrix * translateMatrix * rotateMatrix * translateOriginBack * rotateXMatrix * translateOrigin * scaleMatrix;
    fan.drawCubeWithTexture(lightingShaderWithTexture, model);  // 2

    // Right Fan Blade
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 1.3f, 0.15f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.5f, -0.3f, -2.16f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians( 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    translateOrigin = glm::translate(identityMatrix, glm::vec3(0.0f, -0.325f, 0.0f));
    translateOriginBack = glm::translate(identityMatrix, glm::vec3(0.0f, 0.325f, 0.0f));
    model = combinedMatrix * translateMatrix * rotateMatrix * translateOriginBack * rotateXMatrix * translateOrigin * scaleMatrix;
    fan.drawCubeWithTexture(lightingShaderWithTexture, model);  // 1 P

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 1.3f, 0.15f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.5f, -0.27f, -2.14f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians( 90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    translateOrigin = glm::translate(identityMatrix, glm::vec3(0.0f, -0.325f, 0.0f));
    translateOriginBack = glm::translate(identityMatrix, glm::vec3(0.0f, 0.325f, 0.0f));
    model = combinedMatrix * translateMatrix * rotateMatrix * translateOriginBack * rotateXMatrix * translateOrigin * scaleMatrix;
    fan.drawCubeWithTexture(lightingShaderWithTexture, model);  // 2 H

    if (planeMove)
    {
        fanRotate = fanRotate + 20;
    }

    // Front Glass
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 0.7f, 1.5f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.8f, 0.92f, -0.4f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(48.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    planeBlackGlass.drawCubeWithTexture(lightingShaderWithTexture, model); // M

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 0.6f, 1.5f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.75f, 0.92f, -0.8f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(75.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = combinedMatrix * translateMatrix * rotateYMatrix * rotateMatrix * scaleMatrix;
    planeBlackGlass.drawCubeWithTexture(lightingShaderWithTexture, model); // L


    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 0.6f, 1.5f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.45f, 0.92f, 0.63f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(-75.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = combinedMatrix * translateMatrix * rotateYMatrix * rotateMatrix * scaleMatrix;
    planeBlackGlass.drawCubeWithTexture(lightingShaderWithTexture, model); // R

}

void planeMoveAnimation(BezierCurve& cylinder, BezierCurve& cone, BezierCurve& fanBody, BezierCurve& wheel, Cube& fan, Cube& planeBlackGlass, PlaneHand& wing, Shader& lightingShaderWithTexture, float& r, float& x_pilot_eye, float& y_pilot_eye, float& z_pilot_eye)
{
    if (planeMove)
    {
        glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model, model_i, extra;
        identityMatrix = glm::mat4(1.0f);
        extra = identityMatrix;
        float x_init = 0.0;
        float y_init = 0.0;

        if (r >= 30.0f)
        {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(r + x_init + 2.0f, y_init + (r / 100), 0.0f));
            rotateZMatrix = glm::rotate(identityMatrix, glm::radians(11.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        }
        else if (r >= 20.0f)
        {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(r + x_init + 1.0f, y_init, 0.0f));
            rotateZMatrix = glm::rotate(identityMatrix, glm::radians(r - 20.0f + 1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        }

        else
        {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(r + x_init, y_init, 0.0f));
            rotateZMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        }
        model_i = rotateZMatrix * translateMatrix;
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.6, 0.6f, 0.6f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-14.0f, 0.0f, -3.0f));
        model = model_i * translateMatrix * scaleMatrix;
        drawPlane(cylinder, cone, fanBody, wheel, wing, fan, planeBlackGlass, lightingShaderWithTexture, model);


        r = (r + 0.01);

    }
    else
    {
        glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
        identityMatrix = glm::mat4(1.0f);

        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.6, 0.6f, 0.6f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-14.0f, 0.0f, -3.0f));
        model = translateMatrix * scaleMatrix;
        drawPlane(cylinder, cone, fanBody, wheel, wing, fan, planeBlackGlass, lightingShaderWithTexture, model);

        x_pilot_eye = -14.0;
        y_pilot_eye = 1.0;
        z_pilot_eye = -3.0;
    }
}

void drawHexagon(Hexagon &hexa, Shader shader, glm::mat4 combinedMatrix)
{
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
    glm::mat4 identityMatrix = glm::mat4(1.0f);

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
    model = combinedMatrix * translateMatrix * scaleMatrix;

    hexa.drawHexagon(shader, model);
}


void drawHexagonAdvanced(HexagonAdvanced& hexa, Shader shader, glm::mat4 combinedMatrix)
{
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
    glm::mat4 identityMatrix = glm::mat4(1.0f);

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
    model = combinedMatrix * translateMatrix * scaleMatrix;

    hexa.drawHexagonAdvancedWithTexture(shader, model);
}

void drawHexagonBoundary(Hexagon& hexa, Shader& uniqueColorShader)
{
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
    glm::mat4 identityMatrix = glm::mat4(1.0f);

    

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 1.0f, 0.15f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-28.0f, -1.0f, -16.0f));
    model = translateMatrix * scaleMatrix;
    drawHexagon(hexa, uniqueColorShader, model);

    for (int i = 0; i < 18; i++)  // Back
    {
        translateMatrix = glm::translate(identityMatrix, glm::vec3(3.0f, 0.0f, 0.0f));
        model = translateMatrix * model;
        drawHexagon(hexa, uniqueColorShader, model);
    }

    for (int i = 0; i < 11; i++)  // Right
    {
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, 3.0f));
        model = translateMatrix * model;
        drawHexagon(hexa, uniqueColorShader, model);
    }
    for (int i = 0; i < 11; i++)
    {
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, -3.0f));
        model = translateMatrix * model;
    }
    for (int i = 0; i < 18; i++)
    {
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.0f, 0.0f, 0.0f));
        model = translateMatrix * model;
    }
    for (int i = 0; i < 13; i++) // Left
    {
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, 3.0f));
        model = translateMatrix * model;
        drawHexagon(hexa, uniqueColorShader, model);
    }
}

void drawGround(Cube &cube, Shader lightingShaderWithTexture, glm::mat4 combinedMatrix)
{
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(120.0f, 0.1f, 100.0f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-30.0f, -1.0f, -17.5f));
    model = combinedMatrix * translateMatrix * scaleMatrix;
    cube.drawCubeWithTexture(lightingShaderWithTexture, model);
}



void drawRunway(Cube &cube, Shader ourShader, glm::mat4 combinedMatrix)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
    identityMatrix = glm::mat4(1.0f);
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(85.0f, 0.1f, 6.0f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-15.5f, -0.98f, -3.0f));
    model = combinedMatrix * translateMatrix * scaleMatrix;

    cube.drawCubeWithTexture(ourShader, model);
}

void drawHelipad(Cube& cube, Shader ourShader, glm::mat4 combinedMatrix)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
    identityMatrix = glm::mat4(1.0f);
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(10.0f, 0.15f, 10.0f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-15.5f, -0.98f, 0.0f));
    model = combinedMatrix * translateMatrix * scaleMatrix;

    cube.drawCubeWithTexture(ourShader, model);
}



void drawFroad(Cube& cube, Shader ourShader, glm::mat4 combinedMatrix)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
    identityMatrix = glm::mat4(1.0f);
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(70.0f, 0.1f, 25.0f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-15.5f, -0.98f, 0.0f));
    model = combinedMatrix * translateMatrix * scaleMatrix;

    cube.drawCubeWithTexture(ourShader, model);
}



void ControlTowerGroundWindow(Cube &windw, unsigned int VAO, Shader ourShader, glm::mat4 comMatrix)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
    identityMatrix = glm::mat4(1.0f);
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.8f, 0.8f, .02f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.2f, 0.2f, 1.98f));
    model = comMatrix * translateMatrix * scaleMatrix;
    windw.drawCubeWithTexture(ourShader, model);
}



void drawControlTower(Cube &cube, Cube &wall, Cube& signboard_ct, Frustum& frustum, BezierCurve& dishCone, unsigned int VAO, unsigned int VAO_f, Shader ourShader, glm::mat4 comMatrix)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
    identityMatrix = glm::mat4(1.0f);

    // Ground Floor
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(8.0f, 8.0f, 6.0f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-5.0f, -1.0f, 2.0f));
    model = comMatrix * translateMatrix * scaleMatrix;   
    cube.drawCubeWithTexture(ourShader, model);


    // Control Tower Ground Floor Roof
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(8.2f, 0.02f, 6.2f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-5.1f, 3.0f, 1.9f));
    model = comMatrix * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);
    ourShader.setVec4("color", glm::vec4(0.52, 0.48, 0.54, 1.0));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Control Tower Top Floor

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, 4.0f, 2.0f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.8f, 3.0f, 2.2f));
    model = comMatrix * translateMatrix * scaleMatrix;   
    wall.drawCubeWithTexture(ourShader, model);


    // Frustum Roof

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(3.0f, 3.0f, 3.0f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-5.1f, 5.0f, 1.9f));
    model = comMatrix * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);
    ourShader.setVec4("color", glm::vec4(0.34, 0.63, 0.65, 1.0));

    glBindVertexArray(VAO_f);
    //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    frustum.drawFrustumWithTexture(ourShader, model);

    // Frustum Roof Upper Part

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(3.0f, 0.1f, 3.0f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-5.1f, 6.5f, 1.9f));
    model = comMatrix * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);
    ourShader.setVec4("color", glm::vec4(0.0, 0.2, 0.0, 1.0));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Piller

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 2.5f, 0.2f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.4f, 6.5f, 2.5f));
    model = comMatrix * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);
    ourShader.setVec4("color", glm::vec4(0.34, 0.63, 0.65, 1.0));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Control Tower Sign
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(8.0f, 1.0f, 0.2f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-5.0f, 2.5f, 1.9f));
    model = comMatrix * translateMatrix * scaleMatrix;
    signboard_ct.drawCubeWithTexture(ourShader, model); // 1

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 1.0f, 6.20f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-5.1f, 2.5f, 1.9f));
    model = comMatrix * translateMatrix * scaleMatrix;
    signboard_ct.drawCubeWithTexture(ourShader, model); // 2

    // Dish
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.4, 0.15f, 0.4f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(150.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.3f, 3.67f, 3.0f));
    model = comMatrix * translateMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
    dishCone.drawBezierCurveWithTexture(ourShader, model, glm::vec3(0.6, 0.6f, 0.6f));

    // Dish Piller
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 2.0f, 0.1f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(-30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.65f, 3.1f, 3.15f));
    model = comMatrix * translateMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
    wall.drawCubeWithTexture(ourShader, model); // Middle

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 1.25f, 0.1f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.15f, 3.0f, 2.86f));
    model = comMatrix * translateMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
    wall.drawCubeWithTexture(ourShader, model); //  L

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 1.25f, 0.1f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(-60.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.21f, 3.0f, 3.28f));
    model = comMatrix * translateMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
    wall.drawCubeWithTexture(ourShader, model); //  R
    // Control Tower Window
    /*
    translateMatrix = identityMatrix;
    // Row wise
    ControlTowerGroundWindow(windw, VAO, ourShader, comMatrix * translateMatrix);
    translateMatrix = identityMatrix;
    for (int i = 0; i < 2; i++)
    {
        translateMatrix = translateMatrix * glm::translate(identityMatrix, glm::vec3(-0.8f, 0.0f, 0.0f));
        ControlTowerGroundWindow(windw, VAO, ourShader, comMatrix * translateMatrix);
    }
    // Column wise
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, -0.7f, 0.0f));
    ControlTowerGroundWindow(windw, VAO, ourShader, comMatrix * translateMatrix);
    for (int i = 0; i < 2; i++)
    {
        translateMatrix = translateMatrix * glm::translate(identityMatrix, glm::vec3(-0.8f, 0.0f, 0.0f));
        ControlTowerGroundWindow(windw, VAO, ourShader, comMatrix * translateMatrix);
    }
    */
}


void drawPlaneHouse(Cube &wall, Cube &sidewall, Cube &brick, Shader lightingShaderWithTexture, glm::mat4 combinedMatrix)
{
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model, rotateMatrix;
    glm::mat4 identityMatrix = glm::mat4(1.0f);

    // Backwall
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(7.0f, 4.0f, 0.5f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.0f, -1.0f, 10.25f));
    model = combinedMatrix * translateMatrix * scaleMatrix;
    wall.drawCubeWithTexture(lightingShaderWithTexture, model);


    // Side Wall Right
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 4.0f, 15.0f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.0f, -1.0f, 3.0f));
    model = combinedMatrix * translateMatrix * scaleMatrix;
    sidewall.drawCubeWithTexture(lightingShaderWithTexture, model);

    // Side Wall Left
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 4.0f, 15.0f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(6.4f, -1.0f, 3.0f));
    model = combinedMatrix * translateMatrix * scaleMatrix;
    sidewall.drawCubeWithTexture(lightingShaderWithTexture, model);

    // Roof
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(8.0f, 0.2f, 7.0f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.75f, 1.0f, 3.0f));
    model = combinedMatrix * translateMatrix * scaleMatrix;
   // brick.drawCubeWithTexture(lightingShaderWithTexture, model);

    // Floor
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(12.0f, 0.2f, 20.0f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.75f, -1.0f, -2.0f));
    model = combinedMatrix * translateMatrix * scaleMatrix;
    wall.drawCubeWithTexture(lightingShaderWithTexture, model);

    // Roof Right
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(4.8f, 0.2f, 15.0f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.75f, 0.8f, 3.0f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    
    brick.drawCubeWithTexture(lightingShaderWithTexture, model);

    // Roof Left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(4.7f, 2.00f, 3.0f));
    rotateMatrix = glm::rotate(identityMatrix, glm::radians(-30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = combinedMatrix * translateMatrix * rotateMatrix * scaleMatrix;
    brick.drawCubeWithTexture(lightingShaderWithTexture, model);

}

void mainGate(Cube& wall, Cube& signboard, Cube& brick, Shader lightingShaderWithTexture, glm::mat4 combinedMatrix)
{
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model, rotateMatrix;
    glm::mat4 identityMatrix = glm::mat4(1.0f);

    // Piller 1
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.8f, 7.9f, 1.0f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-15.0f, -1.0f, 12.0f));
    model = combinedMatrix * translateMatrix * scaleMatrix;
    wall.drawCubeWithTexture(lightingShaderWithTexture, model);

    // Piller 2
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.8f, 7.9f, 1.0f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-20.0f, -1.0f, 12.0f));
    model = combinedMatrix * translateMatrix * scaleMatrix;
    wall.drawCubeWithTexture(lightingShaderWithTexture, model);

    // Sign Board
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(12.0f, 2.0f, 1.6f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-20.25f, 2.0f, 11.9f));
    model = combinedMatrix * translateMatrix * scaleMatrix;
    signboard.drawCubeWithTexture(lightingShaderWithTexture, model);
}

void borderWall(Cube& wall, Cube& brick, Shader lightingShaderWithTexture, glm::mat4 combinedMatrix)
{
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model, rotateMatrix;
    glm::mat4 identityMatrix = glm::mat4(1.0f);

    // Wall
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(65.0f, 8.0f, 0.5f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-11.0f, -1.0f, 16.2f));
    model = combinedMatrix * translateMatrix * scaleMatrix;
    wall.drawCubeWithTexture(lightingShaderWithTexture, model);

    //Piller
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.8f, 8.02f, 0.82f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.4f, -1.0f, 16.3f));
    model = combinedMatrix * translateMatrix * scaleMatrix;
    brick.drawCubeWithTexture(lightingShaderWithTexture, model);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(14.6f, 0.0f, 0.0f));
    model = combinedMatrix * translateMatrix * model;
    brick.drawCubeWithTexture(lightingShaderWithTexture, model);
}



void planeAnimation(Cube &demo,Shader ourShader, glm::mat4 comMatrix, float &r, float &x_pilot_eye, float &y_pilot_eye, float &z_pilot_eye)
{
    if (planeMove)
    {
        glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
        identityMatrix = glm::mat4(1.0f);

        float x_init = -8.0;
        float y_init = -1.0;


        // Plane
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, 2.0f, 2.0f));
        if (r >= 8.0)
        {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(r + x_init, r + x_init + y_init, -2.0f));
            x_pilot_eye = r + x_init;
            y_pilot_eye = r + x_init + y_init;
            z_pilot_eye = -2.0f;
        }
        else
        {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(r + x_init, y_init, -2.0f));
        }


        model = comMatrix * translateMatrix * scaleMatrix;
        demo.drawCubeWithTexture(ourShader, model);

        r = (r + 0.01);

    }
    else
    {
        glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
        identityMatrix = glm::mat4(1.0f);

        // Plane
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, 2.0f, 2.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-8.0f, -1.0f, -2.0f));
        model = comMatrix * translateMatrix * scaleMatrix;
        demo.drawCubeWithTexture(ourShader, model);
    }
}

void airportBuilding(Cube& tilesFloor, Cube& brick, Cube& signboard, HexagonAdvanced& pillar, Shader& lightingShaderWithTexture, Shader& ourShader, glm::mat4 combinedmatrix)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
    identityMatrix = glm::mat4(1.0f);

    // Airport Building
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(35.0f, 0.5f, 17.0f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-26.5f, -0.9f, 16.4f));
    model = combinedmatrix * translateMatrix * scaleMatrix;
    tilesFloor.drawCubeWithTexture(lightingShaderWithTexture, model); // floor down

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(30.0f, 0.5f, 15.0f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-25.0f, -0.6f, 16.4f));
    model = combinedmatrix * translateMatrix * scaleMatrix;
    tilesFloor.drawCubeWithTexture(lightingShaderWithTexture, model); // floor middle

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(25.0f, 0.5f, 13.0f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-23.5f, -0.3f, 16.4f));
    model = combinedmatrix * translateMatrix * scaleMatrix;
    tilesFloor.drawCubeWithTexture(lightingShaderWithTexture, model); // floor up

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(30.0f, 0.5f, 15.0f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-25.0f, 5.0f, 16.4f));
    model = combinedmatrix * translateMatrix * scaleMatrix;
    brick.drawCubeWithTexture(lightingShaderWithTexture, model); // Roof

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, 10.2f, 13.0f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-23.5f, -0.1f, 16.4f));
    model = combinedmatrix * translateMatrix * scaleMatrix;
    tilesFloor.drawCubeWithTexture(lightingShaderWithTexture, model); // Left Wall

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, 10.2f, 13.0f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-11.25f, -0.1f, 16.4f));
    model = combinedmatrix * translateMatrix * scaleMatrix;
    tilesFloor.drawCubeWithTexture(lightingShaderWithTexture, model); // Right Wall

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 3.0f, 0.25f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-24.0f, -1.0f, 23.0f));
    model = combinedmatrix * translateMatrix * scaleMatrix;
    drawHexagonAdvanced(pillar, lightingShaderWithTexture, model); // Piller L

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.6f, -1.0f, 23.0f));
    model = combinedmatrix * translateMatrix * scaleMatrix;
    drawHexagonAdvanced(pillar, lightingShaderWithTexture, model); // Piller R

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 3.0f, 0.25f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-24.0f, -1.0f, 17.0f));
    model = combinedmatrix * translateMatrix * scaleMatrix;
    drawHexagonAdvanced(pillar, lightingShaderWithTexture, model); // Piller BL

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.6f, -1.0f, 17.0f));
    model = combinedmatrix * translateMatrix * scaleMatrix;
    drawHexagonAdvanced(pillar, lightingShaderWithTexture, model); // Piller BR

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 3.0f, 0.25f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-20.0f, -1.0f, 23.0f));
    model = combinedmatrix * translateMatrix * scaleMatrix;
    drawHexagonAdvanced(pillar, lightingShaderWithTexture, model); // Piller ML

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-14.6f, -1.0f, 23.0f));
    model = combinedmatrix * translateMatrix * scaleMatrix;
    drawHexagonAdvanced(pillar, lightingShaderWithTexture, model); // Piller MR

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(30.0f, 5.0f, 0.5f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-25.0f, 5.0f, 23.7f));
    model = combinedmatrix * translateMatrix * scaleMatrix;
    signboard.drawCubeWithTexture(lightingShaderWithTexture, model); // signboard

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(24.8f, 10.4f, 0.3f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-23.4f, -0.3f, 22.7f));
    model = combinedmatrix * translateMatrix * scaleMatrix;
    brick.drawCubeTransparent(ourShader, model);// Glass

    scaleMatrix = glm::scale(identityMatrix, glm::vec3(24.8f, 10.4f, 0.3f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-23.4f, -0.3f, 16.405f));
    model = combinedmatrix * translateMatrix * scaleMatrix;
    brick.drawCubeTransparent(ourShader, model);// Glass bck
}




// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    
    
    
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_L && action == GLFW_PRESS)
    {
        if (pointLightOn)
        {
            pointlight1.turnOff();
            pointlight2.turnOff();
            pointlight3.turnOff();
            pointlight4.turnOff();
            pointlight5.turnOff();
            pointlight6.turnOff();
            pointlight7.turnOff();
            pointLightOn = !pointLightOn;
        }
        else
        {
            pointlight1.turnOn();
            pointlight2.turnOn();
            pointlight3.turnOn();
            pointlight4.turnOn();
            pointlight5.turnOn();
            pointlight6.turnOn();
            pointlight7.turnOn();
            pointLightOn = !pointLightOn;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
    {
        camera.MoveToPosition(glm::vec3(0.0f, 1.0f, -10.0f));
        camera.MoveLookAtPosition(glm::vec3(0.0f, 1.0f, 8.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
    {
        camera.MoveToPosition(glm::vec3(-1.0f, 8.0f, 0.0f));
        camera.MoveLookAtPosition(glm::vec3(-1.0f, 0.0f, 0.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
    {
        camera.MoveToPosition(glm::vec3(-10.0f, 0.0f, 0.0f));
        camera.MoveLookAtPosition(glm::vec3(-10.0f, 0.0f, 2.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
    {
        camera.MoveToPosition(glm::vec3(-17.0f, 2.0f, 38.0f));
        camera.MoveLookAtPosition(glm::vec3(-17.0f, 2.5f, 35.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        camera.MoveToPosition(glm::vec3(x_pilot_eye, y_pilot_eye, z_pilot_eye));
        camera.MoveLookAtPosition(glm::vec3(x_pilot_eye + 2.0f, y_pilot_eye, z_pilot_eye));
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    {
        r = 0.0;
        if (planeMove)
            planeMove = false;
        else
            planeMove = true;
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
    {
        rot = 0.0;
        if (skyPlaneMove)
            skyPlaneMove = false;
        else
            skyPlaneMove = true;
    }
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
    {
        heli_r = 0.0;
        if (heliMove)
            heliMove = false;
        else
            heliMove = true;
    }
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------


unsigned int loadTexture(char const* path, GLenum textureWrappingModeS, GLenum textureWrappingModeT, GLenum textureFilteringModeMin, GLenum textureFilteringModeMax)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureWrappingModeS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureWrappingModeT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureFilteringModeMin);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureFilteringModeMax);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}


unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

unsigned int loadTransparentTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}