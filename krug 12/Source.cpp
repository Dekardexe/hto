#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>

GLuint Program;
GLint Attrib_vertex;
GLint Attrib_color;
GLuint VBO;

struct Vertex
{
    GLfloat x, y;
    GLfloat r, g, b;
};

const char* VertexShaderSource = R"(
    #version 330 core
    in vec3 coord;
    in vec3 color;
    out vec3 VertexColor;
    void main() {
        gl_Position = vec4(coord, 2.0);
        VertexColor = color;
    }
)";

const char* FragShaderSource = R"(
    #version 330 core
    in vec3 VertexColor;
    out vec4 color;
    void main() {
        color = vec4(VertexColor, 0);
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

const GLint numberOfVertices = 360 + 1 + 2;

void InitVBO(GLfloat xScale, GLfloat yScale)
{
    glGenBuffers(1, &VBO);

    Vertex triangles[numberOfVertices];

    const GLfloat x0 = 0, y0 = 0;

    // Ѕелый центр
    triangles[0] = { x0 * xScale, y0 * yScale, 1, 1, 1 };

    const GLfloat kPI = 3.14;
    const GLfloat radianMultiplier = kPI / 180;
    const GLfloat kRadius = 1;
    const GLfloat rgbMax = 1.0;

    const GLint numberOfVerticesSixth = numberOfVertices / 6;

    // »нтерполируем цвета по первой 1/6 части дуги окружности (Red -> Yellow)
    for (GLint i = 1; i < numberOfVerticesSixth; i++) {
        GLfloat xCurrentOffset = cos(i * radianMultiplier);
        GLfloat yCurrentOffset = sin(i * radianMultiplier);
        GLfloat g = (GLfloat)(i - 1) / numberOfVerticesSixth;
        triangles[i] = {
            (x0 + kRadius * xCurrentOffset) * xScale,
            (y0 + kRadius * yCurrentOffset) * yScale,
            rgbMax, g, 0
        };
    }

    // »нтерполируем цвета по второй 1/6 части дуги окружности (Yellow -> Green)
    for (GLint i = numberOfVerticesSixth; i < numberOfVerticesSixth * 2; i++) {
        GLfloat xCurrentOffset = cos(i * radianMultiplier);
        GLfloat yCurrentOffset = sin(i * radianMultiplier);
        GLfloat r = (GLfloat)(numberOfVerticesSixth * 2 - i) / numberOfVerticesSixth;
        triangles[i] = {
            (x0 + kRadius * xCurrentOffset) * xScale,
            (y0 + kRadius * yCurrentOffset) * yScale,
            r, rgbMax, 0
        };
    }

    // »нтерполируем цвета по третьей 1/6 части дуги окружности (Green -> Cyan)
    for (GLint i = numberOfVerticesSixth * 2; i < numberOfVerticesSixth * 3; i++) {
        GLfloat xCurrentOffset = cos(i * radianMultiplier);
        GLfloat yCurrentOffset = sin(i * radianMultiplier);
        GLfloat b = (GLfloat)(i - numberOfVerticesSixth * 2) / numberOfVerticesSixth;
        triangles[i] = {
            (x0 + kRadius * xCurrentOffset) * xScale,
            (y0 + kRadius * yCurrentOffset) * yScale,
            0, rgbMax, b
        };
    }

    // »нтерполируем цвета по четвертой 1/6 части дуги окружности (Cyan -> Blue)
    for (GLint i = numberOfVerticesSixth * 3; i < numberOfVerticesSixth * 4; i++) {
        GLfloat xCurrentOffset = cos(i * radianMultiplier);
        GLfloat yCurrentOffset = sin(i * radianMultiplier);
        GLfloat g = (numberOfVerticesSixth * 4 - i) / (GLfloat)numberOfVerticesSixth;
        triangles[i] = {
            (x0 + kRadius * xCurrentOffset) * xScale,
            (y0 + kRadius * yCurrentOffset) * yScale,
            0, g, rgbMax
        };
    }

    // »нтерполируем цвета по п¤той 1/6 части дуги окружности (Blue -> Magenta)
    for (GLint i = numberOfVerticesSixth * 4; i < numberOfVerticesSixth * 5; i++) {
        GLfloat xCurrentOffset = cos(i * radianMultiplier);
        GLfloat yCurrentOffset = sin(i * radianMultiplier);
        GLfloat r = (i - numberOfVerticesSixth * 4) / (GLfloat)numberOfVerticesSixth;
        triangles[i] = {
            (x0 + kRadius * xCurrentOffset) * xScale,
            (y0 + kRadius * yCurrentOffset) * yScale,
            r, 0, rgbMax
        };
    }

    // »нтерполируем цвета по шестой 1/6 части дуги окружности (Magenta -> Red)
    for (GLint i = numberOfVerticesSixth * 5; i < numberOfVerticesSixth * 6 + 3; i++) {
        GLfloat xCurrentOffset = cos(i * radianMultiplier);
        GLfloat yCurrentOffset = sin(i * radianMultiplier);
        GLfloat b = (numberOfVertices - i) / (GLfloat)numberOfVerticesSixth;
        triangles[i] = {
            (x0 + kRadius * xCurrentOffset) * xScale,
            (y0 + kRadius * yCurrentOffset) * yScale,
            rgbMax, 0, b
        };
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangles), triangles, GL_STATIC_DRAW);
    checkOpenGLerror();
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

void InitShader()
{
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &VertexShaderSource, NULL);
    glCompileShader(vShader);
    std::cout << "vertex shader \n";
    ShaderLog(vShader);
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &FragShaderSource, NULL);
    glCompileShader(fShader);
    std::cout << "fragment shader \n";
    ShaderLog(fShader);
    Program = glCreateProgram();
    glAttachShader(Program, vShader);
    glAttachShader(Program, fShader);
    glLinkProgram(Program);
    int link_ok;
    glGetProgramiv(Program, GL_LINK_STATUS, &link_ok);
    if (!link_ok)
    {
        std::cout << "error attach shaders \n";
        return;
    }
    const char* coord_attr_name = "coord";
    Attrib_vertex = glGetAttribLocation(Program, coord_attr_name);
    if (Attrib_vertex == -1)
    {
        std::cout << "could not bind coord attrib " << coord_attr_name << std::endl;
        return;
    }
    const char* color_attr_name = "color";
    Attrib_color = glGetAttribLocation(Program, color_attr_name);
    if (Attrib_color == -1)
    {
        std::cout << "could not bind color attrib " << color_attr_name << std::endl;
        return;
    }
    checkOpenGLerror();
}

