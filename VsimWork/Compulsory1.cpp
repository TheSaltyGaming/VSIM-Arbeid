#include "Shader.h"
#include "ShaderFileLoader.h"
#include <iostream>
#include <map>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "Collision.h"
#include "Math.h"
#include "Mesh/Mesh.h"
#include "Mesh/Surface.h"
#include "glm/mat4x3.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

void CameraView(std::vector<unsigned> shaderPrograms, glm::mat4 trans, glm::mat4 projection);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void DrawObjects(unsigned VAO, Shader ShaderProgram);

void CollisionChecking();

glm::vec3 RandomColor();


std::vector<Mesh*> wallMeshes;
std::vector<Mesh*> sphereMeshes;


Math math;
Collision collision;

Mesh sphere_mesh;

Mesh sphere2Mesh;

Mesh plane_mesh;


Mesh CameraMesh;

Mesh wall1_mesh;
Mesh wall2_mesh;
Mesh wall3_mesh;
Mesh wall4_mesh;


// settings

 unsigned int SCR_WIDTH = 1280;
 unsigned int SCR_HEIGHT = 720;

struct colorStruct
{
    glm::vec3 red = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 green = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 blue = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 white = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 black = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 yellow = glm::vec3(1.0f, 1.0f, 0.0f);
    glm::vec3 cyan = glm::vec3(0.0f, 1.0f, 1.0f);
    glm::vec3 magenta = glm::vec3(1.0f, 0.0f, 1.0f);
    glm::vec3 orange = glm::vec3(1.0f, 0.5f, 0.0f);
    glm::vec3 purple = glm::vec3(0.5f, 0.0f, 0.5f);
    glm::vec3 grey = glm::vec3(0.5f, 0.5f, 0.5f);
};

colorStruct colors;
///Delta time variables
///--------------------
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

///Mouse Input Variables
///---------------------
bool firstMouse = true;
float lastX = SCR_WIDTH / 2.0f, lastY = SCR_HEIGHT / 2.0f;

std::string vfs = ShaderLoader::LoadShaderFromFile("Triangle.vs");
std::string fs = ShaderLoader::LoadShaderFromFile("Triangle.vs");

Camera MainCamera;


std::vector<unsigned> shaderPrograms;

void DrawObjects(unsigned VAO, Shader ShaderProgram)
{
    //Drawmeshes here, draw meshes (this comment is for CTRL + F search)
    ShaderProgram.use();
    glBindVertexArray(VAO);
    // glDrawArrays(GL_TRIANGLES, 0, 3);

    // sphere_mesh.Draw(ShaderProgram.ID);
    // sphere2Mesh.Draw(ShaderProgram.ID);

    //draw all meshes
    for (Mesh* sphere : sphereMeshes)
    {
        sphere->Draw(ShaderProgram.ID);
    }
    
    plane_mesh.Draw(ShaderProgram.ID);

    wall1_mesh.Draw(ShaderProgram.ID);
    wall2_mesh.Draw(ShaderProgram.ID);
    wall3_mesh.Draw(ShaderProgram.ID);
    wall4_mesh.Draw(ShaderProgram.ID);
    //CameraMesh.Draw(ShaderProgram.ID);
    
    
}


