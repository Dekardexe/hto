#include "Vertex.h"
#include "obj-parser.h"

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SOIL/SOIL.h>
#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

GLuint Program0, Program1;
GLint Attrib_vertex_position0, Attrib_vertex_texture_coordinate0;
GLint Attrib_vertex_position1, Attrib_vertex_texture_coordinate1;
GLuint VBO_0, VBO_1;
GLuint texture0, texture1;

unsigned char* image_banana;
unsigned char* image_spider_monkey;

const char* VertexShaderSource0 = R"(
    #version 330 core

    in vec3 position;
    in vec2 texture_coordinate;

    out vec3 vs_position;
    out vec2 vs_texture_coordinate;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    uniform float offsets[10];

    const float radius = 500.f;

    void main() {
        vs_position = vec4(model * vec4(position.x + radius * sin(offsets[gl_InstanceID]), position.y + radius * cos(offsets[gl_InstanceID]), position.z, 1.0f)).xyz;
        vs_texture_coordinate = vec2(texture_coordinate.x, texture_coordinate.y);

        gl_Position = projection * view * model * vec4(position.x + radius * sin(offsets[gl_InstanceID]), position.y + radius * cos(offsets[gl_InstanceID]), position.z, 1.0f);
    }
)";

const char* FragShaderSource0 = R"(
    #version 330 core
    
    in vec3 vs_position;
    in vec2 vs_texture_coordinate;

    out vec4 color;

    uniform sampler2D texture;

    void main() {
        color = texture(texture, vs_texture_coordinate);
    }
)";

const char* VertexShaderSource1 = R"(
    #version 330 core

    in vec3 position;
    in vec2 texture_coordinate;

    out vec3 vs_position;
    out vec2 vs_texture_coordinate;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main() {
        vs_position = vec4(model * vec4(position, 1.0f)).xyz;
        vs_texture_coordinate = vec2(texture_coordinate.x, texture_coordinate.y);

        gl_Position = projection * view * model * vec4(position, 1.0f);
    }
)";

const char* FragShaderSource1 = R"(
    #version 330 core
    
    in vec3 vs_position;
    in vec2 vs_texture_coordinate;

    out vec4 color;

    uniform sampler2D texture;

    void main() {
        color = texture(texture, vs_texture_coordinate);
    }
)";

void checkOpenGLerror()
{
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        std::cout << "GLerror Log: " << err << std::endl;
    }
}

auto banana_mesh = parse_obj("Models/Banana.obj");
auto spider_monkey_mesh = parse_obj("Models/Spider_Monkey.obj");

void initVBOProgram0()
{
    glGenBuffers(1, &VBO_0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * banana_mesh.size(), banana_mesh.data(), GL_STATIC_DRAW);
    checkOpenGLerror();
}

void initVBOProgram1()
{
    glGenBuffers(1, &VBO_1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * spider_monkey_mesh.size(), spider_monkey_mesh.data(), GL_STATIC_DRAW);
    checkOpenGLerror();
}

void InitVBOs()
{
    initVBOProgram0();
    initVBOProgram1();
}

void ShaderLog(unsigned int shader)
{
    int infologLen = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLen);
    if (infologLen > 1)
    {
        int charsWritten = 0;
        std::vector<char> infoLog(infologLen);
        glGetShaderInfoLog(shader, infologLen, &charsWritten, infoLog.data());
        std::cout << "InfoLog: " << infoLog.data() << std::endl;
    }
}