void Draw()
{
    glUseProgram(Program);

    glEnableVertexAttribArray(Attrib_vertex);
    glEnableVertexAttribArray(Attrib_color);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // јтрибут с координатами
    glVertexAttribPointer(Attrib_vertex, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    // јтрибут с цветом
    glVertexAttribPointer(Attrib_color, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_TRIANGLE_FAN, 0, numberOfVertices);
    glDisableVertexAttribArray(Attrib_vertex);
    glDisableVertexAttribArray(Attrib_color);
    glUseProgram(0);
    checkOpenGLerror();
}

void Init(GLfloat x, GLfloat y)
{
    InitShader();
    InitVBO(x, y);
}

void ReleaseVBO()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &VBO);
}

void ReleaseShader()
{
    glUseProgram(0);
    glDeleteProgram(Program);
}

void Release()
{
    ReleaseShader();
    ReleaseVBO();
}

int main()
{
    sf::Window window(sf::VideoMode(1000, 1000), "HSV Hue Circle", sf::Style::Default, sf::ContextSettings(24));
    window.setVerticalSyncEnabled(true);
    window.setActive(true);

    glewInit();
    GLfloat xScale = 1.0, yScale = 1.0;
    Init(xScale, yScale);
    GLfloat scaleStep = 0.1;
    while (window.isOpen())
    {
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
            }
            else if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                case sf::Keyboard::Left:
                    if (xScale - scaleStep > 0) {
                        xScale -= scaleStep;
                        Init(xScale, yScale);
                    }
                    break;
                case sf::Keyboard::Right:
                    if (xScale + scaleStep < 2) {
                        xScale += scaleStep;
                        Init(xScale, yScale);
                    }
                    break;
                case sf::Keyboard::Up:
                    if (yScale + scaleStep < 2) {
                        yScale += scaleStep;
                        Init(xScale, yScale);
                    }
                    break;
                case sf::Keyboard::Down:
                    if (yScale - scaleStep > 0) {
                        yScale -= scaleStep;
                        Init(xScale, yScale);
                    }
                    break;
                }
            }
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Draw();
        window.display();
    }
    Release();
    return 0;
}
