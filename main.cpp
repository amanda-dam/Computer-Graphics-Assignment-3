
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include"shader.h"

#define TINYOBJLOADER_IMPLEMENTATION 
#include "tiny_obj_loader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>

static unsigned int ss_id = 0;
void dump_framebuffer_to_ppm(std::string prefix, unsigned int width, unsigned int height) {
    int pixelChannel = 3;
    int totalPixelSize = pixelChannel * width * height * sizeof(GLubyte);
    GLubyte* pixels = new GLubyte[totalPixelSize];
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    std::string file_name = prefix + std::to_string(ss_id) + ".ppm";
    std::ofstream fout(file_name);
    fout << "P3\n" << width << " " << height << "\n" << 255 << std::endl;
    for (size_t i = 0; i < height; i++)
    {
        for (size_t j = 0; j < width; j++)
        {
            size_t cur = pixelChannel * ((height - i - 1) * width + j);
            fout << (int)pixels[cur] << " " << (int)pixels[cur + 1] << " " << (int)pixels[cur + 2] << " ";
        }
        fout << std::endl;
    }
    ss_id++;
    delete[] pixels;
    fout.flush();
    fout.close();
}

//key board control
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window)
{
    //press escape to exit
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    //press p to capture screen
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        std::cout << "Capture Window " << ss_id << std::endl;
        int buffer_width, buffer_height;
        glfwGetFramebufferSize(window, &buffer_width, &buffer_height);
        dump_framebuffer_to_ppm("floor_tex", buffer_width, buffer_height);
    }
}

// obj file
std::string obj_path = "asset/timmy.obj";
std::string obj_path2 = "asset/bucket.obj";
std::string obj_path3 = "asset/floor.obj";


// settings
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

