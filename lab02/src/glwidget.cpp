#include "glwidget.h"
#include "settings.h"

#include "cs123_lib/resourceloader.h"

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"            // glm::vec*, mat*, and basic glm functions
#include "glm/gtx/transform.hpp"  // glm::translate, scale, rotate
#include "glm/gtc/type_ptr.hpp"   // glm::value_ptr

#include "openglshape.h"
#include "gl/shaders/ShaderAttribLocations.h"
#include <iostream>
#include "glwidget.h"
#include "cs123_lib/resourceloader.h"

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"            // glm::vec*, mat*, and basic glm functions
#include "glm/gtx/transform.hpp"  // glm::translate, scale, rotate
#include "glm/gtc/type_ptr.hpp"   // glm::value_ptr

#include "openglshape.h"
#include "gl/shaders/ShaderAttribLocations.h"
#include <math.h>
#include <iostream>
#include "sphere.h"

GLWidget::GLWidget(QGLFormat format, QWidget *parent, bool tab0)
    : QGLWidget(format, parent),
      m_square(nullptr),
      m_solidProgramID(0),
      m_gradientProgramID(0),
      m_textureProgramID(0),
      m_textureID(0),
      m_tab0(tab0),
      m_program(0),
      m_sphere(nullptr),
      m_timer(this),
      m_fps(60.0f),
      m_increment(0)
{
    // Set up 60 FPS draw loop.
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(tick()));
    m_timer.start(1000.0f / m_fps);
    m_tick = 1.0;
    m_angle = 0.f;
}

GLWidget::~GLWidget()
{
    // TODO: Clean up GPU memory. (Task 15)
    glDeleteTextures(1, &m_textureID);
}


void GLWidget::initializeGL() {
    if(m_tab0) {
        initializeGLTransformationsVertexShaders();
    } else {
        initializeGLFragmentShaders();
    }
}

void GLWidget::initializeGLTransformationsVertexShaders() {
    ResourceLoader::initializeGlew();

    // Set the color to set the screen when the color buffer is cleared.scale
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Creates the shader program that will be used for drawing.
    m_program = ResourceLoader::createShaderProgram(":/shaders/shader.vert", ":/shaders/shader.frag");

    // Initialize sphere with radius 0.5 centered at origin.
    std::vector<GLfloat> sphereData = SPHERE_VERTEX_POSITIONS;

    m_sphere = std::make_unique<OpenGLShape>();
    m_sphere->setVertexData(&sphereData[0], sphereData.size(), VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLES, NUM_SPHERE_VERTICES);
    m_sphere->setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    m_sphere->buildVAO();

    // Initialize square on XY plane that extends from -1 to 1.
    std::vector<GLfloat> squareData = {-1.f, -1.f, 0.f,
                                       -1.f, +1.f, 0.f,
                                       +1.f, -1.f, 0.f,
                                       +1.f, +1.f, 0.f};

    m_square = std::make_unique<OpenGLShape>();
    m_square->setVertexData(&squareData[0], squareData.size(), VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_STRIP, 4);
    m_square->setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    m_square->buildVAO();

    // TODO: Enable depth testing. (Task 6)
    glEnable(GL_DEPTH_TEST);
}

void GLWidget::initializeGLFragmentShaders() {
    ResourceLoader::initializeGlew();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Defines the color the screen will be cleared to.

    // Creates the three shader programs.
    m_solidProgramID = ResourceLoader::createShaderProgram(":/shaders/solid.vert", ":/shaders/solid.frag");
    m_gradientProgramID = ResourceLoader::createShaderProgram(":/shaders/gradient.vert", ":/shaders/gradient.frag");
    m_textureProgramID = ResourceLoader::createShaderProgram(":/shaders/texture.vert", ":/shaders/texture.frag");

    // Smart pointer!
    m_square = std::make_unique<OpenGLShape>();

    static constexpr int kFloatsPerVertex = 8;
    std::vector<float> coordinates = {
        -0.5f,  0.5f,  0.0f,
        1.0f, 0.0f, 0.0f,
        0, 0,
        -0.5f, -0.5f,  0.0f,
        0.0f, 1.0f, 0.0f,
        0, 1,
         0.5f,  0.5f,  0.0f,
        1.0f, 1.0f, 0.0f,
        1, 0,
         0.5f, -0.5f,  0.0f,
        0.0f, 0.0f, 1.0f,
        1, 1
    };
    m_square->setVertexData(coordinates.data(),
                            coordinates.size(),
                            VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_STRIP,
                            coordinates.size() / kFloatsPerVertex);
    m_square->setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    m_square->setAttribute(ShaderAttrib::COLOR, 3, 3*sizeof(float), VBOAttribMarker::DATA_TYPE::FLOAT, false);
    m_square->setAttribute(ShaderAttrib::TEXCOORD0, 2, 6*sizeof(float), VBOAttribMarker::DATA_TYPE::FLOAT, false);

    // TODO: Interleave positions and colors in the array used to intialize m_square (Task 10)

    // TODO: Don't forget to add the color attribute similar to how you did for the position (Task 10)

    // TODO: Interleave UV-coordinates along with positions and colors in your VBO (Task 14)

    m_square->buildVAO();

    // TODO: Initialize texture map.  Follow the handout for specific instructions. (Task 13)
    QImage image(":/images/ostrich.jpg");
    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
}

