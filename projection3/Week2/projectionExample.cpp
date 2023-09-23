// OpenGL headers
#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <iostream>

// SDL headers
#include <SDL_main.h>
#include <SDL.h>
#include <SDL_opengl.h>

// GLM
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

using namespace std;

SDL_Window* window;
SDL_GLContext glContext;

GLuint gProgramID = 0;
GLuint gVBO1 = 0;
GLuint gVBO2 = 0;
GLuint gVAO = 0;

GLuint pMatrixId = 0;

void initGL();
void printShaderLog(GLuint shader);

int main(int argc, char* argv[]){
    SDL_Event sdlEvent;
    bool quit = false;
    //Use OpenGL 3.1 core
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);


    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Hello World!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    glContext = SDL_GL_CreateContext(window);
    glewInit();
    initGL();

 

    // main loop
    while (!quit)
    {
        while (SDL_PollEvent(&sdlEvent)) {
            if (sdlEvent.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Set background color as cornflower blue
        glClearColor(0.39f, 0.58f, 0.93f, 1.f);
        // Clear color buffer
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(gProgramID);
        glBindVertexArray(gVAO);

        glm::mat4 projectionMatrix = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f);
        glUniformMatrix4fv(pMatrixId, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
        glViewport(720, 540, 80, 60);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glViewport(0, 0, 800, 600);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glUseProgram(NULL);
        SDL_GL_SwapWindow(window);
    }

    //Destroy window
    SDL_DestroyWindow(window);
    window = NULL;

    //Quit SDL subsystems
    SDL_Quit();

    return 0;
}

void initGL() {

    gProgramID = glCreateProgram();
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const GLchar* vertexShaderSource[] = {
        "#version 330\n uniform mat4 pMatrix = mat4(1.0); in vec2 LVertexPos2D; in vec3 inColor; out vec3 color; void main() { gl_Position = pMatrix * vec4(LVertexPos2D.x,LVertexPos2D.y,0.0f,1.0f); color = inColor; }"
    };
    glShaderSource(vertexShader, 1, vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    GLint vcompiled = GL_FALSE;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vcompiled);
    if (!vcompiled) {
        cout << "Vertex shader compile error" << endl;
        printShaderLog(vertexShader);
    }
    glAttachShader(gProgramID, vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const GLchar* fragmentShaderShource[] =
    {
        "#version 330\n  out vec4 LFragment; in vec3 color; void main(){ LFragment = vec4( color,1.0f); }"
    };
    glShaderSource(fragmentShader, 1, fragmentShaderShource, NULL);
    glCompileShader(fragmentShader);
    GLint fcompiled = GL_FALSE;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &fcompiled);
    if (!fcompiled) {
        cout << "Fragment shader compile error" << endl;
        printShaderLog(fragmentShader);
    }

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fcompiled);

    glAttachShader(gProgramID, fragmentShader);

    glLinkProgram(gProgramID);

    GLfloat vertexData[] = { -0.5f, -0.5f,   0.5f,-0.5f,   0.5f, 0.5f,  -0.5f, 0.5f };
    GLfloat colorData[] = { 1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.5f, 0.0f, 0.5f };

    glGenVertexArrays(1, &gVAO);
    glBindVertexArray(gVAO);

    glGenBuffers(1, &gVBO1);
    glBindBuffer(GL_ARRAY_BUFFER, gVBO1);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);

    GLuint LVertexPos2DLoc = glGetAttribLocation(gProgramID, "LVertexPos2D");
    glEnableVertexAttribArray(LVertexPos2DLoc);
    glVertexAttribPointer(LVertexPos2DLoc, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);

    glGenBuffers(1, &gVBO2);
    glBindBuffer(GL_ARRAY_BUFFER, gVBO2);
    glBufferData(GL_ARRAY_BUFFER, 3 * 4 * sizeof(GLfloat), colorData, GL_STATIC_DRAW);

    GLuint inColorLoc = glGetAttribLocation(gProgramID, "inColor");
    glEnableVertexAttribArray(inColorLoc);
    glVertexAttribPointer(inColorLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

    pMatrixId = glGetUniformLocation(gProgramID, "pMatrix");

}

void printShaderLog(GLuint shader) {
    int infoLength = 0;
    int maxLength = infoLength;

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
    char* infoLog = new char[maxLength];

    glGetShaderInfoLog(shader, maxLength, &infoLength, infoLog);
    if (infoLength > 0) {
        cout << infoLog;
    }

    delete[] infoLog;
}