void initShaderProgram0()
{
    GLuint vShader0 = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader0, 1, &VertexShaderSource0, NULL);
    glCompileShader(vShader0);
    std::cout << "vertex shader \n";
    ShaderLog(vShader0);

    GLuint fShader0 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader0, 1, &FragShaderSource0, NULL);
    glCompileShader(fShader0);
    std::cout << "fragment shader \n";
    ShaderLog(fShader0);

    Program0 = glCreateProgram();

    glAttachShader(Program0, vShader0);
    glAttachShader(Program0, fShader0);

    glLinkProgram(Program0);
    int link_ok0;
    glGetProgramiv(Program0, GL_LINK_STATUS, &link_ok0);
    if (!link_ok0)
    {
        std::cout << "error attach shaders \n";
        return;
    }

    const char* attr_name_position0 = "position";
    Attrib_vertex_position0 = glGetAttribLocation(Program0, attr_name_position0);
    if (Attrib_vertex_position0 == -1)
    {
        std::cout << "could not bind attrib " << attr_name_position0 << std::endl;
        return;
    }

    const char* attr_name_texture_coordinate0 = "texture_coordinate";
    Attrib_vertex_texture_coordinate0 = glGetAttribLocation(Program0, attr_name_texture_coordinate0);
    if (Attrib_vertex_texture_coordinate0 == -1)
    {
        std::cout << "could not bind attrib " << attr_name_texture_coordinate0 << std::endl;
        return;
    }

    checkOpenGLerror();
}

void initShaderProgram1()
{
    GLuint vShader1 = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader1, 1, &VertexShaderSource1, NULL);
    glCompileShader(vShader1);
    std::cout << "vertex shader \n";
    ShaderLog(vShader1);

    GLuint fShader1 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader1, 1, &FragShaderSource1, NULL);
    glCompileShader(fShader1);
    std::cout << "fragment shader \n";
    ShaderLog(fShader1);

    Program1 = glCreateProgram();

    glAttachShader(Program1, vShader1);
    glAttachShader(Program1, fShader1);

    glLinkProgram(Program1);
    int link_ok1;
    glGetProgramiv(Program1, GL_LINK_STATUS, &link_ok1);
    if (!link_ok1)
    {
        std::cout << "error attach shaders \n";
        return;
    }

    const char* attr_name_position1 = "position";
    Attrib_vertex_position1 = glGetAttribLocation(Program1, attr_name_position1);
    if (Attrib_vertex_position1 == -1)
    {
        std::cout << "could not bind attrib " << attr_name_position1 << std::endl;
        return;
    }

    const char* attr_name_texture_coordinate1 = "texture_coordinate";
    Attrib_vertex_texture_coordinate1 = glGetAttribLocation(Program0, attr_name_texture_coordinate1);
    if (Attrib_vertex_texture_coordinate1 == -1)
    {
        std::cout << "could not bind attrib " << attr_name_texture_coordinate1 << std::endl;
        return;
    }

    checkOpenGLerror();
}

void InitShaders()
{
    // Program0
    initShaderProgram0();
    // Program1
    initShaderProgram1();
}

void initTextureProgram0()
{
    int image_width_banana, image_height_banana;
    const char* filename_banana = "Textures/Banana.png";
    image_banana = SOIL_load_image(filename_banana, &image_width_banana, &image_height_banana, NULL, SOIL_LOAD_RGBA);
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &texture0);
    glBindTexture(GL_TEXTURE_2D, texture0);

    if (image_banana)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width_banana, image_height_banana, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_banana);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "could not load texture " << filename_banana << std::endl;
        return;
    }
}

void initTextureProgram1()
{
    int image_width_spider_monkey, image_height_spider_monkey;
    const char* filename_spider_monkey = "Textures/Spider_Monkey.jpg";
    image_spider_monkey = SOIL_load_image(filename_spider_monkey, &image_width_spider_monkey, &image_height_spider_monkey, NULL, SOIL_LOAD_RGBA);
    glActiveTexture(GL_TEXTURE1);
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    if (image_spider_monkey)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width_spider_monkey, image_height_spider_monkey, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_spider_monkey);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "could not load texture " << filename_spider_monkey << std::endl;
        return;
    }
}

void InitTextures()
{
    initTextureProgram0();
    initTextureProgram1();
}