void loadAndProcessMesh(const std::string& objPath,
                         std::vector<tinyobj::shape_t> shapes,
                         std::vector<float>& vbuffer, 
                         std::vector<float>& nbuffer, 
                         std::vector<float>& tbuffer) {
                            
    tinyobj::attrib_t attrib;
    //std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    // Load OBJ file
    std::string warn, err;
    bool bTriangulate = true;
    bool bSuc = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, objPath.c_str(), nullptr, bTriangulate);
    
    if (!bSuc) {
        std::cout << "Tinyobj error: " << err.c_str() << std::endl;
        return;
    }
    
    // Process mesh data (changed from what was given)
    for (const auto& shape : shapes) {
        for (auto id : shape.mesh.indices) {
            int vid = id.vertex_index;
            int nid = id.normal_index;
            int tid = id.texcoord_index;
            // fill in vertex positions
            vbuffer.push_back(attrib.vertices[vid * 3]);
            vbuffer.push_back(attrib.vertices[vid * 3 + 1]);
            vbuffer.push_back(attrib.vertices[vid * 3 + 2]);
            // normal
            nbuffer.push_back(attrib.normals[nid * 3]);
            nbuffer.push_back(attrib.normals[nid * 3 + 1]);
            nbuffer.push_back(attrib.normals[nid * 3 + 2]);
            // tex coord
            tbuffer.push_back(attrib.texcoords[tid * 2]);
            tbuffer.push_back(attrib.texcoords[tid * 2 + 1]);
        }
    }
    
}

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "A3", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

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

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("default.vert", "default.frag");

    glm::vec3 spotlightPositions[] = {
        glm::vec3( 0.0f,  200.0f,  0.0f),
        glm::vec3( 0.0f,  200.0f,  0.0f),
        glm::vec3( 0.0f,  200.0f,  0.0f)
    };

    glm::vec3 spotlightDirections[] = {
        glm::vec3(50.0f, -200.0f, -50.0f),
        glm::vec3(-50.0f, -200.0f, -50.0f),
        glm::vec3(0.0f, -200.0f, 50.0f)
    };

    std::vector<float> vbuffer, nbuffer, tbuffer;
    std::vector<float> vbuffer2, nbuffer2, tbuffer2;
    std::vector<float> vbuffer3, nbuffer3, tbuffer3;

    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::shape_t> shapes2;
    std::vector<tinyobj::shape_t> shapes3;

    loadAndProcessMesh(obj_path, shapes, vbuffer, nbuffer, tbuffer);
    loadAndProcessMesh(obj_path2, shapes2, vbuffer2, nbuffer2, tbuffer2);
    loadAndProcessMesh(obj_path3, shapes3, vbuffer3, nbuffer3, tbuffer3);

    // load and create texture 1
    unsigned int textureT;
    glGenTextures(1, &textureT);
    glBindTexture(GL_TEXTURE_2D, textureT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load("asset/timmy.png", &width, &height, &nrChannels, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);//don’t forget to release the image data

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureT);
    ourShader.setInt("textureT", 0);

    // load and create texture 2
    unsigned int texture2;
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width2, height2, nrChannels2;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data2 = stbi_load("asset/bucket.jpg", &width2, &height2, &nrChannels2, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
    stbi_image_free(data2);//don’t forget to release the image data

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    ourShader.setInt("texture2", 1);

    // load and create texture 3
    unsigned int texture3;
    glGenTextures(1, &texture3);
    glBindTexture(GL_TEXTURE_2D, texture3);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width3, height3, nrChannels3;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data3 = stbi_load("asset/floor.jpeg", &width3, &height3, &nrChannels3, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width3, height3, 0, GL_RGB, GL_UNSIGNED_BYTE, data3);
    stbi_image_free(data3);//don’t forget to release the image data

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texture3);
    ourShader.setInt("texture3", 2);

    // create two VBOs and VAOs
    unsigned int vertexVBO[3], normalVBO[3], VAO[3], tcoordVBO[3];
    glGenVertexArrays(3, VAO);
    glGenBuffers(3, vertexVBO);
    glGenBuffers(3, normalVBO);
    glGenBuffers(3, tcoordVBO);

    // first mesh and texture set up
    glBindVertexArray(VAO[0]);

    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, vbuffer.size() * sizeof(float), &vbuffer[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, normalVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, nbuffer.size() * sizeof(float), &nbuffer[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    
    //texture coord attribute
    glBindBuffer(GL_ARRAY_BUFFER, tcoordVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, tbuffer.size() * sizeof(float), &tbuffer[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);

    // second mesh and texture set up
    
    glBindVertexArray(VAO[1]);

    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO[1]);
    glBufferData(GL_ARRAY_BUFFER, vbuffer2.size() * sizeof(float), &vbuffer2[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, normalVBO[1]);
    glBufferData(GL_ARRAY_BUFFER, nbuffer2.size() * sizeof(float), &nbuffer2[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    
    //texture coord attribute
    glBindBuffer(GL_ARRAY_BUFFER, tcoordVBO[1]);
    glBufferData(GL_ARRAY_BUFFER, tbuffer2.size() * sizeof(float), &tbuffer2[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);

    // third mesh and texture set up
    glBindVertexArray(VAO[2]);

    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO[2]);
    glBufferData(GL_ARRAY_BUFFER, vbuffer3.size() * sizeof(float), &vbuffer3[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, normalVBO[2]);
    glBufferData(GL_ARRAY_BUFFER, nbuffer3.size() * sizeof(float), &nbuffer3[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    
    //texture coord attribute
    glBindBuffer(GL_ARRAY_BUFFER, tcoordVBO[2]);
    glBufferData(GL_ARRAY_BUFFER, tbuffer3.size() * sizeof(float), &tbuffer3[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);

    ourShader.use();
    ourShader.setInt("textureT", 0);
    ourShader.setInt("texture2", 1);
    ourShader.setInt("texture3", 2);

    ourShader.use();

    // lighting
    //ourShader.setVec3("diffuseColor", 1.0f, 1.0f, 1.0f);
   // ourShader.setVec3("ambientColor", 0.2f, 0.2f, 0.2f);

    ourShader.setFloat("kc", 1.0f);
    ourShader.setFloat("kl", 0.35 * 1e-4);
    ourShader.setFloat("kq", 0.44 * 1e-4);
    float theta = 0.0f;

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.3f, 0.4f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

        //glm::vec3 lightPos(sin(theta) * 150, 100, cos(theta) * 150);
        //theta += 0.05f;
        //glm::vec3 lightPos(0.0, 200.0, 0.0);
        //ourShader.setVec3("lightPos", lightPos);

        // set spotlight 0
        theta += 0.05f;
        glm::vec3 position0(sin(theta) * 150, 100, cos(theta) * 150);
        ourShader.setVec3("spotlights[0].position", position0);
        ourShader.setVec3("spotlights[0].direction", spotlightDirections[0]);
        ourShader.setFloat("spotlights[0].cutoff",glm::cos(M_PI/6.0f));
        ourShader.setVec3("spotlights[0].diffuseColor", 1.0f, 0.0f, 0.0f);
        ourShader.setVec3("spotlights[0].ambientColor", 0.2f, 0.2f, 0.2f);

        // set spotlight 1
        glm::vec3 position1(sin(theta) * 150, 100, cos(theta) * 150);
        ourShader.setVec3("spotlights[1].position", position1);
        ourShader.setVec3("spotlights[1].direction",spotlightDirections[1]);
        ourShader.setFloat("spotlights[1].cutoff", glm::cos(M_PI/6.0f));
        ourShader.setVec3("spotlights[1].diffuseColor", 0.0f, 1.0f, 0.0f);
        ourShader.setVec3("spotlights[1].ambientColor", 0.2f, 0.2f, 0.2f);

        // set spotlight 2
        glm::vec3 position2(sin(theta) * 150, 100, cos(theta) * 150);
        ourShader.setVec3("spotlights[2].position", position2);
        ourShader.setVec3("spotlights[2].direction", spotlightDirections[2]);
        ourShader.setFloat("spotlights[2].cutoff", glm::cos(M_PI/6.0f));
        ourShader.setVec3("spotlights[2].diffuseColor", 0.0f, 0.0f, 1.0f);
        ourShader.setVec3("spotlights[2].ambientColor", 0.2f, 0.2f, 0.2f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureT);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texture3);

        // activate shader

        // create transformations
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);

        //model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
        ourShader.setMat4("model", model);

        view = glm::lookAt(glm::vec3(50, 100, 200), glm::vec3(0, 80, 0), glm::vec3(0, 1, 0)); // Camera is at (0,0,10), in World Space
        ourShader.setMat4("view", view);

        projection = glm::perspective(glm::radians(60.0f), 4.0f / 3.0f, 0.1f, 1000.0f); // Camera is at (0,0,10), in World Space
        ourShader.setMat4("projection", projection);

        // render boxes
        // Bind and set textureT for the first mesh
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureT);
        ourShader.setInt("textureT", 0);

        // Render the first mesh
        glUniform1i(glGetUniformLocation(ourShader.ID, "shapeID"), 0);
        glBindVertexArray(VAO[0]);
        glDrawArrays(GL_TRIANGLES, 0, vbuffer.size() / 3);

        // Bind and set texture2 for the second mesh
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        ourShader.setInt("texture2", 1); 

        // Render the second mesh
        glUniform1i(glGetUniformLocation(ourShader.ID, "shapeID"), 1);
        glBindVertexArray(VAO[1]);
        glDrawArrays(GL_TRIANGLES, 0, vbuffer2.size() / 3);

        // Bind and set texture3 for the third mesh
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texture3);
        ourShader.setInt("texture3", 2); 

        // Render the third mesh
        glUniform1i(glGetUniformLocation(ourShader.ID, "shapeID"), 2);
        glBindVertexArray(VAO[2]);
        glDrawArrays(GL_TRIANGLES, 0, vbuffer3.size() / 3);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, VAO);
    glDeleteBuffers(1, vertexVBO);
    glDeleteBuffers(1, normalVBO);


    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