void render(GLFWwindow* window, Shader ourShader, unsigned VAO)
{
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f)); 

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f)); 
    
    
    
    
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        
        glLineWidth(12);
        
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        int modelLoc = glGetUniformLocation(ourShader.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        
        int viewLoc = glGetUniformLocation(ourShader.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        
        int projectionLoc = glGetUniformLocation(ourShader.ID, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


        CameraView(shaderPrograms, model, projection);
        

        CameraMesh.globalPosition = MainCamera.cameraPos;
        CameraMesh.CalculateBoundingBox();

        plane_mesh.CalculateBoundingBox();
        
        //for every sphere do physics
        for (Mesh* sphere : sphereMeshes)
        {
            sphere->Physics(deltaTime);
        }
        
        //cout camera position
        //std::cout << "Camera Position: " << MainCamera.cameraPos.x << " " << MainCamera.cameraPos.y << " " << MainCamera.cameraPos.z << std::endl;
        
        
        
        // input
        // -----
        processInput(window);
        
        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        DrawObjects(VAO, ourShader);

        CollisionChecking();
        

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void SetupMeshes()
{
    //Create meshes here, Make meshes here, Setup meshes here, define meshes here, setupObjects setup objects create objects
    //(this comment is for CTRL + F search)
    int SphereCount = 200;
    
    for (int i = 0; i < SphereCount; ++i) {
        Mesh* sphere = new Mesh(Sphere, 1.f, 4, RandomColor());

        sphere->globalPosition = glm::vec3(
        math.RandomVec3(-3.7, 3.7).x,
        0.5, // y
        math.RandomVec3(-3.7, 3.7).z);
        
        sphere->globalScale = glm::vec3(0.1f, 0.1f, 0.1f);
        sphere->velocity = glm::vec3(0.f);

        sphereMeshes.push_back(sphere);
    }

#pragma region OtherMeshes
    plane_mesh = Mesh(Plane, 4, colors.green);
    plane_mesh.globalPosition.y = -0.5f;
    wallMeshes.push_back(&plane_mesh);
    

    CameraMesh = Mesh(Cube, 0.5f, colors.white);
    CameraMesh.globalPosition = MainCamera.cameraPos;
    
    float wallScale = 4.f;
    float heightScale = 0.4f;
    wall1_mesh = Mesh(Cube, 1.f, colors.orange);
    wall1_mesh.globalPosition = glm::vec3(0.0f, 0.0f, -4.0f);
    wall1_mesh.globalScale = glm::vec3(wallScale, wallScale*heightScale, 0.1f);
    wallMeshes.push_back(&wall1_mesh);
    
    wall2_mesh = Mesh(Cube, 1.f, colors.cyan);
    wall2_mesh.globalPosition = glm::vec3(0.0f, 0.0f, 4.0f);
    wall2_mesh.globalScale = glm::vec3(wallScale, wallScale*heightScale, 0.1f);
    wallMeshes.push_back(&wall2_mesh);
    
    wall3_mesh = Mesh(Cube, 1.f, colors.yellow);
    wall3_mesh.globalPosition = glm::vec3(-4.0f, 0.0f, 0.0f);
    wall3_mesh.globalScale = glm::vec3(0.1f, wallScale*heightScale, wallScale);
    wallMeshes.push_back(&wall3_mesh);
    
    wall4_mesh = Mesh(Cube, 1.f, colors.blue);
    wall4_mesh.globalPosition = glm::vec3(4.0f, 0.0f, 0.0f);
    wall4_mesh.globalScale = glm::vec3(0.1f, wallScale*heightScale, wallScale);
    wallMeshes.push_back(&wall4_mesh);
#pragma endregion
}

int main()
{
    srand(time(0));
    
    
    
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Architect Engine", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback); // Set the mouse callback function

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader program
    // ------------------------------------

    //Shader ourShader("Triangle.vert", "Triangle.frag"); // you can name your shader files however you like
    Shader ourShader("VertShaderOld.vert", "FragShaderOld.frag"); // you can name your shader files however you like

    shaderPrograms.push_back(ourShader.ID);
    


    /// SETUP MESHES HER
    SetupMeshes();
    
    
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // color attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    // glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    

    
    //RENDER FUNCTION HERE!!!!!!!
    render(window, ourShader, VAO);

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    glm::vec3 cameraFrontXZ = glm::normalize(glm::vec3(MainCamera.cameraFront.x, 0.0f, MainCamera.cameraFront.z));

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        //make random sphere move
        int randomSphere = rand() % sphereMeshes.size();
        if (sphereMeshes[randomSphere]->velocity == glm::vec3(0.f,0.f,0.f))
        {
            sphereMeshes[randomSphere]->velocity = glm::vec3(math.RandomVec3(-4, 4).x, 0.0f, math.RandomVec3(-4, 4).z);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
    {
        //make all spheres move
        for (auto ballsphere : sphereMeshes)
        {
            ballsphere->velocity = glm::vec3(math.RandomVec3(-2, 2).x, 0.0f, math.RandomVec3(-2, 2).z);

        }
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
    {
        //stop all velocity
        for (Mesh* sphere : sphereMeshes)
        {
            sphere->velocity = glm::vec3(0.0f, 0.0f, 0.0f);
        }
    }
    

    float cameraSpeed = 2.5f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        MainCamera.cameraPos += cameraSpeed * cameraFrontXZ;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        MainCamera.cameraPos -= cameraSpeed * cameraFrontXZ;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        MainCamera.cameraPos -= glm::normalize(glm::cross(MainCamera.cameraFront, MainCamera.cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        MainCamera.cameraPos += glm::normalize(glm::cross(MainCamera.cameraFront, MainCamera.cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        MainCamera.cameraPos += cameraSpeed * MainCamera.cameraUp; // Move camera up
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        MainCamera.cameraPos -= cameraSpeed * MainCamera.cameraUp; // Move camera down
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        MainCamera.cameraPos.y += 0.01;
    }
    
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
    {
        if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    
    // Prevents sudden jump in camera movement
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 
    lastX = xpos;
    lastY = ypos;
 
    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
 
    MainCamera.yaw   += xoffset;
    MainCamera.pitch += yoffset;
 
    if(MainCamera.pitch > 89.0f)
        MainCamera.pitch = 89.0f;
    if(MainCamera.pitch < -89.0f)
        MainCamera.pitch = -89.0f;
 
    glm::vec3 direction;
    direction.x = cos(glm::radians(MainCamera.yaw)) * cos(glm::radians(MainCamera.pitch));
    direction.y = sin(glm::radians(MainCamera.pitch));
    direction.z = sin(glm::radians(MainCamera.yaw)) * cos(glm::radians(MainCamera.pitch));
    MainCamera.cameraFront = glm::normalize(direction);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

/// \brief Handles Camera view functions
/// \param shaderPrograms vector of all shaders
/// \param trans transformation matrix
/// \param projection projection matrix
void CameraView(std::vector<unsigned> shaderPrograms, glm::mat4 trans, glm::mat4 projection)
{
    for (unsigned shaderProgram : shaderPrograms)
    {
        glUseProgram(shaderProgram);

        int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        MainCamera.tick();

        glm::mat4 view;
        view = glm::lookAt(MainCamera.cameraPos, MainCamera.cameraPos + MainCamera.cameraFront, MainCamera.cameraUp);

        int viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        // Pass the transformation matrix to the vertex shader
        unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
    }
}

void CollisionChecking()
{
    int W = 0;
    for (Mesh* wall : wallMeshes)
    {
        for (int i = 0; i < sphereMeshes.size(); ++i)
        {
            collision.SphereToAABBCollision(sphereMeshes[i], wall);
        }
        W++;
    }
    
    int p = 0;
    for (Mesh* sphere : sphereMeshes)
    {

        for (int i = p+1; i < sphereMeshes.size(); ++i)
        {
            collision.SphereCollision(sphere, sphereMeshes[i]);
        }
        p++;
    }
}

glm::vec3 RandomColor()
{
    return glm::vec3(
    (rand() % 256) / 255.0f,
    (rand() % 256) / 255.0f,
    (rand() % 256) / 255.0f
);
}