GLfloat xAngle = 0, yAngle = 0, zAngle = 0;

GLuint windowWidth = 1200, windowHeight = 1200;

GLfloat cameraX = 0.f, cameraY = 0.f, cameraZ = 1.f;
GLfloat pitch = 0.0f, yaw = -90.0f, roll = 0.0f;  // Ð¢Ð°Ð½Ð³Ð°Ð¶, ÑÑÑÐºÐ°Ð½ÑÐµ Ð¸ ÐºÑÐµÐ½

void drawProgram0()
{
    glUseProgram(Program0);

    glEnableVertexAttribArray(Attrib_vertex_position0);
    glEnableVertexAttribArray(Attrib_vertex_texture_coordinate0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_0);

    glVertexAttribPointer(Attrib_vertex_position0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
    glVertexAttribPointer(Attrib_vertex_texture_coordinate0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texture_coordinate));

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    const GLuint bananasCount = 10;

    GLfloat offsets[bananasCount];
    GLfloat offset = 0.0;
    offsets[0] = 2.f * 3.14f / ((GLfloat)(bananasCount * bananasCount));
    for (GLint i = 1; i < bananasCount; i += 1)
    {
        offsets[i] += offsets[i-1];
    }
    glUniform1fv(glGetUniformLocation(Program0, "offsets"), bananasCount, offsets);

    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(0.f, 0.f, 0.f));
    //model = glm::rotate(model, glm::radians(xAngle), glm::vec3(1.f, 0.f, 0.f));
    //model = glm::rotate(model, glm::radians(yAngle), glm::vec3(0.f, 1.f, 0.f));
    model = glm::rotate(model, glm::radians(zAngle), glm::vec3(0.f, 0.f, 1.f));
    model = glm::scale(model, glm::vec3(0.1f));

    glm::vec3 camera_position(cameraX, cameraY, cameraZ);
    glm::vec3 camera_up(0.0f, 0.0f, 1.0f);
    glm::vec3 camera_front = glm::normalize(
        glm::vec3(
            cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
            sin(glm::radians(pitch)),
            sin(glm::radians(yaw)) * cos(glm::radians(pitch))
        )
    );

    glm::mat4 view(1.0f);
    view = glm::lookAt(camera_position, camera_position + camera_front, camera_up);

    GLfloat field_of_view = 120.0f;
    GLfloat near_plane = 0.01f;
    GLfloat far_plane = 10000.0f;
    glm::mat4 projection(1.0f);
    projection = glm::perspective(glm::radians(field_of_view), static_cast<GLfloat>(windowWidth) / windowHeight, near_plane, far_plane);

    glUniformMatrix4fv(glGetUniformLocation(Program0, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(Program0, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(Program0, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glUniform1i(glGetUniformLocation(Program0, "texture"), 0);
    glDrawArraysInstanced(GL_QUADS, 0, banana_mesh.size(), bananasCount);

    glDisableVertexAttribArray(Attrib_vertex_position0);
    glDisableVertexAttribArray(Attrib_vertex_texture_coordinate0);

    glUseProgram(0);

    checkOpenGLerror();
}

void drawProgram1()
{
    glUseProgram(Program1);

    glEnableVertexAttribArray(Attrib_vertex_position1);
    glEnableVertexAttribArray(Attrib_vertex_texture_coordinate1);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_1);

    glVertexAttribPointer(Attrib_vertex_position1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
    glVertexAttribPointer(Attrib_vertex_texture_coordinate1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texture_coordinate));

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(0.f, 0.f, 0.f));
    //model = glm::rotate(model, glm::radians(xAngle), glm::vec3(1.f, 0.f, 0.f));
    //model = glm::rotate(model, glm::radians(yAngle), glm::vec3(0.f, 1.f, 0.f));
    //model = glm::rotate(model, glm::radians(zAngle), glm::vec3(0.f, 0.f, 1.f));
    model = glm::scale(model, glm::vec3(0.5f));

    glm::vec3 camera_position(cameraX, cameraY, cameraZ);
    glm::vec3 camera_up(0.0f, 0.0f, 1.0f);
    glm::vec3 camera_front = glm::normalize(
        glm::vec3(
            cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
            sin(glm::radians(pitch)),
            sin(glm::radians(yaw)) * cos(glm::radians(pitch))
        )
    );

    glm::mat4 view(1.0f);
    view = glm::lookAt(camera_position, camera_position + camera_front, camera_up);

    GLfloat field_of_view = 120.0f;
    GLfloat near_plane = 0.01f;
    GLfloat far_plane = 10000.0f;
    glm::mat4 projection(1.0f);
    projection = glm::perspective(glm::radians(field_of_view), static_cast<GLfloat>(windowWidth) / windowHeight, near_plane, far_plane);

    glUniformMatrix4fv(glGetUniformLocation(Program1, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(Program1, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(Program1, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glUniform1i(glGetUniformLocation(Program1, "texture"), 1);
    glDrawArrays(GL_QUADS, 0, spider_monkey_mesh.size());

    glDisableVertexAttribArray(Attrib_vertex_position1);
    glDisableVertexAttribArray(Attrib_vertex_texture_coordinate1);

    glUseProgram(0);

    checkOpenGLerror();
}

void Draw()
{
    drawProgram1();
    drawProgram0();
}

void InitOptions()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Init()
{
    InitOptions();
    InitShaders();
    InitTextures();
    InitVBOs();
}

void ReleaseVBOs()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &VBO_0);
    glDeleteBuffers(1, &VBO_1);
}

void ReleaseShaders()
{
    glUseProgram(0);
    glDeleteProgram(Program0);
    glDeleteProgram(Program1);
}

void ReleaseTextures()
{
    glActiveTexture(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    SOIL_free_image_data(image_banana);
    SOIL_free_image_data(image_spider_monkey);
}

void Release()
{
    ReleaseShaders();
    ReleaseVBOs();
    ReleaseTextures();
}

int main()
{
    sf::Window window(sf::VideoMode(windowWidth, windowHeight), "Solar System Model", sf::Style::Default, sf::ContextSettings(24));
    window.setVerticalSyncEnabled(true);
    window.setActive(true);

    glewInit();
    Init();
    while (window.isOpen())
    {
        xAngle += 0.5f;
        yAngle += 0.5f;
        zAngle += 0.5f;
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            else if (event.type == sf::Event::Resized)
            {
                glViewport(0, 0, event.size.width, event.size.height);
                windowWidth = event.size.width;
                windowHeight = event.size.height;
            }
            else if (event.type == sf::Event::KeyPressed)
            {
                switch (event.key.code) 
                {
                case sf::Keyboard::A:
                    pitch += 5.0f;
                    break;
                case sf::Keyboard::Z:
                    pitch -= 5.0f;
                    break;
                case sf::Keyboard::S:
                    yaw += 5.0f;
                    break;
                case sf::Keyboard::X:
                    yaw -= 5.0f;
                    break;
                case sf::Keyboard::D:
                    roll += 5.0f;
                    break;
                case sf::Keyboard::C:
                    roll -= 5.0f;
                    break;
                case sf::Keyboard::G:
                    cameraX += 1.0f;
                    break;
                case sf::Keyboard::B:
                    cameraX -= 1.0f;
                    break;
                case sf::Keyboard::H:
                    cameraY += 1.0f;
                    break;
                case sf::Keyboard::N:
                    cameraY -= 1.0f;
                    break;
                case sf::Keyboard::J:
                    cameraZ += 0.5;
                    break;
                case sf::Keyboard::M:
                    cameraZ -= 0.5;
                    break;
                }
            }
        }
        glClearColor(0.1f, 0.6f, 0.2f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Draw();
        window.display();
    }
    Release();
    return 0;
}