void GLWidget::paintGLTransformationsVertexShaders() {
    // Clear the color and depth buffers.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float time = m_increment++ / (float) m_fps;      // Time in seconds.

        float fieldOfViewY = 0.8f;                       // Vertical field of view angle, in radians.
        float aspectRatio = (float)width() / height();   // Aspect ratio of the window.
        float nearClipPlane = 0.1f;                      // Near clipping plane.
        float farClipPlane = 100.f;                      // Far clipping plane.

        // TODO: Adjust the eye coordinates so the camera goes in a circle of radius 6 where
        // y is always equal to 1. (Task 7)

        glm::vec3 eye = glm::vec3(6.f*cos(time), 1.f, 6.f*sin(time));        // Camera position.
        glm::vec3 center = glm::vec3(0.f, 1.f, 0.f);     // Where camera is looking.
        glm::vec3 up = glm::vec3(0.f, 1.f, 0.f);         // Up direction.


        glUseProgram(m_program);

        glUniform3f(glGetUniformLocation(m_program, "color"), 0.5, 0.4, 0.8);

        // TODO: Use the equation to translate the ball. (Task 7)

        // TODO: Generate model matrix and pass it to vertex shader. (Task 3)
        glm::mat4 trans_matrix = glm::translate(glm::vec3(0, 0.5 + fabs(sin(3* time)), 0));
        GLint uniformloc = glGetUniformLocation(m_program, "model");
        glUniformMatrix4fv(uniformloc, 1, GL_FALSE, glm::value_ptr(trans_matrix));
        // TODO: Generate view matrix and pass it to vertex shader. (Task 4)
        glm::mat4 view_matrix = glm::lookAt(eye, center, up);
        uniformloc = glGetUniformLocation(m_program, "view");
        glUniformMatrix4fv(uniformloc, 1, GL_FALSE, glm::value_ptr(view_matrix));
        // TODO: Generate projection matrix and pass it to vertex shader. (Task 4)
        glm::mat4 proj_matrix = glm::perspective(fieldOfViewY, aspectRatio, nearClipPlane, farClipPlane);
        uniformloc = glGetUniformLocation(m_program, "perspective");
        glUniformMatrix4fv(uniformloc, 1, GL_FALSE, glm::value_ptr(proj_matrix));
        // TODO: Draw sphere here! (Task 1)
        m_sphere->draw();

        // TODO: Change color. (Task 5)
        glUniform3f(glGetUniformLocation(m_program, "color"), 0.0, 0.5, 0.7);


        // TODO: Scale the square x2. (Task 7)
        glm::mat4 scale_matrix = glm::scale(glm::vec3(2, 2, 2));
        // TODO: Rotate the square to lie flat on the XZ plane. (Task 7)
        glm::mat4 rotate_matrix = glm::rotate(static_cast<float>(M_PI/2), glm::vec3(1, 0, 0));
        uniformloc = glGetUniformLocation(m_program, "model");
        glUniformMatrix4fv(uniformloc, 1, GL_FALSE, glm::value_ptr(rotate_matrix * scale_matrix));
        trans_matrix = glm::translate(glm::vec3(0, 0.5 + fabs(sin(3*time)), 0));


        // TODO: Draw the square. (Task 5)
        m_square->draw();

        glUseProgram(0);
}

void GLWidget::paintGLFragmentShaders() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    switch (settings.shaderProgram) {
        case SOLID_SHADER_PROGRAM:
            // TODO: Use m_solidProgramID as the program. (Task 8)
            glUseProgram(m_solidProgramID);
            // TODO: Set the uniform's value to a color other than white. (Task 9)
            glUniform3f(glGetUniformLocation(m_solidProgramID, "color"), 0.2, 0.5, 0.5);

            // TODO: Draw the square, and then unbind the program. (Task 8)
            m_square->draw();
            glUseProgram(0);
            break;
        case GRADIENT_SHADER_PROGRAM:
            // TODO: Draw the square using m_gradientProgramID. (Task 12)
            glUseProgram(m_gradientProgramID);
            m_square->draw();
            glUseProgram(0);
            // TODO: Draw the square, and then unbind the program. (Task 12)
            break;
        case TEXTURE_SHADER_PROGRAM:
            // TODO: Use m_textureProgramID. (Task 15)
            glUseProgram(m_textureProgramID);
            // TODO: Bind the texture. (Task 15)
            glBindTexture(GL_TEXTURE_2D, m_textureID);
            // TODO: Draw the square and unbind the program. (Task 15)
            m_square->draw();
            glUseProgram(0);
            break;
    }

}

/** Repaints the canvas. Called 60 times per second. */
void GLWidget::tick()
{
    m_tick += 1;

    m_angle += M_PI / 60.f;

    if(m_angle >= 2 * M_PI) {
        m_angle = 0.f;
    }

    update();
}

void GLWidget::paintGL() {
    if(m_tab0) {
        paintGLTransformationsVertexShaders();
    } else {
        paintGLFragmentShaders();
    }
}

void GLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}
