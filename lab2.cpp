#include <SFML/Graphics.hpp>
#include <GL/glew.h>
#include <iostream>
#include <string>
#include <vector>
#include <format>
using namespace std;

// C�������� �������
struct Vertex {
	GLfloat x;
	GLfloat y;
};

map <size_t, string> ErrType = {{0x0500, "GL_INVALID_ENUM"},
								{0x0501, "GL_INVALID_VALUE"},
								{0x0502, "GL_INVALID_OPERATION"},
								{0x0503, "GL_STACK_OVERFLOW"},
								{0x0504, "GL_STACK_UNDERFLOW"},
								{0x0505, "GL_OUT_OF_MEMORY"},
								{0x0506, "GL_INVALID_FRAMEBUFFER_OPERATION"},
								{0x0507, "GL_CONTEXT_LOST"},
								{0x8031, "GL_TABLE_TOO_LARGE"},
};

size_t active_figure = 0;
size_t cnt = 0;
GLenum mode = GL_TRIANGLES;

vector<Vertex> quad = {		{0.9f, 0.9f },
							{ 0.9f, -0.9f },
							{ -0.9f, -0.9f },
							{ -0.9f, 0.9f } };
vector<Vertex> beep = {		{ -0.9f, -0.9f },
							{ -0.9f, 0.2f },
							{ -0.1f, 0.6f },
							{ 0.6f, 0.3f },
							{ 0.8f, -0.6f} };
vector<Vertex> pentagon = { { 0.0f, 0.92f },
							{ 0.95f, 0.23f },
							{ 0.59f, -0.89f },
							{ -0.59f, -0.89f },
							{ -0.95f, 0.23f } };
vector<Vertex> triangle = { { 1.0f, 1.0f },
							{ -1.0, 1.0f },							
							{ 0.0f, -1.0f } };
vector<vector<Vertex>> figures = vector<vector<Vertex>>{quad, beep, pentagon, triangle};
// ID ��������� ���������
GLuint Program;
// ID ��������
GLint Attrib_vertex;
// ID Vertex Buffer Object
GLuint VBO;


// �������� ��� ���������� �������
const char* VertexShaderSource = R"(
#version 330 core
in vec3 coord;
void main() {
gl_Position = vec4(coord, 1.0);
}
)";

// �������� ��� ������������ �������
const char* FragShaderSource = R"(
#version 330 core
out vec4 color;
void main() {
color = vec4((800 - gl_FragCoord.x)/800, 1 - (800 - gl_FragCoord.x)/800, (800 - gl_FragCoord.y)/800, 1);
}
)";

void ShaderLog(unsigned int shader)
{
	int infologLen =  0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLen);
	if (infologLen > 1)
	{
		int charsWritten = 0;
		std::vector<char> infoLog(infologLen);
		glGetShaderInfoLog(shader, infologLen, &charsWritten, infoLog.data());
		std::cout << "InfoLog: " << infoLog.data() << std::endl;
	}
}

void checkOpenGLerror() {
	auto err = glGetError();
	if (err != GL_NO_ERROR)
		std::cout << ErrType[err] << std::endl;
}

void InitVBO() {
	glGenBuffers(1, &VBO);
	// ������� ����� ������
	vector<Vertex> fig = figures[active_figure];
	const int len = fig.size();
	Vertex figure[5];
	std::copy(fig.begin(), fig.end(), figure);	
	// �������� ������� � �����
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(figure), figure, GL_STATIC_DRAW);
	checkOpenGLerror(); //������ ������� ���� � ������������
	// �������� ������ OpenGL, ���� ���� �� ����� � ������� ��� ������
}

void InitShader() {
	// ������� ��������� ������
	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	// �������� �������� ���
	glShaderSource(vShader, 1, &VertexShaderSource, NULL);
	// ����������� ������
	glCompileShader(vShader);
	std::cout << "vertex shader \n";
	// ������� ������ ���� �������
	ShaderLog(vShader); //������ ������� ���� � ������������
	// ������� ����������� ������
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
	// �������� �������� ���
	glShaderSource(fShader, 1, &FragShaderSource, NULL);
	// ����������� ������
	glCompileShader(fShader);
	std::cout << "fragment shader \n";
	// ������� ������ ���� �������
	ShaderLog(fShader);
	// ������� ��������� � ����������� ������� � ���
	Program = glCreateProgram();
	glAttachShader(Program, vShader);
	glAttachShader(Program, fShader);
	// ������� ��������� ���������
	glLinkProgram(Program);

	const char* attr_name = "coord"; //��� � �������
	Attrib_vertex = glGetAttribLocation(Program, attr_name);
	if (Attrib_vertex == -1) {
		std::cout << "could not bind attrib " << attr_name << std::endl;
		return;
	}

	// ��������� ������ ������
	int link_ok;
	glGetProgramiv(Program, GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		std::cout << "error attach shaders \n";
		return;
	}
	
	checkOpenGLerror();
}

void Init() {
	// �������
	InitShader();
	//	��������������
	InitVBO();
}

void Draw(size_t vertex_count, GLenum mode) {
	// ������������� ��������� ��������� �������
	glUseProgram(Program);
	// �������� ������ ���������
	glEnableVertexAttribArray(Attrib_vertex);
	// ���������� VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// �������� pointer 0 ��� ������������ ������, �� ��������� ��� ������ � VBO
	glVertexAttribPointer(Attrib_vertex, 2, GL_FLOAT, GL_FALSE, 0, 0);
	// ��������� VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// �������� ������ �� ����������(������)
	glDrawArrays(mode, 0, vertex_count);
	// ��������� ������ ���������
	glDisableVertexAttribArray(Attrib_vertex);
	// ��������� ��������� ���������
	glUseProgram(0);
	checkOpenGLerror();
}

// ������������ ��������
void ReleaseShader() {
	// ��������� ����, �� ��������� ��������� ���������
	glUseProgram(0);
	// ������� ��������� ���������
	glDeleteProgram(Program);
}
// ������������ ������
void ReleaseVBO() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VBO);
}

void Release() {
	// �������
	ReleaseShader();
	// ��������� �����
	ReleaseVBO();
}

void SelectFigure(size_t num) {
	switch (active_figure)
	{
	case 0: {
		cnt = 4;
		mode = GL_QUADS;
		break;
	}
	case 1: {
		cnt = 5;
		mode = GL_TRIANGLE_FAN;
		break;
	}
	case 2: {
		cnt = 5;
		mode = GL_POLYGON;
		break;
	}
	case 3: {
		cnt = 3;
		mode = GL_TRIANGLES;
		break;
	}
	default:
		std::cout << "wrong figure" << std::endl;
		break;
	}
}


int main() {
    active_figure = 3; // ����� ������	
	SelectFigure(active_figure);
	sf::Window window(sf::VideoMode(800, 800), "My OpenGL window",
		sf::Style::Default, sf::ContextSettings(24));
	window.setVerticalSyncEnabled(true);
	window.setActive(true);
	glewInit();
	Init();
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) { window.close(); }
			else if (event.type == sf::Event::Resized) {
				glViewport(0, 0, event.size.width,
					event.size.height);
			}
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Draw(cnt, mode);
		window.display();
	}
	Release();
	return 0;
}

